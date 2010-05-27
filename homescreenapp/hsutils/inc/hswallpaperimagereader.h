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

#include <QThread>
#include <QImage>

#include "hsutils_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsWallpaperImageReader : public QThread
{
    Q_OBJECT

public:    
    HsWallpaperImageReader(QObject *parent = 0);
    ~HsWallpaperImageReader();

    void setSourcePath(const QString &sourcePath);
    QString sourcePath() const;
    void setSourceRect(const QRect &sourceRect);
    QRect sourceRect() const;
    void setTargetRect(const QRect &targetRect);
    QRect targetRect() const;
    void setCenterTarget(bool center);
    bool centerTarget();
    QImage processedImage() const;
    
signals:
    void processingFinished();

protected:
    void run();

private:
    Q_DISABLE_COPY(HsWallpaperImageReader)

private:
    QString mSourcePath;
    QRect mSourceRect;
    QRect mTargetRect;
    bool mCenterTarget;
    QString mKey;
    QImage mProcessedImage;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSWALLPAPERIMAGEREADER_H
