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

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "hsshortcutwidgetplugin.h"
#include "hsshortcutwidget.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/**
    \class HsShortcutWidgetPlugin
    \ingroup group_hsshortcutwidgetplugin
    \brief Homescreen shortcut widget plugin collects ready-made homescreen shortcut widget(s).

    This library includes homescreen shortcut widgets implemented by homescreen scrum team.
    This plugin provides widgets that are described in the  hsshortcutwidgetplugin.xml plugin
    xml file.
*/

/*!
    Creates widget object.
*/
QObject *HsShortcutWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                QServiceContext *context,
                                                QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new HsShortcutWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(hsshortcutwidgetplugin, HsShortcutWidgetPlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
