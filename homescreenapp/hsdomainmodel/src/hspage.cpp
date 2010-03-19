/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <QPainter>
#include <QGraphicsLinearLayout>
#include <HbInstance>

#include "hspage.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswidgetdata.h"
#include "hswidgetpresentationdata.h"
#include "hspagedata.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"


/*!
    \class HsPage
    \ingroup group_hsutils
    \brief Represents a page in the framework.
    HsPage is a parent for a group of widgets. HsPage can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
    \a aFlags Window flags.
*/
HsPage::HsPage(QGraphicsItem* parent)
    : HbWidget(parent),
      mDatabaseId(-1),
      mRemovable(true)
{
    setFlag(QGraphicsItem::ItemHasNoContents);
    setSizePolicy(QSizePolicy(QSizePolicy::Ignored, 
                              QSizePolicy::Ignored));
}

/*!
    Destructor.
*/
HsPage::~HsPage()
{
}

/*!
    Returns the database id.
*/
int HsPage::databaseId() const
{
    return mDatabaseId;
}
 
/*!
    Sets the database id to \a id.
*/
void HsPage::setDatabaseId(int id)
{
    mDatabaseId = id;
}

/*!
    Loads widgets.
*/
bool HsPage::load()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    HsPageData pageData;
    if (!db->page(mDatabaseId, pageData, true)) {
        return false;
    }

    QList<HsWidgetData> widgetDatas = pageData.widgets();
    foreach (HsWidgetData widgetData, widgetDatas) {
        HsWidgetHost *widget = new HsWidgetHost(widgetData.id());
        connectWidget(widget);
        widget->load();

		if (!widget->isValid()) {
			qDebug() << "HsPage: Widget loading failed.";
			continue;
		}

        widget->setPage(this);
        mWidgets << widget;
    }

    foreach (HsWidgetHost *widget, mWidgets) {
        widget->initializeWidget();
        widget->showWidget();
        widget->setParentItem(this);
    }
    
    return true;    
}

bool HsPage::addExistingWidget(HsWidgetHost *widgetHost)
{
    if (!widgetHost) {
        return false;
    }
    
    if (mWidgets.contains(widgetHost)) {
        return true;
    }

    if (!widgetHost->setPage(this)) {
        return false;
    }

    connectWidget(widgetHost);
    mWidgets << widgetHost;
    widgetHost->setParentItem(this);
    
    return true;
 }

bool HsPage::removeWidget(HsWidgetHost *widgetHost)
{
    if (!widgetHost || !widgetHost->setPage(0)) {
        return false;
    }

    disconnectWidget(widgetHost);
    mWidgets.removeOne(widgetHost);
    widgetHost->setParentItem(0);

    return true;
}

QList<HsWidgetHost *> HsPage::newWidgets()
{
    return mNewWidgets;
}

/*!
    Adds new widget into a page. Returns true if successfull. 
*/
bool HsPage::addNewWidget(HsWidgetHost* widgetHost)
{
    if (!widgetHost || mWidgets.contains(widgetHost)) {
        return false;
    }

    if (mNewWidgets.contains(widgetHost)) {
        return true;
    }
    
    // Set presentation.
    QString key = HsScene::orientation() == Qt::Horizontal ? 
        "landscape" : "portrait";

    HsWidgetPresentationData presentation;
    if (!widgetHost->widgetPresentationData(key, presentation)) {
        presentation.setKey(key);
        presentation.setPosition(QPointF());
        presentation.setSize(widgetHost->preferredSize());
        presentation.setZValue(0);       
        widgetHost->setWidgetPresentationData(presentation);
    }

    widgetHost->hide();
    widgetHost->setGeometry(QRectF(presentation.position(), 
                                   presentation.size()));
    widgetHost->setZValue(presentation.zValue());   
    
    connectWidget(widgetHost);
    mNewWidgets << widgetHost;

    return true;
}

