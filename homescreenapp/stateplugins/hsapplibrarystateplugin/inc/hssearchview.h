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
 * Description: Search view.
 *
 */

#ifndef HSSEARCHVIEW_H
#define HSSEARCHVIEW_H

#include <QModelIndex>
#include <QScopedPointer>

#include <HbShrinkingVkbHost>

#include "hsmenustates_global.h"
#include "hsmenuviewbuilder.h"

class QSortFilterProxyModel;
class HsMainWindow;
class HbAbstractViewItem;
class HbLineEdit;
class HbSearchPanel;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsSearchView : public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsSearchView(
            HsMenuViewBuilder &builder,
            HsStateContext stateContext,
            HsMainWindow &mainWindow);
    ~HsSearchView();

    void setSearchPanelVisible(bool visible);
    bool isActive() const;
private:

    QModelIndex firstVisibleItemIndex(const HbListView *view) const;
    void searchBegins();

    void connectSearchItemViewsSignals();
    void disconnectSearchItemViewsSignals();

    void connectSearchPanelSignals();

    void disconnectSearchPanelSignals();

    void searchFinished();

signals:
    void activated(const QModelIndex &index);
    void longPressed(HbAbstractViewItem *item, const QPointF &coords);

public slots:
    void hideSearchPanel();
    void activatedProxySlot(const QModelIndex &index);
    void longPressedProxySlot(
            HbAbstractViewItem *item, const QPointF &coords);

private slots:
    void findItem(QString criteriaStr);
private:
    void setOriginatingContext();
    void setSearchContext();
    void initSearchPanel(HbSearchPanel &searchPanel);
    HbLineEdit *searchPanelLineEdit(HbSearchPanel &searchPanel) const;

private:

    QModelIndex mSearchViewInitialIndex;
    QModelIndex mIndexToScrollAfterSearchDone;
    QModelIndex mSearchViewLongPressedIndex;
    QSortFilterProxyModel *mProxyModel; // owned
    HbView *mSearchView;
    HbListView *mSearchListView;
    HbSearchPanel *mSearchPanel;
    const HsStateContext mStateContext;
    HsOperationalContext mAfterSearchContext;
    HsMenuViewBuilder &mBuilder;
    HsMainWindow &mMainWindow;
    HbListView *mListView;
    QScopedPointer<HbShrinkingVkbHost> mVkbHost;

};

#endif /* HSSEARCHVIEW_H_ */
