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
#include <HbInstance>
#include <HbSearchPanel>
#include <HbToolBar>
#include <HbView>
#include <HbToolBarExtension>

#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
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

 \param builder Menu View builder.
 \param viewContext variable representing view context the view is to be prepared for.
 */
HsMenuView::HsMenuView(HsMenuViewBuilder &builder, HsViewContext viewContext):
    mViewContext(viewContext),
    mBuilder(builder),
    mProxyModel(new QSortFilterProxyModel(this)),
    mView(NULL),
    mListView(NULL),
    mViewLabel(NULL),
    mSearchListView(NULL),
    mSearchPanel(NULL)
{
    mBuilder.setOperationalContext(HsItemViewContext);
    mBuilder.setViewContext(mViewContext);
    mBuilder.build();

    mView = mBuilder.currentView();
    mListView = mBuilder.currentListView();
    mViewLabel= mBuilder.currentViewLabel();

    mProxyModel->setFilterRole(CaItemModel::TextRole);
    mProxyModel->setFilterKeyColumn(1);
    mProxyModel->setSortRole(CaItemModel::TextRole);

    connect(mListView,
            SIGNAL(activated(QModelIndex)),
            this, SIGNAL(activated(QModelIndex)));
    connect(mListView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Destructor

 Disconnects signals.
 */
HsMenuView::~HsMenuView()
{
}


/*!
 Sets model for item view.

 \param model Model the view is to represent in HsItemViewMode.
 */
void HsMenuView::setModel(HsMenuItemModel *model)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setModel");

    disconnect(mListView->model(),
               SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)),
               this,
               SLOT(scrollToRow(int, QAbstractItemView::ScrollHint)));

    mListView->setModel(model);

    connect(mListView->model(),
            SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)),
            this,
            SLOT(scrollToRow(int, QAbstractItemView::ScrollHint)));

    HSMENUTEST_FUNC_EXIT("HsMenuView::setModel");
}


/*!
    Returns label appropriate for the view.
    \return pointer to the label or NULL if the view has no label.
 */
HbGroupBox *HsMenuView::viewLabel()
{
    return mViewLabel;
}

/*!
    \return first visible item index if any or default QModelIndex otherwise.
 */
QModelIndex HsMenuView::firstVisibleItemIndex(const HbListView *view) const
{
    QModelIndex result;

    const QList<HbAbstractViewItem *> array =
        view->visibleItems();

    if (array.count() >= 1) {
        result = array[0]->modelIndex();
    }
    return result;
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

    if (visible) {

        mSearchViewInitialIndex = firstVisibleItemIndex(mListView);

        searchBegins();

        connectSearchItemViewsSignals();
        connectSearchPanelSignals();
        
    } else if (mSearchListView != NULL) {

        mIndexToScrollAfterSearchDone =
            firstVisibleItemIndex(mSearchListView);

        disconnectSearchPanelSignals();
        disconnectSearchItemViewsSignals();

        searchFinished();
    }
    HSMENUTEST_FUNC_EXIT("HsMenuView::setSearchPanelVisible");
}

/*!
\return View widget of the menu view.
 */
HbView *HsMenuView::view()
{
    return mView;
}

/*!
\return List view widget of the menu view.
 */
HbListView *HsMenuView::listView()
{
    return mListView;
}

/*!
 Makes search panel visible.
 Equivalent to \a setSearchPanelVisible(true)
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
 Makes search panel invisible.
 Equivalent to \a setSearchPanelVisible(false).
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
 \param row The row which is to get at the position pointed by \a hint.
 \param hint Position in the view the row should be scrolled to.
  */
void HsMenuView::scrollToRow(int row, QAbstractItemView::ScrollHint hint)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::scrollToRow");

    mListView->scrollTo(mListView->model()->index(row, 0),
                        convertScrollHint(hint));

    HSMENUTEST_FUNC_EXIT("HsMenuView::scrollToRow");
}

/*!
     Converts scroll hints.
     \param Qt hint Where the target item should be positioned in
     visible area after scroll.
     \return Orbit version of hint
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

/*
 Connects \a activated and \a longPressed signals coming from search list
 view to emit corresponding signal of this object with translated model index
*/
void HsMenuView::connectSearchItemViewsSignals()
{
    connect(mSearchListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(activatedProxySlot(QModelIndex)));
    connect(mSearchListView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)));

}

/*!
 Disconnects \a activated and \a longPressed signals coming from list
 view from to emit corresponding signal
 of this object with translated model index.
*/
void HsMenuView::disconnectSearchItemViewsSignals()
{
    disconnect(mSearchListView, SIGNAL(activated(QModelIndex)),
               this, SLOT(activatedProxySlot(QModelIndex)));
    disconnect(mSearchListView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
               this,
               SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)));
}


/*!
 Connects signals \a exitClicked and \a criteriaChanged emitted
 by search panel with handling slots of the object or its members.
*/
void HsMenuView::connectSearchPanelSignals()
{
    connect(mSearchPanel, SIGNAL(exitClicked()),
            this, SLOT(hideSearchPanel()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)),
            this, SLOT(findItem(QString)));


}

