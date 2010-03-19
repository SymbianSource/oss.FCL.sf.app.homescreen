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

#include <HbMainWindow>
#include "hsloadscenestate.h"
#include "hsscene.h"

/*!
    \class HsLoadSceneState
    \ingroup group_hshomescreenstateplugin
    \brief View part of the home screen idle state.

    Loads the home screen scene.
*/

/*!
    Constructs a new load scene state with the given \a parent.
*/
HsLoadSceneState::HsLoadSceneState(QState *parent)
    : QState(parent)
{
    connect(this, SIGNAL(entered()), SLOT(action_loadScene()));
}

/*!
    Destroys this idle state.
*/
HsLoadSceneState::~HsLoadSceneState()
{
}

/*!
    \fn HsLoadSceneState::event_idle()

    Initiates a transition to idle state.
*/

/*!
    Loads the home screen scene.
*/
void HsLoadSceneState::action_loadScene()
{
    HsScene::instance()->load();
    HsScene::mainWindow()->show();
    emit event_idle();
}
