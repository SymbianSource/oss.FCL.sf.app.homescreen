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
class HbAction;
class HsAppsCheckList;
class HsCollectionNameDialog;
class HsCollectionsListDialog;

class HsAddAppsToCollectionState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    enum CollectionActionType {
        NoActionType,
        ViaItemSpecificMenuType = 1000,
        ViaAllViewOptionMenuType,
        ViaAllCollectionsViewType,
        ViaCollectionsViewOptionsMenuType,
        ViaCollectionSpecificMenuType
    };

    HsAddAppsToCollectionState(QState *parent = 0);
    virtual ~HsAddAppsToCollectionState();

signals:

    void transitToSaveState(const QString &collectionName);

    void transitToAppsCheckListState(const QString &collectionName);

    void transitToSaveState(int collectionId);

    void transitToAppsCheckListState(int collectionId);

    void transitToNewCollectionState();

    void transitToSaveState(const QList<int> &appList);

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

    void editorDialogFinished(HbAction* finishedAction);

    void listDialogFinished(HbAction* finishedAction);

private:

    void construct();

    void createStates();

private:

    QString mCollectionName;

    int mCollectionId;

    QList<int> mAppList;

    bool mShowConfirmation;

    QState *mInitialState;

    QState *mSelectCollectionState;

    QState *mNewCollectionState;

    QState *mAppsCheckListState;

    CollectionActionType mActionType;

    HsSortAttribute mApplicationsSortAttribute;

    HsSortAttribute mCollectionsSortAttribute;

    HsAppsCheckList *mAppsCheckList;

    HsCollectionNameDialog *mEditorDialog;

    bool mEditorFinishedEntered;

    HsCollectionsListDialog *mListDialog;

    bool mListFinishedEntered;

};

#endif /* ADDAPPSTOCOLLECTIONSTATE_H */
