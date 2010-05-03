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
#include "hsapp_defs.h"
#include "hsmenuviewbuilder.h"


class QPointF;
class QActionGroup;
class HbView;
class HbMainWindow;
class HbAction;
class HbAbstractItemView;
class HbGroupBox;
class HbWidget;
class HsMenuItemModel;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuView: public QObject
{
    Q_OBJECT

public:
    HsMenuView();
    ~HsMenuView();

    void setModel(HsMenuItemModel *model);
    void setModelToView(QAbstractItemModel *model);

    void setLabel(const QString &label);
    void scrollTo(const QModelIndex &index,
                  HbAbstractItemView::ScrollHint hint =
                      HbAbstractItemView::EnsureVisible);

    void setLabelVisible(bool visible);
    void setSearchPanelVisible(bool visible);

    HbView *view();
    QActionGroup *toolBarActionGroup();
    HbAction *allAppsAction();
    HbAction *allCollectionsAction();
    HbAction *oviStoreAction();

    QModelIndex firstVisibleItemIndex();

    HsMenuMode getHsMenuMode();

    void setHsMenuMode(HsMenuMode menuMode);
    void hideToolBar();
    void showToolBar();
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


private:
    HbAbstractItemView::ScrollHint convertScrollHint(
        QAbstractItemView::ScrollHint hint);

    void connectItemViewsSignals();
    void disconnectItemViewsSignals();

    void connectSearchItemViewsSignals();
    void disconnectSearchItemViewsSignals();


    void connectModelSignals();
    void disconnectModelSignals();

    void connectSearchPanelSignals();
    void disconnectSearchPanelSignals();

    void searchFinished();
    void searchBegins();

    void setUpToolBar();

private:

    HsMenuViewBuilder mBuilder;

    HsMenuItemModel *mModel;

    QModelIndex mFirstVisibleIndex;
    QModelIndex mLastVisibleIndex;
    QModelIndex mSearchIndex;

    QActionGroup *mToolBarActionGroup;

    HsMenuMode mMenuMode;

    QSortFilterProxyModel *mProxyModel; // owned

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};


#endif // HSMENUVIEW_H
