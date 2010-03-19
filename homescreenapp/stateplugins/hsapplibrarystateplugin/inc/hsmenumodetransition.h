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

#ifndef HSMENUMODETRANSITION_H
#define HSMENUMODETRANSITION_H

#include <QAbstractTransition>
#include "hsmenuview.h"

class QState;

class HsMenuModeTransition: public QAbstractTransition
{

public:

    HsMenuModeTransition(
        HsMenuView &menuView, HsMenuMode menuMode, QState *target);

    bool eventTest(QEvent *event);

    void onTransition(QEvent *event);

private:

    HsMenuView &mMenuView;
    HsMenuMode mMode;
};

#endif //HSMENUMODETRANSITION_H
