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
* Description:
*
*/
#include "tstaskpopuphandler.h"

#include <hbmenu.h>
#include <hbabstractviewitem.h>

#include "tsdataroles.h"

TsTaskPopupHandler::TsTaskPopupHandler(QObject *parent) : QObject(parent)
{
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void TsTaskPopupHandler::showTaskPopup(HbAbstractViewItem *item, const QPointF &coords)
{
    QModelIndex itemIndex(item->modelIndex());

    //initialize popup menu
    HbMenu taskPopup;
    taskPopup.setTimeout(0);
    taskPopup.setDismissPolicy(HbPopup::TapOutside);

    HbAction *closeAction = NULL;
    HbAction *closeAllAction = NULL;

    if (isClosable(itemIndex)) {
        closeAction = taskPopup.addAction(
                hbTrId("txt_tsw_menu_clear_this_task"));
    }
    if (closeAction || containsClosableItems(*itemIndex.model())) {
        closeAllAction = taskPopup.addAction(
                hbTrId("txt_tsw_menu_clear_all_tasks"));
    }

    if (closeAllAction || closeAction) {
        // show popup
        HbAction *selectionAction = taskPopup.exec(coords);

        //check selection and emit right signal
        if (selectionAction == closeAction) {
            emit closeTask(itemIndex);
        } else if (selectionAction == closeAllAction) {
            emit closeAllTasks();
        }
    }

    //release resources
    taskPopup.close();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

bool TsTaskPopupHandler::isClosable(const QModelIndex &index) const
{
    const QVariant state(index.data(TsDataRoles::Closable));
    return state.isValid() && state.toBool();
}

bool TsTaskPopupHandler::containsClosableItems(const QAbstractItemModel &model) const
{
    bool retVal(false);
    for (int row(0); !retVal && row < model.rowCount(); ++row) {
        for (int col(0); !retVal && col < model.columnCount(); ++col) {
            retVal = isClosable(model.index(row, col));
        }
    }
    return retVal;
}