/*!
 Disconnects signals \a exitClicked and \a criteriaChanged emitted
 by search panel from handling slots of the object or its members
 Scrolls view to state before connections.
*/
void HsMenuView::disconnectSearchPanelSignals()
{
    disconnect(mSearchPanel, SIGNAL(exitClicked()),
               this, SLOT(hideSearchPanel()));

    disconnect(mSearchPanel, SIGNAL(criteriaChanged(QString)),
               this, SLOT(findItem(QString)));

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
        mSearchListView->scrollTo(mProxyModel->index(0,0),
                                  HbAbstractItemView::PositionAtTop);
    } else {
        mProxyModel->setFilterRegExp(QRegExp(QString(".*"),
                                             Qt::CaseInsensitive, QRegExp::RegExp));

        // scroll to first item in model
        mSearchListView->scrollTo(
            mProxyModel->index(0, 0),

            HbAbstractItemView::PositionAtTop);
    }
    HSMENUTEST_FUNC_EXIT("hsmenuview::findItem");
}

/*!
 Makes the view display search panel with view representing search results.
 */
void HsMenuView::searchBegins()
{
    HSMENUTEST_FUNC_ENTRY("hsmenuview::searchBegins");


    mBuilder.setViewContext(mViewContext);
    mBuilder.setOperationalContext(HsSearchContext);
    mBuilder.build();
    mSearchListView = mBuilder.currentListView();
    mSearchPanel = mBuilder.currentSearchPanel();

    mView->hideItems(Hb::AllItems);

    mProxyModel->invalidate();
    mProxyModel->setSourceModel(mListView->model());
    mProxyModel->setFilterRegExp(QRegExp(QString(".*"), Qt::CaseInsensitive,
                                         QRegExp::RegExp));
    mSearchListView->setModel(mProxyModel);
    mSearchListView->scrollTo(
        mProxyModel->mapFromSource(mSearchViewInitialIndex),
        HbAbstractItemView::PositionAtTop);
    HSMENUTEST_FUNC_EXIT("hsmenuview::searchBegins");
}

/*!
  Ensures view does not contain search related elements and is scrolled
  to item chosen in search mode.
 */
void HsMenuView::searchFinished()
{
    HSMENUTEST_FUNC_ENTRY("hsmenuview::searchFinished");
    
    mBuilder.setViewContext(mViewContext);
    mBuilder.setOperationalContext(HsItemViewContext);
    mBuilder.build();
    mView->showItems(Hb::AllItems);

    const QModelIndex indexToScroll =
        mProxyModel->mapToSource(mIndexToScrollAfterSearchDone);

    mListView->scrollTo(indexToScroll, HbAbstractItemView::PositionAtTop);

    mSearchListView = NULL;
    mSearchPanel = NULL;
    HSMENUTEST_FUNC_EXIT("hsmenuview::searchFinished");
}

/*!
 Slot used to translate activated signal from proxy model to normal model.
 \param index representing an item activated in search list view.
 */
void HsMenuView::activatedProxySlot(const QModelIndex &index)
{
    emit activated(mProxyModel->mapToSource(index));
}

/*!
 Slot used to forward 'long pressed' signal with item description transladed 
 from search view context to context of the view search was requested from. 
 */
void HsMenuView::longPressedProxySlot(HbAbstractViewItem *item,
                                      const QPointF &coords)
{
    /*
      this is a kind of hack, introduced for reasons:
      item object should be reusable later, but orbit (or qt) prevents setting
      its index model to previous state
      */
    mSearchViewLongPressedIndex =
        mProxyModel->mapToSource(item->modelIndex());
    QScopedPointer<HbAbstractViewItem> itemNew(item->createItem());
    itemNew->setModelIndex(mSearchViewLongPressedIndex);
    emit longPressed(itemNew.data(), coords);
}

/*!
 Add the \view to first instance of HbMainWindow registered in HbInstance.
 \param view View to be added to HbMainWindow.
 */
void HsMenuView::addViewToMainWindow(HbView *view)
{
    HbMainWindow *const hbW(
        HbInstance::instance()->allMainWindows().value(0));

    if (!hbW->views().contains(view)) {
        hbW->addView(view);
    }
    hbW->setCurrentView(view);
}


/*!
 Add the view to main window and search action with \a showSearchPanel
 slot of the window.
 */
void HsMenuView::activate()
{
    addViewToMainWindow(mView);

    connect(mBuilder.searchAction(), SIGNAL(triggered()),
            this, SLOT(showSearchPanel()));

}

/*!
 Disconnecs search action and \a showSearchPanel slot of the window.
 */
void HsMenuView::inactivate()
{
    // handle app key event
    mBuilder.toolBarExtension()->close();

    disconnect(mBuilder.searchAction(), SIGNAL(triggered()),
               this, SLOT(showSearchPanel()));
}



