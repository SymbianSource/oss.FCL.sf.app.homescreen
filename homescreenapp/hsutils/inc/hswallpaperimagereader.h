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

#ifndef HSWALLPAPERIMAGEREADER_H
#define HSWALLPAPERIMAGEREADER_H

#include <QObject>
#include <QImage>

#include "hsutils_global.h"

class HSUTILS_EXPORT HsWallpaperImageReader : public QObject
{
    Q_OBJECT

public:    
    HsWallpaperImageReader(QObject *parent = 0);
    ~HsWallpaperImageReader();

    void setSourcePath(const QString &sourcePath);
    QString getSourcePath() const;
    void setSourceRect(const QRect &sourceRect);
    QRect getSourceRect() const;
    void setTargetRect(const QRect &targetRect);
    QRect getTargetRect() const;
    void setCenterTarget(bool center);
    bool getCenterTarget();
    QImage getProcessedImage() const;

private:
    Q_DISABLE_COPY(HsWallpaperImageReader)

signals:
    void processingFinished();

public slots:
    void processImage();

private:
    QString mSourcePath;
    QRect mTargetRect;
    QRect mSourceRect;
    bool mCenterTarget;

    QImage mProcessedImage;
};

#endif // HSWALLPAPERIMAGEREADER_H
