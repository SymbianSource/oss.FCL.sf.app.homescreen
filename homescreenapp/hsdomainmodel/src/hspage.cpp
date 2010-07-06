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

#include <HbInstance>

#include "hsdomainmodeldatastructures.h"
#include "hspage.h"
#include "hspagetoucharea.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"


/*!
    \class HsPage
    \ingroup group_hsdomainmodel
    \brief Represents a page in the framework.
    HsPage contains group of widgets. HsPage can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
    \a aFlags Window flags.
*/
HsPage::HsPage(QGraphicsItem* parent)
    : HbWidget(parent),
      mDatabaseId(-1),
      mWallpaper(0),
      mRemovable(true),
      mTouchArea(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents);
    setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                              QSizePolicy::Ignored));

    setupTouchArea();
}

/*!
    Destructor.
*/
HsPage::~HsPage()
{
    delete mWallpaper;
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

void HsPage::setGeometry(const QRectF &rect)
{
    if (mTouchArea) {
        mTouchArea->resize(rect.size());
    }

    HbWidget::setGeometry(rect);
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

    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mWallpaper = new HsPageWallpaper(this);
    }

    foreach (HsWidgetData data, datas) {
        HsWidgetHost *widget = new HsWidgetHost(data.id);
        mWidgets.append(widget);
        connectWidget(widget);
        widget->setPage(this);
        widget->setParentItem(this);
        widget->startWidget(isDefaultPage());
    }

    connect(HsScene::mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(onOrientationChanged(Qt::Orientation)));

    return true;
}

/*!
    Return wallpaper. 
*/
HsWallpaper *HsPage::wallpaper() const
{
    return mWallpaper;
}
/*!
    Add given existing \a widgetHost to a page. Returns true if successful
*/
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
/*!
    Remove given \a widgetHost from a page. Widget is not deleted. 
    Returns true if successful
*/
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
/*!
    Returns list of new widgets belonging to a page. Widgets which are
    not yet layouted are considered as new widgets. 
*/
QList<HsWidgetHost *> HsPage::newWidgets()
{
    return mNewWidgets;
}

/*!
    Adds new widget into a page. Returns true if successfull.
*/
bool HsPage::addNewWidget(HsWidgetHost* widgetHost, const QPointF &touchPoint)
{
    mTouchPoint = touchPoint;

    if (!widgetHost || mWidgets.contains(widgetHost)) {
        return false;
    }

    if (mNewWidgets.contains(widgetHost)) {
        return true;
    }

    HsWidgetPresentationData presentation;
    presentation.orientation = HsScene::orientation();
    if (!widgetHost->getPresentation(presentation)) {
        presentation.orientation = HsScene::orientation();
        presentation.setPos(mTouchPoint);
        presentation.zValue = 0;
        widgetHost->savePresentation(presentation);
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

    HsPageNewWidgetLayout *newWidgetLayout = static_cast<HsPageNewWidgetLayout *>(layout());
    if (!newWidgetLayout) {
        newWidgetLayout = new HsPageNewWidgetLayout(mTouchPoint);
        setLayout(newWidgetLayout);
    }    
    updateZValues();
    HsWidgetHost *widget = 0;
    for (int i = 0; i < mNewWidgets.count(); ++i) {
        widget = mNewWidgets.at(i);
        newWidgetLayout->addItem(widget);
        widget->setPage(this);
        widget->setParentItem(this);
        widget->showWidget();
        widget->show();
    }
    mWidgets << mNewWidgets;
    mNewWidgets.clear();
}

/*!
    Remove page and all it's contained widgets from database 
*/
bool HsPage::deleteFromDatabase()
{
    foreach (HsWidgetHost *widget, mWidgets) {
        widget->remove();
    }
    mWidgets.clear();

    foreach (HsWidgetHost *widget, mNewWidgets) {
        widget->remove();
    }
    mNewWidgets.clear();

    foreach (HsWidgetHost *widget, mUnavailableWidgets) {
        widget->remove();
    }
    mUnavailableWidgets.clear();

    return HsDatabase::instance()->deletePage(mDatabaseId);
}
/*!
    Return list of widgets belonging to a page 
*/
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
/*!
    Return true if page is default page.
*/
bool HsPage::isDefaultPage() const
{
    return mDatabaseId == HSCONFIGURATION_GET(defaultPageId);
}
/*!
    Return true if page is active page.
*/
bool HsPage::isActivePage() const
{
    return this == HsScene::instance()->activePage();
}
/*!
    Create page into database and return instance of a new page.
*/
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
/*!
    Update widgets z-values and persist those. Active widget has top most 
    z-value.
*/
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
            widget->savePresentation();
        }
    }

    if (!mNewWidgets.isEmpty()) {
        foreach (HsWidgetHost *widget, mNewWidgets) {
            widget->setZValue(z++);
            widget->savePresentation();
        }
    }
}
/*!
    Return this page's index.
*/
int HsPage::pageIndex()
{
    return HsScene::instance()->pages().indexOf(this);
}
/*!
    Create touch area for page.
*/
void HsPage::setupTouchArea()
{
    mTouchArea = new HsPageTouchArea(this);
    mTouchArea->setZValue(-1);
}
/*!
    Utility to connect widget signals to page.
*/
void HsPage::connectWidget(HsWidgetHost *widget)
{
    connect(widget, SIGNAL(finished()), SLOT(onWidgetFinished()));
    connect(widget, SIGNAL(faulted()), SLOT(onWidgetFaulted()));
    connect(widget, SIGNAL(resized()), SLOT(onWidgetResized()));
    connect(widget, SIGNAL(available()), SLOT(onWidgetAvailable()));
    connect(widget, SIGNAL(unavailable()), SLOT(onWidgetUnavailable()));
}
/*!
    Disconnect widget signals from page
*/
void HsPage::disconnectWidget(HsWidgetHost *widget)
{
    widget->disconnect(this);
}
/*!
    Disconnect and remove widget
*/
void HsPage::onWidgetFinished()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());

    // It can be in new widget list if we haven't layouted it yet
    // or layouted new widget and widget list
    if (!mNewWidgets.removeOne(widget)) {
        mWidgets.removeOne(widget);
    }

    disconnectWidget(widget);
    widget->remove();
}
/*!
    Remove widget if it faulted
*/
void HsPage::onWidgetFaulted()
{
    onWidgetFinished();
}

