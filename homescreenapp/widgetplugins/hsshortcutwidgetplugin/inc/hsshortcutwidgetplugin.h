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
* Description:  Homescreen widget plugin
*
*/


#ifndef HSSHORTCUTWIDGETPLUGIN_H
#define HSSHORTCUTWIDGETPLUGIN_H

#include <QObject>
#include <qserviceplugininterface.h>
#include <hstest_global.h>

QTM_USE_NAMESPACE

HOMESCREEN_TEST_CLASS(TestShortcutWidgetPlugin)
class HsShortcutWidgetPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);
HOMESCREEN_TEST_FRIEND_CLASS(TestShortcutWidgetPlugin)
};

#endif //HSSHORTCUTWIDGETPLUGIN_H
