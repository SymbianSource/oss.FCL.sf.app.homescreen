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
#include <HbPushButton>
#include <HbToolBar>
#include <HbView>
#include <HbToolBarExtension>
#include <HbShrinkingVkbHost>

#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
#include "hsmainwindow.h"
#include "hsmenuitemmodel.h"
#include "hsmenuview.h"
#include "hslistviewitem.h"
#include "hssearchview.h"

/*!
    \class HsMenuView
    \ingroup group_hsmenustateplugin

    \brief Menu View.

    Provides UI for Application Library states.

    \sa HsAppLibraryState
*/

/*!

 Retrieves UI objects for a requested context and sets up signals' connections.

 \param builder Menu View builder.
 \param stateContext Variable representing view context the view is to be prepared for.
 \param mainWindow Object responsible for making a given view
    a currently displayed view.
 */
HsMenuView::HsMenuView(HsMenuViewBuilder &builder,
                       HsStateContext stateContext,
                       HsMainWindow &mainWindow):
    mBuilder(builder),
    mStateContext(stateContext),
    mOperationalContext(HsItemViewContext),
    mMainWindow(mainWindow),
    mHsSearchView(new HsSearchView(mBuilder, mStateContext, mMainWindow))
{
    synchronizeCache();

    connect(mListView,
            SIGNAL(activated(QModelIndex)),
            this, SIGNAL(activated(QModelIndex)));
    connect(mListView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)));
    
    connect(mHsSearchView.data(), SIGNAL(activated(QModelIndex)),
            this, SIGNAL(activated(QModelIndex)));
    connect(mHsSearchView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
                        this, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)));
}

/*!
    Empty.
 */
HsMenuView::~HsMenuView()
{
}


/*!
 Sets model for list item view if available in current context otherwise
 ingores the request.

 \param model Model the view is to represent in HsItemViewMode.
 */
void HsMenuView::setModel(HsMenuItemModel *model)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setModel");

    if (mListView != NULL) {
        if (mListView->model()) {
            disconnect(mListView->model(),
                       SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)),
                       this,
                       SLOT(scrollToRow(int, QAbstractItemView::ScrollHint)));
            disconnect(mListView->model(), SIGNAL(countChange()),
                       this,
                        SIGNAL(listViewChange()));
        }

        mListView->setModel(model, new HsListViewItem());

        if (mListView->model()) {
            connect(mListView->model(),
                    SIGNAL(scrollTo(int, QAbstractItemView::ScrollHint)),
                    this,
                    SLOT(scrollToRow(int, QAbstractItemView::ScrollHint)));
            connect(mListView->model(), SIGNAL(countChange()),
                    this,
                    SIGNAL(listViewChange()));
        }
    }

    HSMENUTEST_FUNC_EXIT("HsMenuView::setModel");
}


/*!
\return View widget of the menu view.
 */
HbView *HsMenuView::view() const
{
    return mView;
}

/*!
\return List view widget of the menu view 
 if available in the context or NULL otherwise.
 */
HbListView *HsMenuView::listView() const
{
    return mListView;
}

/*!
    Returns label appropriate for the view.
    \return Pointer to the label 
    if available in the context or NULL otherwise.
 */
HbGroupBox *HsMenuView::viewLabel() const
{
    return mViewLabel;
}

/*!
\return Collection button 
 if available in the context or NULL otherwise.
 */
HbPushButton *HsMenuView::contentButton() const
{
    return mAddContentButton;
}

/*!
 Makes search panel visible.
 Equivalent to \a setSearchPanelVisible(true)
 */
void HsMenuView::showSearchPanel()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::showSearchPanel");

    mHsSearchView->setSearchPanelVisible(true);

    HSMENUTEST_FUNC_EXIT("HsMenuView::showSearchPanel");
}

/*!
 Makes search panel invisible.
 Equivalent to \a setSearchPanelVisible(false).
 */
void HsMenuView::hideSearchPanel()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::hideSearchPanel");

    mHsSearchView->setSearchPanelVisible(false);

    HSMENUTEST_FUNC_EXIT("HsMenuView::hideSearchPanel");
}

/*!
 Disable or enable search action button.
 \param disable If true action gets disabled.
 */
void HsMenuView::disableSearch(bool disable)
{
    mBuilder.searchAction()->setDisabled(disable);
}

/*!
 Scrolls list item view to requested row.
 \param row The row which is to get at the position pointed by \a hint.
 \param hint Position in the view the row should be scrolled to.
  */
void HsMenuView::scrollToRow(int row, QAbstractItemView::ScrollHint hint)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::scrollToRow");

    if (mListView != NULL) {
        mListView->scrollTo(
            mListView->model()->index(row, 0), convertScrollHint(hint));
    }

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

/*!
 Add the view to main window and connect search action with \a showSearchPanel
 slot of the window.
 */
void HsMenuView::activate()
{
    mMainWindow.setCurrentView(mView);
    connect(mBuilder.searchAction(), SIGNAL(triggered()),
            this, SLOT(showSearchPanel()), Qt::UniqueConnection);
}

/*!
 Disconnects search action and disconnects \a showSearchPanel slot from the window.
 */
void HsMenuView::inactivate()
{
    // handle app key event
    mBuilder.toolBarExtension()->close();

    disconnect(mBuilder.searchAction(), SIGNAL(triggered()),
               this, SLOT(showSearchPanel()));
}

/*!
  Makes a new view to be activated. Ensure navigation action and view label
  heading are preserved after reset.
  \param operationalContext Context indicating which view to activate.
 */
void HsMenuView::reset(HsOperationalContext operationalContext)
{
    QString viewLabelHeading;

    // before changing context read current view label heading ...
    synchronizeCache();

    if (mBuilder.currentViewLabel() != 0) {
        viewLabelHeading = mBuilder.currentViewLabel()->heading();
    }
    // ... and back-key action
    HbAction *const backKeyAction(mView->navigationAction());

    // now we can switch the context

    if (operationalContext != HsSearchContext) {

        mOperationalContext = operationalContext;
        synchronizeCache();

        if (mBuilder.currentViewLabel() != 0) {
            mBuilder.currentViewLabel()->setHeading(viewLabelHeading);
        }

        mView->setNavigationAction(backKeyAction);

    } else {
        showSearchPanel();
    }
}

/*!
 Builder can be shared between many instances of HsMenuView
 being in different contexts. Before using builder make sure
 it is in context matching the current HsMenuView.
 */

void HsMenuView::switchBuilderContext() {
    mBuilder.setStateContext(mStateContext);
    mBuilder.setOperationalContext(mOperationalContext);
}

/*!
 Updates internal data references to widgets.
 */
void HsMenuView::synchronizeCache()
{
    switchBuilderContext();

    mView = mBuilder.currentView();
    mListView = mBuilder.currentListView();
    mViewLabel = mBuilder.currentViewLabel();
    mAddContentButton = mBuilder.currentAddContentButton();
}
