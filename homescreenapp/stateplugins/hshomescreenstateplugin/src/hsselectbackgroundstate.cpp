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
#include <QFileInfo>
#include <QImageReader>

#include <HbMainWindow>
#include <HbView>

#include "hsselectbackgroundstate.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswallpaper.h"
#include "hsdatabase.h"
#include "hshomescreenstatecommon.h"
#include "hswallpaperhelper.h"

#ifdef Q_OS_SYMBIAN
#include "hsimagefetcherclient.h"
#else
#include "xqaiwgetimageclient.h"
#include "xqaiwcommon.h"
#endif
/*! 
    \class HsSelectBackgroundState
    \ingroup group_hshomescreenstateprovider
    \brief Implements imagefetcher event listening and handles those events.

    \sa StateMachine
*/

/*!
    Constructor.         
    \a parent Owner.
*/
HsSelectBackgroundState::HsSelectBackgroundState(QState *parent):
    QState(parent),
    mImageFetcher(0),
    mSourceView(0)
{
#ifdef Q_OS_SYMBIAN
    mImageFetcher = new HsImageFetcherClient(this);
#else    
    mImageFetcher = new XQAIWGetImageClient;
#endif
    connect(this, SIGNAL(entered()), SLOT(action_selectWallpaper()));
    connect(this, SIGNAL(exited()), SLOT(action_disconnectImageFetcher()));
}

/*!
    Destructor.
    
*/
HsSelectBackgroundState::~HsSelectBackgroundState()
{
    delete mImageFetcher;
}

/*!
    \internal
    Connects to image fetcher and launches "remote" ui from photos
    from which user can select background image
 */
void HsSelectBackgroundState::action_selectWallpaper()
{
    mSourceView = HsScene::mainWindow()->currentView();

#ifdef Q_OS_SYMBIAN    
    connect(mImageFetcher, SIGNAL(fetchCompleted(const QString&)),
            this, SLOT(fetchCompleted(const QString&)));
    connect(mImageFetcher, SIGNAL(fetchFailed(int, const QString&)),
            this, SLOT(fetchFailed(int, const QString&)));
    mImageFetcher->fetch();
#else
    connect(mImageFetcher, SIGNAL(fetchComplete(QStringList)),
            SLOT(onFetchComplete(QStringList)));
    connect(mImageFetcher, SIGNAL(fetchFailed(int)),
            SLOT(onFetchFailed(int)));
    mImageFetcher->fetch(QVariantMap(), SelectionSingle);
#endif    
}

/*!
    \internal
    disconnects photos image fetcher services slots.
*/

void HsSelectBackgroundState::action_disconnectImageFetcher()
{
    HsScene::mainWindow()->setCurrentView(mSourceView);
    mImageFetcher->disconnect(this);
}

/*!
    \internal
    Called when user has selected an image on emulator or HW
*/
#ifdef Q_OS_SYMBIAN
void HsSelectBackgroundState::fetchCompleted(const QString& imageFileName)
{
    QStringList imageFileNameAsList(imageFileName);
    onFetchComplete(imageFileNameAsList);
}
#endif

/*!
    \internal
    Called when user has selected an image 
*/
void HsSelectBackgroundState::onFetchComplete(QStringList imageStringList)
{
    HsScene *scene = HsScene::instance();
        
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        emit event_waitInput();
        return;
    }
   
    QFileInfo fileInfo(imageStringList.first());
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
    
    QImage portraitImage = HsWallpaperHelper::processImage(imageStringList.first(),
            portraitRect, sourceRect);
    
    QImage landscapeImage = HsWallpaperHelper::processImage(imageStringList.first(),
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
  
    emit event_waitInput();
}

/*!
    \internal
    Called when selection of background image fails  
*/
void HsSelectBackgroundState::onFetchFailed(int error)
{
    Q_UNUSED(error)
    emit event_waitInput();
}

/*!
    \internal
    Called when selection of background image fails on emulator or HW
*/
#ifdef Q_OS_SYMBIAN
void HsSelectBackgroundState::fetchFailed(int error, const QString& errorString)
{
    Q_UNUSED(errorString)
    onFetchFailed(error);        
}
#endif