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
 * Description: Menu view.
 *
 */

#ifndef HSMENUVIEW_H
#define HSMENUVIEW_H

#include <QObject>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QSortFilterProxyModel>
#include <HbAbstractItemView>
#include "hsmenustates_global.h"
#include "hsmenuviewbuilder.h"
#include "hsapp_defs.h"

class QPointF;
class QActionGroup;
class HbView;
class HbMainWindow;
class HbAction;
class HbAbstractItemView;
class HbListView;
class HbGroupBox;
class HbWidget;
class HbStaticVkbHost;
class HsMenuItemModel;


HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuView: public QObject
{
    Q_OBJECT

public:
    HsMenuView(HsMenuViewBuilder &builder, HsViewContext viewContext);
    ~HsMenuView();

    void setSearchPanelVisible(bool visible);

    HbView *view() const;

    HbListView *listView() const;

    void activate();
    void inactivate();

    HbGroupBox *viewLabel() const;

    void setModel(HsMenuItemModel *model);


signals:
    void activated(const QModelIndex &index);
    void longPressed(HbAbstractViewItem *item, const QPointF &coords);

public slots:

    void activatedProxySlot(const QModelIndex &index);
    void longPressedProxySlot(HbAbstractViewItem *item,
                              const QPointF &coords);

    void showSearchPanel();
    void hideSearchPanel();


private slots:
    void scrollToRow(int row,
                     QAbstractItemView::ScrollHint hint =
                         QAbstractItemView::PositionAtTop);
    void findItem(QString criteriaStr);

    void vkbOpened();
    void vkbClosed();

private:

    QModelIndex firstVisibleItemIndex(const HbListView *view) const;

    void connectSearchPanelSignals();
    void disconnectSearchPanelSignals();
    void connectSearchItemViewsSignals();
    void disconnectSearchItemViewsSignals();

    void searchFinished();
    void searchBegins();
    HbAbstractItemView::ScrollHint convertScrollHint(
        QAbstractItemView::ScrollHint hint);

private:

    const HsViewContext mViewContext;
    HsMenuViewBuilder &mBuilder;

    QModelIndex mSearchViewInitialIndex;
    QModelIndex mIndexToScrollAfterSearchDone;
    QModelIndex mSearchViewLongPressedIndex;

    QSortFilterProxyModel *mProxyModel; // owned

    HbView *mView;
    HbListView *mListView;
    HbGroupBox *mViewLabel;

    HbListView *mSearchListView;
    HbSearchPanel *mSearchPanel;
    HbStaticVkbHost* mVkbHost;

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};


#endif // HSMENUVIEW_H
