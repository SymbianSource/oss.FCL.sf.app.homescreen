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

#ifndef HSVIEWAPPDETAILSSTATE_H_
#define HSVIEWAPPDETAILSSTATE_H_

#include <QState>
#include <QVariant>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HbListWidget;
class HsMenuItemModel;
class HbDialog;
class CaNotifier;

class HsViewAppDetailsState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsViewAppDetailsState(QState *parent = 0);

    virtual ~HsViewAppDetailsState();

protected:

    void onEntry(QEvent *event);
    void onExit(QEvent *event);

private slots:

    void stateExited();
    void memoryCardRemoved();
    
signals:
    void exit();    

private:

    void construct();
    void subscribeForMemoryCardRemove(int entryId);

private:

    HbDialog *mDialog;
    
    QObjectList mObjectList;
    
    CaNotifier *mNotifier;
};

#endif /* HSVIEWAPPDETAILSSTATE_H_ */
