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
#include <HbDocumentLoader>
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
 \ingroup group_hsworkerstateplugin
 \brief Application Library State.
 State responsible for arrange collection's entries.
 */

/*!
 \var HsArrangeState::mDialog
 Dialog consists of collection's entries.
 */

/*!
 \var HsArrangeState::mEntriesList
 Entries list collected from DB.
 */

/*!
 \var HsArrangeState::mItemModel
 Collection name.
 */

/*!
 \var HsArrangeState::mTopItemId
 Item id visible on top.
 */

/*!
 \var HsArrangeState::mTopModelIndex
 ModelIndex visible on top.
 */

/*!
 \var HsArrangeState::mCollIdList
 Collection list.
 */

/*!
 \var HsArrangeState::mArrangedCollIdList
 Collection list.
 */

/*!
 Constructor
 \param parent owner
 \retval void
 */
HsArrangeState::HsArrangeState(QState *parent) :
    QState(parent), mDialog(0), mEntriesList(0), 
    mItemModel(0)
{
    construct();
}

/*!
 Destructor
 \retval void
 */
HsArrangeState::~HsArrangeState()
{
    // in case of throw
    if (mDialog) {
        mDialog->close();
    }
    if (mItemModel) {
        delete mItemModel;
    }
}

/*!
 Constructs contained objects.
 */
void HsArrangeState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::construct");
    setObjectName("homescreen.nokia.com/state/arrangestate");
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    HSMENUTEST_FUNC_EXIT("HsArrangeState::construct");
}

/*!
  Send data to storage (selected applications in collection).
  \param listWidget List of items to be stored. 
 */
void HsArrangeState::save(const HbListWidget& listWidget)
{
    qDebug("HsArrangeState::save()");
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::save");

    getArrangedEntriesIds(listWidget);
    if (mArrangedCollIdList != mCollIdList) {
        HsMenuService::organizeCollection(
            HsMenuService::allCollectionsId(), mArrangedCollIdList);
    }

    HSMENUTEST_FUNC_EXIT("HsArrangeState::save");
}

/*!
 Derived from QState.
 Method invoked when a state is entered.
 \param event an event causing the entrance the state.
 */
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

    HbDocumentLoader loader;
    bool loadStatusOk = false;
    mObjectList = 
        loader.load(HS_ARRANGE_DIALOG_LAYOUT, &loadStatusOk);
    Q_ASSERT_X(loadStatusOk,
        HS_ARRANGE_DIALOG_LAYOUT,
           "Error while loading docml file.");
    
    mEntriesList = qobject_cast<HbListWidget*>(
        loader.findWidget(HS_ARRANGE_LIST_NAME));
    
    mDialog = qobject_cast<HbDialog*>(
            loader.findWidget(HS_ARRANGE_DIALOG_NAME));

    if (mEntriesList != NULL && mDialog != NULL) {

        mItemModel = HsMenuService::getAllCollectionsModel();
        // as we copy the model contents to the list widget
        // we do not need the model to auto update
        mItemModel->setAutoUpdate(false);
        
        fulfillEntriesList(*mEntriesList);
    
        mEntriesList->scrollTo(mTopModelIndex,
                               HbAbstractItemView::PositionAtTop);
    
        mEntriesList->setArrangeMode(true);
    
        mDialog->setTimeout(HbPopup::NoTimeout);
        mDialog->setAttribute(Qt::WA_DeleteOnClose, true);

        mDialog->setPreferredSize(
            HbInstance::instance()->allMainWindows().at(0)->size());
            
        mDialog->open(this, SLOT(arrangeDialogFinished(HbAction*)));
        
    }
    HSMENUTEST_FUNC_EXIT("HsArrangeState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Slot invoked on closing the dialog.
 */
void HsArrangeState::arrangeDialogFinished(HbAction* finishedAction)
{
    mEntriesList->setArrangeMode(false);
    if (finishedAction == mDialog->actions().value(0)) {
        save(*mEntriesList);
    }
    emit exit();
}

/*!
 Slot invoked when a state is exited.
 */
void HsArrangeState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsArrangeState::stateExited");
    if (mDialog) {
        disconnect(mDialog, SIGNAL(finished(HbAction*)), this, SLOT(arrangeDialogFinished(HbAction*)));
        mDialog->close();
        mDialog = NULL;
    }

    mObjectList.clear(); // mDialog as parent for all confml items
    
    mEntriesList = NULL;
    delete mItemModel;
    mItemModel = NULL;

    mArrangedCollIdList.clear();
    mCollIdList.clear();

    HSMENUTEST_FUNC_EXIT("HsArrangeState::stateExited");
    qDebug("HsArrangeState::stateExited()");
}


/*!
 Put items from model into arrangable list of items represented by \a listWidget
  \param listWidget Object to be filled with list of items from model.
 */
void HsArrangeState::fulfillEntriesList(HbListWidget& listWidget)
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

        listWidget.addItem(widgetItem);
        if (mTopItemId == itemId) {
            listWidget.setCurrentItem(widgetItem);
            mTopModelIndex = listWidget.currentIndex();
        }
    }
}


/*!
 Copy items in from \a listWidget to arranged collection id list
 \param listWidget List of items in the requested order.
 */
void HsArrangeState::getArrangedEntriesIds(const HbListWidget& listWidget)
{
    for (int i(0); i < listWidget.count(); ++i) {
        HbListWidgetItem *widgetItem = listWidget.item(i);
        QVariant entryId = widgetItem->data(CaItemModel::IdRole);
        mArrangedCollIdList.append(entryId.toInt());
    }
}
