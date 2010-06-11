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

#include <QCoreApplication>
#include <QDir>

#include "hswidgetcomponent.h"
#include "hsapp_defs.h"
#include "caservice.h"
#include "caquery.h"


/*!

*/
HsWidgetComponent::~HsWidgetComponent()
{
}

/*!

*/
QString HsWidgetComponent::rootPath() const
{
    return mRootPath;
}

/*!

*/
QString HsWidgetComponent::uri() const
{
    return mUri;
}


bool HsWidgetComponent::isAvailable() const
{
    bool ret(true);
    if (mRootPath.isEmpty() || !QDir(mRootPath).exists()) {
        ret = false;
    }
    return  ret;
}
/*!

*/
HsWidgetComponent::HsWidgetComponent(const QString &uri, QObject *parent)
  : QObject(parent),
    mUri(uri),
    mState(Available)
{
	resolveRootPath();
	installTranslator();
	
}


/*!

*/
void HsWidgetComponent::resolveRootPath()
{
	CaQuery query;
    query.setEntryTypeNames(QStringList(widgetTypeName()));
	query.setAttribute(widgetUriAttributeName(), mUri);
	QList< QSharedPointer<CaEntry> > widgetEntries = CaService::instance()->getEntries(query);
	
    if (widgetEntries.isEmpty()) {
        return;
	}
    QSharedPointer<CaEntry> entry = widgetEntries.first();
    mRootPath = entry->attribute(widgetPathAttributeName());
    if (mRootPath.isEmpty() || !QDir(mRootPath).exists()) {
        mState = Unavailable;
    }
}

/*!

*/
void HsWidgetComponent::installTranslator()
{
    QStringList possiblePaths;
#ifdef Q_OS_WIN32
    possiblePaths << "resource/qt/translations";
#else
    
    QDir currentDir(mRootPath);
    if (!currentDir.exists() || mRootPath.isEmpty()) {
        return;
    }
    while (currentDir.cdUp()) {}
    QString drive = currentDir.path();
    drive.append("resource/qt/translations");
    possiblePaths << drive;
    // if it is not in installed,then check z drive
    possiblePaths << "z:/resource/qt/translations";
#endif
    
    for(int i=0; i<possiblePaths.count(); ++i) {
        QString candidate = QDir::toNativeSeparators(possiblePaths.at(i));   
        if (QDir(candidate).exists()) {
            QString fileName(mUri);
		    fileName.append("_");
		    fileName.append(QLocale::system().name());
            if (mTranslator.load(fileName, candidate)) {
                QCoreApplication::installTranslator(&mTranslator);
		        break;
            }
	    }
    }
}

/*!

*/
void HsWidgetComponent::uninstallTranslator()
{
    QCoreApplication::removeTranslator(&mTranslator);
}

void HsWidgetComponent::emitAboutToUninstall()
{
    if (mState == Available) {
        mState = Uninstalling;
        emit aboutToUninstall();
    }
}
void HsWidgetComponent::emitUninstalled()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit uninstalled();
    }
}
void HsWidgetComponent::emitUpdated()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit updated();
    }
}
void HsWidgetComponent::emitUnavailable()
{
    if (mState == Available) {
        mState = Unavailable;
        emit unavailable();
    }
}
void HsWidgetComponent::emitAvailable()
{
    if (mState == Unavailable) {
        mState = Available;
        emit available();
    }
}
