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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QThread>
#include <QTimer>

#include <HbMainWindow>
#include <HbProgressDialog>
#include <HbView>

#include "hsselectbackgroundstate.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswallpaper.h"
#include "hsdatabase.h"
#include "hshomescreenstatecommon.h"

#ifdef Q_OS_SYMBIAN
#include "hsimagefetcherclient.h"
#else
#include "xqaiwgetimageclient.h"
#include "xqaiwcommon.h"
#endif

const char hsLocTextId_ProgressDialog_WallpaperLoading[] = "txt_homescreen_dpopinfo_loading_wallpaper";

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
    mWallpaperImageReaderThread(0),
    mWallpaperImageReader(0),
    mProgressDialog(0),
    mImageProcessingState(NotRunning),
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
    delete mWallpaperImageReaderThread;
    delete mWallpaperImageReader;
}

/*!
    \internal
    Connects to image fetcher and launches "remote" ui from photos
    from which user can select background image
*/
void HsSelectBackgroundState::action_selectWallpaper()
{
    mSourceView = HsScene::mainWindow()->currentView();

    mImageProcessingState = NotRunning;
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
    if (mImageProcessingState == NotRunning) {
        // TODO: temporarily show animation immediately (~0.5 sec delay)
        onShowAnimation();
        // start counting time for possible animation
        // TODO: from UX the real response time
        // TODO: cannot use timer since UI does not respond during hardcore image processing
        //QTimer::singleShot(1000, this, SLOT(onShowAnimation()));
    }

    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        emit handleError();
        return;
    }

    // clean thread instances
    delete mWallpaperImageReader;
    delete mWallpaperImageReaderThread;
    mWallpaperImageReader = NULL;
    mWallpaperImageReaderThread = NULL;

    mWallpaperImageReaderThread = new QThread();
    mWallpaperImageReader = new HsWallpaperImageReader();

    // setup processing when image is fetched at first time
    if (mImageProcessingState == NotRunning) {
        // delete old wallpapers
        QFile::remove(sceneData.portraitWallpaper);
        QFile::remove(sceneData.landscapeWallpaper);
        
        QString wallpaperDir = HsWallpaper::wallpaperDirectory();            
        QDir dir(wallpaperDir);
        if (!dir.exists()) {
            dir.mkpath(wallpaperDir);
        }
        HsScene *scene = HsScene::instance();
        Qt::Orientation orientation = scene->orientation();
        // based on screen orientation select first image to process
        if (orientation == Qt::Vertical) {
            mImageProcessingState = ProcessPortraitAsFirst;
        } else {
            mImageProcessingState = ProcessLandscapeAsFirst;
        }
    }

    QRect targetRect;

    switch (mImageProcessingState) {
    case ProcessPortraitAsFirst:
    case ProcessPortraitAsSecond:
        targetRect = QRect(0, 0, (2 * 360) + HSBOUNDARYEFFECT, 640);
        break;
    case ProcessLandscapeAsFirst:
    case ProcessLandscapeAsSecond:
        targetRect = QRect(0, 0, (2 * 640) + HSBOUNDARYEFFECT, 360);
        break;
    default:
        emit handleError();
        return;        
    }

    // left empty to signal we want to use full size image as source
    QRect sourceRect;
    mWallpaperImageReader->setSourcePath(imageStringList.first());
    mWallpaperImageReader->setSourceRect(sourceRect);
    mWallpaperImageReader->setTargetRect(targetRect);
    mWallpaperImageReader->setCenterTarget(true);
    mWallpaperImageReader->moveToThread(mWallpaperImageReaderThread);

    mWallpaperImageReader->connect(mWallpaperImageReaderThread,
                      SIGNAL(started()),
                      SLOT(processImage()));
    
    connect(mWallpaperImageReader,
            SIGNAL(processingFinished()),
            SLOT(onImageProcessed()));
  
    // start image processing in thread
    mWallpaperImageReaderThread->start(QThread::IdlePriority);
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
    HsScene *scene = HsScene::instance();
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);
    HsSceneData sceneData;
    if (!db->scene(sceneData)) {
        emit handleError();
        return;
    }
    QFileInfo fileInfo(mWallpaperImageReader->getSourcePath());
    QString fileExtension("");
    if (!fileInfo.suffix().isEmpty()) {
        fileExtension = fileInfo.suffix();
    }
    // set image path to sceneData
    QString path;
    if (mImageProcessingState == ProcessPortraitAsFirst ||
        mImageProcessingState == ProcessPortraitAsSecond) {
        path = HsWallpaper::wallpaperPath(Qt::Vertical, QString(), fileExtension);
        sceneData.portraitWallpaper = path;
    } else {
        path = HsWallpaper::wallpaperPath(Qt::Horizontal, QString(), fileExtension);
        sceneData.landscapeWallpaper = path;
    }
    // get image from renderer and save it
    QImage image = mWallpaperImageReader->getProcessedImage();
    image.save(path);
    if (!image.isNull()) {
        // update scenedata and set new image to background
        if (db->updateScene(sceneData)) {
            switch (mImageProcessingState) {
            case ProcessPortraitAsFirst:
                scene->wallpaper()->setPortraitImage(path, true);
                break;
            case ProcessPortraitAsSecond:
                // if orientation changed during first image settings
                if (HsScene::orientation() == Qt::Vertical) {
                    scene->wallpaper()->setPortraitImage(path, true);
                } else {
                    scene->wallpaper()->setPortraitImage(path);
                }
                break;
            case ProcessLandscapeAsFirst:
                scene->wallpaper()->setLandscapeImage(path, true);
                break;
            case ProcessLandscapeAsSecond:
                if (HsScene::orientation() == Qt::Horizontal) {
                    scene->wallpaper()->setLandscapeImage(path, true);
                } else {
                    scene->wallpaper()->setLandscapeImage(path);
                }
                break;
            default:
                emit handleError();
                break;
            }
        }
    } else {
        emit handleError();
        return;
    }

    switch (mImageProcessingState) {
    case ProcessPortraitAsFirst:
        mImageProcessingState = ProcessLandscapeAsSecond;
        if (mShowAnimation) {
            mProgressDialog->setProgressValue(2);
        }
        // process second orientation
        onFetchComplete(QStringList(mWallpaperImageReader->getSourcePath()));
        break;
    case ProcessLandscapeAsFirst:
        mImageProcessingState = ProcessPortraitAsSecond;
        if (mShowAnimation) {
            mProgressDialog->setProgressValue(2);
        }
        onFetchComplete(QStringList(mWallpaperImageReader->getSourcePath()));
        break;
    case ProcessPortraitAsSecond:
    case ProcessLandscapeAsSecond:
        mImageProcessingState = NotRunning;
        if (mShowAnimation) {
            mProgressDialog->setProgressValue(3);
        }
        // let user control again homescreen
        emit event_waitInput();
        break;
    default:
        emit handleError();
        break;
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
    // TODO: setPrimaryAction is deprecated, clearActions does the same but crashes when dialog closes, check orbit list
    mProgressDialog->setPrimaryAction(0);
    mProgressDialog->setIcon(HbIcon("note_info"));
    mProgressDialog->setText(hbTrId(hsLocTextId_ProgressDialog_WallpaperLoading));
    mProgressDialog->setMinimum(0);
    mProgressDialog->setMaximum(3);
    mProgressDialog->setBackgroundFaded(true);
    mProgressDialog->setAutoClose(true);
    mProgressDialog->setProgressValue(1);
    mProgressDialog->show();
    // TODO: temporary solution to minimize progress dialog resizing problem
    QApplication::processEvents();
    mShowAnimation = true;
}

/*!
    \internal
    Called when error occurs during image processing  
*/
void HsSelectBackgroundState::onHandleError()
{
    mImageProcessingState = Error;
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
