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
* Description:  Homescreen wallpaper helper class.
*
*/

#ifndef HSWALLPAPERHELPER_H
#define HSWALLPAPERHELPER_H

#include "hsutils_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsWallpaperHelper
{	
public:
    static QImage processImage(const QString &sourcePath, 
    						   const QRect &targetRect,
    						   const QRect &sourceRect,
    						   bool centerTarget = true);
    
private:  
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSWALLPAPERHELPER_H
