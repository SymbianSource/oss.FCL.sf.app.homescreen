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
#include <QActionGroup>

#include <HbAction>
#include <HbAbstractItemView>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbListView>
#include <HbMainWindow>
#include <HbSearchPanel>
#include <HbToolBar>
#include <HbView>

#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
#include "hsovistorehandler.h"
#include "hsmenuitemmodel.h"
#include "hsmenuview.h"

/*!
    \class HsMenuView
    \ingroup group_hsmenustateplugin

    \brief Menu View.

    Provides UI for Application Library states.

    \sa HsAppLibraryState
*/

/*!
 Constructor

 Builds UI objects
 Sets up signals connections.

 \param window Main window.
 */
HsMenuView::HsMenuView():
    mBuilder(),
    mModel(NULL),
    mFirstVisibleIndex(),
    mLastVisibleIndex(),
    mToolBarActionGroup(new QActionGroup(mBuilder.toolBar())),
    mMenuMode(NormalHsMenuMode),
    mProxyModel(new QSortFilterProxyModel(this))
{
    setUpToolBar();
    connectItemViewsSignals();

    mProxyModel->setFilterRole(CaItemModel::TextRole);
    mProxyModel->setFilterKeyColumn(1);
    mProxyModel->setSortRole(CaItemModel::TextRole);
}

/*!
 Destructor

 Disconnects signals.
 */
HsMenuView::~HsMenuView()
{
    disconnectItemViewsSignals();
}

/*!
 Sets model for item view.

 \param model Model to show by item view
 */
void HsMenuView::setModel(HsMenuItemModel *model)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setModel");

    if (model != mModel) {
        if (mModel != NULL) {
            disconnectModelSignals();
        }
        mModel = model; // mModel preserves information on actual model type
    }

    mBuilder.listView()->setModel(model);
    if (model != NULL) {
        connectModelSignals();
    }
    HSMENUTEST_FUNC_EXIT("HsMenuView::setModel");
}

/*!
 Sets model for item view without connecting scroll signals

 \param model Model to show by item view
 */
void HsMenuView::setModelToView(QAbstractItemModel *model)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setModelToView");

    mBuilder.listView()->setModel(model);

    HSMENUTEST_FUNC_EXIT("HsMenuView::setModelToView");
}

/*!
    Sets label text to be shown when label is enabled.
    \param label Label text.
 */
void HsMenuView::setLabel(const QString &label)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setLabel");
    mBuilder.label()->setHeading(label);
    HSMENUTEST_FUNC_EXIT("HsMenuView::setLabel");
}

/*!
 Scrolls item view to requested item.
 If search panel is open it will be closed after the fuction call.
 \param index The index of the item the view is to be scrolled.
 \param hint Where the target item should be positioned in visible area
 after scroll.
 */
void HsMenuView::scrollTo(const QModelIndex &index,
                          HbAbstractItemView::ScrollHint hint)
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::scrollTo");

    if (index != QModelIndex()) {
        mBuilder.listView()->scrollTo(index, hint);
    }
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::scrollTo");
}


/*!
    Makes the UI to show or hide view label.
     \param visible When true label will be shown,
     otherwise it will be hidden.
 */
void HsMenuView::setLabelVisible(bool visible)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setLabelVisible");
    mBuilder.setLabelVisible(visible);
    HSMENUTEST_FUNC_EXIT("HsMenuView::setLabelVisible");
}

/*!
    Makes the UI to show or hide view search panel.
    When search panel is shown the view toolbar and status pane
    are hidden until search panel is hidden.
     \param visible When true search panel will be shown,
     otherwise it will be hidden.
 */
void HsMenuView::setSearchPanelVisible(bool visible)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setSearchPanelVisible");
    if (mBuilder.searchPanel()->isVisible() != visible) {
        if (visible) {
            mFirstVisibleIndex = firstVisibleItemIndex();
            mBuilder.setSearchPanelVisible(visible);
            mBuilder.view()->hideItems(Hb::AllItems);
            connectSearchPanelSignals();

        } else {
            mLastVisibleIndex = firstVisibleItemIndex();
            mBuilder.view()->showItems(Hb::AllItems);
            mBuilder.setSearchPanelVisible(visible);
            disconnectSearchPanelSignals();
        }
    }
    HSMENUTEST_FUNC_EXIT("HsMenuView::setSearchPanelVisible");
}

/*!
    \return Application Library UI view.
 */
HbView *HsMenuView::view()
{
    return mBuilder.view();
}

/*!
    Returns toolbar action group of Application Library view.
    The group contains All Applications Action and All Collections Action.
    \return Toolbar action group.
 */
QActionGroup *HsMenuView::toolBarActionGroup()
{
    return mToolBarActionGroup;
}

/*!
    \return All Applications Action.
 */
HbAction *HsMenuView::allAppsAction()
{
    return mBuilder.allAppsAction();
}

/*!
    \return All Collections Action.
 */
HbAction *HsMenuView::allCollectionsAction()
{
    return mBuilder.allCollectionsAction();
}

