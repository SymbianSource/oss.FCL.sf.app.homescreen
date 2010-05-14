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

#include <hbaction.h>
#include <hbinstance.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbdocumentloader.h>
#include <hsmenuservice.h>
#include <hsmenuitemmodel.h>

#include "hsappschecklist.h"

/*!
 \class HsAppsCheckList
 \ingroup group_hsworkerstateplugin
 \brief Application Library State.
 State responsible for selecting a group of applications to be added to
 a collection.
 */

/*!
 \fn void HsAppsCheckList::commit(const QList<int> &appList);
 Signal emitted when applications are selected.
 \param appList application list.
 */

/*!
 \var HsAppsCheckList::mView
 View. Owned.
 */

/*!
 \var HsAppsCheckList::mPreviousView
 Previous view. Not owned.
 */

/*!
 \var HsAppsCheckList::mActionConfirm
 Confirm action. Owned.
 */

/*!
 \var HsAppsCheckList::mListView
 List view. Owned.
 */

/*!
 \var HsAppsCheckList::mModel
 Standard Item Model for the List.
 Own.
 */

/*!
 \var HsAppsCheckList::mSortAttribute
 Sort order for applications.
 */

/*!
 \var HsAppsCheckList::mLoadedObjects
 List of objects loaded by document loader.
 Content owned.
 */

/*!
 Constructor
 \retval void
 */
HsAppsCheckList::HsAppsCheckList() :
    mView(0), mPreviousView(0), mActionConfirm(0), mListView(0), mModel(0),
    mSortAttribute(AscendingNameHsSortAttribute)
{
}

/*!
 Destructor
 */
HsAppsCheckList::~HsAppsCheckList()
{
    cleanUp();
}

/*!
 Cleans data
 \retval void
 */
void HsAppsCheckList::cleanUp()
{
    //clean up

    qDeleteAll(mLoadedObjects);
    mLoadedObjects.clear();
    mListView = NULL;
    mView = NULL;

    delete mActionConfirm;
    mActionConfirm = NULL;
    delete mModel;
    mModel = NULL;
    mSortAttribute = AscendingNameHsSortAttribute;
}

/*!
 Sets sort order for applications.
 \param sortAttribute sort order.
 */
void HsAppsCheckList::setSortOrder(HsSortAttribute sortOrder)
{
    mSortAttribute = sortOrder;
}

/*!
 Shows check box list with all application.
 \param sortAttribute order to sort applications.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAppsCheckList::showAppsCheckboxList(HsSortAttribute sortOrder)
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::showAppsCheckboxList");
    if (!mModel) {
        mModel = HsMenuService::getAllApplicationsModel(sortOrder);
    }
    // fills model with data
    // constucts controls such as checklist
    constructControls();

    // Add mListView to main window
    HbMainWindow *hbMainWindow = mainWindow();

    // add confirm action
    mView->setNavigationAction(mActionConfirm);

    hbMainWindow->addView(mView);
    // record the current view in order to activate it once done
    mPreviousView = hbMainWindow->currentView();
    hbMainWindow->setCurrentView(mView);
    hbMainWindow->show();
    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::showAppsCheckboxList");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Construct contrlos.
 */
void HsAppsCheckList::constructControls()
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::constructControls");
    if (!mActionConfirm) {
        mActionConfirm = new HbAction(Hb::ConfirmNaviAction, mView);
        connect(mActionConfirm, SIGNAL(triggered()),SLOT(selectApplicationsDone()));
    }

    if (!mView) { // it implies that mListView is NULL as well
        bool loadStatusOk = false;

        HbDocumentLoader loader;
        mLoadedObjects = loader.load(HS_APP_CHECK_LIST_LAYOUT, &loadStatusOk);

        Q_ASSERT_X(loadStatusOk,
                    HS_APP_CHECK_LIST_LAYOUT,
                   "Error while loading docml file.");

        static const QString VIEW_WIDGET_NAME("view");
        mView = qobject_cast<HbView *> (loader.findWidget(VIEW_WIDGET_NAME));
        mView->setParent(this);

        static const QString LIST_VIEW_WIDGET_NAME("listView");
        mListView = qobject_cast<HbListView *> (loader.findWidget(
                LIST_VIEW_WIDGET_NAME));
        mListView->setModel(mModel);
    }
    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::constructControls");
}

/*!
 Slot connected trrigger action of secondary soft key of check list box.
 It is called when done button is selected.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAppsCheckList::selectApplicationsDone()
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::selectApplicationsDone");
    // Remove mListView from main window and restore previous view.
    HbMainWindow *hbMainWindow = mainWindow();
    hbMainWindow->setCurrentView(mPreviousView);
    hbMainWindow->removeView(mView);
    
    QItemSelectionModel *itemSelectionModel = mListView->selectionModel();
    QList<int> itemsList;
    if (itemSelectionModel) {
        QModelIndexList modelIndexList =
            itemSelectionModel->selectedIndexes();
        itemsList = getSortedItemsList(modelIndexList);
    }
    emit commit(itemsList);
    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::selectApplicationsDone");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Returns list with selected items ids.
 \param modelIndexList list with selected model indexes.
 \return list with items ids.
 */
QList<int> HsAppsCheckList::getSortedItemsList(
    const QModelIndexList &modelIndexList)
{
    QMap<QString, int> itemsMap;

    foreach(QModelIndex index, modelIndexList) {
        int itemId = index.data(CaItemModel::IdRole).toInt();
        QString itemName(index.data(Qt::DisplayRole).toString());
        if (itemName.isNull()) {
            itemName
            = index.data(Qt::DisplayRole).toList()[0].toString();
        }
        itemsMap.insertMulti(itemName.toLower(), itemId);
    }

    QList<int> list = itemsMap.values();
    if (mSortAttribute == DescendingNameHsSortAttribute) {
        QList<int> reversedList;
        int count = list.count();
        for (int i = 0; i < count; i++) {
            reversedList.append(list.takeLast());
        }
        list = reversedList;
    }
    return list;
}

/*!
 Returns pointer to tha main window.
 \return Pointer to the main window.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
HbMainWindow *HsAppsCheckList::mainWindow() const
{
    return HbInstance::instance()->allMainWindows().value(0);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
