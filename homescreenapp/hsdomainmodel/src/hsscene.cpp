/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QApplication>

#include <HbInstance>
#include <HbMainWindow>
#include <HbDeviceProfile>

#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hsdatabase.h"
#include "hswallpaper.h"

/*!
    Destructor.
*/
HsScene::~HsScene()
{
    delete mWallpaper;
    qDeleteAll(mPages);
}

/*!
    Return database id.
*/
int HsScene::databaseId() const
{
    return mDatabaseId;
}

/*!
    Load scene from database.
*/
bool HsScene::load()
{
    HsDatabase *db = HsDatabase::instance();

    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        return false;
    }

    mMaximumPageCount = sceneData.maximumPageCount;

    calculateWidgetSizeLimitations(sceneData);

    if (sceneData.portraitWallpaper.isEmpty()) {
        mWallpaper->setImagesById();
    } else {
        mWallpaper->setImagesByPaths(
            sceneData.landscapeWallpaper, 
            sceneData.portraitWallpaper);
    }

    QList<HsPageData> pageDatas;
    if (!db->pages(pageDatas) || pageDatas.empty()) {
        return false;
    }
    
    foreach (HsPageData pageData, pageDatas) {
        HsPage *page = new HsPage;
        page->setDatabaseId(pageData.id);
        if (page->load()) {
            mPages.append(page);
        } else {
            delete page;
            continue;
        }
        if (pageData.id == sceneData.defaultPageId) {
            mActivePage = page;
            mActivePage->setRemovable(false);
        }
    }

    QApplication::instance()->installEventFilter(this);

	return true;
}

/*!
    Calculate maximum and minimum widget sizes 
*/
void HsScene::calculateWidgetSizeLimitations(HsSceneData &sceneData)
{
    // 1un = 6.7px = 2mm
    mMaximumWidgetSizeInUnits = QSizeF(sceneData.maximumWidgetWidth,
                                       sceneData.maximumWidgetHeight);
    mMinimumWidgetSizeInUnits = QSizeF(sceneData.minimumWidgetWidth,
                                       sceneData.minimumWidgetHeight);
    HbDeviceProfile profile;
    qreal unitToPixelFactor = profile.unitValue();
    mMaximumWidgetSizeInPixels = QSizeF(sceneData.maximumWidgetWidth * unitToPixelFactor,
                                        sceneData.maximumWidgetHeight * unitToPixelFactor);
    mMinimumWidgetSizeInPixels = QSizeF(sceneData.minimumWidgetWidth * unitToPixelFactor,
                                        sceneData.minimumWidgetHeight * unitToPixelFactor);
}

/*!
    Return wallpaper. 
*/
HsWallpaper *HsScene::wallpaper() const
{
    return mWallpaper;
}

/*!
   Return pages array.
*/
QList<HsPage *> HsScene::pages() const
{
    return mPages;
}
    
/*!
    Add page \a page.
*/
bool HsScene::addPage(HsPage *page)
{
    if (!page) {
        return false;
    }
   
    if (mPages.contains(page)) {
        return true;
    }

    HsDatabase *db = HsDatabase::instance();
    
    HsPageData data;
    data.id = page->databaseId();
    if (!db->page(data)) {
        return false;
    }

    db->transaction();

    for (int i = data.indexPosition; i < mPages.count(); ++i) {
        data.id = mPages.at(i)->databaseId();
        data.indexPosition = i + 1;
        if (!db->updatePage(data)) {
            db->rollback();
            return false;
        }
    }
    
    db->commit();
    
    mPages.insert(data.indexPosition, page);
    return true;
}
    
/*!
    Removes page \a page.
*/
bool HsScene::removePage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (!mPages.contains(page)) {
        return true;
    }

    int index = mPages.indexOf(page) + 1;

    HsDatabase *db = HsDatabase::instance();
    db->transaction();

    HsPageData data;
    for (int i = index; i < mPages.count(); ++i) {
        data.id = mPages.at(i)->databaseId();
        data.indexPosition = i - 1;
        if (!db->updatePage(data)) {
            db->rollback();
            return false;
        }
    }

    if (!page->deleteFromDatabase()) {
        db->rollback();
        return false;
    }

    db->commit();

    mPages.removeOne(page);
    return true;
}
 
