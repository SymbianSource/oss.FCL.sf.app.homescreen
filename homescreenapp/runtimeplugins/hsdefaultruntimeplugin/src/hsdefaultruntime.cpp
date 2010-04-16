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
* Description:  Default implementation of the home screen runtime.
*
*/

#include <QState>
#include <QFinalState>
#include <QSignalTransition>
#include <QKeyEventTransition>

#include <qvaluespacepublisher.h>
#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstance>

#include "homescreendomainpskeys.h"
#include "hsdefaultruntime.h"
#include "hsdatabase.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenueventtransition.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hstest_global.h"

QTM_USE_NAMESPACE

#ifdef S60APP_KEY
int applicationKey = Qt::Key_Launch0;
#else
int applicationKey = Qt::Key_Any;
#endif

/*!
    \class HsDefaultRuntime
    \ingroup group_hsdefaultruntimeplugin
    \brief Default implementation of the home screen runtime.
    Creates an execution context (EC) and populates it with
    runtime services. States are loaded from state plugins.
    Each state is given an access to the EC. States
    are added to a state machine. Finally, the state machine
    is started.
*/

/*!
    Constructs runtime with \a parent as the parent object.
*/
HsDefaultRuntime::HsDefaultRuntime(QObject *parent)
    : QStateMachine(parent),
      mContentService(0),	  
	  mHomeScreenActive(false),
	  mIdleStateActive(false),
	  mPublisher(NULL)
{
    HSTEST_FUNC_ENTRY("HS::HsDefaultRuntime::HsDefaultRuntime");

    HsDatabase *db = new HsDatabase;
    db->setConnectionName("homescreen.dbc");
#ifdef Q_OS_SYMBIAN
    db->setDatabaseName("c:/private/20022f35/homescreen.db");
#else
    db->setDatabaseName("homescreen.db");
#endif    
    db->open();
    HsDatabase::setInstance(db);

    HsWidgetPositioningOnOrientationChange::setInstance(
        new HsAdvancedWidgetPositioningOnOrientationChange);

    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    createStatePublisher();
    createContentServiceParts();
    createStates();
    assignServices();
    
	QCoreApplication::instance()->installEventFilter(this);
    HSTEST_FUNC_EXIT("HS::HsDefaultRuntime::HsDefaultRuntime");
}

/*!
    Destructor.
*/
HsDefaultRuntime::~HsDefaultRuntime()
{
    HsWidgetPositioningOnOrientationChange::setInstance(0);
	delete mPublisher;
}

/*!
    \fn void HsDefaultRuntime::stopStateMachine()
    Emission of this signal initiates a transition to the final state.
*/

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsDefaultRuntime::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
		case QEvent::ApplicationActivate:
		{			
			qDebug() << "HsDefaultRuntime::eventFilter: QEvent::ApplicationActivate";
			mHomeScreenActive = true;
			updatePSKeys();
		}
		break;
		case QEvent::ApplicationDeactivate:
		{		
			qDebug() << "HsDefaultRuntime::eventFilter: QEvent::ApplicationDeactivate";
			mHomeScreenActive = false;
            updatePSKeys();			
		}
		break;
	}
    
	return QStateMachine::eventFilter(watched, event);
}

/*!
    Creates Home screen state publisher.
*/
void HsDefaultRuntime::createStatePublisher()
{
    mPublisher = new QValueSpacePublisher(QValueSpace::PermanentLayer, HsStatePSKeyPath);

    if (!mPublisher->isConnected()){
    	// No permanent layer available
    	mPublisher = new QValueSpacePublisher(HsStatePSKeyPath);
    }

    mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenInactive);
}

/*!
    Creates content service parts.
*/
void HsDefaultRuntime::createContentServiceParts()
{
    HSTEST_FUNC_ENTRY("HS::HsDefaultRuntime::createContentServiceParts");

    mContentService = new HsContentService(this);

    HSTEST_FUNC_EXIT("HS::HsDefaultRuntime::createContentServiceParts");
}

