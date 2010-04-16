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

#include <QDir>
#include <QPainter>
#include "hswallpaper.h"
#include "hsscene.h"

HsWallpaper::HsWallpaper(QGraphicsItem *parent)
  : HbWidget(parent),
    mOrientation(Qt::Vertical)
{
}

HsWallpaper::~HsWallpaper()
{
}

void HsWallpaper::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option, 
                        QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    Qt::Orientation orientation = HsScene::orientation();
    if (orientation != mOrientation) {
        mOrientation = orientation;
        mImage = QPixmap();
        if (orientation == Qt::Horizontal) {
            mImage.load(mLImagePath);
        } else {
            mImage.load(mPImagePath);
        }
    }

    if (!mImage.isNull()) {
        painter->drawPixmap(rect().toRect(), mImage);
    }
}

bool HsWallpaper::setImagesById(const QString &id,
                                const QString &ext)
{
    return setImagesByPaths(
        wallpaperPath(Qt::Horizontal, id, ext),
        wallpaperPath(Qt::Vertical, id, ext));
}

bool HsWallpaper::setImagesByPaths(const QString &landscapeImagePath,
                                   const QString &portraitImagePath)
{
    mLImagePath = landscapeImagePath;
    mPImagePath = portraitImagePath;
    mOrientation = HsScene::orientation();
    
    if (mOrientation == Qt::Horizontal) {
        return mImage.load(mLImagePath);
    } else {
        return mImage.load(mPImagePath);
    }
}

bool HsWallpaper::removeImages()
{
    if (mLImagePath != wallpaperPath(Qt::Horizontal)) {
        QFile::remove(mLImagePath);
        mLImagePath.clear();
    }
    if (mPImagePath != wallpaperPath(Qt::Vertical)) {
        QFile::remove(mPImagePath);
        mPImagePath.clear();
    }    
    return true;
}

QString HsWallpaper::wallpaperDirectory()
{
#ifdef Q_OS_SYMBIAN
    static QString directory =
        QDir::toNativeSeparators("c:/private/20022f35/wallpapers/");
#else
    static QString directory = 
        QDir::toNativeSeparators(QDir::currentPath() + "/wallpapers/");
#endif

    return directory;
}

QString HsWallpaper::wallpaperPath(Qt::Orientation orientation,
                                   const QString &id,
                                   const QString &ext)
{
    QString orientationString = orientation == Qt::Horizontal ?
        "_landscape." : "_portrait.";
    return wallpaperDirectory() + id +
           orientationString + ext;
}
