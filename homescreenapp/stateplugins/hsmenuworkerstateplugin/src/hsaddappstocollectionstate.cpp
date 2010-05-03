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

#include <qfinalstate.h>
#include <QDebug>
#include <hsmenuservice.h>
#include <hbnotificationdialog.h>
#include <QAbstractTransition>

#include "hsaddappstocollectionstate.h"
#include "hsmenuevent.h"
#include "hscollectionnamedialog.h"
#include "hscollectionslistdialog.h"
#include "hsappschecklist.h"

/*!
 \class HsAddAppsToCollectionState
 \ingroup group_hsmenuworkerstateplugin
 \brief Manages adding to homescreen state
 */

/*!
 \fn void transitToSaveState(const QString &collectionName);
 Signal emitted when collection name is selected.
 \param collectionName name of collection.
 */

/*!
 \fn void transitToAppsCheckListState(const QString &collectionName);
 Signal emitted when collection name is selected - version to trigger
 transition to mAppCheckListState.
 \param collectionName name of collection.
 */

/*!
 \fn void void transitToSaveState(int collectionId);
 Signal emitted when collection id is selected.
 \param collectionId id of collection.
 */

/*!
 \fn void transitToAppsCheckListState(int collectionId);
 Signal emitted when collection name is selected - version to trigger
 transition to mAppCheckListState.
 \param collectionId id of collection.
 */

/*!
 \fn void transitToNewCollectionState();
 Signal emitted when user selects creating new collection.
 */

/*!
 \fn void transitToSaveState(const QList<int> &appList);
 Signal emitted when applications are selected.
 \param appList application list.
 */

/*!
 \fn void transitToFinalState();
 Signal emitted when user selects cancel.
 \param collectionName name of collection.
 */

/*!
 Constructor.
 \return \a true on success, \a false otherwise.
 \param parent Owner.
 */
HsAddAppsToCollectionState::HsAddAppsToCollectionState(QState *parent) :
    QState(parent), mCollectionName(0), mCollectionId(0), mAppList(),
    mShowConfirmation(0), mInitialState(0), mSelectCollectionState(0),
    mNewCollectionState(0), mAppsCheckListState(0), mActionType(
        NoActionType), mApplicationsSortAttribute(NoHsSortAttribute),
    mCollectionsSortAttribute(NoHsSortAttribute), mAppsCheckList(0)
{
    construct();
}

/*!
 Destructor.

 */
HsAddAppsToCollectionState::~HsAddAppsToCollectionState()
{
    delete mAppsCheckList;
}

/*!
 Constructor part.
 */
void HsAddAppsToCollectionState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::construct");
    setObjectName("homescreen.nokia.com/state/addappstocollectionstate");
    createStates();
    connect(this, SIGNAL(exited()),SLOT(stateExited()));

    mAppsCheckList = new HsAppsCheckList();
    connect(mAppsCheckList, SIGNAL(commit(QList<int>)),
            SLOT(selectApplicationsDone(QList<int>)));

    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::construct");
}

/*!
 Creates behavioural model i.e states
 */