/*!
    \return Ovi Store Action.
 */
HbAction *HsMenuView::oviStoreAction()
{
    return mBuilder.oviStoreAction();
}

/*!
    \return Index of the first item in the item view if
    any is shown or invalid index otherwise.
 */
QModelIndex HsMenuView::firstVisibleItemIndex()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::firstVisibleItemIndex");

    QModelIndex result;

    const QList<HbAbstractViewItem *> array =
        mBuilder.listView()->visibleItems();

    if (array.count() >= 1) {
        result = array[0]->modelIndex();
    }
    HSMENUTEST_FUNC_EXIT("HsMenuView::firstVisibleItemIndex");

    return result;
}

/*!
 * Makes search panel visible.
 * Equivalent to \a setSearchPanelVisible(true)
 */
void HsMenuView::showSearchPanel()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::showSearchPanel");
    setSearchPanelVisible(true);
    HSMENUTEST_FUNC_EXIT("HsMenuView::showSearchPanel");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT skipped: it doubles other method

/*!
 * Makes search panel visible.
 * Equivalent to \a setSearchPanelVisible(false).
 */
void HsMenuView::hideSearchPanel()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::hideSearchPanel");
    setSearchPanelVisible(false);
    HSMENUTEST_FUNC_EXIT("HsMenuView::hideSearchPanel");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Scrolls item view to requested row.
 If search panel is open it will be closed after the fuction call.
 \param row The row which is to get on the top of item view.
 \param hint Ignored.
  */
void HsMenuView::scrollToRow(int row, QAbstractItemView::ScrollHint hint)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::scrollToRow");
    scrollTo(mModel->index(row), convertScrollHint(hint));
    HSMENUTEST_FUNC_EXIT("HsMenuView::scrollToRow");
}

/*!
     Converts scroll hints.
     \param Qt hint Where the target item should be positioned in
     visible area after scroll.
     \return Qrbit version of hint
 */
HbAbstractItemView::ScrollHint HsMenuView::convertScrollHint(
        QAbstractItemView::ScrollHint hint)
{
    switch (hint) {
    case QAbstractItemView::EnsureVisible:
        return HbAbstractItemView::EnsureVisible;
    case QAbstractItemView::PositionAtTop:
        return HbAbstractItemView::PositionAtTop;
    case QAbstractItemView::PositionAtBottom:
        return HbAbstractItemView::PositionAtBottom;
    default:
        return HbAbstractItemView::PositionAtCenter;
    }
}

/*!
 Connects \a activated and \a longPressed signals coming from list
 view to trigger corresponding signal of this object.
*/
void HsMenuView::connectItemViewsSignals()
{
    const HbListView *const listView = mBuilder.listView();

    connect(listView, SIGNAL(activated(QModelIndex)),
            this, SIGNAL(activated(QModelIndex)));
    connect(listView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)));

}

/*!
 Disconnects \a activated and \a longPressed signals coming from list
 view from corresponding signal of this object.
*/
void HsMenuView::disconnectItemViewsSignals()
{
    const HbListView *const listView = mBuilder.listView();

    disconnect(listView, SIGNAL(activated(QModelIndex)),
               this, SIGNAL(activated(QModelIndex)));
    disconnect(listView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
               this, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Connects \a activated and \a longPressed signals coming from search list
 view to emit corresponding signal of this object with translated model index
*/
void HsMenuView::connectSearchItemViewsSignals()
{
    const HbListView *const listView = mBuilder.listView();

    connect(listView, SIGNAL(activated(QModelIndex)),
            this, SLOT(activatedProxySlot(QModelIndex)));
    connect(listView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)));

}

/*!
 Disconnects \a activated and \a longPressed signals coming from list
 view from to emit corresponding signal
 of this object with translated model index
*/
void HsMenuView::disconnectSearchItemViewsSignals()
{
    const HbListView *const listView = mBuilder.listView();

    disconnect(listView, SIGNAL(activated(QModelIndex)),
               this, SLOT(activatedProxySlot(QModelIndex)));
    disconnect(listView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
               this, SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)));
}

/*!
 Connects model's \a scrollTo signal with the object \a scrollToRow slot.
*/
void HsMenuView::connectModelSignals()
{
    connect(mModel,SIGNAL(scrollTo(int,QAbstractItemView::ScrollHint)),
            this, SLOT(scrollToRow(int,QAbstractItemView::ScrollHint)));
}

/*!
 Disconnects model's \a scrollTo signal from the object \a scrollToRow slot.
*/
void HsMenuView::disconnectModelSignals()
{
    disconnect(mModel, SIGNAL(scrollTo(int,QAbstractItemView::ScrollHint)),
               this, SLOT(scrollToRow(int,QAbstractItemView::ScrollHint)));
}

