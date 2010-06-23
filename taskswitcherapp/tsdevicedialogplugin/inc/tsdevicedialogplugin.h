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

#ifndef TSDEVICEDIALOGPLUGIN_H
#define TSDEVICEDIALOGPLUGIN_H

#include <hbdevicedialogplugin.h>

#include <QObject>
#include <QVariantMap>
#include <QSharedPointer>
#include <qabstractitemmodel.h>

#include "tsdocumentloader.h"

class TsTaskMonitor;
class TsDeviceDialogPlugin : public HbDeviceDialogPlugin
{
    Q_OBJECT

public:
    TsDeviceDialogPlugin();
    ~TsDeviceDialogPlugin();

    bool accessAllowed(const QString &deviceDialogType, const QVariantMap &parameters, const QVariantMap &securityInfo) const;
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType, const QVariantMap &parameters);
    bool deviceDialogInfo(const QString &deviceDialogType, const QVariantMap &parameters, DeviceDialogInfo *info) const;
    QStringList deviceDialogTypes() const;
    PluginFlags pluginFlags() const;
    int error() const;

private:
    int mError;
    TsDocumentLoader mLoader;
    QAbstractListModel *mModel;
    TsTaskMonitor *mStorage;

    bool mTriedToLoadTranslation;
    
    Q_DISABLE_COPY(TsDeviceDialogPlugin)
};

#endif // TSDEVICEDIALOGPLUGIN_H
