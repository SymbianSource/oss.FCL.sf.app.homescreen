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
 * Description: Menu All Applications state.
 *
 */

#include <qstatemachine.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbabstractviewitem.h>
#include <hsmenueventfactory.h>
#include <hbinstance.h>
#include <caentry.h>
#include <hbsearchpanel.h>

#include "hsmenuitemmodel.h"
#include "hsmenuview.h"
#include "cadefs.h"
#include "hsallappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"
#include "hsmenumodetransition.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsAllAppsState::HsAllAppsState(HsMenuView &menuView, QState *parent) :
    QState(parent), mSortAttribute(AscendingNameHsSortAttribute),
    mCollectionsSortAttribute(LatestOnTopHsSortAttribute),
    mMenuView(menuView), mAllAppsModel(0)
{
    construct();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::construct");

    QState *initialState = new QState(this);
    setInitialState(initialState);

    QState *addModeState = new QState(this);
    connect(addModeState, SIGNAL(entered()),SLOT(addModeEntered()));

    QState *normalModeState = new QState(this);
    connect(normalModeState, SIGNAL(entered()),SLOT(normalModeEntered()));

    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuView, NormalHsMenuMode, normalModeState));
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuView, AddHsMenuMode, addModeState));

    setObjectName(this->parent()->objectName() + "/allappsstate");
    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    mAllAppsModel = HsMenuService::getAllApplicationsModel(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::construct");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::setMenuOptions");
    HbMenu *const mOptions = new HbMenu();
    mOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                        this,
                        SLOT(openTaskSwitcher()));
    mOptions->addAction(hbTrId("txt_applib_opt_add_to_collection"),
                        this,
                        SLOT(addToCollection()));

    HbMenu *const sortMenu = mOptions->addMenu(hbTrId(
                                 "txt_applib_opt_sort_by"));
    //Grouped options are exclusive by default.
    QActionGroup *sortGroup = new QActionGroup(mOptions);

    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_ascending"),
                            this,
                            SLOT(ascendingMenuAction())));
    sortGroup->addAction(
        sortMenu->addAction(hbTrId("txt_applib_opt_sub_descending"),
                            this,
                            SLOT(descendingMenuAction())));
    mOptions->setParent(this);
    foreach(QAction *action, sortMenu->actions()) {
        action->setCheckable(true);
    }
    int currentSortingPosition(-1);
    switch (mSortAttribute) {
    case AscendingNameHsSortAttribute:
        currentSortingPosition = 0;
        break;
    case DescendingNameHsSortAttribute:
        currentSortingPosition = 1;
        break;
    default:
        break;
    }
    if (currentSortingPosition >= 0) {
        sortGroup->actions().at(currentSortingPosition)->setChecked(true);
    }
    mMenuView.view()->setMenu(mOptions);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::setMenuOptions");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsAllAppsState::~HsAllAppsState()
{
    delete mAllAppsModel;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::scrollToBeginning()
{
    mBookmark = mAllAppsModel->index(0);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::stateEntered()
{
    qDebug("AllAppsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateEntered");

    mMenuView.setModel(mAllAppsModel);
    mMenuView.scrollTo(mBookmark, HbAbstractItemView::PositionAtTop);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateEntered");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::normalModeEntered()
{
    setMenuOptions();

    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::addModeEntered()
{
    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(addActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(addLongPressed(HbAbstractViewItem *, QPointF)));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateExited");
    mBookmark = mMenuView.firstVisibleItemIndex();

    mMenuView.disconnect(this);
    mMenuView.view()->setMenu(NULL);

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsAllAppsState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::listItemActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::listItemActivated");

    const CaEntry *entry = mAllAppsModel->entry(index);

    if (entry->entryTypeName() == widgetTypeName()) {
        machine()->postEvent(
            HsMenuEventFactory::createPreviewHSWidgetEvent(
                entry->id(), entry->entryTypeName(), entry->attribute(
                    widgetUriAttributeName()), entry->attribute(
                    widgetLibraryAttributeName())));

        const int itemId = index.data(CaItemModel::IdRole).toInt();
        HsMenuService::touch(itemId);
    } else {
        QVariant data = mAllAppsModel->data(index, CaItemModel::IdRole);
        HsMenuService::executeAction(data.toInt());
    }

    mMenuView.setSearchPanelVisible(false);

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::listItemActivated");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::addActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addActivated");
    mMenuView.setSearchPanelVisible(false);
    addToHomeScreen(index);
    machine()->postEvent(
        HsMenuEventFactory::createOpenHomeScreenEvent());
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addActivated");
}

/*!
 Handles long-item-pressed event in all apps view by showing context menu
 \param item the event pertains to
 \param position at which context menu is shown
 */
void HsAllAppsState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::listItemLongPressed");

    mMenuView.setSearchPanelVisible(false);

    // create context menu
    HbMenu *menu = new HbMenu();

    HbAction *addToHomeScreenAction = menu->addAction(hbTrId(
                                          "txt_applib_menu_add_to_home_screen"));

    HbAction *addToCollectionAction = menu->addAction(hbTrId(
                                          "txt_applib_menu_add_to_collection"));

    HbAction *uninstallAction = menu->addAction(hbTrId(
                                    "txt_common_menu_delete"));

    // check conditions and hide irrelevant menu items

    EntryFlags flags = item->modelIndex().data(
                           CaItemModel::FlagsRole).value<EntryFlags> ();

    if (!(flags & RemovableEntryFlag)) {
        uninstallAction->setVisible(false);
    }
    // choose proper action
    if (HbAction *selectedAction = menu->exec(coords)) {
        if (selectedAction == addToHomeScreenAction) {
            addToHomeScreen(item->modelIndex());
        } else if (selectedAction == addToCollectionAction) {
            // Addding a specific application to
            // an existing collection via item specific menu.
            const int itemId =
                item->modelIndex().data(CaItemModel::IdRole).toInt();
            machine()->postEvent(
                HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
                    mSortAttribute, mCollectionsSortAttribute, itemId));

        } else if (selectedAction == uninstallAction) {
            const int itemId =
                item->modelIndex().data(CaItemModel::IdRole).toInt();
            HsMenuService::executeAction(itemId, removeActionIdentifier());
        }
    }
    delete menu;
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::listItemLongPressed");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::addLongPressed(HbAbstractViewItem *item,
                                    const QPointF &coords)
{
    Q_UNUSED(coords);
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addLongPressed");
    mMenuView.setSearchPanelVisible(false);
    addToHomeScreen(item->modelIndex());
    machine()->postEvent(
        HsMenuEventFactory::createOpenHomeScreenEvent());
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addLongPressed");

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::addToCollection()
{
    // Add one/many applications to
    // a new/an existing collection via the All view
    machine()->postEvent(
        HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
            mSortAttribute, mCollectionsSortAttribute));
}

/*!
 Triggers event so that a state adding to Home Screen is reached
 \param index of an item to be added to homescreen
 \retval void
 */
void HsAllAppsState::addToHomeScreen(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::addToHomeScreen");
    const CaEntry *entry = mAllAppsModel->entry(index);
    
    QMap<QString, QString> attributes = entry->attributes();
    
    machine()-> postEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                              entry->id(),
                              entry->entryTypeName(),
                              entry->attribute(widgetUriAttributeName()),
                              entry->attribute(widgetLibraryAttributeName()),
                              &attributes));

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::addToHomeScreen");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::ascendingMenuAction");
    mSortAttribute = AscendingNameHsSortAttribute;
    mAllAppsModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::ascendingMenuAction");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::collectionsSortOrder(
    HsSortAttribute sortOrder)
{
    mCollectionsSortAttribute = sortOrder;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsAllAppsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::descendingMenuAction");
    mSortAttribute = DescendingNameHsSortAttribute;
    mAllAppsModel->setSort(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::descendingMenuAction");

}
