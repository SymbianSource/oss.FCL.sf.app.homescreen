/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Example of home screen widget
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "pannablewidgetplugin.h"
#include "pannablewidget.h"

/**
    @page page_creating_pannable_widget Creating pannable Home Screen Widget Plugin

    When widget need to use horizontal pan gesture itself, e.g when slider is used
    it must implement bool isPannable(QGraphicsMouseEvent *)  function. 
    
    Home Screen will call this when it recognize a pan gesture on a widget. 
    By default, horizontal pan gesture is used to switch pages, but if isPannable() 
    returns true, event is not filtered by Home Screen. 
*/

/*!
    \ingroup group_pannablewidget
    \class PannableWidgetPlugin
    \brief Example implementation for home screen widget plugin.
*/    
    
/*!
    Initialize plugin for pannable widget. Contains necessary information about 
    the pannable widget that it can be loaded through QT Service Framework.
*/
QObject *PannableWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new PannableWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(pannablewidgetplugin, PannableWidgetPlugin)
