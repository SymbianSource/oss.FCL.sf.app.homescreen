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
 * Description: Menu search.
 *
 */

#include <qdebug.h>
#include "hsmenuitemmodel.h"
#include "hsmenuview.h"
#include "hsmenusearch.h"

/*!
    \class Menu Search
    \ingroup group_hsmenustateplugin

    \brief Menu View

    Provides handler for \a criteriaChanged signal emited
    by menu view search panel

    \sa HsMenuView
*/

/*!
 Constructor
 \param menuView Menu view the object is doing search job for.
 */
HsMenuSearch::HsMenuSearch(HsMenuView *menuView): mMenuView(menuView)
{
}

/*!
 Looks up for item and if found scrolls to it.
 \param criteriaStr The item name to find.
 */
void HsMenuSearch::findItem(QString criteriaStr)
{
    qDebug() << QString("HsMenuSearch::findItem: %1").arg(criteriaStr);
    HSMENUTEST_FUNC_ENTRY("HsMenuSearch::findItem");

    if ("" != criteriaStr) {
        const HsMenuItemModel *const model = mMenuView->model();

        const QModelIndexList list =
            model->match(
                model->index(0, 0),
                CaItemModel::TextRole,
                criteriaStr);

        if (list.count() > 0) {
            qDebug() << QString("HsMenuSearch::findItem:row %1"). arg(
                         list.at(0).row());

            mMenuView->scrollToWithoutHidingSearchPanel(
                list.at(0),
                HbAbstractItemView::PositionAtTop);
        }
    }

    HSMENUTEST_FUNC_EXIT("HsMenuSearch::findItem");
}
