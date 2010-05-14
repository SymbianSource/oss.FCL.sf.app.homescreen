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

#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgetcomponentdescriptor.h"
#include "hsapp_defs.h"
#include "caservice.h"
#include "caquery.h"
#include "caentry.h" 
#include "cadefs.h"
#include "canotifier.h"
#include "hsscene.h"

#include <QSignalMapper>
#include <QDir>
#include <QCoreApplication>

#include <HbMainWindow>

/*!
    \class HsWidgetComponentRegistry
    \ingroup group_hsdomainmodel
    \brief Homescreen widget component registry.
*/

HsWidgetComponentRegistry *HsWidgetComponentRegistry::mInstance = 0;


/*!
    Singleton. 
*/
HsWidgetComponentRegistry* HsWidgetComponentRegistry::instance()
{
    if (!mInstance) {
        mInstance = new HsWidgetComponentRegistry;
        mInstance->setParent(HsScene::mainWindow());
    }
    return mInstance;
}

/*!
    Destructor
*/
HsWidgetComponentRegistry::~HsWidgetComponentRegistry()
{
    qDeleteAll(mRegistry);
}

/*!
    Returns component object of the given \a uri.
*/
HsWidgetComponent *HsWidgetComponentRegistry::component(const QString &uri)
{
	if (mRegistry.contains(uri)) {
        return mRegistry.value(uri);
	}

	QScopedPointer<HsWidgetComponent> c(new HsWidgetComponent(uri));
	mRegistry.insert(uri, c.data());
 	return c.take();
}

/*!
    Requests widget component to emit 'aboutToInstall' signal to free widget resources when uninstalling the widget.
*/
void HsWidgetComponentRegistry::uninstallComponent(const HsWidgetComponentDescriptor &componentDescriptor)
{
    HsWidgetComponent *component = mRegistry.value(componentDescriptor.uri);
    if (component) {
        component->emitAboutToUninstall();
        }
        
}

/*!
    Constructor
*/
HsWidgetComponentRegistry::HsWidgetComponentRegistry(QObject *parent)
  : QObject(parent)
{
    qRegisterMetaType<HsWidgetComponentDescriptor>("HsWidgetComponentDescriptor");
    
    CaQuery query;
    query.setEntryTypeNames(QStringList(widgetTypeName()));
    CaNotifierFilter filter(query);  
    CaNotifier *notifier = CaService::instance()->createNotifier(filter);
    notifier->setParent(this);
    connect(notifier,
           SIGNAL(entryChanged(CaEntry,ChangeType)),
           SLOT(onEntryChanged(CaEntry,ChangeType)),Qt::QueuedConnection);
}

/*!
    Called when entry is added to content service.
*/
void HsWidgetComponentRegistry::handleEntryAdded(const CaEntry &entry, const QString &uri)
{
    /* Unfortunately we don't know why the entry was added. It might be because widget 
     * was missing and is available again (media attached) or it's installed on device 
     */ 
    registerService(entry,uri);
    HsWidgetComponent *component = mRegistry.value(uri);
    if (component) {
        component->emitAvailable();
    }

}

/*!
    Called when entry is removed from content service.
*/
void HsWidgetComponentRegistry::handleEntryRemoved(const CaEntry &entry, const QString &uri)
{
    HsWidgetComponent *component = mRegistry.value(uri);
    
    if (entry.flags() & MissingEntryFlag) {
        if (component) {
            component->emitUnavailable();
        }
    } else {
       // uninstalled
        QString rootPath = entry.attribute(widgetPathAttributeName());
        mServiceManager.removeService(uri);
        QCoreApplication::removeLibraryPath(rootPath);
        if (component) {
            // for now support versions without uinstaller
            component->emitAboutToUninstall();
            component->emitUninstalled();
            mRegistry.take(uri)->deleteLater();
        }
    }
}

/*!
    Called when entry is updated in content service.
*/
void HsWidgetComponentRegistry::handleEntryUpdated(const CaEntry &entry, const QString &uri)
{
    /* Unfortunately we don't know why the entry was updated. It might be update of a widget 
     * component thus we need to register it into service fw 
     */ 
    
    registerService(entry,uri,true);
    HsWidgetComponent *component = mRegistry.value(uri);
    if (component) {
        component->emitUpdated();
    }
}

/*!
    Registers widget to qt service framework and adds widget path to application's library path.
*/
void HsWidgetComponentRegistry::registerService(const CaEntry &entry, const QString& uri, bool reset)
{
    QString path = entry.attribute(widgetPathAttributeName());
    QString serviceXml = entry.attribute("widget:servicexml");
    if (!path.isEmpty() && !serviceXml.isEmpty()) {
        QString service = QDir::toNativeSeparators(path + "/" + serviceXml);
        // if it already exists, it's not added
        if (reset) {
            mServiceManager.removeService(uri);
        }
        QCoreApplication::addLibraryPath(path);
        mServiceManager.addService(service);
    }    
}

/*!
    Slot to handle entry changes in content service.
*/
void HsWidgetComponentRegistry::onEntryChanged(const CaEntry &entry, ChangeType changeType)
{
    QString uri = entry.attribute(widgetUriAttributeName());
    switch (changeType) {
        case AddChangeType: 
            handleEntryAdded(entry,uri);
            break;
           
        case RemoveChangeType:
            handleEntryRemoved(entry,uri);
            break;           
           
        case UpdateChangeType: 
            handleEntryUpdated(entry,uri);
            break;
           
        default: break;     
    }
}

