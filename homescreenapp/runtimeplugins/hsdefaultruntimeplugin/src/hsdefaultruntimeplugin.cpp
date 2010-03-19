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
* Description:  Default runtime plugin.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "hsdefaultruntimeplugin.h"
#include "hsdefaultruntime.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

QObject *HsDefaultRuntimePlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                QServiceContext *context,
                                                QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.homescreen.runtime.HsRuntime")) {
        return new HsDefaultRuntime(this);
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(hsdefaultruntimeplugin, HsDefaultRuntimePlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
