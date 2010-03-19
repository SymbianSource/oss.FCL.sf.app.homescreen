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
#include "hswallpaper_p.h"
#include "hsscene.h"

HsWallpaperPrivate::HsWallpaperPrivate()
  : mOrientation(Qt::Vertical)
{
}
 
HsWallpaperPrivate::~HsWallpaperPrivate()
{
}

HsWallpaper::HsWallpaper(QGraphicsItem *parent)
    : HbWidget(parent)
{
    mD.reset(new HsWallpaperPrivate);
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
    if (orientation != mD->mOrientation) {
        mD->mOrientation = orientation;
        mD->mImage = QPixmap();
        if (orientation == Qt::Horizontal) {
            mD->mImage.load(mD->mLImagePath);
        } else {
            mD->mImage.load(mD->mPImagePath);
        }
    }

    if (!mD->mImage.isNull()) {
        painter->drawPixmap(rect().toRect(), mD->mImage);
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
    mD->mLImagePath = landscapeImagePath;
    mD->mPImagePath = portraitImagePath;
    mD->mOrientation = HsScene::orientation();
    
    if (mD->mOrientation == Qt::Horizontal) {
        return mD->mImage.load(mD->mLImagePath);
    } else {
        return mD->mImage.load(mD->mPImagePath);
    }
}

bool HsWallpaper::removeImages()
{
    if (mD->mLImagePath != wallpaperPath(Qt::Horizontal)) {
        QFile::remove(mD->mLImagePath);
        mD->mLImagePath.clear();
    }
    if (mD->mPImagePath != wallpaperPath(Qt::Vertical)) {
        QFile::remove(mD->mPImagePath);
        mD->mPImagePath.clear();
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
