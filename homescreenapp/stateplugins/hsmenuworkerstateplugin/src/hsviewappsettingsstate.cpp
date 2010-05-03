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
* Description:  Menu Application Library state.
 *
*/

#include <hbmainwindow.h>
#include <hbview.h>
#include <hbinstance.h>
#include <qstatemachine.h>
#include <hbaction.h>
#include <qpluginloader.h>

#include "hsdomainmodel_global.h"
#include "hsapp_defs.h"
#include "hsmenuevent.h"
#include "hsviewappsettingsstate.h"
#include "caentry.h"
#include "canotifier.h"
#include "canotifierfilter.h"

#include "hsmenuservice.h"

/*!
 \class HsCollectionNameDialog
 \ingroup HsViewAppSettingsState
 \brief State for showing Application Settings HbView from provided plugin
 */

/*!
 Constructor
 \param parent: parent state
 \retval void
 */
HsViewAppSettingsState::HsViewAppSettingsState(QState *parent) :
        QState(parent),
        mView(0), 
        mPreviousView(0), 
        mActionConfirm(0),
        mNotifier(0)
        
{
        construct();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsViewAppSettingsState::construct()
{
    setObjectName(this->parent()->objectName()
                  + "/ViewAppSettingsState");
}

/*!
 Destructor.
 */
HsViewAppSettingsState::~HsViewAppSettingsState()
{
    if (mNotifier) {
        delete mNotifier;
    }    
}

/*!
 onEntry method invoked on entering the state
 \param event: event
 \retval void
 */
void HsViewAppSettingsState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppSettingsState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    
    const int entryId = data.value(itemIdKey()).toInt();   
    QSharedPointer<const CaEntry> entry = CaService::instance()->getEntry(entryId);    
    
    if(mView)
        {
    
        delete mActionConfirm;  
        mActionConfirm = NULL;    
    
        delete mView;  
        mView = NULL;

        }
    
    QString pluginPath(entry->attribute(appSettingsPlugin()));
    QPluginLoader loader(pluginPath);
    mView = qobject_cast<HbView *>(loader.instance()); 
    
    mActionConfirm = new HbAction(Hb::ConfirmNaviAction, mView);
    connect(mActionConfirm, SIGNAL(triggered()),SLOT(settingsDone()));
    
    if(mView)
        {    
        subscribeForMemoryCardRemove(entryId);
        QObject::connect(this, SIGNAL(initialize(QString)), mView, SLOT(initialize(QString)));        
        mView->setParent(this);
        emit initialize(entry->attribute(applicationUidEntryKey()));        
        // Add View to main window
        HbMainWindow *hbMainWindow = mainWindow();
        // add confirm action
        mView->setNavigationAction(mActionConfirm);
    
        hbMainWindow->addView(mView);
        // record the current view in order to activate it once done
        mPreviousView = hbMainWindow->currentView();
        hbMainWindow->setCurrentView(mView);
        hbMainWindow->show();
        }

    
    HSMENUTEST_FUNC_EXIT("HsViewAppSettingsState::onEntry");
}


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
HbMainWindow *HsViewAppSettingsState::mainWindow() const
{
    return HbInstance::instance()->allMainWindows().value(0);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsViewAppSettingsState::settingsDone()
{
    // Remove mView from main window and restore previous view.
    HbMainWindow *hbMainWindow = mainWindow();
    hbMainWindow->setCurrentView(mPreviousView);
    hbMainWindow->removeView(mView);
    delete mActionConfirm;
    mActionConfirm = NULL;
    delete mView;
    mView = NULL;
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Subscribe for memory card remove.
 \param entryId: entry id.
 \retval void
 */
void HsViewAppSettingsState::subscribeForMemoryCardRemove(int entryId)
{
    CaNotifierFilter filter;
    QList<int> entryIds;
    entryIds.append(entryId);
    filter.setIds(entryIds);
    mNotifier = CaService::instance()->createNotifier(filter);
    mNotifier->setParent(this);
    connect(mNotifier,
            SIGNAL(entryChanged(CaEntry,ChangeType)),
            SLOT(settingsDone()));
}

