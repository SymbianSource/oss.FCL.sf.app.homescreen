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

#ifndef HSWALLPAPER_H
#define HSWALLPAPER_H

#include <HbWidget>
#include <QScopedPointer>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

class HbIconItem;

HOMESCREEN_TEST_CLASS(TestRuntimeServices)

class HSDOMAINMODEL_EXPORT HsWallpaper : public HbWidget
{
    Q_OBJECT

public:
    HsWallpaper(QGraphicsItem *parent = 0);
    ~HsWallpaper();

    bool setImagesById(const QString &id = "d",
                       const QString &ext = "png");

    bool setImagesByPaths(const QString &landscapeImagePath,
                          const QString &portraitImagePath);

    void setPortraitImage(const QString &path, bool activate = false);
    void setLandscapeImage(const QString &path, bool activate = false);

    bool removeImages();

    static QString wallpaperDirectory();
    static QString wallpaperPath(Qt::Orientation orientation,
                                 const QString &id = "d",
                                 const QString &ext = "png");

private:    
    Q_DISABLE_COPY(HsWallpaper)

private slots:
    void onOrientationChanged(Qt::Orientation orientation);

private:
    QString mLImagePath;
    QString mPImagePath;
    Qt::Orientation mOrientation;

    HbIconItem *mIconItem;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};

#endif // HSWALLPAPER_H
