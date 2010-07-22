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
 * Description: Base for menu view states.
 *
 */

#ifndef HSBASEVIEWSTATE_H
#define HSBASEVIEWSTATE_H

#include <qstate.h>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"
#include "hsmenuview.h"

class HbMessageBox;
class CaNotifier;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsBaseViewState: public QState
{
    Q_OBJECT
    
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
    
public:
    
    HsBaseViewState(QState *parent);    
    ~HsBaseViewState();
    
private slots:

    void applicationLaunchFailMessageFinished(HbAction*);
    
protected slots:
    
    void stateExited();
    
protected:
    
    void createApplicationLaunchFailMessage(int errorCode,int itemId);
    void subscribeForMemoryCardRemove();
    
private:
    
    void construct();
    void cleanUpApplicationLaunchFailMessage();
    
private:
    
    CaNotifier *mNotifier;
    
    int mMessageRelatedItemId;
    
    HbMessageBox *mApplicationLaunchFailMessage;

};


#endif // HSBASEVIEWSTATE_H