/*!
 Connects signals \a exitClicked and \a criteriaChanged emitted
 by search panel with handling slots of the object or its members
*/
void HsMenuView::connectSearchPanelSignals()
{
    disconnectModelSignals();    
    disconnectItemViewsSignals();

    searchBegins();
    connectSearchItemViewsSignals();

    connect(mBuilder.searchPanel(), SIGNAL(exitClicked()),
            this, SLOT(hideSearchPanel()));
    connect(mBuilder.searchPanel(), SIGNAL(criteriaChanged(QString)),
            this, SLOT(findItem(QString)));
    scrollTo(mProxyModel->mapFromSource(mFirstVisibleIndex),
             HbAbstractItemView::PositionAtTop);

}

/*!
 Disconnects signals \a exitClicked and \a criteriaChanged emitted
 by search panel from handling slots of the object or its members
 Scrolls view to state before connections
*/
void HsMenuView::disconnectSearchPanelSignals()
{
    disconnectSearchItemViewsSignals();

    disconnect(mBuilder.searchPanel(), SIGNAL(exitClicked()),
               this, SLOT(hideSearchPanel()));

    disconnect(mBuilder.searchPanel(), SIGNAL(criteriaChanged(QString)),
               this, SLOT(findItem(QString)));

    searchFinished();
    connectModelSignals();
    connectItemViewsSignals();
}

/*!
 Groups All Application and All Collection actions. Connects Search Action with
 the object's \a showSearchPanel slot.
 */
void HsMenuView::setUpToolBar()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::construct()");

    mBuilder.allAppsAction()->setActionGroup(mToolBarActionGroup);

    mBuilder.allCollectionsAction()->setActionGroup(mToolBarActionGroup);

    connect(mBuilder.searchAction(), SIGNAL(triggered()),
            this, SLOT(showSearchPanel()));

    HSMENUTEST_FUNC_EXIT("HsMenuView::construct()");
}

void HsMenuView::hideToolBar()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::hideToolBar()");
    mBuilder.view()->hideItems(Hb::ToolBarItem);
    HSMENUTEST_FUNC_EXIT("HsMenuView::hideToolBar()");
}

void HsMenuView::showToolBar()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::showToolBar()");
    mBuilder.view()->showItems(Hb::ToolBarItem);
    HSMENUTEST_FUNC_EXIT("HsMenuView::showToolBar()");
}

HsMenuMode HsMenuView::getHsMenuMode()
{
    return mMenuMode;
}

void HsMenuView::setHsMenuMode(HsMenuMode menuMode)
{
    mMenuMode = menuMode;
}
/*!
 Looks up for item and if found scrolls to it.
 \param criteriaStr The item name to find.
 */
void HsMenuView::findItem(QString criteriaStr)
{
    qDebug
        () << QString("hsmenuview::findItem: %1").arg(criteriaStr);
    HSMENUTEST_FUNC_ENTRY("hsmenuview::findItem");

    if ("" != criteriaStr) {
        mProxyModel->setFilterRegExp(QRegExp(criteriaStr,
            Qt::CaseInsensitive, QRegExp::FixedString));
    }
    else {
        mProxyModel->setFilterRegExp(QRegExp(QString(".*"),
            Qt::CaseInsensitive, QRegExp::RegExp));
        // scroll to first item in model
        mBuilder.listView()->scrollTo(
                mProxyModel->mapFromSource(mModel->index(0)),
                HbAbstractItemView::PositionAtTop);
    }
    HSMENUTEST_FUNC_EXIT("hsmenuview::findItem");
}

/*!
 Initializes search.
 */
void HsMenuView::searchBegins()
{
    HSMENUTEST_FUNC_ENTRY("hsmenuview::searchBegins");
    mProxyModel->invalidate();
    mProxyModel->setSourceModel(mModel);
    mProxyModel->setFilterRegExp(QRegExp(QString(".*"), Qt::CaseInsensitive,
        QRegExp::RegExp));
    setModelToView(mProxyModel);
    HSMENUTEST_FUNC_EXIT("hsmenuview::searchBegins");
}

/*!
 Ends search, scrolls view to last visible item in search view
 */
void HsMenuView::searchFinished()
{
    HSMENUTEST_FUNC_ENTRY("hsmenuview::searchFinished");
    QModelIndex temp = mProxyModel->mapToSource(mLastVisibleIndex);
    setModel(mModel);    
    mBuilder.listView()->scrollTo(temp, HbAbstractItemView::PositionAtTop);
    HSMENUTEST_FUNC_EXIT("hsmenuview::searchFinished");
}

/*!
 * Slot used to translate activated signal from proxy model to normal model
 */
void HsMenuView::activatedProxySlot(const QModelIndex &index)
{
    emit activated(mProxyModel->mapToSource(index));
}

/*!
 * Slot used to translate longPressed signal from proxy model to normal model
 */
void HsMenuView::longPressedProxySlot(HbAbstractViewItem *item,
                                      const QPointF &coords)
{
    /*
      this is a kind of hack, introduced for reasons:
      item object should be reusable later, but orbit (or qt) prevents setting
      its index model to previous state
      */
    mSearchIndex = mProxyModel->mapToSource(item->modelIndex());
    HbAbstractViewItem* itemNew = item->createItem();
    itemNew->setModelIndex(mSearchIndex);
    emit longPressed(itemNew, coords);
    delete itemNew;
}

