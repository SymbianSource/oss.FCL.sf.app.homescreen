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
    QState(parent)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionState::~HsDeleteCollectionState()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsDeleteCollectionState::construct()
{
    setObjectName(this->parent()->objectName() + "/DeleteCollectionState");
    setProperty(HS_SERVICES_REGISTRATION_KEY, QList<QVariant> ()
                << SHORTCUT_SERVICE_KEY);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
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
    const int itemId = data.value(itemIdKey()).toInt();

    QString message;
    if (shortcutService()->isItemShortcutWidget(itemId)) {
        message.append(hbTrId(
                           "txt_applib_dialog_deletes_1_also_from_home_screen"). arg(
                           HsMenuService::getName(itemId)));
    } else {
        message.append(hbTrId("txt_applib_dialog_delete_1").arg(
                           HsMenuService::getName(itemId)));
    }

    if (HbMessageBox::question(message, hbTrId(
                                   "txt_common_button_ok"), hbTrId("txt_common_button_cancel"))) {
        HsMenuService::removeCollection(itemId);
        machine()->postEvent(
            HsMenuEventFactory::createCollectionDeletedEvent());
    }
    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsShortcutService *HsDeleteCollectionState::shortcutService() const
{
    return property(SHORTCUT_SERVICE_KEY).value<HsShortcutService *> ();
}
