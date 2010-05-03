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
* Description:  Example of localised home screen widget
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "localisedhellowidgetplugin.h"
#include "localisedhellowidget.h"

/**
    @page page_creating_widget_plugin Creating Home Screen Widget Plugin

    Widgets are exposed to the home screen through QT Service Framework.
    Widget plugins are implemented according to
    <a href="http://qt.nokia.com/doc/qtmobility-1.0-tp/service-frameworks.html">Qt service framework plugin model</a>.

    See @ref page_nativewidgetmodel for the instructions how to create widget for the home screen.

    The steps to create a widget plugin are:
    <ol>
    <li> Declare a plugin class that inherits from QObject and from the \c QServicePluginInterface interface.
    
    <li> Use the Q_INTERFACES() macro to tell Qt's meta-object system about the \c QServicePluginInterface interface.
    
    <li> Export the plugin using the Q_EXPORT_PLUGIN2() macro.
    
    <li> Build the plugin using an suitable .pro file. See @ref page_deploying_widget_plugin
    </ol>
    
    An example (full example source code can be found from section @ref page_nativewidgetmodel):

    Each widget plugin has a XML file that allows searching widgets through QT service framework without first loading it.
    The XML file contains information on widgets inside the plugin:
    
    \li \c name             The name of the plugin binary.
    \li \c filepath         The absolute path and name of plugin without suffix
    \li \c interface name   Uniquely identifies the widget.
    \li \c title            Widget's human-readable name.
    \li \c iconuri          URI of the widget's icon image file.
    
    Example: XML for a widget plugin.
    
    @code
    <?xml version="1.0" encoding="UTF-8"?>
    <service>
        <name>helloworldwidgetplugin</name>
        <filepath>helloworldwidgetplugin</filepath>
        <description>Example widget</description>
        <interface>
            <name>com.nokia.symbian.IHomeScreenWidget</name>
            <version>1.0</version>
            <description>Example of home screen widget</description>
            <capabilities></capabilities>
            <customproperty key="iconuri">helloworldwidgetplugin.png</customproperty>
            <customproperty key="title">HelloWorld</customproperty>
        </interface>
    </service>
    @endcode
*/

/**
    @page page_deploying_widget_plugin Deploying Home Screen Widget Plugin
    
    Widget's binaries and xml file(s) must be deployed to correct folders on emulator and in target. 
    Below are the needed .pro file for the \c helloworldwidgetplugin.
    
    For example:
    
    @code
    # helloworldwidgetplugin.pro

    TEMPLATE = lib
    CONFIG += plugin mobility hb
    MOBILITY = serviceframework
    
    HEADERS += ./inc/ .h
    SOURCES += ./src/ .cpp
  
    DESTDIR = $${EPOCROOT}epoc32/data/c/private/20022F35/import/widgetregistry/20022F7E
    
    INCLUDEPATH += ./inc               
    
    symbian: {
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
        TARGET.UID3 = 0x20022F7E
        TARGET.EPOCALLOWDLLDATA=1
        TARGET.CAPABILITY = ALL -TCB
        
        plugins.path = $${DESTDIR}
        plugins.sources = $${TARGET}.dll
        
        widgetResources.path = $${DESTDIR}
        widgetResources.sources += resource/$${TARGET}.xml    
        widgetResources.sources += resource/$${TARGET}.manifest
        widgetResources.sources += resource/$${TARGET}.png
        
        DEPLOYMENT += plugins \
                      widgetResources
    }
    
    @endcode
    
    For detailed information on DEPLOYMENT macro, see <a HREF="http://pepper.troll.no/s60prereleases/doc/qmake-variable-reference.html#deployment">here</a>.
*/

/*!
    \ingroup group_helloworld_widget
    \class HelloWorldWidgetPlugin
    \brief Example implementation for home screen widget plugin.
*/    
    
/*!
    Initialize plugin for hello world widget. Contains necessary information about 
    the hello world widget that it can be loaded through QT Service Framework.
*/
QObject *LocalisedHelloWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new LocalisedHelloWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(localisedhellowidgetplugin, LocalisedHelloWidgetPlugin)
