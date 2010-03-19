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

#include <QDebug>
#include <hblistwidget.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hsmenuservice.h>
#include <hblistwidgetitem.h>
#include <hbinstance.h>

#include "hsarrangestate.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"

/*!
 \class HsArrangeState
 \ingroup group_hsmenustateplugin
 \brief Application Library State.
 State responsible for arrange collection's entries.
 */

/*!
 Constructor
 \param parent owner
 \retval void
 */
HsArrangeState::HsArrangeState(QState *parent) :
    QState(parent), mDialog(0), mEntriesList(0), mItemModel(0)
{
    construct();
}

/*!
 Destructor
 \retval void
 */
HsArrangeState::~HsArrangeState()
{
    if (mEntriesList) {
        delete mEntriesList;
    }
    if (mDialog) {
        delete mDialog;
    }
    if (mItemModel) {
        delete mItemModel;
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsArrangeState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::construct");
    setObjectName("homescreen.nokia.com/state/arrangestate");
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    HSMENUTEST_FUNC_EXIT("HsArrangeState::construct");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsArrangeState::save()
{
    qDebug("HsArrangeState::save()");
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::save");

    getArrangedEntriesIds();
    if (mArrangedCollIdList.count() == mCollIdList.count()) {
        for (int i(0); i < mArrangedCollIdList.count(); i++) {
            if (mArrangedCollIdList.at(i) != mCollIdList.at(i)) {
                HsMenuService::organizeCollection(
                    HsMenuService::allCollectionsId(), mArrangedCollIdList);
                break;
            }
        }
    }

    HSMENUTEST_FUNC_EXIT("HsArrangeState::save");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsArrangeState::onEntry(QEvent *event)
{
    qDebug("HsArrangeState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mTopItemId = data.value(itemIdKey()).toInt();

    mItemModel = HsMenuService::getAllCollectionsModel();

    mEntriesList = new HbListWidget();
    fulfillEntriesList();

    mEntriesList->scrollTo(mTopModelIndex,
                           HbAbstractItemView::PositionAtTop);
    mEntriesList->setArrangeMode(true);

    mDialog = new HbDialog();
    mDialog->setDismissPolicy(HbPopup::NoDismiss);
    mDialog->setTimeout(HbPopup::NoTimeout);

    mDialog->setPreferredSize(
        HbInstance::instance()->allMainWindows().at(0)->size());

    HbLabel *label = new HbLabel(hbTrId("txt_applib_title_arrange"));
    mDialog->setHeadingWidget(label);
    mDialog->setContentWidget(mEntriesList);

    mDialog->setPrimaryAction(new HbAction(hbTrId("txt_common_button_ok"),
                                           mDialog));

    HbAction *result = mDialog->exec();
    mEntriesList->setArrangeMode(false);
    if (result == mDialog->primaryAction()) {
        save();
    }
    delete result;
    result = NULL;

    HSMENUTEST_FUNC_EXIT("HsArrangeState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsArrangeState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::stateExited");

    mDialog->setHeadingWidget(0); //delete label
    mDialog->setContentWidget(0); //delete mEntriesList
    mEntriesList = NULL;
    delete mDialog;
    mDialog = NULL;
    delete mItemModel;
    mItemModel = NULL;

    mArrangedCollIdList.clear();
    mCollIdList.clear();

    HSMENUTEST_FUNC_EXIT("HsArrangeState::stateExited");
    qDebug("HsArrangeState::stateExited()");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsArrangeState::fulfillEntriesList()
{
    qDebug() << "HsArrangeState::fulfillEntriesList";

    mTopModelIndex = QModelIndex();
    for (int i(0); i < mItemModel->rowCount(); i++) {
        QModelIndex idx = mItemModel->index(i);
        HbListWidgetItem *widgetItem = new HbListWidgetItem();
        int itemId = mItemModel->data(idx, CaItemModel::IdRole).toInt();
        widgetItem->setData(mItemModel->data(idx, CaItemModel::IdRole),
                            CaItemModel::IdRole);
        mCollIdList.append(itemId);
        widgetItem->setData(mItemModel->data(idx, Qt::DisplayRole),
                            Qt::DisplayRole);
        widgetItem->setData(mItemModel->data(idx, Qt::DecorationRole),
                            Qt::DecorationRole);

        mEntriesList->addItem(widgetItem);
        if (mTopItemId == itemId) {
            mEntriesList->indexCount();
            mEntriesList->setCurrentItem(widgetItem);
            mTopModelIndex = mEntriesList->currentIndex();
        }
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsArrangeState::getArrangedEntriesIds()
{
    for (int i(0); i < mEntriesList->count(); i++) {
        HbListWidgetItem *widgetItem = mEntriesList->item(i);
        QVariant entryId = widgetItem->data(CaItemModel::IdRole);
        mArrangedCollIdList.append(entryId.toInt());
    }
}
