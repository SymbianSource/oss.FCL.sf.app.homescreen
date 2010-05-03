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
 * Description: Menu All Collections state.
 *
 */

#include <qstatemachine.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <hsmenueventfactory.h>
#include <hbinstance.h>
#include <hbsearchpanel.h>

#include "hsmenuview.h"
#include "hsmenuitemmodel.h"
#include "cadefs.h"
#include "hsallcollectionsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsmenumodetransition.h"

/*!
 Constructor.
 \param menuView Menu view.
 \param parent Owner.
 */
HsAllCollectionsState::HsAllCollectionsState(
    HsMenuView &menuView, QState *parent) :
    QState(parent), mSortAttribute(CustomHsSortAttribute),
    mMenuView(menuView), mAllCollectionsModel(0)
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsAllCollectionsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::construct");

    QState *initialState = new QState(this);
    setInitialState(initialState);

    QState *addModeState = new QState(this);
    connect(addModeState, SIGNAL(entered()),SLOT(addModeEntered()));

    QState *normalModeState = new QState(this);
    connect(normalModeState, SIGNAL(entered()),SLOT(normalModeEntered()));
    connect(normalModeState, SIGNAL(exited()),SLOT(normalModeExited()));

    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuView, NormalHsMenuMode, normalModeState));
    initialState->addTransition(new HsMenuModeTransition(
                                    mMenuView, AddHsMenuMode, addModeState));

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allcollectionsstate");
    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    mAllCollectionsModel = HsMenuService::getAllCollectionsModel(
                               mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::construct");
}

/*!
 Creates and installs menu options for the view
 */
void HsAllCollectionsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::setMenuOptions");
    HbMenu *const options = new HbMenu();
    options->addAction(hbTrId("txt_applib_opt_task_switcher"),
                       this,
                       SLOT(openTaskSwitcher()));
    options->addAction(hbTrId("txt_applib_opt_new_collection"),
                       this, SLOT(createNewCollection()));

    HbMenu *const sortMenu = options->addMenu(
                                 hbTrId("txt_applib_opt_sort_by"));

    //Grouped options are exclusive by default.
    QActionGroup *sortGroup = new QActionGroup(options);
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_custom"),
                             this, SLOT(customMenuAction())));
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_ascending"),
                             this, SLOT(ascendingMenuAction())));
    sortGroup->addAction(sortMenu->addAction(
                             hbTrId("txt_applib_opt_sub_descending"),
                             this, SLOT(descendingMenuAction())));

    if (mSortAttribute == CustomHsSortAttribute) {
        options->addAction(hbTrId("txt_applib_opt_arrange"),
                           this, SLOT(createArrangeCollection()));
    }
    options->setParent(this);

    foreach(QAction *action, sortMenu->actions()) {
        action->setCheckable(true);
    }

    switch (mSortAttribute) {
    case AscendingNameHsSortAttribute:
        sortGroup->actions().at(1)->setChecked(true);
        break;
    case DescendingNameHsSortAttribute:
        sortGroup->actions().at(2)->setChecked(true);
        break;
    case CustomHsSortAttribute:
    default:
        sortGroup->actions().at(0)->setChecked(true);
        break;
    }

    mMenuView.view()->setMenu(options);


    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::setMenuOptions");
}
/*!
 Destructor.
 */
HsAllCollectionsState::~HsAllCollectionsState()
{
    delete mAllCollectionsModel;
}

/*!
 Populates all folders
 */
void HsAllCollectionsState::scrollToBeginning()
{
    mBookmark = mAllCollectionsModel->index(0);
}

/*!
 Slot invoked when a state is entered.
 */
void HsAllCollectionsState::stateEntered()
{
    qDebug("AllCollectionsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::stateEntered");

    mMenuView.setModel(mAllCollectionsModel);
    mMenuView.scrollTo(mBookmark, HbAbstractItemView::PositionAtTop);

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::stateEntered");
}

