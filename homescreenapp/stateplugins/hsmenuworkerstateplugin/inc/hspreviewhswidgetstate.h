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
* Description:  Menu Application Library state.
*
*/


#ifndef HSPREVIEWHSWIDGETSTATE_H
#define HSPREVIEWHSWIDGETSTATE_H

#include <hsmenubasestate.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HbDialog;
class HsWidgetHost;
class CaNotifier;
class HbAction;
class HbScrollArea;
class HbMessageBox;

class HsPreviewHSWidgetState : public HsMenuBaseState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsPreviewHSWidgetState(QState *parent = 0);

    virtual ~HsPreviewHSWidgetState();

private slots:

    void previewDialogFinished(HbAction* finishedAction);

    void messageWidgetCorruptedFinished(HbAction* finishedAction);

    void onEntry(QEvent *event);

    void cleanUp();

    void memoryCardRemoved();

signals:
    void exit();

private:

    void showMessageWidgetCorrupted();

    void subscribeForMemoryCardRemove();

private:
    
    HbDialog *mPopupDialog;

    CaNotifier *mNotifier;

    HbScrollArea *mScrollArea;

    HsWidgetHost *mWidget;

    QObjectList mObjectList;

    int mEntryId;

    HbMessageBox *mCorruptedMessage;

};



#endif //HSPREVIEWHSWIDGETSTATE_H
