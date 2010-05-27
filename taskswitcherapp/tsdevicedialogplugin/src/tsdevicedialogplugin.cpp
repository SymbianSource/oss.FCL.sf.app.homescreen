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

#include <QtPlugin>
#include <hbdevicedialog.h>
#include <HbMainWindow>
#include <qservicemanager.h>

#include "tsdevicedialogplugin.h"
#include "tsdevicedialog.h"
#include "tstasksgrid.h"
#include "tstasksgriditem.h"
#include "tsdocumentloader.h"
#include "tsmodel.h"

/*!
    \class TsDeviceDialogPlugin
    \ingroup group_tsdevicedialogplugin
    \brief TaskSwitcher Device Dialog Plug-in.
 */

const QString KTsDialogType = "com.nokia.taskswitcher.tsdevicedialogplugin/1.0";

/*!
    Constructor.
 */
TsDeviceDialogPlugin::TsDeviceDialogPlugin() : mError(0), mModel(0), mStorage(0)
{
}

TsDeviceDialogPlugin::~TsDeviceDialogPlugin()
{
    delete mModel;
}
/*!
    \reimp
 */
bool TsDeviceDialogPlugin::accessAllowed(const QString &deviceDialogType, const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

/*!
    \reimp
 */
HbDeviceDialogInterface *TsDeviceDialogPlugin::createDeviceDialog(const QString &deviceDialogType, const QVariantMap &parameters)
{
    Q_UNUSED(parameters)
    TsDeviceDialog *dialog(0);
    if (deviceDialogType == KTsDialogType) {
        if (0 == mModel) {
            mStorage = new TsTaskMonitor(this);
            if (0 == mStorage) {
                return 0;
            }
            QtMobility::QServiceManager serviceManager;
            QObject *objPtr(serviceManager.loadInterface("com.nokia.qt.activities.ActivityManager"));
            if (objPtr) {
                objPtr->setParent(this);//make it autodestucted
            } else {
                objPtr = this;//activity plugin is not present. provide invalid instance because its not critical functionality.
                //QMetaObject::invokeMethod is safe to use in such a case.
            }
            mModel = new TsModel(*mStorage, *objPtr);
        }

        mLoader.reset();
        bool ok(true);
        mLoader.load(":/xml/resource/layout.docml", &ok);
        Q_ASSERT(ok);

        dialog = qobject_cast<TsDeviceDialog *>(mLoader.findWidget("tsdevicedialog"));
        TsTasksGrid *grid = qobject_cast<TsTasksGrid *>(mLoader.findWidget("taskgrid"));
        Q_ASSERT(dialog);
        Q_ASSERT(grid);

        dialog->changeOrientation(dialog->mainWindow()->orientation());

        grid->setItemPrototype(new TsTasksGridItem());
        grid->setModel(mModel);

        //static_cast<TsModel *>(mModel)->updateModel();

        // connect the grid and model
        qRegisterMetaType<QModelIndex>("QModelIndex");
        
        disconnect(grid, SIGNAL(activated(QModelIndex)), mModel, SLOT(openApplication(QModelIndex)));
        disconnect(grid, SIGNAL(activated(QModelIndex)), dialog, SLOT(close()));
        disconnect(grid, SIGNAL(deleteButtonClicked(QModelIndex)), mModel, SLOT(closeApplication(QModelIndex)));
        
        connect(grid, SIGNAL(activated(QModelIndex)), mModel, SLOT(openApplication(QModelIndex)));
        connect(grid, SIGNAL(activated(QModelIndex)), dialog, SLOT(close()));
        connect(grid, SIGNAL(deleteButtonClicked(QModelIndex)), mModel, SLOT(closeApplication(QModelIndex)), Qt::QueuedConnection);
    }
    return dialog;
}

/*!
    \reimp
 */
bool TsDeviceDialogPlugin::deviceDialogInfo(const QString &deviceDialogType, const QVariantMap &parameters, DeviceDialogInfo *info) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(deviceDialogType)

    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;

    return true;
}

/*!
    \reimp
 */
QStringList TsDeviceDialogPlugin::deviceDialogTypes() const
{
    return QStringList(KTsDialogType);
}

/*!
    \reimp
 */
HbDeviceDialogPlugin::PluginFlags TsDeviceDialogPlugin::pluginFlags() const
{
    return PluginFlags(PreloadPlugin | KeepPluginLoaded);
}

/*!
    \reimp
 */
int TsDeviceDialogPlugin::error() const
{
    return mError;
}

Q_EXPORT_PLUGIN2(tsdevicedialogplugin, TsDeviceDialogPlugin)
