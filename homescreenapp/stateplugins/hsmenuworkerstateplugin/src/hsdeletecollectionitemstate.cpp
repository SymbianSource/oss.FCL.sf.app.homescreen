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
 \param parent Owner.
 */
HsDeleteCollectionItemState::HsDeleteCollectionItemState(QState *parent) :
    QState(parent)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionItemState::~HsDeleteCollectionItemState()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsDeleteCollectionItemState::construct()
{
    setObjectName(this->parent()->objectName()
                  + "/DeleteCollectionItemState");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsDeleteCollectionItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionItemState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    const int itemId = data.value(itemIdKey()).toInt();

    const int collectionId = data.value(collectionIdKey()).toInt();

    QString message;
    message.append(
        hbTrId("txt_applib_dialog_remove_1_from_collection").arg(
            HsMenuService::getName(itemId)));

    if (HbMessageBox::question(message, hbTrId(
                                   "txt_common_button_ok"), hbTrId("txt_common_button_cancel"))) {
        HsMenuService::removeApplicationFromCollection(itemId, collectionId);
    }
    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionItemState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
