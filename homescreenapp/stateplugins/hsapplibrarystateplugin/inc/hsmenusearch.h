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
 * Description: Menu search panel.
 *
 */

#ifndef HSMENUSEARCH_H
#define HSMENUSEARCH_H

#include <QObject>
#include "hsmenustates_global.h"

class HsMenuView;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuSearch: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsMenuSearch(HsMenuView *menuView);
public slots:

    void findItem(QString criteriaStr);

private:
    HsMenuView *const mMenuView;//not own
};

#endif // HSMENUSEARCH_H
