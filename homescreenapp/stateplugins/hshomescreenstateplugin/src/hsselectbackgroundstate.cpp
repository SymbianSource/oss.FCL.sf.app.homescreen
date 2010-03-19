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

#include <HbMainWindow>
#include <HbView>

#include "hsselectbackgroundstate.h"
#include "hsscene.h"
#include "hsscenedata.h"
#include "hswallpaper.h"
#include "hsdatabase.h"

#include "xqaiwgetimageclient.h"
#include "xqaiwcommon.h"

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
    mXQAIWGetImageClient(0),
    mSourceView(0)
{
    mXQAIWGetImageClient = new XQAIWGetImageClient;
    connect(this, SIGNAL(entered()), SLOT(selectPageBackgroundAction()));
    connect(this, SIGNAL(exited()), SLOT(disconnectImageFetcherAction()));
}

/*!
    Destructor.
    
*/
HsSelectBackgroundState::~HsSelectBackgroundState()
{
    delete mXQAIWGetImageClient;
}

/*!
    \internal
    Scales and crops (if needed) \a image for given \a orientation
    and saves it to \a path. Return true on successful.
*/
bool HsSelectBackgroundState::saveImage(QImage &image, const QString &path, Qt::Orientation orientation) 
{
    QRect rect;
    if (orientation == Qt::Vertical) {
        rect.setRect(0, 0, 2 * 360, 640);
    } else {
        rect.setRect(0, 0, 2 * 640, 360);
    }

    if (image.rect().contains(rect)) {        
        rect.moveCenter(image.rect().center());
        return image.copy(rect).save(path);
    } else {
        QImage scaledImage = image.scaledToWidth(rect.width());        
        rect.moveCenter(scaledImage.rect().center());
        return scaledImage.copy(rect).save(path);
    }
}

/*!
    \internal
    Connects to image fetcher and launches "remote" ui from photos
    from which user can select background image
 */
void HsSelectBackgroundState::selectPageBackgroundAction()
{
    mSourceView = HsScene::mainWindow()->currentView();

    connect(mXQAIWGetImageClient, SIGNAL(fetchComplete(QStringList)),
            SLOT(onBackgroundImageFetched(QStringList)));
    connect(mXQAIWGetImageClient, SIGNAL(fetchFailed(int)),
            SLOT(onBackgroundImageFetchFailed(int)));
    
    QVariantMap filter;
    mXQAIWGetImageClient->fetch(filter, SelectionSingle);
}

/*!
    \internal
    disconnects photos image fetcher services slots.
*/

void HsSelectBackgroundState::disconnectImageFetcherAction()
{
    HsScene::mainWindow()->setCurrentView(mSourceView);
    mXQAIWGetImageClient->disconnect(this);
}

/*!
    \internal
    Called when user has selected an image    
*/
void HsSelectBackgroundState::onBackgroundImageFetched(QStringList imageStringList)
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

    QFile::remove(sceneData.portraitWallpaper());
    QFile::remove(sceneData.landscapeWallpaper()); 

    QString wallpaperDir = HsWallpaper::wallpaperDirectory();            
    QDir dir(wallpaperDir);
    if (!dir.exists()) {
        dir.mkpath(wallpaperDir);
    }

    QString portraitPath = HsWallpaper::wallpaperPath(
        Qt::Vertical, QString(), fileExtension);
    QString landscapePath = HsWallpaper::wallpaperPath(
        Qt::Horizontal, QString(), fileExtension);
    
    QImage image(imageStringList.first());
    
    if (saveImage(image, portraitPath, Qt::Vertical) && 
        saveImage(image, landscapePath, Qt::Horizontal)) {
        sceneData.setPortraitWallpaper(portraitPath);
        sceneData.setLandscapeWallpaper(landscapePath);
        if (db->updateScene(sceneData)) {
            scene->wallpaper()->setImagesById(QString(), fileInfo.suffix());
        }
    }
   
    emit event_waitInput();
}

/*!
    \internal
    Called when selection of background image fails  
*/
void HsSelectBackgroundState::onBackgroundImageFetchFailed(int error)
{
    Q_UNUSED(error)
    emit event_waitInput();
}
