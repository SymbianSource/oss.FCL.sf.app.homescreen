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

#ifndef ADDAPPSTOCOLLECTIONSTATE_H
#define ADDAPPSTOCOLLECTIONSTATE_H

#include <QState>
#include <QVariant>
#include <hsmenuservice.h>
#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class QStandardItemModel;
class HsAppsCheckList;

/**
 * @ingroup group_hsworkerstateplugin
 * @brief Application Library State.
 *
 * State responsible for adding new applications to collections.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsAddAppsToCollectionState: public QState
{
    Q_OBJECT

    // Friend classes
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    /**
     * Types of collection's related actions.
     */
    enum CollectionActionType {
        // No action
        NoActionType,
        // Adding a specific application to an existing collection via item specific menu
        ViaItemSpecificMenuType = 1000,
        // Add one/many applications to a new/an existing collection via the All view
        ViaAllViewOptionMenuType,
        // Adding a new collection via the Collections view
        ViaAllCollectionsViewType,
        // Add items to a collection via the collection's view options meu
        ViaCollectionsViewOptionsMenuType,
        // Add a specific item to a collection via collection specific menu
        ViaCollectionSpecificMenuType
    };

    HsAddAppsToCollectionState(QState *parent = 0);
    virtual ~HsAddAppsToCollectionState();

signals:

    /**
     * Signal emitted when collection name is selected.
     *
     * @since S60 ?S60_version.
     * @param collectionName name of collection.
     */
    void transitToSaveState(const QString &collectionName);

    /**
     * Signal emitted when collection name is selected - version to trigger
     * transition to mAppCheckListState.
     *
     * @since S60 ?S60_version.
     * @param collectionName name of collection.
     */
    void transitToAppsCheckListState(const QString &collectionName);

    /**
     * Signal emitted when collection id is selected.
     *
     * @since S60 ?S60_version.
     * @param collectionId id of collection.
     */
    void transitToSaveState(int collectionId);

    /**
     * Signal emitted when collection id is selected - version to trigger
     * transition to mAppCheckListState.
     *
     * @since S60 ?S60_version.
     * @param collectionId id of collection.
     */
    void transitToAppsCheckListState(int collectionId);

    /**
     * Signal emitted when user selects creating new collection.
     *
     * @since S60 ?S60_version.
     */
    void transitToNewCollectionState();

    /**
     * Signal emitted when applications are selected.
     *
     * @param appList application list.
     * @since S60 ?S60_version.
     */
    void transitToSaveState(const QList<int> &appList);

    /**
     * Signal emitted when user selects cancel.
     *
     * @since S60 ?S60_version.
     */
    void transitToFinalState();

protected:

    void onEntry(QEvent *event);

    void showMessageAppsAdded(int id);

protected slots:

    void setCollectionName(const QString &collectionName);

    void setCollectionId(int collectionId);

    void setAppList(const QList<int> &appList);

    void save();

    void cleanData();

    void selectApplicationsDone(const QList<int> &appList);

private slots:

    void stateExited();

    void newCollection();

    void selectCollection();

    void appsCheckListState();

private:

    void construct();

    void createStates();

private:

    /**
     * Collection name.
     */
    QString mCollectionName;

    /**
     * Collection id.
     */
    int mCollectionId;

    /**
     * Applications list.
     */
    QList<int> mAppList;

    /**
     * Boll indicating need of confirmation note after saving in content arsenal.
     */
    bool mShowConfirmation;

    /**
     * Initial state.
     */
    QState *mInitialState;

    /**
     * Select collection state.
     */
    QState *mSelectCollectionState;

    /**
     * Collection name state.
     */
    QState *mNewCollectionState;

    /**
     * Collection name state.
     */
    QState *mAppsCheckListState;

    /**
     * Collection action type.
     */
    CollectionActionType mActionType;

    /**
     * Applications sort order.
     */
    HsSortAttribute mApplicationsSortAttribute;

    /**
     * Collections sort order.
     */
    HsSortAttribute mCollectionsSortAttribute;

    /**
     * Applications check list.
     */
    HsAppsCheckList *mAppsCheckList;

};

#endif /* ADDAPPSTOCOLLECTIONSTATE_H */
