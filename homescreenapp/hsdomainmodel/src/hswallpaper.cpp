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
#include <QGraphicsLinearLayout>
#include <HbIconItem>
#include <HbMainWindow>
#include "hswallpaper.h"
#include "hsscene.h"

HsWallpaper::HsWallpaper(QGraphicsItem *parent)
  : HbWidget(parent),
    mOrientation(Qt::Vertical),
    mIconItem(0)
{
    mIconItem = new HbIconItem();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mIconItem);
    setLayout(layout);

    connect(HsScene::mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(onOrientationChanged(Qt::Orientation)));
}

HsWallpaper::~HsWallpaper()
{
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
        setLandscapeImage(mLImagePath, true);
    } else {
        setPortraitImage(mPImagePath, true);
    }
    return true;
}

void HsWallpaper::setPortraitImage(const QString &path, bool activate)
{
    mPImagePath = path;
    if (activate) {
        mIconItem->setIcon(HbIcon(QIcon(path)));
    }
}

void HsWallpaper::setLandscapeImage(const QString &path, bool activate)
{
    mLImagePath = path;
    if (activate) {
        mIconItem->setIcon(HbIcon(QIcon(path)));
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
        QDir::toNativeSeparators(QDir::currentPath() + "/private/20022f35/wallpapers/");
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

void HsWallpaper::onOrientationChanged(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        mIconItem->setIcon(HbIcon(QIcon(mLImagePath)));
    } else {
        mIconItem->setIcon(HbIcon(QIcon(mPImagePath)));
    }
}