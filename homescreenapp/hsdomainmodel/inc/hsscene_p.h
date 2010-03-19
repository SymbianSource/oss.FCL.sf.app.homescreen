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

#ifndef HSSCENE_P_H
#define HSSCENE_P_H

class HsWallpaper;
class HsPage;
class HsWidgetHost;
#ifndef Q_OS_SYMBIAN
class HsIpcConnectionDispatcher;
#endif

class HsScenePrivate
{
public:
    HsScenePrivate();
    ~HsScenePrivate();

    int mDatabaseId;
    bool mIsOnline;
    HsWallpaper *mWallpaper;
    QList<HsPage *> mPages;
    HsPage *mActivePage;
    HsWidgetHost *mActiveWidget;
#ifndef Q_OS_SYMBIAN
    HsIpcConnectionDispatcher *mIpcConnectionDispatcher;
#endif
    
};

#endif // HSSCENE_P_H
