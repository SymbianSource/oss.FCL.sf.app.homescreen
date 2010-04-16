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

#ifndef ARRANGE_H
#define ARRANGE_H

#include <QState>
#include <QVariant>
#include <hsmenuservice.h>
#include <QAbstractItemModel>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HbListWidget;
class HsMenuItemModel;
class HbDialog;

class HsArrangeState: public QState
{
    Q_OBJECT

    // Friend classes
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsArrangeState(QState *parent = 0);

    virtual ~HsArrangeState();

protected:

    /**
     * Derived from QState.
     * Method invoked when a state is entered.
     * @param event an event causing the entrance the state.
     *
     * @since S60 ?S60_version.
     */
    void onEntry(QEvent *event);

    /**
     * Slot connected to saving action of state.
     * It is called when new application are addend to collection.
     *
     * @since S60 ?S60_version.
     * @param id Id of collection to which applications were added.
     */
    //    void showMessageAppsAdded(int id);

private slots:

    /**
     * Slot invoked when a state is exited.
     *
     * @since S60 ?S60_version.
     */
    void stateExited();

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();

    void fulfillEntriesList(HbListWidget& listWidget);

    void getArrangedEntriesIds(const HbListWidget& listWidget);

    void save(const HbListWidget& listWidget);

private:

    /**
     * Dialog consists of collection's entries.
     */
    HbDialog *mDialog;

    /**
     * Entries list collected from DB.
     */
    HbListWidget *mEntriesList;

    /**
     * Collection name.W
     */
    HsMenuItemModel *mItemModel;

    /**
     * Item id visible on top.
     */
    int mTopItemId;

    /*
     * ModelIndex visible on top.
     */
    QModelIndex mTopModelIndex;

    /**
     * Collection list.
     */
    QList<int> mCollIdList;

    /**
     * Collection list.
     */
    QList<int> mArrangedCollIdList;
    
    QObjectList mObjectList;

};

#endif /* ARRANGE_H_ */
