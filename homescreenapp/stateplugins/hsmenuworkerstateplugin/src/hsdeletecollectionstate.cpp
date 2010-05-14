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
 \param parent Owner.
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
    setObjectName(this->parent()->objectName() + "/DeleteCollectionState");
    setProperty(HS_SERVICES_REGISTRATION_KEY, QList<QVariant> ()
                << SHORTCUT_SERVICE_KEY);
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsDeleteCollectionState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");

    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    mItemId = data.value(itemIdKey()).toInt();

    QString message;
    if (shortcutService()->isItemShortcutWidget(mItemId)) {
        message.append(hbTrId(
                           "txt_applib_dialog_deletes_1_also_from_home_screen"). arg(
                           HsMenuService::getName(mItemId)));
    } else {
        message.append(hbTrId("txt_applib_dialog_delete_1").arg(
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
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Convenience method returning the shortcut service.
 \since S60 ?S60_version.
 \return Shortcut Service.
 */
HsShortcutService *HsDeleteCollectionState::shortcutService() const
{
    return property(SHORTCUT_SERVICE_KEY).value<HsShortcutService *> ();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsDeleteCollectionState::deleteMessageFinished(HbAction* finishedAction)
{
    if (mItemId !=0 ) { // (work-around for crash if more then one action is selected in HbDialog)

        if (finishedAction == mConfirmAction) {
            HsMenuService::removeCollection(mItemId);
            machine()->postEvent(
            HsMenuEventFactory::createCollectionDeletedEvent());
        }
        mItemId = 0;
        emit exit();
    } else {
        // (work-around for crash if more then one action is selected in HbDialog)
        qWarning("Another signal finished was emited.");
    }
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsDeleteCollectionState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mDeleteMessage) {
        mDeleteMessage->close();
    }

    mDeleteMessage = NULL;
    mConfirmAction= NULL;
    mItemId = 0;
}
