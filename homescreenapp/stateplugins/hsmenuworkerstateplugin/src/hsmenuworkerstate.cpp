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

#include <hsmenueventtransition.h>

#include "hsaddtohomescreenstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsdeletecollectionstate.h"
#include "hsdeletecollectionitemstate.h"
#include "hsuninstallitemstate.h"
#include "hsmenuworkerstate.h"
#include "hscollectionnamestate.h"
#include "hsarrangestate.h"
#include "hspreviewhswidgetstate.h"
#include "hsviewappsettingsstate.h"
#include "hsviewappdetailsstate.h"

/*!
 \class HsMenuWorkerState
 \ingroup group_hsworkerstateplugin
 \brief Menu Worker State.
 */

/*!
 \var HsMenuWorkerState::mInitialState
 Initial state.
 Own.
 */

/*!
 Constructor.
 \param parent Parent state. 
 */
HsMenuWorkerState::HsMenuWorkerState(QState *parent) :
    QState(parent), mInitialState(0)
{
    construct();
}

/*!
 Destructor.
 */
HsMenuWorkerState::~HsMenuWorkerState()
{

}

/*!
 Constructs contained objects.
 */
void HsMenuWorkerState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuWorkerState::construct");
    setObjectName("homescreen.nokia.com/state/MenuWorkerState");

    mInitialState = new QState(this);
    setInitialState(mInitialState);

    // use templated creation method - less boilerplate code
    createChildState<HsAddToHomeScreenState> (HsMenuEvent::AddToHomeScreen);
    createChildState<HsDeleteCollectionState> (
        HsMenuEvent::DeleteCollection);
    createChildState<HsDeleteCollectionItemState> (
        HsMenuEvent::RemoveAppFromCollection);
    createChildState<HsUninstallItemState> (
        HsMenuEvent::UninstallApplication);
    createChildState<HsArrangeState> (HsMenuEvent::ArrangeCollection);

    // create a new child state based on the template
    HsCollectionNameState *newChildState = new HsCollectionNameState(this);
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *renameCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::RenameCollection,
                                  mInitialState, newChildState);
    mInitialState->addTransition(renameCollectionTransition);

    HsMenuEventTransition *createCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::CreateCollection,
                                  mInitialState, newChildState);
    mInitialState->addTransition(createCollectionTransition);
    // set a transition to the initial state after child processing finished
    newChildState->addTransition(newChildState, SIGNAL(exit()), mInitialState);

    HsAddAppsToCollectionState *addAppsToCollectionState =
        new HsAddAppsToCollectionState(this);
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *addAppsToCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::AddAppsToCollection,
                                  mInitialState, addAppsToCollectionState);
    mInitialState->addTransition(addAppsToCollectionTransition);
    // set a transition to the initial state after child processing finished
    addAppsToCollectionState->addTransition(addAppsToCollectionState,
                                            SIGNAL(finished()), mInitialState);
    connect(this, SIGNAL(reset()), addAppsToCollectionState, SIGNAL(finished()));
    createChildState<HsPreviewHSWidgetState> (HsMenuEvent::PreviewHSWidget);
    createChildState<HsViewAppSettingsState> (HsMenuEvent::ShowAppSettings);
    createChildState<HsViewAppDetailsState> (HsMenuEvent::ShowAppDetails);

    HSMENUTEST_FUNC_EXIT("HsMenuWorkerState::construct");
}

/*!
 Template method for constructing child states.
 \param operation Operation type triggering transition to the
 newely created state.
 \return newely created state.
 */
template<class T>
T *HsMenuWorkerState::createChildState(
    HsMenuEvent::OperationType operation)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuWorkerState::createChildState");
    // create a new child state based on the template
    T *newChildState = new T(this);
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *newChildStateTransition =
        new HsMenuEventTransition(operation, mInitialState, newChildState);
    mInitialState->addTransition(newChildStateTransition);
    // set a transition to the initial state after child processing finished
    newChildState->addTransition(newChildState, SIGNAL(exit()), mInitialState);
    connect(this, SIGNAL(reset()), newChildState, SIGNAL(exit()));
    HSMENUTEST_FUNC_EXIT("HsMenuWorkerState::createChildState");

    return newChildState;
}
