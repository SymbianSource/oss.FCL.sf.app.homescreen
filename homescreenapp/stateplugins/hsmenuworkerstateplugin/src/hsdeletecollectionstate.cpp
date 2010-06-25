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
 * Description: Menu delete collection state
 *
 */

#include <QStateMachine>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <hsmenuservice.h>
#include <HbParameterLengthLimiter>
#include <hsshortcutservice.h>
#include <hsmenueventfactory.h>

#include "hsdeletecollectionstate.h"
#include "hsmenuevent.h"

/*!
 \class HsDeleteCollectionState
 \ingroup group_hsworkerstateplugin
 \brief Delete Collection State.
 Deletes collections.
 */

/*!
 Constructor.
 \param parent Parent state.
 */
HsDeleteCollectionState::HsDeleteCollectionState(QState *parent) :
    QState(parent),
    mItemId(0),
    mDeleteMessage(NULL),
    mConfirmAction(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionState::~HsDeleteCollectionState()
{
    cleanUp(); // in case of throw
}

/*!
 Constructs contained objects.
 */
void HsDeleteCollectionState::construct()
{
    setObjectName("/DeleteCollectionState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsDeleteCollectionState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");

    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    mItemId = data.value(itemIdKey()).toInt();

    QString message;
    if (HsShortcutService::instance()->isItemShortcutWidget(mItemId)) {
        message.append(HbParameterLengthLimiter(
                           "txt_applib_dialog_deletes_1_also_from_home_screen"). arg(
                           HsMenuService::getName(mItemId)));
    } else {
        message.append(HbParameterLengthLimiter("txt_applib_dialog_delete_1").arg(
                           HsMenuService::getName(mItemId)));
    }

    // create and show message box
    mDeleteMessage = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    mDeleteMessage->setAttribute(Qt::WA_DeleteOnClose);

    mDeleteMessage->setText(message);

    mDeleteMessage->clearActions();
    mConfirmAction = new HbAction(hbTrId("txt_common_button_ok"), mDeleteMessage);
    mDeleteMessage->addAction(mConfirmAction);

    HbAction *secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), mDeleteMessage);
    mDeleteMessage->addAction(secondaryAction);

    mDeleteMessage->open(this, SLOT(deleteMessageFinished(HbAction*)));
    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionState::onEntry");
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsDeleteCollectionState::deleteMessageFinished(HbAction* finishedAction)
{
    if (finishedAction == mConfirmAction) {
        HsMenuService::removeCollection(mItemId);
        machine()->postEvent(
        HsMenuEventFactory::createCollectionDeletedEvent());
    }
    emit exit();
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsDeleteCollectionState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mDeleteMessage) {
        disconnect(mDeleteMessage, SIGNAL(finished(HbAction*)), this, SLOT(deleteMessageFinished(HbAction*)));
        mDeleteMessage->close();
        mDeleteMessage = NULL;
    }

    mConfirmAction = NULL;
    mItemId = 0;
}
