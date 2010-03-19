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

#ifndef HSWALLPAPER_P_H
#define HSWALLPAPER_P_H

#include <QPixmap>

class HsWallpaperPrivate
{
public:
    HsWallpaperPrivate();
    ~HsWallpaperPrivate();

    QPixmap mImage;
    QString mLImagePath;
    QString mPImagePath;
    Qt::Orientation mOrientation;
};

#endif // HSWALLPAPER_P_H
