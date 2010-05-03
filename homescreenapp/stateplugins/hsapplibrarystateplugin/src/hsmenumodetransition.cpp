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
 * Description: Hs Menu Mode Transition.
 *
 */

#include <QState>
#include "hsmenumodetransition.h"

/*!
 \class HsMenuModeTransition
 \ingroup group_hsmenustateplugin
 \brief Transition to normal or add mode
 */

/*!
 Constructor.
 \param menuView homescreen menu view.
 \param menuMode homescreen menu mode.
 \param target target state.
 */
HsMenuModeTransition::HsMenuModeTransition(
    HsMenuView &menuView, HsMenuMode menuMode, QState *target):
    mMenuView(menuView),mMode(menuMode)
{
    setTargetState(target);
}

/*!
 Event test.
 \return \a true on test passed, \a false otherwise.
 \param event event.
 */
bool HsMenuModeTransition::eventTest(QEvent *event)
{
    Q_UNUSED(event);
    return mMenuView.getHsMenuMode() == mMode;
}

/*!
 This function is called when the transition is triggered.
 \param event event.
 */
void HsMenuModeTransition::onTransition(QEvent *event)
{
    Q_UNUSED(event);
}