void HsAddAppsToCollectionState::createStates()
{
    //child states
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::createStates");

    mInitialState = new QState(this);
    QState *saveState = new QState(this);
    QFinalState *finalState = new QFinalState(this);

    mSelectCollectionState = new QState(this);
    mNewCollectionState = new QState(this);
    mAppsCheckListState = new QState(this);

    setInitialState(mInitialState);
    connect(saveState, SIGNAL(entered()),SLOT(save()));
    connect(finalState, SIGNAL(entered()),SLOT(cleanData()));
    connect(mNewCollectionState, SIGNAL(entered()),SLOT(newCollection()));
    connect(mSelectCollectionState, SIGNAL(entered()),
            SLOT(selectCollection()));
    connect(mAppsCheckListState, SIGNAL(entered()),
            SLOT(appsCheckListState()));

    //child states will gather needed data in parent sate
    //with signals and slots communication
    connect(this, SIGNAL(transitToSaveState(int)),
            SLOT(setCollectionId(int)));
    connect(this, SIGNAL(transitToSaveState(QString)),
            SLOT(setCollectionName(QString)));
    connect(this, SIGNAL(transitToAppsCheckListState(int)),
            SLOT(setCollectionId(int)));
    connect(this, SIGNAL(transitToAppsCheckListState(QString)),
            SLOT(setCollectionName(QString)));
    connect(this, SIGNAL(transitToSaveState(QList<int>)),
            SLOT(setAppList(QList<int>)));

    // Add a specific application to an existing collection
    // via item specific menu.
    // Add a specific item to a collection via collection specific menu.
    // Adding a new collection via the Collections view.
    mSelectCollectionState->addTransition(this,
    		SIGNAL(transitToNewCollectionState()), mNewCollectionState);
    mSelectCollectionState->addTransition(this,
    		SIGNAL(transitToSaveState(int)), saveState);
    mNewCollectionState->addTransition(this,
    		SIGNAL(transitToSaveState(QString)), saveState);

    // Add one/many applications to an existing/a new collection
    // via the All view options menu.
    // Add items to a collection via the collection's view options menu.
    mSelectCollectionState->addTransition(this,
    		SIGNAL(transitToAppsCheckListState(int)), mAppsCheckListState);
    mNewCollectionState->addTransition(this,
    		SIGNAL(transitToAppsCheckListState(QString)), mAppsCheckListState);
    mAppsCheckListState->addTransition(this,
    		SIGNAL(transitToSaveState(QList<int>)), saveState);

    mSelectCollectionState->addTransition(this,
    		SIGNAL(transitToFinalState()),finalState);
    mNewCollectionState->addTransition(this,
    		SIGNAL(transitToFinalState()), finalState);
    mAppsCheckListState->addTransition(this,
    		SIGNAL(transitToFinalState()), finalState);

    //transition to final state after save
    saveState->addTransition(finalState);
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::createStates");

}

/*!
 Saves data in content arsenal(selected applications in collection).
 */
void HsAddAppsToCollectionState::save()
{
    qDebug("AddAppsToCollectionState::save() - slot");
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::save");
    //Adds new collection.
    if (!mCollectionName.isEmpty()) {
        mCollectionId = HsMenuService::createCollection(mCollectionName);
    }
    //Adds applications to colection.
    if ((mCollectionId > 0) && mAppList.count()) {
        HsMenuService::addApplicationsToCollection(mAppList, mCollectionId);
        if (mShowConfirmation) {
            showMessageAppsAdded(mCollectionId);
        }
    }
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::save");
}

/*!
 Cleans data after cancel() signal.
 */
void HsAddAppsToCollectionState::cleanData()
{
    qDebug("AddAppsToCollectionState::cleanData() - slot");
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::cleanData");
    mCollectionId = 0;
    mAppList.clear();
    mCollectionName.clear();
    mShowConfirmation = false;
    mActionType = NoActionType;
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::cleanData");
}

/*!
 Derived from QState.
 Method invoked when a state is entered.
 \param event an event causing the entrance the state.
 */
void HsAddAppsToCollectionState::onEntry(QEvent *event)
{
    qDebug("HsAddAppsToCollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mApplicationsSortAttribute = static_cast<HsSortAttribute>(data.value(
                                     appSortOrderKey()).toInt());
    mCollectionsSortAttribute = static_cast<HsSortAttribute>(data.value(
                                    collectionSortOrderKey()).toInt());

    const int itemId = data.value(itemIdKey()).toInt();
    mCollectionId = data.value(collectionIdKey()).toInt();

    if (itemId && (mCollectionsSortAttribute != NoHsSortAttribute
                   || mCollectionId)) {
        //add selected app item form allAppView or collectionView
        mAppList.append(itemId);

        mInitialState->addTransition(mSelectCollectionState);

        mShowConfirmation = true;
    } else if (mApplicationsSortAttribute != NoHsSortAttribute
               && mCollectionsSortAttribute != NoHsSortAttribute) {
        //add apps from allAppView options menu
        mActionType = ViaAllViewOptionMenuType;

        mInitialState->addTransition(mSelectCollectionState);

        mShowConfirmation = true;
    } else if (mCollectionId) {
        //add apps from collectionView options menu
        mApplicationsSortAttribute = AscendingNameHsSortAttribute;
        mInitialState->addTransition(mAppsCheckListState);
    }
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::onEntry");
}

/*!
 Slot invoked when a state is exited.
 */
void HsAddAppsToCollectionState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::stateExited");
    QList<QAbstractTransition *> transitionsList =
        mInitialState->findChildren<QAbstractTransition *> ();
    if (transitionsList.count()) {
        mInitialState->removeTransition(transitionsList[0]);
    }
    mAppsCheckList->cleanUp();
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::stateExited");
    qDebug("AddAppsToCollectionState::stateExited()");
}

