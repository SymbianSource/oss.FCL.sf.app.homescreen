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
#include <hsmenuservice.h>
#include <hsshortcutservice.h>

#include "hsuninstallitemstate.h"
#include "hsmenuevent.h"

/*!
 \class HsUninstallItemState
 \ingroup group_hsworkerstateplugin
 \brief Uninstall Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Owner.
 */
HsUninstallItemState::HsUninstallItemState(QState *parent) :
    QState(parent),
    mItemId(0),
    mUninstallMessage(NULL),
    mConfirmAction(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsUninstallItemState::~HsUninstallItemState()
{
    cleanUp(); // in case of throw
}

/*!
 Constructs contained objects.
 */
void HsUninstallItemState::construct()
{
    setObjectName(this->parent()->objectName()
                  + "/UninstallItemState");
    setProperty(HS_SERVICES_REGISTRATION_KEY, QList<QVariant> ()
                << SHORTCUT_SERVICE_KEY);
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsUninstallItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsUninstallItemState::onEntry");
    QState::onEntry(event);
    qDebug("HsUninstallItemState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mItemId = data.value(itemIdKey()).toInt();

    QString message;
    if (shortcutService()->isItemShortcutWidget(mItemId)) {
        message.append(
            hbTrId("txt_applib_dialog_uninstalls_1_and_deletes_all_sh").arg(
                HsMenuService::getName(mItemId)));
    } else {
        message.append(
            hbTrId("txt_applib_dialog_1_will_be_removed_from_phone_c").arg(
                HsMenuService::getName(mItemId)));
    }

    // create and show message box
    mUninstallMessage = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    mUninstallMessage->setAttribute(Qt::WA_DeleteOnClose);

    mUninstallMessage->setText(message);

    mUninstallMessage->clearActions();
    mConfirmAction = new HbAction(hbTrId("txt_common_button_ok"), mUninstallMessage);
    mUninstallMessage->addAction(mConfirmAction);

    HbAction *secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), mUninstallMessage);
    mUninstallMessage->addAction(secondaryAction);

    mUninstallMessage->open(this, SLOT(uninstallMessageFinished(HbAction*)));

    HSMENUTEST_FUNC_EXIT("HsUninstallItemState::onEntry");
}

/*!
 Action after closed confirmation dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsUninstallItemState::uninstallMessageFinished(HbAction* finishedAction)
{
    if (finishedAction == mConfirmAction) {
        HsMenuService::executeAction(mItemId, removeActionIdentifier());
    }
    emit exit();
}

/*!
 Convenience method returning the shortcut service.
 \since S60 ?S60_version.
 \return Shortcut Service.
 */
HsShortcutService *HsUninstallItemState::shortcutService() const
{
    return property(SHORTCUT_SERVICE_KEY).value<HsShortcutService *> ();
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsUninstallItemState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mUninstallMessage) {
        disconnect(mUninstallMessage, SIGNAL(finished(HbAction*)), this, SLOT(uninstallMessageFinished(HbAction*)));
        mUninstallMessage->close();
        mUninstallMessage = NULL;
    }

    mConfirmAction = NULL;
    mItemId = 0;
}
