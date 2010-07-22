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
 * Description: Menu delete collection item state
 *
 */

#include <hbmessagebox.h>
#include <hbaction.h>
#include <HbParameterLengthLimiter>
#include <hsmenuservice.h>
#include <hsmenueventfactory.h>

#include "hsdeletecollectionitemstate.h"
#include "hsmenuevent.h"

/*!
 \class HsDeleteCollectionItemState
 \ingroup group_hsworkerstateplugin
 \brief Delete Collection Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Parent state. 
 */
HsDeleteCollectionItemState::HsDeleteCollectionItemState(QState *parent) :
    QState(parent),
    mItemId(0),
    mCollectionId(0),
    mDeleteMessage(NULL),
    mConfirmAction(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionItemState::~HsDeleteCollectionItemState()
{
    cleanUp(); // in case of throw
}

/*!
 Constructs contained objects.
 */
void HsDeleteCollectionItemState::construct()
{
    setObjectName("/DeleteCollectionItemState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsDeleteCollectionItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionItemState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mItemId = data.value(itemIdKey()).toInt();
    mCollectionId = data.value(collectionIdKey()).toInt();

    QString message;
    message.append(
        HbParameterLengthLimiter("txt_applib_dialog_remove_1_from_collection").arg(
            HsMenuService::getName(mItemId)));

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

    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionItemState::onEntry");
}

/*!
 Action after closed confirmation dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsDeleteCollectionItemState::deleteMessageFinished(HbAction* finishedAction)
{
    if (finishedAction == mConfirmAction) {
        HsMenuService::removeApplicationFromCollection(mItemId, mCollectionId);
    }
    emit exit();
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsDeleteCollectionItemState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mDeleteMessage) {
		disconnect(mDeleteMessage, SIGNAL(finished(HbAction*)), this, SLOT(deleteMessageFinished(HbAction*)));
        mDeleteMessage->close();
		mDeleteMessage = NULL;
    }
	
    mConfirmAction = NULL;
    mItemId = 0;
    mCollectionId = 0;
}