/*!
    Layouts all the new widgets
*/
void HsPage::layoutNewWidgets()
{
    QList<QRectF> oldWidgetGeometries;

    foreach(HsWidgetHost *widget, mNewWidgets) {
        oldWidgetGeometries << widget->rect();
    }

    QList<QRectF> newWidgetGeometries = 
        HsWidgetPositioningOnWidgetAdd::instance()->convert(
        rect().adjusted(10, 10, -10, -10),
        oldWidgetGeometries);

    updateZValues();

    HsWidgetHost *widget = NULL;
    for(int i = 0; i < mNewWidgets.count(); ++i) {  
        widget = mNewWidgets[i];
        widget->setGeometry(newWidgetGeometries[i]);
        widget->setWidgetPresentation();
        widget->setPage(this);
        widget->setParentItem(this);
        widget->show();
        mWidgets << widget;
    }
}

/*!
    Resets the new widgets list
*/
void HsPage::resetNewWidgets()
{
    mNewWidgets.clear();
}

bool HsPage::deleteFromDatabase()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    db->transaction();

    foreach (HsWidgetHost *widget, mWidgets) {
        if (!widget->deleteFromDatabase()) {
            db->rollback();
            return false;
        }
    }

    if (!db->deletePage(mDatabaseId))  {
        db->rollback();
        return false;
    }

    db->commit();
    return true;
}

QList<HsWidgetHost *> HsPage::widgets() const
{
    return mWidgets;
}

/*!
    Returns true if the page can be removed. Otherwise,
    returns false.
*/
bool HsPage::isRemovable() const
{
    return mRemovable; 
}

/*!
    Sets removable flag to \a removable.
*/
void HsPage::setRemovable(bool removable)
{
    mRemovable = removable; 
}

bool HsPage::updateZValues()
{
    if (mWidgets.isEmpty()) {
        return true;
    }
    
    QMultiMap<qreal, HsWidgetHost *> map;
    foreach (HsWidgetHost *widget, mWidgets) {
        map.insert(widget->zValue(), widget);
    }
    
    QList<HsWidgetHost *> sortedWidgets = map.values();
            
    HsWidgetHost *activeWidget = HsScene::instance()->activeWidget();    
    if (sortedWidgets.contains(activeWidget)) {
        sortedWidgets.removeOne(activeWidget);
        sortedWidgets.append(activeWidget);        
    }

    int z = 0;
    foreach (HsWidgetHost *widget, sortedWidgets) {
        widget->setZValue(z++);
        widget->setWidgetPresentation();
    }

    foreach (HsWidgetHost *widget, mNewWidgets) {
        widget->setZValue(z++);
        widget->setWidgetPresentation();
    }

    return true;
}
/*!
    Propogate online state to widgets.

*/
void HsPage::setOnline(bool online)
{
    foreach (HsWidgetHost *widget, mNewWidgets) {
        widget->setOnline(online);
    }
    foreach (HsWidgetHost *widget, mWidgets) {
        widget->setOnline(online);
    }
}

HsPage *HsPage::createInstance(const HsPageData &pageData)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    
    HsPageData data(pageData);    
    if (!db->insertPage(data)) {
        return 0;
    }

    HsPage *page = new HsPage;
    page->setDatabaseId(data.id());
    return page;
}

void HsPage::connectWidget(HsWidgetHost *widget)
{
    connect(widget, SIGNAL(widgetFinished(HsWidgetHost*)),
            SLOT(onWidgetFinished(HsWidgetHost*)));
}
 
void HsPage::disconnectWidget(HsWidgetHost *widget)
{
    widget->disconnect(this);
}

void HsPage::onWidgetFinished(HsWidgetHost *widget)
{
    if (mNewWidgets.contains(widget)) {
        mNewWidgets.removeOne(widget);
    } else if (mWidgets.contains(widget)) {
        mWidgets.removeOne(widget);        
    } else {
        return;
    }

    disconnectWidget(widget);
    widget->uninitializeWidget();
    widget->deleteFromDatabase();
    widget->deleteLater();
}
