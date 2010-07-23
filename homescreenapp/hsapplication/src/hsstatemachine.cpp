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
* Description:  Default implementation of the home state machine.
*
*/

#include <QState>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QKeyEventTransition>
#include <QKeyEvent>

#include <qvaluespacepublisher.h>
#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbApplication>
#include <HbActivityManager>
#include <HbInstance>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>

#include "hsmenueventfactory.h"
#include "homescreendomainpskeys.h"
#include "hsstatemachine.h"
#include "hsdatabase.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenueventtransition.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hsconfiguration.h"
#include "hstest_global.h"
#include "hswidgetpositioningonwidgetmove.h"

QTM_USE_NAMESPACE
#define hbApp qobject_cast<HbApplication*>(qApp)

#ifdef Q_OS_SYMBIAN
const static Qt::Key applicationKey = Qt::Key_Menu;
#else
const static Qt::Key applicationKey = Qt::Key_Home;
#endif

namespace
{
    const char KHsRootStateInterface[] = "com.nokia.homescreen.state.HsRootState";
    const char KHsLoadSceneStateInterface[] = "com.nokia.homescreen.state.HsLoadSceneState";
    const char KHsIdleStateInterface[] = "com.nokia.homescreen.state.HsIdleState";
    const char KHsAppLibraryStateInterface[] = "com.nokia.homescreen.state.HsAppLibraryState";
    const char KHsMenuWorkerStateInterface[] = "com.nokia.homescreen.state.HsMenuWorkerState";
    const char KHsBacupRestoreStateInterface[] = "com.nokia.homescreen.state.HsBackupRestoreState";
}


/*!
    \class HsStateMachine
    \ingroup group_hsstatemachine
    \brief Default implementation of the home screen state machine.
    Creates an execution context (EC) and populates it with
    runtime services. States are loaded from state plugins.
    Each state is given an access to the EC. States
    are added to a state machine. Finally, the state machine
    is started.
*/

/*!
    Constructs state machine with \a parent as the parent object.
*/
HsStateMachine::HsStateMachine(QObject *parent)
    : QStateMachine(parent),
      mContentService(0),	  
      mHomeScreenActive(false),
      mIdleStateActive(false),
      mPublisher(NULL)
#ifdef Q_OS_SYMBIAN
	  ,keyCapture()
#endif
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::HsStateMachine");

    HsDatabase *db = new HsDatabase(QCoreApplication::instance());
    db->setConnectionName("homescreen.dbc");
#ifdef Q_OS_SYMBIAN
    db->setDatabaseName("c:/private/20022f35/homescreen.db");
#else
    db->setDatabaseName("private/20022f35/homescreen.db");
#endif    
    db->open();
    HsDatabase::setInstance(db);

    HsConfiguration::setInstance(new HsConfiguration(QCoreApplication::instance()));
    HsConfiguration::instance()->load();
            
    HsWidgetPositioningOnOrientationChange::setInstance(
        new HsAdvancedWidgetPositioningOnOrientationChange);
#ifdef HSWIDGETORGANIZER_ALGORITHM
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsWidgetOrganizer);
#else
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);
#endif

    HsWidgetPositioningOnWidgetMove::setInstance(
        new HsSnapToLines);

    registerAnimations();

    createStatePublisher();
    createContentServiceParts();
    createStates();    

    // create the instance so that singleton is accessible from elsewhere
    HsShortcutService::instance(this);

    QCoreApplication::instance()->installEventFilter(this);

    if (hbApp) { // Qt test framework uses QApplication.
        connect(hbApp->activityManager(), SIGNAL(activityRequested(QString)), 
                this, SLOT(activityRequested(QString)));
    }
    HSTEST_FUNC_EXIT("HS::HsStateMachine::HsStateMachine");
}

/*!
    Destructor.
*/
HsStateMachine::~HsStateMachine()
{
    HsWidgetPositioningOnOrientationChange::setInstance(0);
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
    HsWidgetPositioningOnWidgetMove::setInstance(0);
    delete mPublisher;
}

