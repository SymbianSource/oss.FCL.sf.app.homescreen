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
#include "hsscene.h"
#include "hsscene_p.h"
#include "hsscenedata.h"
#include "hspage.h"
#include "hspagedata.h"
#include "hswidgethost.h"
#include "hsdatabase.h"
#include "hswallpaper.h"
#ifndef Q_OS_SYMBIAN
#include "hsipcconnectiondispatcher.h"
#endif

HsScenePrivate::HsScenePrivate()
    : mDatabaseId(-1),
      mIsOnline(true),
      mWallpaper(0),
      mActivePage(0),
      mActiveWidget(0)
#ifndef Q_OS_SYMBIAN
      ,mIpcConnectionDispatcher(0)
#endif
{
    mWallpaper = new HsWallpaper;
}
 
HsScenePrivate::~HsScenePrivate()
{
    delete mWallpaper;
    qDeleteAll(mPages);
}
    
HsScene::~HsScene()
{
}

int HsScene::databaseId() const
{
    return mD->mDatabaseId;
}

bool HsScene::load()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        return false;
    }

    if (sceneData.portraitWallpaper().isEmpty()) {
        mD->mWallpaper->setImagesById();
    } else {
        mD->mWallpaper->setImagesByPaths(
            sceneData.landscapeWallpaper(), 
            sceneData.portraitWallpaper());
    }

    QList<HsPageData> pageDatas;
    if (!db->pages(pageDatas) || pageDatas.empty()) {
        return false;
    }
    
    HsPage *page = 0;
    foreach (HsPageData pageData, pageDatas) {
        page = new HsPage;
        page->setDatabaseId(pageData.id());
        if (page->load()) {
            mD->mPages << page;
        } else {
            qDebug() << "HsScene: Page loading failed.";
            // TODO
        }
    }

    int defaultPageIndex = sceneData.defaultPage().index();        
    if(defaultPageIndex > -1){
        mD->mActivePage = mD->mPages[defaultPageIndex];
        mD->mActivePage->setRemovable(false);
    }
#ifndef Q_OS_SYMBIAN
    mD->mIpcConnectionDispatcher = new HsIpcConnectionDispatcher("hs_content_publish");
    mD->mIpcConnectionDispatcher->setParent(this);
    return mD->mIpcConnectionDispatcher->start();
#endif    
}

HsWallpaper *HsScene::wallpaper() const
{
    return mD->mWallpaper;
}

QList<HsPage *> HsScene::pages() const
{
    return mD->mPages;
}
    
bool HsScene::addPage(HsPage *page)
{
    if (!page) {
        return false;
    }
   
    if (mD->mPages.contains(page)) {
        return true;
    }

    HsDatabase *db = HsDatabase::instance();

    HsPageData pageData;
    if (!db->page(page->databaseId(), pageData, false)) {
        return false;
    }
    int index = mD->mPages.count();
    if (pageData.index() != index) {
        pageData.setIndex(index);
        if (!db->updatePage(pageData, false)) {
            return false;
        }
    }
    
    mD->mPages << page;
    return true;
}
    
bool HsScene::removePage(HsPage *page)
{
    if (!page) {
        return false;
    }
    return mD->mPages.removeOne(page);
}
 
bool HsScene::setActivePage(HsPage *page)
{
    if (!page) {
        return false;
    }
   
    if (!mD->mPages.contains(page)) {
        return false;
    }

    mD->mActivePage = page;
    return true;
}

bool HsScene::setActivePageIndex(int index)
{
    if (index < 0 || mD->mPages.count() <= index) {
        return false;
    }
    return setActivePage(mD->mPages[index]);
}

HsPage *HsScene::activePage() const
{
    return mD->mActivePage;
}

int HsScene::activePageIndex() const
{
    return mD->mPages.indexOf(mD->mActivePage);
}

void HsScene::setActiveWidget(HsWidgetHost *widget)
{
    mD->mActiveWidget = widget;
}
 
HsWidgetHost *HsScene::activeWidget() const
{
    return mD->mActiveWidget;
}
/*!
    Toggle application online state. Defaults 
    to true.
*/
void HsScene::setOnline(bool online)
{
    mD->mIsOnline = online;
    foreach (HsPage *page, mD->mPages) {
        page->setOnline(online);
    }
}

bool HsScene::isOnline()const
{
    return mD->mIsOnline;
}

HsScene *HsScene::instance()
{
    if (mInstance.isNull()) {
        mInstance.reset(new HsScene);
    }
    return mInstance.data();
}

Qt::Orientation HsScene::orientation()
{
    return hbInstance->orientation();
}

HbMainWindow *HsScene::mainWindow()
{
    return hbInstance->allMainWindows().first();
}

HsScene::HsScene(QObject *parent)
    : QObject(parent)
{
    mD.reset(new HsScenePrivate);
}

QScopedPointer<HsScene> HsScene::mInstance(0);
