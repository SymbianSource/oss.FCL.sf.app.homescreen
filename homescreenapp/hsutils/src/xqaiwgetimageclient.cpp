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
* Description:  Moc impl for image fetcher api.
*
*/


#include "xqaiwgetimageclient.h"

#include "hsimagegridwidget.h"
#include "hbinstance.h"
#include <QDir>
#include <hbaction.h>
#include <QFileInfoList>
#include <QFileInfo>




#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
XQAIWGetImageClient::XQAIWGetImageClient():
    mImageGrid(0)
{
    mImageGrid = new HsImageGridWidget(QStringList(),0);
    mBackAction = new HbAction(Hb::BackAction, this);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
XQAIWGetImageClient::~XQAIWGetImageClient()
{
    if(mImageGrid)
        delete mImageGrid;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::fetch( QVariantMap filter, XQAIWMultimediaFlags flag)
{
    Q_UNUSED(filter)
    Q_UNUSED(flag)

    QStringList imageDirNames;
    QStringList imageDirs;

#ifdef Q_OS_SYMBIAN
    imageDirNames << "data/images/kqtihswallpapers/" << "images/kqtihswallpapers/";
#else
    imageDirNames << "resource/kqtihswallpapers/";
#endif

    foreach(QString imageDirName, imageDirNames)
    {
        //Check image dirs from root of different drives
        QFileInfoList drives = QDir::drives();
        for(int i=0; i < drives.count(); i++)
        {
            QFileInfo drive = drives.at(i);
            QString driveLetter = drive.absolutePath();
            QString path = QDir::toNativeSeparators(driveLetter + imageDirName);
            if(QDir(path).exists())
            {
                imageDirs << path;
            }
        }

        //Check image dir relative to current dir
        QString isRelativeToCurrent = QDir::toNativeSeparators(QDir(imageDirName).absolutePath() + QDir::separator());
        if(QDir(imageDirName).exists() && !imageDirs.contains(isRelativeToCurrent))
        {
            imageDirs << imageDirName;
        }
    }

    QStringList filters;
    filters << "*.png";
    filters << "*.jpg";

    QStringList images;

    foreach(QString imageDir, imageDirs)
    {
        findImages(QDir(imageDir).absolutePath(), filters, images);
    }

    HbMainWindow *window = HbInstance::instance()->allMainWindows().at(0);
    window->addSoftKeyAction(Hb::SecondarySoftKey, mBackAction);

    mImageGrid->setContent(images);

    connect(mImageGrid,SIGNAL(imageSelected(const QString&)),this,SLOT(imageSelected(const QString&)));

    HbInstance::instance()->allMainWindows().at(0)->addView(mImageGrid);
    HbInstance::instance()->allMainWindows().at(0)->setCurrentView(mImageGrid,false);

    connect(mBackAction,SIGNAL(triggered()),this,SLOT(imageSelectionCancelled()));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::imageSelected(const QString& val)
{
    Q_UNUSED(val)
    mImageGrid->disconnect(this);
    QStringList list;

    list << val;

    HbMainWindow *window = HbInstance::instance()->allMainWindows().at(0);
    window->removeView(mImageGrid);
    window->removeSoftKeyAction(Hb::SecondarySoftKey, mBackAction);

    emit fetchComplete(list);
    QStringList images;
    mImageGrid->setContent(images);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::requestCompleted(const QVariant& val)
{
    Q_UNUSED(val)
}

void XQAIWGetImageClient::imageSelectionCancelled()
{
    mImageGrid->disconnect(this);

    HbMainWindow *window = HbInstance::instance()->allMainWindows().at(0);
    window->removeView(mImageGrid);
    window->removeSoftKeyAction(Hb::SecondarySoftKey, mBackAction);

    emit fetchFailed(-1);//KErrNotFound
    QStringList images;
    mImageGrid->setContent(images);

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::findImages(const QString &aDir, const QStringList &aFilters, QStringList &aFoundImages)
{
    QStringList foundFiles;

    //Add image files from dir
    QDir searchDir = QDir(aDir);
    searchDir.setNameFilters(aFilters);
    searchDir.setFilter(QDir::Files);
    QStringList files = searchDir.entryList();
    foreach(QString file, files)
    {
        aFoundImages << searchDir.absoluteFilePath(file);
    }

    //Go through sub directories
    searchDir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList subdirs = searchDir.entryList();
    foreach(QString subdir, subdirs)
    {
        //Recursive call
        findImages(QString(searchDir.absolutePath() + QDir::separator() + subdir),
                           aFilters,
                           aFoundImages);
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
