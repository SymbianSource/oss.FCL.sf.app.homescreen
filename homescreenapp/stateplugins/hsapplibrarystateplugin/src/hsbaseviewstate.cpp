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

#include <hbmessagebox.h>
#include <HbParameterLengthLimiter>
#include <hbaction.h>
#include <canotifier.h>

#include "hsbaseviewstate.h"

/*!
 Constructor.
 */
HsBaseViewState::HsBaseViewState(
    QState *parent):
    QState(parent),
    mNotifier(0),
    mMessageRelatedItemId(0),
    mApplicationLaunchFailMessage(0)    
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsBaseViewState::construct()
{
}

/*!
 Creates and open application launch fail message.
 \param errorCode eroor code to display.
 */
void HsBaseViewState::createApplicationLaunchFailMessage(int errorCode,int itemId)
{
    QString message;
    message.append(
        HbParameterLengthLimiter("txt_applib_info_launching_the_application_failed").arg(
            errorCode));
    
    mMessageRelatedItemId = itemId;

    // create and show message box
    mApplicationLaunchFailMessage = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    mApplicationLaunchFailMessage->setAttribute(Qt::WA_DeleteOnClose);

    mApplicationLaunchFailMessage->setText(message);

    mApplicationLaunchFailMessage->clearActions();
    HbAction *mClosemAction = new HbAction(hbTrId("txt_common_button_close"),
        mApplicationLaunchFailMessage);
    mApplicationLaunchFailMessage->addAction(mClosemAction);

    mApplicationLaunchFailMessage->open(this, SLOT(applicationLaunchFailMessageFinished(HbAction*)));
    
    subscribeForMemoryCardRemove();
}

/*!
 Subscribe for memory card remove.
 */
void HsBaseViewState::subscribeForMemoryCardRemove()
{
    if (mMessageRelatedItemId !=0 ) {
        CaNotifierFilter filter;
        filter.setIds(QList<int>() << mMessageRelatedItemId);
        mNotifier = CaService::instance()->createNotifier(filter);
        mNotifier->setParent(this);
        connect(mNotifier,
            SIGNAL(entryChanged(CaEntry,ChangeType)),
            SLOT(cleanUpApplicationLaunchFailMessage()));
    }
}

/*!
 Clean up application launch fail message box.
 \retval void
 */
void HsBaseViewState::cleanUpApplicationLaunchFailMessage()
{
    if (mApplicationLaunchFailMessage) {
        mApplicationLaunchFailMessage->close();
        mApplicationLaunchFailMessage = NULL;
    }
    if (mNotifier) {
        delete mNotifier;
        mNotifier = NULL;
    }
    mMessageRelatedItemId = 0;
}

/*!
 Action after closed application launch fail dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsBaseViewState::applicationLaunchFailMessageFinished(HbAction*)
{
    mApplicationLaunchFailMessage = NULL;
    cleanUpApplicationLaunchFailMessage();
}

/*!
 Slot invoked when a state is exited.
 */
void HsBaseViewState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::stateExited");
    cleanUpApplicationLaunchFailMessage();
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::stateExited");
}

/*!
 Destructor.
 */
HsBaseViewState::~HsBaseViewState()
{
}
