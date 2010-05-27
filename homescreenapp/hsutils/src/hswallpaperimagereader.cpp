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

#include <QImageReader>
#include <QTime>

#include "hswallpaperimagereader.h"

/*!
    Constructor.         
*/
HsWallpaperImageReader::HsWallpaperImageReader(QObject *parent):
    QThread(parent),
    mCenterTarget(false)
{

}

/*!
    Destructor.
*/
HsWallpaperImageReader::~HsWallpaperImageReader()
{

}

/*!
    Saves image source path
*/
void HsWallpaperImageReader::setSourcePath(const QString &sourcePath)
{
    mSourcePath = sourcePath;
}

/*!
    Returns image source path
*/
QString HsWallpaperImageReader::sourcePath() const
{
    return mSourcePath;
}

/*!
    Saves image source rect
*/
void HsWallpaperImageReader::setSourceRect(const QRect &sourceRect)
{
    mSourceRect = sourceRect;
}

/*!
    Returns source rect
*/
QRect HsWallpaperImageReader::sourceRect() const
{
    return mSourceRect;
}

/*!
    Saves image target rect
*/
void HsWallpaperImageReader::setTargetRect(const QRect &targetRect)
{
    mTargetRect = targetRect;
}

/*!
    Returns target rect
*/
QRect HsWallpaperImageReader::targetRect() const
{
    return mTargetRect;
}

/*!
    Centers image target rect
*/
void HsWallpaperImageReader::setCenterTarget(bool center)
{
    mCenterTarget = center;
}

/*!
    Returns target centering
*/
bool HsWallpaperImageReader::centerTarget()
{
    return mCenterTarget;
}

/*!
    Returns processed image
*/
QImage HsWallpaperImageReader::processedImage() const
{
    return mProcessedImage;
}

/*!
    \internal
    Scales and crops (if needed) image using target rect. 
    Centers target rect automatically if mCenterTarget is true.
    Pass empty set sourceRect to empty to use full size source image as starting point. 
    Returns processed image or null image if operation fails.
*/
void HsWallpaperImageReader::run() 
{
    QImageReader imageReader(mSourcePath);

    QRect tempTargetRect = mTargetRect;
    QRect tempSourceRect = mSourceRect;
    
    if (imageReader.canRead()) {
        QSize sourceSize = imageReader.size();
        if (tempSourceRect.isEmpty()) {
            // If sourceRect not defined, uses full size image as source.
            tempSourceRect.setRect(0, 0, sourceSize.width(), sourceSize.height());
        }
        sourceSize.scale(tempTargetRect.width(), tempTargetRect.height(),
                         Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(sourceSize);

        if (mCenterTarget) {
            tempTargetRect.moveCenter(QPoint(sourceSize.width() / 2, sourceSize.height() / 2));
        }
        imageReader.setScaledClipRect(tempTargetRect);
        mProcessedImage = imageReader.read();
    } else {
        mProcessedImage = QImage();
    }
}
