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

/**
 * @ingroup group_hsworkerstateplugin
 * @brief Application Library State.
 *
 * Parent state for Application Library functionality (browsing applications and collections)
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsPreviewHSWidgetState : public HsMenuBaseState
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
    HsPreviewHSWidgetState(QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsPreviewHSWidgetState();

private slots:

    /**
     * Slot invoked when preview dialog is dismissed.
     *
     * @since S60 ?S60_version.
     */
    void previewDialogFinished(HbAction* finishedAction);

    /**
     * Slot invoked when message for corrupted widget is dismissed
     *
     * @since S60 ?S60_version.
     */
    void messageWidgetCorruptedFinished(HbAction* finishedAction);
    /**
     * Inherited from HsMenuBaseState.
     *
     * @since S60 ?S60_version.
     */
    void onEntry(QEvent *event);

    /**
     * Memory card removed.
     */
    void memoryCardRemoved();


private:

    /**
     * Show message widget corrupted.
     *
     */
    void showMessageWidgetCorrupted();

    /**
     * Subscribe for memory card remove.
     *
     */
    void subscribeForMemoryCardRemove();

private:
    
    HbDialog *mPopupDialog;

    CaNotifier *mNotifier;

    HbScrollArea *mScrollArea;

    HsWidgetHost *mWidget;

    QObjectList mObjectList;

    int mEntryId;

};



#endif //HSPREVIEWHSWIDGETSTATE_H
