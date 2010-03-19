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

#ifndef HSMENUWORKERSTATE_H
#define HSMENUWORKERSTATE_H

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <hsmenuevent.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuWorkerState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param parent Owner.
     */
    HsMenuWorkerState(QState *parent = 0);

    /**

     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsMenuWorkerState();

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();

    /**
     * Template method for constructing child states.
     *
     * @since S60 ?S60_version.
     * @param operation Operation type triggering transition to the
     * newely created state.
     * @return newely created state.
     */
    template<class T>
    T *createChildState(HsMenuEvent::OperationType operation);

private:

    /**
     * Initial state.
     * Own.
     */
    QState *mInitialState;

};

#endif //HSMenuWorkerState_H
