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
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hsconfiguration.h"

HsScene::~HsScene()
{
    delete mWallpaper;
    qDeleteAll(mPages);
}

int HsScene::databaseId() const
{
    return mDatabaseId;
}

bool HsScene::load()
{
    return true;
}

HsWallpaper *HsScene::wallpaper() const
{
    return mWallpaper;
}

QList<HsPage *> HsScene::pages() const
{
    return mPages;
}

bool HsScene::addPage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (mPages.contains(page)) {
        return true;
    }

    mPages << page;
    return true;
}

bool HsScene::removePage(HsPage *page)
{
    if (!page) {
        return false;
    }
    return mPages.removeOne(page);
}

bool HsScene::setActivePage(HsPage *page)
{
    if (!page) {
        return false;
    }

    if (!mPages.contains(page)) {
        return false;
    }

    mActivePage = page;
    return true;
}

bool HsScene::setActivePageIndex(int index)
{
    if (index < 0 || mPages.count() <= index) {
        return false;
    }
    return setActivePage(mPages[index]);
}

HsPage *HsScene::activePage() const
{
    return mActivePage;
}

int HsScene::activePageIndex() const
{
    return mPages.indexOf(mActivePage);
}

void HsScene::setActiveWidget(HsWidgetHost *widget)
{
    mActiveWidget = widget;
}

HsWidgetHost *HsScene::activeWidget() const
{
    return mActiveWidget;
}

void HsScene::setOnline(bool online)
{
    mIsOnline = online;
    foreach (HsPage *page, mPages) {
        page->setOnline(online);
    }
}

bool HsScene::isOnline()const
{
    return mIsOnline;
}

/*!
    Singleton.
*/
HsScene *HsScene::instance()
{
    return mInstance;
}

HsScene *HsScene::takeInstance()
{
    HsScene *instance = mInstance;
    mInstance = 0;
    return instance;
}
 
void HsScene::setInstance(HsScene *instance)
{
    if (mInstance != instance) {
        delete mInstance;
        mInstance = instance; 
    }    
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

HsScene::HsScene(QObject *parent)
  : QObject(parent),
    mDatabaseId(-1),
    mWallpaper(0),
    mActivePage(0),
    mActiveWidget(0),
    mIsOnline(true)
{
}

/*!
    Points to the scene instance.
*/
HsScene *HsScene::mInstance = 0;
