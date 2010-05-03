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
* Description:  Homescreen wallpaper helper class.
*
*/

#include <QString>
#include <QImageReader>

#include "hswallpaperhelper.h"


/*!
    \class HsWallpaperHelper
    \ingroup group_hsapplication
    \brief Homescreen wallpaper helper class.
    Implements image scaling and cropping services. 
*/

/*!
    \internal
    Scales and crops (if needed) \a sourcePath image using \a targetRect. 
    Centers target rect automatically if a\ centerTarget is true.
    Pass empty a\ sourceRect to use full size source image as starting point. 
    Returns processed image or null image if operation fails.
*/
QImage HsWallpaperHelper::processImage(const QString &sourcePath,
                                       const QRect &targetRect,
                                       const QRect &sourceRect,
                                       bool centerTarget) 
{
    QImageReader imageReader(sourcePath);
    
    QRect tempTargetRect = targetRect;
    QRect tempSourceRect = sourceRect;
    
    if (imageReader.canRead()) {
        QSize sourceSize = imageReader.size();
        if (tempSourceRect.isEmpty()) {
            // If sourceRect not defined, uses full size image as source.
            tempSourceRect.setRect(0, 0, sourceSize.width(), sourceSize.height());
        }
        sourceSize.scale(tempTargetRect.width(), tempTargetRect.height(), 
            Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(sourceSize);

        if (centerTarget) {
            tempTargetRect.moveCenter(QPoint(sourceSize.width() / 2, sourceSize.height() / 2));
        }
        imageReader.setScaledClipRect(tempTargetRect);
        
        return imageReader.read();
    }
    return QImage(); // returns null QImage
}
