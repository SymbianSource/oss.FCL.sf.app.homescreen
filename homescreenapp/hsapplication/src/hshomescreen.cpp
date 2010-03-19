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
* Description:  Homescreen application main class.
*
*/

#include <QStateMachine>
#include <QDir>

#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstance>

#include "hshomescreen.h"
#include "hstest_global.h"

QTM_USE_NAMESPACE

/*!
    \class HsHomeScreen
    \ingroup group_hsapplication
    \brief Homescreen application main class.
    Loads a runtime from a runtime provider plugin. Manages
    the state machine execution.
*/

/*!
    Constructor.
    \a parent Parent object.
*/
HsHomeScreen::HsHomeScreen(QObject *parent)
  : QObject(parent),
    mRuntime(0)
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::HsHomeScreen");

    registerServicePlugins();

    QServiceManager manager;
    QServiceFilter filter("com.nokia.homescreen.runtime.HsRuntime");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
    QObject *interface = manager.loadInterface(interfaces.first().interfaceName());
    mRuntime = qobject_cast<QStateMachine*>(interface);
    if (mRuntime) {
        mRuntime->setParent(this);
        connect(mRuntime, SIGNAL(started()), SLOT(onRuntimeStarted()));
        connect(mRuntime, SIGNAL(stopped()), SLOT(onRuntimeStopped()));
        hbInstance->allMainWindows().at(0)->installEventFilter(this);
    } else {
        emit exit();
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::HsHomeScreen");
}

/*!
    Destructor.
*/
HsHomeScreen::~HsHomeScreen()
{
}

/*!
    \fn void HsHomeScreen::exit()
    Emitted when the home screen application needs to exit.
*/

/*!
    Starts the runtime.
*/
void HsHomeScreen::start()
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::start");
    if (mRuntime) {
        mRuntime->start();
    } else {
        HSTEST_FUNC_EXIT("HS::HsHomeScreen::start, mRuntime not created, exit application");
        emit exit();
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::start");
}

/*!
    Stops the runtime.
*/
void HsHomeScreen::stop()
{
	if (mRuntime && mRuntime->isRunning()) {
		QMetaObject::invokeMethod(mRuntime, "event_exit");
	}
}

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsHomeScreen::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    if (event->type() == QEvent::Close) {
        return true;
    }
    return false;
}

/*!
    Called after the runtime has started.
*/
void HsHomeScreen::onRuntimeStarted()
{
}

/*!
    Called after the runtime has stopped.
*/
void HsHomeScreen::onRuntimeStopped()
{
    emit exit();
}

/*!
    Registers service plugins pre-installed on the device.
*/
void HsHomeScreen::registerServicePlugins()
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::registerServicePlugins()");
    QStringList pluginPaths;

    pluginPaths << "hsresources/plugins";
#ifdef Q_OS_SYMBIAN
    pluginPaths << "private/20022F35/import/widgetregistry";
#else
    pluginPaths << "hsresources/import/widgetregistry";
#endif

    QFileInfoList drives = QDir::drives();
    foreach(const QString pluginPath, pluginPaths) {
#ifdef Q_OS_SYMBIAN
        //Check plugin dirs from root of different drives
        foreach(QFileInfo drive, drives) {
            QString driveLetter = drive.absolutePath();
            QString path = driveLetter + pluginPath;
            if(QDir(path).exists()) {
                registerServicePlugins(path);
            }
        }
#endif
        //Check plugin path relative to current dir
        if(QDir(pluginPath).exists()) {
            registerServicePlugins(pluginPath);
        }
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::registerServicePlugins()");
}

/*!
    Recursively registers service plugins starting from given /a root
    directory. All directories containing plugins are added to
    application's library paths at the same time.
*/
void HsHomeScreen::registerServicePlugins(const QString &root)
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::registerServicePlugins(const QString &)");
    QDir dir = QDir(root);
    QFileInfoList fileInfos = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo fileInfo, fileInfos) {
        registerServicePlugins(fileInfo.absoluteFilePath());
    }

    fileInfos = dir.entryInfoList(QStringList("*.xml"));

    if (!fileInfos.isEmpty()) {
        //Plugin dll and xml are in the same directory
        QApplication::addLibraryPath(root);
        qDebug() << QString("HS::HsHomeScreen::registerServicePlugins - Directory added to application's library paths: ")
                 << root;
        QServiceManager manager;
        foreach(QFileInfo fileInfo, fileInfos) {
            manager.addService(fileInfo.absoluteFilePath());
            qDebug() << QString("HS::HsHomeScreen::registerServicePlugins - Plugin registered: ") + fileInfo.fileName();
        }
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::registerServicePlugins(const QString &)");
}
