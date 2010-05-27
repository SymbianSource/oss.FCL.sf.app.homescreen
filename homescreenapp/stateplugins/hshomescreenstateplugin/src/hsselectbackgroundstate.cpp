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

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include <HbMainWindow>
#include <HbProgressDialog>

#include "hsselectbackgroundstate.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswallpaper.h"
#include "hsdatabase.h"
#include "hshomescreenstatecommon.h"
#include "hsconfiguration.h"

#ifdef Q_OS_SYMBIAN
#include "hsimagefetcherclient.h"
#else
#include "xqaiwgetimageclient.h"
#include "xqaiwcommon.h"
#endif

const char hsLocTextId_ProgressDialog_WallpaperLoading[] =
            "txt_homescreen_dpopinfo_loading_wallpaper";

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
    mSourceView(0),
    mPortraitWallpaperImageReader(0),
    mLandscapeWallpaperImageReader(0),
    mProgressDialog(0),
    mRunningThreadAmount(0),
    mShowAnimation(false)
{
#ifdef Q_OS_SYMBIAN
    mImageFetcher = new HsImageFetcherClient(this);
#else    
    mImageFetcher = new XQAIWGetImageClient;
#endif

    connect(this, SIGNAL(entered()), SLOT(action_selectWallpaper()));
    connect(this, SIGNAL(exited()), SLOT(action_disconnectImageFetcher()));
    connect(this, SIGNAL(handleError()), SLOT(onHandleError()));
}

/*!
    Destructor.
    
*/
HsSelectBackgroundState::~HsSelectBackgroundState()
{
    delete mImageFetcher;
    delete mPortraitWallpaperImageReader;
    delete mLandscapeWallpaperImageReader;
}

/*!
    \internal
    Connects to image fetcher and launches "remote" ui from photos
    from which user can select background image
*/
void HsSelectBackgroundState::action_selectWallpaper()
{
    mSourceView = HsScene::mainWindow()->currentView();

    mRunningThreadAmount = 0;
    mShowAnimation = false;

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
    Called when user has selected an image 
*/
void HsSelectBackgroundState::onFetchComplete(QStringList imageStringList)
{
    // start animation immediately
    onShowAnimation();

    // check that sceneData is available
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        emit handleError();
        return;
    }

    // clean thread instances
    delete mPortraitWallpaperImageReader;
    delete mLandscapeWallpaperImageReader;
    mPortraitWallpaperImageReader = NULL;
    mLandscapeWallpaperImageReader = NULL;

    mPortraitWallpaperImageReader = new HsWallpaperImageReader();
    mLandscapeWallpaperImageReader = new HsWallpaperImageReader();

    mRunningThreadAmount = 2;

    // delete old wallpapers
    QFile::remove(sceneData.portraitWallpaper);
    QFile::remove(sceneData.landscapeWallpaper);

    QString wallpaperDir = HsWallpaper::wallpaperDirectory();            
    QDir dir(wallpaperDir);
    if (!dir.exists()) {
        dir.mkpath(wallpaperDir);
    }

    // left empty to signal we want to use full size image as source
    QRect sourceRect;

    // Initialize portrait image threading
    QRect targetRectPortrait(0, 0, (2 * 360) + HsConfiguration::bounceEffect(), 640);
    mPortraitWallpaperImageReader->setSourcePath(imageStringList.first());
    mPortraitWallpaperImageReader->setSourceRect(sourceRect);
    mPortraitWallpaperImageReader->setTargetRect(targetRectPortrait);
    mPortraitWallpaperImageReader->setCenterTarget(true);

    connect(mPortraitWallpaperImageReader,
            SIGNAL(finished()),
            SLOT(onImageProcessed()));

    // Initialize landscape image threading
    QRect targetRectLandscape(0, 0, (2 * 640) + HsConfiguration::bounceEffect(), 360);
    mLandscapeWallpaperImageReader->setSourcePath(imageStringList.first());
    mLandscapeWallpaperImageReader->setSourceRect(sourceRect);
    mLandscapeWallpaperImageReader->setTargetRect(targetRectLandscape);
    mLandscapeWallpaperImageReader->setCenterTarget(true);

    connect(mLandscapeWallpaperImageReader,
            SIGNAL(finished()),
            SLOT(onImageProcessed()));
  
    // start image processing in thread
    mPortraitWallpaperImageReader->start();
    mLandscapeWallpaperImageReader->start();
}