/*!
    \fn void HsStateMachine::stopStateMachine()
    Emission of this signal initiates a transition to the final state.
*/

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsStateMachine::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
        case QEvent::ApplicationActivate:
            qDebug() << "HsStateMachine::eventFilter: QEvent::ApplicationActivate";
#ifdef Q_OS_SYMBIAN
            keyCapture.captureKey(applicationKey);
#endif
            mHomeScreenActive = true;
            updatePSKeys();
            break;
        case QEvent::ApplicationDeactivate:
            qDebug() << "HsStateMachine::eventFilter: QEvent::ApplicationDeactivate";
#ifdef Q_OS_SYMBIAN
            keyCapture.cancelCaptureKey(applicationKey);
#endif
            mHomeScreenActive = false;
            updatePSKeys();
            break;
        default:
            break;
    }
   
    bool result =  QStateMachine::eventFilter(watched, event);
    // temporary hack as we should not register twice for events
    if (event->type() == QEvent::KeyPress ) {
        QKeyEvent* ke = static_cast<QKeyEvent *>(event);         
        // Key_Launch0 should be removed when QT starts to send Key_Menu
        result = (ke->key() == applicationKey) || ke->key() == Qt::Key_Launch0;        
    }
    return result;
}


/*!
    Registers framework animations.
*/
void HsStateMachine::registerAnimations()
{
    HbIconAnimationManager *manager = HbIconAnimationManager::global();
    manager->addDefinitionFile(QLatin1String("qtg_anim_loading.axml"));
}

/*!
    Creates Home screen state publisher.
*/
void HsStateMachine::createStatePublisher()
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
void HsStateMachine::createContentServiceParts()
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::createContentServiceParts");

    mContentService = new HsContentService(this);

    HSTEST_FUNC_EXIT("HS::HsStateMachine::createContentServiceParts");
}