/*!
 Slot invoked when new collection state is entered..
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAddAppsToCollectionState::newCollection()
{
    qDebug("HsAddAppsToCollectionState::newCollection");

    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::newCollection");
    HsCollectionNameDialog editor;
    HbAction *result = editor.exec();
    if (result == editor.primaryAction()) {
        QString newName(editor.newName(editor.value().toString(), true));
        if (mActionType == ViaAllViewOptionMenuType) {
            qDebug("HsAddAppsToCollectionState::newCollection() "
                   "- emit collectionNameSelectedCl(newName)");
            emit transitToAppsCheckListState(newName);
        } else {
            qDebug("HsAddAppsToCollectionState::newCollection() "
                   "- emit collectionNameSelected(newName)");
            emit transitToSaveState(newName);
        }
    } else {
        qDebug(
            "HsAddAppsToCollectionState::newCollection() - emit cancel()");
        emit transitToFinalState();
    }
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::newCollection");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Slot invoked when select collection state is entered.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAddAppsToCollectionState::selectCollection()
{
    qDebug("HsAddAppsToCollectionState::selectCollection()");

    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::selectCollection");
    HsCollectionsListDialog listView(mCollectionsSortAttribute,
                                     mCollectionId);
    if (listView.exec() != listView.secondaryAction()) {
        int itemId = listView.getItemId();
        if (itemId) {
            if (mActionType == ViaAllViewOptionMenuType) {
                qDebug("emit collectionSelectedCl(%d)", itemId);
                emit transitToAppsCheckListState(itemId);
            } else {
                qDebug("emit collectionSelected(%d)", itemId);
                emit transitToSaveState(itemId);
            }
        } else {
            qDebug("emit createNewCollection()");
            emit transitToNewCollectionState();
        }
    } else {
        qDebug("emit cancel()");
        emit transitToFinalState();
    }
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::selectCollection");
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Slot connected to saving action of state.
 It is called when new application are addend to collection.
 \param id Id of collection to which applications were added.
 */
void HsAddAppsToCollectionState::showMessageAppsAdded(int id)
{
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::showMessageAppsAdded");
    HbNotificationDialog *notificationDialog = new HbNotificationDialog();
    notificationDialog->setAttribute(Qt::WA_DeleteOnClose);
    notificationDialog->setTitle(hbTrId(
                                     "txt_applib_dpopinfo_added_to_collection_1") .arg(
                                     HsMenuService::getName(id)));
    notificationDialog->show();
    mShowConfirmation = false;
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::showMessageAppsAdded");
}

/*!
 Slot setting collection name.
 \param collectionName name of collection.
 */
void HsAddAppsToCollectionState::setCollectionName(
    const QString &collectionName)
{
    qDebug() << "AddAppsToCollectionState::setCollectionName("
             << collectionName << ") - slot";

    mCollectionName = collectionName;
}

/*!
 Slot setting collection id..
 \param collectionId id of collection.
 */
void HsAddAppsToCollectionState::setCollectionId(int collectionId)
{
    qDebug() << "AddAppsToCollectionState::setCollectionId("
             << collectionId << ") - slot";

    mCollectionId = collectionId;
}

/*!
 Slot sets app list.
 \param appList list of applications ids.
 */
void HsAddAppsToCollectionState::setAppList(const QList<int> &appList)
{
    qDebug() << "AddAppsToCollectionState::setAppList(" << appList
             << ") - slot";

    mAppList = appList;
}

/*!
 Slot invoked when apps checklist state is entered.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAddAppsToCollectionState::appsCheckListState()
{
    qDebug("HsAddAppsToCollectionState::appsCheckListState()");
    HSMENUTEST_FUNC_ENTRY("HsAddAppsToCollectionState::appsCheckListState");
    if (!mAppsCheckList) {
        mAppsCheckList = new HsAppsCheckList();
    }
    mAppsCheckList->setSortOrder(mApplicationsSortAttribute);
    mAppsCheckList->showAppsCheckboxList(mApplicationsSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAddAppsToCollectionState::appsCheckListState");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Slot connected trrigger action of secondary soft key of check list box.
 It is called when done button is selected.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAddAppsToCollectionState::selectApplicationsDone(
    const QList<int> &appList)
{
    if (appList.count()) {
        qDebug("AppsCheckListState::selectApplicationsDone() "
               "- emit appsSelected(mAppList)");
        emit transitToSaveState(appList);
    } else {
        qDebug(
            "AppsCheckListState::selectApplicationsDone() - emit cancel()");
        emit transitToFinalState();
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