/*!
    \internal
    Called when selection of background image fails  
*/
void HsSelectBackgroundState::onFetchFailed(int error)
{
    Q_UNUSED(error)
    emit handleError();
}

/*!
    \internal
    Called when image processing is finished in thread  
*/
void HsSelectBackgroundState::onImageProcessed()
{
    mRunningThreadAmount--;

    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        emit handleError();
        return;
    }

    QFileInfo fileInfo;
    // get image path
    if (sender() == mPortraitWallpaperImageReader) {
        fileInfo = mPortraitWallpaperImageReader->sourcePath();
    } else {
        fileInfo = mLandscapeWallpaperImageReader->sourcePath();
    }
    // suffix is same for both orientations
    QString fileExtension("");
    if (!fileInfo.suffix().isEmpty()) {
        fileExtension = fileInfo.suffix();
    }

    QImage image;
    // set portrait image path to sceneData
    QString portraitPath(HsWallpaper::wallpaperPath(Qt::Vertical, QString(),
                                                    fileExtension));
    // we need to set this always as image to be activated can be either orientation
    sceneData.portraitWallpaper = portraitPath;
    QString landscapePath(HsWallpaper::wallpaperPath(Qt::Horizontal, QString(),
                                                     fileExtension));
    sceneData.landscapeWallpaper = landscapePath;

    if (sender() == mPortraitWallpaperImageReader) {
        // get image from thread
        image = mPortraitWallpaperImageReader->processedImage();
        // save image
        image.save(portraitPath);
    } else {
        image = mLandscapeWallpaperImageReader->processedImage();
        image.save(landscapePath);
    }

    if (image.isNull()) {
        emit handleError();
        return;
    }
    // update scenedata and set new image to background
    if (db->updateScene(sceneData)) {
        HsScene *scene = HsScene::instance();
        // set image to wallpaper (don't activate yet)
        if (sender() == mPortraitWallpaperImageReader) {
            scene->wallpaper()->setPortraitImage(portraitPath, false);
        } else {
            scene->wallpaper()->setLandscapeImage(landscapePath, false);
        }
        // if last thread running
        if (mRunningThreadAmount == 0) {
            // if current orientation matches -> set to active wallpaper
            if (HsScene::orientation() == Qt::Vertical) {
                scene->wallpaper()->setPortraitImage(portraitPath, true);
            } else {
                scene->wallpaper()->setLandscapeImage(landscapePath, true);
            }
            // finish progress bar
            if (mShowAnimation) {
                mProgressDialog->setProgressValue(3);
            }
            // let user control again homescreen
            emit event_waitInput();
        } else {
            // update progress information
            if (mShowAnimation) {
                mProgressDialog->setProgressValue(2);
            }
        }
    }     
}

/*!
    \internal
    Shows animation for longer processing
*/
void HsSelectBackgroundState::onShowAnimation()
{
    delete mProgressDialog;
    mProgressDialog = new HbProgressDialog(HbProgressDialog::ProgressDialog);
    /* TODO: workaround to remove cancel button (setPrimaryAction(0) is deprecated).
     * clearActions does the same but crashes, fix should be in wk16, check orbit list
     */
    QList<QAction*> actions = mProgressDialog->actions();
    actions[0]->setVisible(false);
    mProgressDialog->setIcon(HbIcon("note_info"));
    mProgressDialog->setText(hbTrId(hsLocTextId_ProgressDialog_WallpaperLoading));
    mProgressDialog->setMinimum(0);
    mProgressDialog->setMaximum(3);
    mProgressDialog->setBackgroundFaded(true);
    mProgressDialog->setAutoClose(true);
    // set initial value to appear as loading
    mProgressDialog->setProgressValue(1);
    mProgressDialog->show();
    mShowAnimation = true;
}

/*!
    \internal
    Called when error occurs during image processing  
*/
void HsSelectBackgroundState::onHandleError()
{
    if (mShowAnimation) {
        mProgressDialog->close();
    }
    emit event_waitInput();
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
    Called when selection of background image fails on emulator or HW
*/
#ifdef Q_OS_SYMBIAN
void HsSelectBackgroundState::fetchFailed(int error, const QString& errorString)
{
    Q_UNUSED(errorString)
    onFetchFailed(error);        
}
#endif