/*!
    Calculates new widget position on page when widget size changes. If page has layout then there are new widgets
    and we use layout to calculate new widget positions.
*/
void HsPage::onWidgetResized()
{
    if ( !layout() ) {
        HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());
        QRectF widgetRect = widget->geometry();
        QRectF pageRect = HsScene::mainWindow()->layoutRect();
        qreal widgetX = qBound(qreal(0), widgetRect.x(), pageRect.width() - widgetRect.width());
        qreal widgetY = qBound(qreal(64), widgetRect.y(), pageRect.height() - widgetRect.height());
        widget->setPos(widgetX, widgetY);
    } else {
        layout()->invalidate();
    }
}
/*!
    Show widget if it came available
*/
void HsPage::onWidgetAvailable()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());

    mUnavailableWidgets.removeOne(widget);
    mWidgets.append(widget);

    widget->setParentItem(this);
    widget->startWidget(isActivePage());
    widget->show();
}
/*!
    Update internal bookkeeping and hide widget
*/
void HsPage::onWidgetUnavailable()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());

    if (mWidgets.contains(widget)) {
        mWidgets.removeOne(widget);
    } else if (mNewWidgets.contains(widget)) {
        mNewWidgets.removeOne(widget);
    } else {
        return;
    }

    mUnavailableWidgets.append(widget);

    widget->hide();
    widget->setParentItem(0);
}
/*!
    Run positioning algorithm for widgets which don't have position on 
    target orientation. Otherwise set orientation positions for widgets.
*/
void HsPage::onOrientationChanged(Qt::Orientation orientation)
{
    QRectF rect = HsScene::mainWindow()->layoutRect();

    HsWidgetPositioningOnOrientationChange *converter =
        HsWidgetPositioningOnOrientationChange::instance();

    qreal chrome = 64;
    QRectF from(0, chrome, rect.height(), rect.width() - chrome);
    QRectF to(0, chrome, rect.width(), rect.height() - chrome);

    HsWidgetPresentationData presentation;
    presentation.orientation = orientation;

    foreach (HsWidgetHost *widget, mWidgets) {
        if (!widget->getPresentation(presentation)) {
            QList<QRectF> geometries = converter->convert(
                from, QList<QRectF>() << widget->geometry(), to);
            widget->setGeometry(geometries.first());
            widget->savePresentation();
        } else {
            widget->setPos(presentation.pos());
            widget->setZValue(presentation.zValue);
        }
    }
}

/*!
    Clears new widgets list and resets layout.
*/
void HsPage::resetNewWidgets()
{
    mNewWidgets.clear();
    setLayout(0);
}

