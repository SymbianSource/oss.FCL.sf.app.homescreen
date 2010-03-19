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

#include "tsserviceplugin.h"

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "tsitemprovider.h"
#include "tsactivation.h"
#include "tsdeactivation.h"
#include "tspresentation.h"

QObject *TsServicePlugin::createInstance(const QServiceInterfaceDescriptor &descriptor, QServiceContext *context, QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.taskswitcher.itemprovider") {
        return new TsItemProvider(this);
    } else if (descriptor.interfaceName() == "com.nokia.taskswitcher.activation") {
        return new TsActivation(this);
    } else if (descriptor.interfaceName() == "com.nokia.taskswitcher.deactivation") {
        return new TsDeactivation(this);
    } else if (descriptor.interfaceName() == "com.nokia.taskswitcher.presentation") {
        return new TsPresentation(this);
    } else {
        return NULL;
    }
}

Q_EXPORT_PLUGIN2(tsserviceplugin, TsServicePlugin)
