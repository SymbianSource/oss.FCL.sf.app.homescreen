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
#include <QRectF>
#include <QGraphicsLinearLayout>
#include <HbInstance>

#include "hsdomainmodeldatastructures.h"
#include "hspage.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
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
    if (mDatabaseId < 0) {
        return false;
    }

    HsDatabase *db = HsDatabase::instance();
    
    QList<HsWidgetData> datas;
    if (!db->widgets(mDatabaseId, datas)) {
        return false;
    }

    foreach (HsWidgetData data, datas) {
        QScopedPointer<HsWidgetHost> widget(new HsWidgetHost(data.id));
        if(!widget->load() || !widget->isValid()) {
            continue;
		}
        connectWidget(widget.data());
        widget->setPage(this);
        widget->initializeWidget();
        widget->showWidget();
        mWidgets.append(widget.data());
        widget->setParentItem(this);
        widget.take(); // now this page owns widget
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
        presentation.key = key;
        presentation.setPos(QPointF());
        presentation.zValue = 0;
        widgetHost->setWidgetPresentationData(presentation);
    }

    widgetHost->hide();
    widgetHost->setPos(presentation.x, presentation.y);
    widgetHost->setZValue(presentation.zValue);
    
    connectWidget(widgetHost);
    mNewWidgets << widgetHost;

    return true;
}

/*!
    Layouts all the new widgets
*/
void HsPage::layoutNewWidgets()
{
    if (mNewWidgets.isEmpty()) {
        return;
    }
    
    QString key = HsScene::orientation() == Qt::Horizontal ? 
        "landscape" : "portrait";

    QList<QRectF> rects;

    foreach (HsWidgetHost *newWidget, mNewWidgets) {
        rects << newWidget->rect();
    }
    
    HsWidgetPositioningOnWidgetAdd *algorithm = 
        HsWidgetPositioningOnWidgetAdd::instance();
   
    QList<QRectF> calculatedRects = 
        algorithm->convert(HsScene::mainWindow()->layoutRect(), rects, QPointF());
    
    updateZValues();

    HsWidgetHost *widget = 0;
    for (int i = 0; i < mNewWidgets.count(); ++i) {
        widget = mNewWidgets.at(i);
        widget->setGeometry(calculatedRects.at(i));
        widget->setWidgetPresentation();
        widget->setPage(this);
        widget->setParentItem(this);
        widget->show();
    }
    mWidgets << mNewWidgets;
    mNewWidgets.clear();
}


bool HsPage::deleteFromDatabase()
{
    HsDatabase *db = HsDatabase::instance();    
    
    foreach (HsWidgetHost *widget, mWidgets) {
        if (!widget->deleteFromDatabase()) {            
            return false;
        }
    }

    if (!db->deletePage(mDatabaseId))  {
        return false;
    }

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

HsPage *HsPage::createInstance(const HsPageData &pageData)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    
    HsPageData data(pageData);    
    if (db->insertPage(data)) {
        HsPage *page = new HsPage;
        page->setDatabaseId(data.id);
        return page;
    }

    return 0;
}

/*!
    Calls onShow() for contained widgets.
*/
void HsPage::showWidgets()
{
    foreach (HsWidgetHost *widget, mWidgets) {
        if (widget->parentItem() == this) {
            widget->showWidget();
        }
    }
}
 
/*!
    Calls onHide() for contained widgets.
*/
void HsPage::hideWidgets()
{
    foreach (HsWidgetHost *widget, mWidgets) {
        if (widget->parentItem() == this) {
            widget->hideWidget();
        }
    }
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

void HsPage::updateZValues()
{
    int z = 0;

    if (!mWidgets.isEmpty()) {
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
        
        foreach (HsWidgetHost *widget, sortedWidgets) {
            widget->setZValue(z++);
            widget->setWidgetPresentation();
        }
    }

    if (!mNewWidgets.isEmpty()) {
        foreach (HsWidgetHost *widget, mNewWidgets) {
            widget->setZValue(z++);
            widget->setWidgetPresentation();
        }
    }
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
    Q_ASSERT(widget);
    // It can be in new widget list if we haven't layouted it yet
    // or layouted new widget and widget list
    if (!mNewWidgets.removeOne(widget)) {
        mWidgets.removeOne(widget);
    }
   
    disconnectWidget(widget);
    widget->uninitializeWidget();
    widget->deleteFromDatabase();
    widget->deleteLater();
}