/*!
 Slot invoked when normal mode is entered.
*/
void HsAllCollectionsState::normalModeEntered()
{
    setMenuOptions();

    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Slot invoked when normal mode is exited.
*/
void HsAllCollectionsState::normalModeExited()
{
    mMenuView.view()->setMenu(NULL);
}

/*!
 Slot invoked when add mode is entered.
 */
void HsAllCollectionsState::addModeEntered()
{
    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(addActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(addLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Slot invoked when a state is exited.
 */
void HsAllCollectionsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::stateExited");

    mMenuView.setSearchPanelVisible(false);

    mMenuView.disconnect(this);

    mBookmark = mMenuView.firstVisibleItemIndex();

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::stateExited");
    qDebug("AllCollectionsState::stateExited()");
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsAllCollectionsState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsAllCollectionsState::listItemActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::listItemActivated");
    QVariant data = mAllCollectionsModel->data(index, CaItemModel::IdRole);
    int id = data.toInt();
    QString collectionType =
        mAllCollectionsModel->data(index, CaItemModel::TypeRole).toString();
    qDebug("AllCollectionsState::listItemActivated - MCS ID: %d",
           data.toInt());

    mMenuView.setSearchPanelVisible(false);

    machine()->postEvent(HsMenuEventFactory::createOpenCollectionEvent(id,
                         collectionType));
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::listItemActivated");
}

/*!
 Slot connected to List widget in add mode.
 \param index Model index of the activated item.
 */
void HsAllCollectionsState::addActivated(const QModelIndex &index)
{
    const int itemId = index.data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(itemId));
    machine()->postEvent(
        HsMenuEventFactory::createOpenHomeScreenEvent());
}

/*!
 Slot connected to List widget in add mode.
 Called when item long pressed.
 \param item View item.
 \param coords Press point coordinates.
 */
void HsAllCollectionsState::addLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    Q_UNUSED(coords);
    const int itemId = item->modelIndex().data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(itemId));
    machine()->postEvent(
        HsMenuEventFactory::createOpenHomeScreenEvent());
}

/*!
 Slot connected to List widget in normal mode.
 Called when item long pressed.
 \param item View item.
 \param coords Press point coordinates.
 */
void HsAllCollectionsState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::listItemLongPressed");


    const int itemId = item->modelIndex().data(CaItemModel::IdRole).toInt();
    HbMenu *menu = new HbMenu();

    // create context menu
    HbAction *addShortcutAction = menu->addAction(hbTrId(
                                      "txt_applib_menu_add_to_home_screen"));
    HbAction *renameAction = NULL;
    HbAction *deleteAction = NULL;

    EntryFlags flags = item->modelIndex().data(CaItemModel::FlagsRole).value<
                       EntryFlags> ();

    if ((flags & RemovableEntryFlag)) {
        renameAction = menu->addAction(
                           hbTrId("txt_common_menu_rename_item"));
        deleteAction = menu->addAction(hbTrId("txt_common_menu_delete"));
    }

    // choose proper action
    if (HbAction *selectedAction = menu->exec(coords)) {

        if (selectedAction == addShortcutAction) {
            machine()->postEvent(
                HsMenuEventFactory::createAddToHomeScreenEvent(itemId));
        } else if (renameAction && selectedAction == renameAction) {
            machine()->postEvent(
                HsMenuEventFactory::createRenameCollectionEvent(itemId));
        } else if (deleteAction && selectedAction == deleteAction) {
            machine()->postEvent(
                HsMenuEventFactory::createDeleteCollectionEvent(itemId));
        }

        mMenuView.setSearchPanelVisible(false);
    }
    delete menu;
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::listItemLongPressed");
}

/*!
 Slot connected to constructMenu.
 */
void HsAllCollectionsState::createNewCollection()
{
    // Adding a new collection via the Collections view
    machine()->postEvent(HsMenuEventFactory::createNewCollectionEvent());
}

/*!
 Slot connected to constructMenu.
 */
void HsAllCollectionsState::createArrangeCollection()
{
    // Arrange collection via the Arrange view
    QModelIndex idx = mMenuView.firstVisibleItemIndex();
    int topItemId = idx.data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createArrangeCollectionEvent(topItemId));
}

/*!
 Menu custom sort action slot.
 */
void HsAllCollectionsState::customMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::customMenuAction");
    mSortAttribute = CustomHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::customMenuAction");
}

/*!
 Menu ascending sort action slot.
 */
void HsAllCollectionsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::ascendingMenuAction");
    mSortAttribute = AscendingNameHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::ascendingMenuAction");
}

/*!
 Menu descending sort action slot.
 */
void HsAllCollectionsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::descendingMenuAction");
    mSortAttribute = DescendingNameHsSortAttribute;
    setMenuOptions();
    mAllCollectionsModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::descendingMenuAction");
}
