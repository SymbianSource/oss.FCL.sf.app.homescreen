/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QFileInfo>

#include "hshomescreenclientserviceprovider.h"
#include "hscontentservice.h"

#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswallpaper.h"
#include "hsdatabase.h"
#include "hswallpaperhelper.h"

#define HSBOUNDARYEFFECT 20 // amount of extra pixels in wallpaper width reserved for boundary effect 

namespace
{
    const char gInterfaceName[] = "com.nokia.services.hsapplication.IHomeScreenClient";
}

/*!
    Constructor.
*/
HsHomeScreenClientServiceProvider::HsHomeScreenClientServiceProvider(QObject *parent)
  : XQServiceProvider(gInterfaceName, parent)
{
    publishAll();
}

/*!
    Destructor.
*/
HsHomeScreenClientServiceProvider::~HsHomeScreenClientServiceProvider()
{
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Adds new widget, with the give \a uri and \a preferences, 
    to the active home screen page.
*/
bool HsHomeScreenClientServiceProvider::addWidget(const QString &uri, const QVariantHash &preferences)
{
    return HsContentService::instance()->addWidget(uri, preferences);
}

/*!
    Changes \a fileName image to the active home screen page's wallpaper.
*/
bool HsHomeScreenClientServiceProvider::setWallpaper(const QString &fileName)
{
    HsScene *scene = HsScene::instance();
        
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    
    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        return false;
    }

    QFileInfo fileInfo(fileName);
    QString fileExtension("");
    if (!fileInfo.suffix().isEmpty()) {
        fileExtension = fileInfo.suffix();
    }
    
    QFile::remove(sceneData.portraitWallpaper);
    QFile::remove(sceneData.landscapeWallpaper); 
    
    QString wallpaperDir = HsWallpaper::wallpaperDirectory();            
    QDir dir(wallpaperDir);
    if (!dir.exists()) {
        dir.mkpath(wallpaperDir);
    }
    
    QString portraitPath = HsWallpaper::wallpaperPath(
        Qt::Vertical, QString(), fileExtension);
    QString landscapePath = HsWallpaper::wallpaperPath(
        Qt::Horizontal, QString(), fileExtension);
    
    QRect portraitRect = QRect(0, 0, (2 * 360) + HSBOUNDARYEFFECT, 640);
    QRect landscapeRect = QRect(0, 0, (2 * 640) + HSBOUNDARYEFFECT, 360);
    QRect sourceRect; // left empty to signal we want to use full size image as source
    
    QImage portraitImage = HsWallpaperHelper::processImage(fileName,
            portraitRect, sourceRect);

    QImage landscapeImage = HsWallpaperHelper::processImage(fileName,
            landscapeRect, sourceRect);

    if (!portraitImage.isNull() && !landscapeImage.isNull()) {
        portraitImage.save(portraitPath);
        sceneData.portraitWallpaper = portraitPath;
        
        landscapeImage.save(landscapePath);
        sceneData.landscapeWallpaper = landscapePath;
        
        if (db->updateScene(sceneData)) {
            scene->wallpaper()->setImagesById(QString(), fileInfo.suffix());
        }
    }
    else {
        // display some error note here
    }
    return true;
}


#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT