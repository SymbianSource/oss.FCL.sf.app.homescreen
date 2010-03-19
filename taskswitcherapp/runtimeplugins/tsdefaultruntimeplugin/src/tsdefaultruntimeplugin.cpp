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
* Description: Default runtime provider.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "tsdefaultruntimeplugin.h"
#include "tsdefaultruntime.h"

/*!
    \class TsDefaultRuntimePlugin
    \ingroup group_tsdefaultruntimeprovider
    \brief Provides a default implementation of the taskswitcher runtime.

    This provider includes a default implementation of the taskswitcher runtime.
    The runtime is described in the tsdefaultruntimeplugin.xml file.
*/

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Creates a runtime based on the given descriptor.
*/
QObject *TsDefaultRuntimePlugin::createInstance(const QServiceInterfaceDescriptor &descriptor, QServiceContext *context, QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.taskswitcher.runtime.defaultruntime") {
        return new TsDefaultRuntime(this);
    } else {
        return NULL;
    }
}

Q_EXPORT_PLUGIN2(tsdefaultruntimeplugin, TsDefaultRuntimePlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
