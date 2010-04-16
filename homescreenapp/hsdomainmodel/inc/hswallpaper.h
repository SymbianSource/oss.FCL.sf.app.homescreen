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

HOMESCREEN_TEST_CLASS(TestRuntimeServices)

class HsWallpaperPrivate;
class HSDOMAINMODEL_EXPORT HsWallpaper : public HbWidget
{
    Q_OBJECT

public:
    HsWallpaper(QGraphicsItem *parent = 0);
    ~HsWallpaper();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, 
               QWidget *widget = 0);

    bool setImagesById(const QString &id = "d",
                       const QString &ext = "png");
    bool setImagesByPaths(const QString &landscapeImagePath,
                          const QString &portraitImagePath);

    bool removeImages();

    static QString wallpaperDirectory();
    static QString wallpaperPath(Qt::Orientation orientation,
                                 const QString &id = "d",
                                 const QString &ext = "png");

private:    
    Q_DISABLE_COPY(HsWallpaper)

private:
    QPixmap mImage;
    QString mLImagePath;
    QString mPImagePath;
    Qt::Orientation mOrientation;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};

#endif // HSWALLPAPER_H
