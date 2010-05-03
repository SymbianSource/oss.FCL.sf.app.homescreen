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
 * Description: Menu name collection state
 *
 */

#include <hbaction.h>
#include <hblineedit.h>
#include <hsmenuservice.h>

#include "hscollectionnamestate.h"
#include "hsmenuevent.h"
#include "hscollectionnamedialog.h"

/*!
 \class HsCollectionNameState
 \ingroup group_hsworkerstateplugin
 \brief Rename Collection State.
 Collection name state
 */

/*!
 \fn void commit(const QString &collectionName);
 Signal emitted when collection name is given.
 \param collectionName name of collection.
 */

/*!
 \fn void commitCheckList(const QString &collectionName);
 Signal emitted when collection name is given - version to trigger
 transition to HsAppsCheckListState.
 \param collectionName name of collection.
 */

/*!
 \fn void cancel();
 Signal emitted when user selects cancel.
 */

/*!
 Constructor
 \param parent owner
 \retval void
 */
HsCollectionNameState::HsCollectionNameState(QState *parent) :
    QState(parent)
{
    construct();
}

/*!
 Destructor
 \retval void
 */
HsCollectionNameState::~HsCollectionNameState()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameState::construct()
{
    setObjectName(this->parent()->objectName() + "/collectionnamestate");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsCollectionNameState::onEntry(QEvent *event)
{
    qDebug("CollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsCollectionNameState::onEntry");
    QState::onEntry(event);

    int itemId = 0;
    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();

        itemId = data.value(itemIdKey()).toInt();
    }

    HsCollectionNameDialog editor(itemId);

    HbAction *result = editor.exec();
    if (result == editor.primaryAction()) {
        QString newName(editor.newName(editor.value().toString(), true));
        if (itemId) {
            if (newName != HsMenuService::getName(itemId)) {
                HsMenuService::renameCollection(itemId, newName);
            }
        } else {
            HsMenuService::createCollection(newName);
        }
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionNameState::onEntry");
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