/*!
    Creates states.
*/
void HsDefaultRuntime::createStates()
{
    HSTEST_FUNC_ENTRY("HS::HsDefaultRuntime::createStates");

    QFinalState *finalState = new QFinalState();
    addState(finalState);

    QState *guiRootState = new QState();
    addState(guiRootState);

    guiRootState->addTransition(this, SIGNAL(event_exit()), finalState);

    QServiceManager manager;

    QServiceFilter filter;

    filter.setInterface("com.nokia.homescreen.state.HsLoadSceneState");
#ifdef HSDEFAULTRUNTIMEPLUGIN_UNITTEST
    filter.setServiceName("mockstateplugins");
#endif    
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);

    QObject *loadSceneStateObj = manager.loadInterface(interfaces.first());
    QState *loadSceneState = qobject_cast<QState *>(loadSceneStateObj);
    loadSceneState->setParent(guiRootState);
    loadSceneState->setObjectName(interfaces.first().interfaceName());

    filter.setInterface("com.nokia.homescreen.state.HsIdleState");
    interfaces = manager.findInterfaces(filter);
    QObject *idleStateObj = manager.loadInterface(interfaces.first());
    QState *idleState = qobject_cast<QState *>(idleStateObj);
    idleState->setParent(guiRootState);
    idleState->setObjectName(interfaces.first().interfaceName());
	connect(idleState, SIGNAL(entered()), SLOT(onIdleStateEntered()));
	connect(idleState, SIGNAL(exited()), SLOT(onIdleStateExited()));

    loadSceneState->addTransition(
            loadSceneState, SIGNAL(event_idle()), idleState);

    //menu state
    QState *menuParallelState = new QState(
            QState::ParallelStates, guiRootState);
    QState *menuRootState = new QState(menuParallelState);

    filter.setInterface("com.nokia.homescreen.state.HsAppLibraryState");
    interfaces = manager.findInterfaces(filter);
    QObject *appLibraryStateObj = manager.loadInterface(interfaces.first());
    QState *appLibraryState = qobject_cast<QState *>(appLibraryStateObj);
    appLibraryState->setParent(menuRootState);
    appLibraryState->setObjectName(interfaces.first().interfaceName());
    menuRootState->setInitialState(appLibraryState);

    filter.setInterface("com.nokia.homescreen.state.HsMenuWorkerState");
    interfaces = manager.findInterfaces(filter);
    QObject *menuWorkerStateObj = manager.loadInterface(interfaces.first());
    QState *menuWorkerState = qobject_cast<QState *>(menuWorkerStateObj);
    menuWorkerState->setParent(menuParallelState);
    menuWorkerState->setObjectName(interfaces.first().interfaceName());

    // root state transitions
    idleState->addTransition(idleState, SIGNAL(event_applicationLibrary()), menuRootState);
    appLibraryState->addTransition(
            appLibraryState, SIGNAL(toHomescreenState()), idleState);
    // opening shortcut to Application Library
    HsMenuEventTransition *idleToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
                                  idleState, appLibraryState);
    idleState->addTransition(idleToAppLibTransition);

    HsMenuEventTransition *appLibToIdleTransition =
        new HsMenuEventTransition(
            HsMenuEvent::OpenHomeScreen, appLibraryState, idleState);
    appLibraryState->addTransition(appLibToIdleTransition);

    HbMainWindow *window = hbInstance->allMainWindows().first();

    // key driven transition from idle to menu
    QKeyEventTransition *idleToMenuRootTransition =
        new QKeyEventTransition(
                window, QEvent::KeyRelease, applicationKey, idleState);
    idleToMenuRootTransition->setTargetState(menuRootState);
    idleState->addTransition(idleToMenuRootTransition);
    // key driven transition from menu to idle
    QKeyEventTransition *menuToIdleTransition =
        new QKeyEventTransition(
                window, QEvent::KeyRelease, applicationKey, idleState);
    menuToIdleTransition->setTargetState(idleState);
    menuRootState->addTransition(menuToIdleTransition);

    // transitions to child states
    // opening shortcut to a colleciton
    QList<QState *> collectionStates =
        appLibraryState->
            findChildren<QState *>
                ("homescreen.nokia.com/state/applibrarystate/collectionstate");
    qDebug(
        "Found %d \"collectionstate\" children for Application Library State",
            collectionStates.count());
    if (collectionStates.count()) {
        HsMenuEventTransition *idleToCollectionTransition =
            new HsMenuEventTransition(HsMenuEvent::OpenCollection,
                                      idleState, collectionStates[0]);
        idleState->addTransition(idleToCollectionTransition);
    }

    guiRootState->setInitialState(loadSceneState);
    setInitialState(guiRootState);
    
    HSTEST_FUNC_EXIT("HS::HsDefaultRuntime::createStates");
}

/*!
    Assigns services to states based on value of property HS_SERVICES_REGISTRATION_KEY.
*/
void HsDefaultRuntime::assignServices()
{
    HSTEST_FUNC_ENTRY("HS::HsDefaultRuntime::assignServices");

    QList<QState*> children = findChildren<QState*>();
    foreach (QState *state, children) {
        QList<QVariant> services = state->property(HS_SERVICES_REGISTRATION_KEY).toList();
        foreach (const QVariant &service, services) {
            QString name = service.toString();
            qDebug() << "Assign service:" << name << "\n to " << state->objectName();
            if (name == CONTENT_SERVICE_KEY) {
                state->setProperty(name.toAscii().data(), qVariantFromValue(mContentService));
            } else if (name == SHORTCUT_SERVICE_KEY) {
                state->setProperty(name.toAscii().data(),
                    qVariantFromValue(HsShortcutService::instance(this)));
            } else {
                qWarning() << "WARNING: Service " << name << " is unknown";
            }
        }
    }

    HSTEST_FUNC_EXIT("HS::HsDefaultRuntime::assignServices");
}

/*!
    Publishes Home screen states via Publish & Subscribe.
*/
void HsDefaultRuntime::updatePSKeys()
{	
	if (!mPublisher){
		createStatePublisher();
	}

	if (mHomeScreenActive && mIdleStateActive){
    	qDebug() << "HsDefaultRuntime::updatePSKeys: EHomeScreenIdleState";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenIdleState);
    }
    else{
    	qDebug() << "HsDefaultRuntime::updatePSKeys: EHomeScreenInactive";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenInactive);
    }	
}

/*!
    Called when state machine is in Idle state.
*/
void HsDefaultRuntime::onIdleStateEntered()
{
	mIdleStateActive = true;
	updatePSKeys();
}

/*!
    Called when state machine leaves the Idle state.
*/
void HsDefaultRuntime::onIdleStateExited()
{
	mIdleStateActive = false;
	updatePSKeys();
}