/*!
    Set active page \a page.
*/
bool HsScene::setActivePage(HsPage *page)
{
    if (!page) {
        return false;
    }
   
    if (!mPages.contains(page)) {
        return false;
    }

    if (page == mActivePage) {
        return true;
    }

    mActivePage = page;

    foreach (HsPage *p, mPages) {
        if (p == mActivePage) {
            p->showWidgets();
        } else {
            p->hideWidgets();
        }
    }

    return true;
}

/*!
    Set active page \a index.
*/
bool HsScene::setActivePageIndex(int index)
{
    if (index < 0 || mPages.count() <= index) {
        return false;
    }
    return setActivePage(mPages[index]);
}

/*!
    Return active page.
*/
HsPage *HsScene::activePage() const
{
    return mActivePage;
}

/*!
    Return active page index.
*/
int HsScene::activePageIndex() const
{
    return mPages.indexOf(mActivePage);
}

/*!
    Return maximum number of pages.
*/
int HsScene::maximumPageCount() const
{
    return mMaximumPageCount;
}

/*!
    Return maximum widget size in pixels.
*/
QSizeF HsScene::maximumWidgetSizeInPixels() const
{
    return mMaximumWidgetSizeInPixels;
}

/*!
    Return minimum widget size in pixels.
*/
QSizeF HsScene::minimumWidgetSizeInPixels() const
{
    return mMinimumWidgetSizeInPixels;
}

/*!
    Return maximum widget size in units.
*/
QSizeF HsScene::maximumWidgetSizeInUnits() const
{
    return mMaximumWidgetSizeInUnits;
}

/*!
    Return minimum widget size in units.
*/
QSizeF HsScene::minimumWidgetSizeInUnits() const
{
    return mMinimumWidgetSizeInUnits;
}

/*!
    Set active widget \a widget.
*/
void HsScene::setActiveWidget(HsWidgetHost *widget)
{
    mActiveWidget = widget;
}
 
/*!
   Return active widget.
*/
HsWidgetHost *HsScene::activeWidget() const
{
    return mActiveWidget;
}
/*!
    Toggle application online state. Defaults 
    to true.
*/
void HsScene::setOnline(bool online)
{
    mIsOnline = online;
    foreach (HsPage *page, mPages) {
        page->setOnline(online);
    }
}

/*!
    Return current online setting.
*/
bool HsScene::isOnline()const
{
    return mIsOnline;
}

/*!
    Singleton. 
*/
HsScene *HsScene::instance()
{
    if (!mInstance) {
        mInstance.reset(new HsScene);
    }
    return mInstance.data();
}

/*!
    Return main window.
*/
HbMainWindow *HsScene::mainWindow()
{
    return hbInstance->allMainWindows().first();
}

/*!
    Return current orientation.
*/
Qt::Orientation HsScene::orientation()
{
    return mainWindow()->orientation();
}

/*!
    Listens for application background/foreground changes.
*/
bool HsScene::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
        case QEvent::ApplicationActivate:
            mActivePage->showWidgets();
            break;
		case QEvent::ApplicationDeactivate:
            mActivePage->hideWidgets();
            break;
        default:
            break;
	}
    return QObject::eventFilter(watched, event);
}

/*!
    Constructor
*/
HsScene::HsScene(QObject *parent)
  : QObject(parent),
    mDatabaseId(-1),    
    mWallpaper(0),    
    mActivePage(0),
    mActiveWidget(0),
    mIsOnline(true),
    mMaximumPageCount(1),
    mMaximumWidgetSizeInPixels(341, 268),
    mMinimumWidgetSizeInPixels(20, 20),
    mMaximumWidgetSizeInUnits(51, 40),
    mMinimumWidgetSizeInUnits(3, 3)
{
    mWallpaper = new HsWallpaper;
}

/*!
    Points to the scene instance.
*/
QScopedPointer<HsScene> HsScene::mInstance(0);