/*!
    Creates states.
*/
void HsStateMachine::createStates()
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::createStates");

    QFinalState *finalState = new QFinalState();
    addState(finalState);

    QState *guiRootState = new QState();
    addState(guiRootState);

    guiRootState->addTransition(this, SIGNAL(event_exit()), finalState);

    QServiceManager manager;

    
    QObject *loadSceneStateObj = manager.loadInterface(KHsLoadSceneStateInterface);
    QState *loadSceneState = qobject_cast<QState *>(loadSceneStateObj);
    loadSceneState->setParent(guiRootState);
    loadSceneState->setObjectName(KHsLoadSceneStateInterface);

    QObject *rootStateObj = manager.loadInterface(KHsRootStateInterface);
    QState *rootState = qobject_cast<QState *>(rootStateObj);   
    rootState->setParent(guiRootState);
    rootState->setObjectName(KHsRootStateInterface);       

    QObject *idleStateObj = manager.loadInterface(KHsIdleStateInterface);
    QState *idleState = qobject_cast<QState *>(idleStateObj);
    idleState->setParent(rootState);
    idleState->setObjectName(KHsIdleStateInterface);
	connect(idleState, SIGNAL(entered()), SLOT(onIdleStateEntered()));
	connect(idleState, SIGNAL(exited()), SLOT(onIdleStateExited()));


    //menu state
    QState *menuParallelState = new QState(
            QState::ParallelStates, rootState);
    QState *menuRootState = new QState(menuParallelState);

    QObject *appLibraryStateObj = manager.loadInterface(KHsAppLibraryStateInterface);
    QState *appLibraryState = qobject_cast<QState *>(appLibraryStateObj);
    appLibraryState->setParent(menuRootState);
    appLibraryState->setObjectName(KHsAppLibraryStateInterface);
    menuRootState->setInitialState(appLibraryState);

    QHistoryState *historyState = new QHistoryState(rootState);
    historyState->setDefaultState(idleState);
    
    loadSceneState->addTransition(
            loadSceneState, SIGNAL(event_history()), historyState);            

    QObject *menuWorkerStateObj = manager.loadInterface(KHsMenuWorkerStateInterface);
    QState *menuWorkerState = qobject_cast<QState *>(menuWorkerStateObj);
    menuWorkerState->setParent(menuParallelState);
    menuWorkerState->setObjectName(KHsMenuWorkerStateInterface);

    connect(appLibraryState, SIGNAL(collectionEntered()), 
            menuWorkerState, SIGNAL(reset()));

    //Backup/Restore state
    QObject *backupRestoreStateObj = manager.loadInterface(KHsBacupRestoreStateInterface);
    QState *backupRestoreState = qobject_cast<QState *>(backupRestoreStateObj);   
    backupRestoreState->setParent(guiRootState);
    backupRestoreState->setObjectName(KHsBacupRestoreStateInterface);
    backupRestoreState->addTransition(
            backupRestoreState, SIGNAL(event_loadScene()), loadSceneState);

    // root state transitions
    idleState->addTransition(idleState, SIGNAL(event_applicationLibrary()), menuRootState);
    appLibraryState->addTransition(
            appLibraryState, SIGNAL(toHomescreenState()), idleState);
    rootState->addTransition(rootState, SIGNAL(event_backupRestore()), backupRestoreState);
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
                window, QEvent::KeyPress, applicationKey);
    idleToMenuRootTransition->setTargetState(menuRootState);
    idleState->addTransition(idleToMenuRootTransition);
    // key driven transition from menu to idle
    QKeyEventTransition *menuToIdleTransition =
        new QKeyEventTransition(
                window, QEvent::KeyPress, applicationKey);
    menuToIdleTransition->setTargetState(idleState);
    menuRootState->addTransition(menuToIdleTransition);
    
    // transition for Key_Launch0 should be removed 
    // when OT starts to send Key_Menu (maybe wk14)
    QKeyEventTransition *idleToMenuRootTransition2 =
        new QKeyEventTransition(
                window, QEvent::KeyPress, Qt::Key_Launch0);
    idleToMenuRootTransition2->setTargetState(menuRootState);
    idleState->addTransition(idleToMenuRootTransition2);
    // key driven transition from menu to idle
    QKeyEventTransition *menuToIdleTransition2 =
        new QKeyEventTransition(
                window, QEvent::KeyPress, Qt::Key_Launch0);
    menuToIdleTransition2->setTargetState(idleState);
    menuRootState->addTransition(menuToIdleTransition2);
    // add transition to switch to idle
    menuRootState->addTransition( this, SIGNAL(event_toIdle()), idleState);    
    
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
    
    HSTEST_FUNC_EXIT("HS::HsStateMachine::createStates");
}


/*!
    Publishes Home screen states via Publish & Subscribe.
*/
void HsStateMachine::updatePSKeys()
{	
	if (!mPublisher){
		createStatePublisher();
	}

	if (mHomeScreenActive && mIdleStateActive){
    	qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenIdleState";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenIdleState);
    }
    else{
    	qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenInactive";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenInactive);
    }	
}

/*!
    Called when state machine is in Idle state.
*/
void HsStateMachine::onIdleStateEntered()
{
	mIdleStateActive = true;
	updatePSKeys();
}

/*!
    Called when state machine leaves the Idle state.
*/
void HsStateMachine::onIdleStateExited()
{
	mIdleStateActive = false;
	updatePSKeys();
}

/*!
    Activity requested by another client 
*/
void HsStateMachine::activityRequested(const QString &name) 
{
    if (name == groupAppLibRecentView()) {
        this->postEvent(
            HsMenuEventFactory::createOpenCollectionEvent(0,
            collectionDownloadedTypeName()));
    } else if (name == activityHsIdleView()) {
        emit event_toIdle();
    }
}
