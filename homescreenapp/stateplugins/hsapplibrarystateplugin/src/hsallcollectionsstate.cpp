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
#include <QScopedPointer>
#include <QStateMachine>
#include <HbView>
#include <HbListView>
#include <HbMenu>
#include <HbAction>
#include <HbAbstractViewItem>

#include <cadefs.h>

#include "hsmenueventfactory.h"
#include "hsmenumodewrapper.h"
#include "hsmenuitemmodel.h"
#include "hsallcollectionsstate.h"
#include "hsaddappstocollectionstate.h"
//#include "hsmenumodetransition.h"
#include "hsmainwindow.h"

/*!
 \class HsAllCollectionsState
 \ingroup group_hsmenustateplugin
 \brief All Collections State.
 Displays all collections present on the device.
 \lib ?library
 \see StateMachine
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode reference to object representing menu mode (add mode on/add mode off).
 \param parent Owner.
 */
HsAllCollectionsState::HsAllCollectionsState(
    HsMenuViewBuilder &menuViewBuilder,
    HsMenuModeWrapper &menuMode,
    HsMainWindow &mainWindow,
    QState *parent):
    HsBaseViewState(mainWindow, menuMode, parent)
{
    initialize(menuViewBuilder, HsAllCollectionsContext);
    construct();
}

/*!
 Constructs contained objects.
 */
void HsAllCollectionsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::construct");
    defineTransitions();
    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allcollectionsstate");

    connect(mBackKeyAction, SIGNAL(triggered()), SIGNAL(toAppLibraryState()));

    mModel = HsMenuService::getAllCollectionsModel();
    mMenuView->setModel(mModel);

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::construct");
}

/*!
 Creates and installs menu options for the view.
 */
void HsAllCollectionsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::setMenuOptions");

    QScopedPointer<HbMenu> viewOptions(new HbMenu);

    viewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                           static_cast<HsBaseViewState*>(this),
                           SLOT(openTaskSwitcher()));
    viewOptions->addAction(hbTrId("txt_applib_opt_new_collection"),
                           this, SLOT(createNewCollection()));

    viewOptions->addAction(hbTrId("txt_applib_opt_arrange"),
        this, SLOT(createArrangeCollection()));
    mMenuView->view()->setMenu(viewOptions.take());

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::setMenuOptions");
}

/*!
 Method seting context menu options.
 \param item the context menu is requested for.
 \param flags related to \a item.
 */
void HsAllCollectionsState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    Q_UNUSED(item)
    // create context menu
    HbAction *addShortcutAction = mContextMenu->addAction(hbTrId(
                                      "txt_applib_menu_add_to_home_screen"));
    addShortcutAction->setData(AddToHomeScreenContextAction);
    HbAction *renameAction = NULL;
    HbAction *deleteAction = NULL;

    if ((flags & RemovableEntryFlag)) {
        renameAction = mContextMenu->addAction(
                           hbTrId("txt_common_menu_rename_item"));
        renameAction->setData(RenameContextAction);
        deleteAction = mContextMenu->addAction(hbTrId("txt_common_menu_delete"));
        deleteAction->setData(DeleteContextAction);
    }
}

/*!
 Destructor.
 */
HsAllCollectionsState::~HsAllCollectionsState()
{
}

/*!
 Slot invoked when normal mode is entered.
*/
void HsAllCollectionsState::normalModeEntered()
{
    HsBaseViewState::normalModeEntered();
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            static_cast<HsBaseViewState*>(this),
            SLOT(openCollection(QModelIndex)));
}

/*!
 Slot invoked when normal mode is exited.
*/
void HsAllCollectionsState::normalModeExited()
{
    mMenuView->view()->setMenu(NULL);
}


/*!
 Slot invoked when a state is exited.
 */
void HsAllCollectionsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::stateExited");

    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::stateExited");
    qDebug("AllCollectionsState::stateExited()");
}

/*!
 Slot connected to List widget in add mode.
 \param index Model index of the activated item.
 */
void HsAllCollectionsState::addActivated(const QModelIndex &index)
{
    mMenuView->disconnect(this);
    const int itemId = index.data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(
            itemId, mMenuMode->getHsMenuMode(), mMenuMode->getHsToken()));
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
    mMenuView->disconnect(this);
    const int itemId = item->modelIndex().data(CaItemModel::IdRole).toInt();
    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(itemId,
                mMenuMode->getHsMenuMode(), mMenuMode->getHsToken()));
}

/*!
 Handles context menu actions.
 \param action to be handled.
 */
void HsAllCollectionsState::contextMenuAction(HbAction *action)
{
    HsContextAction command = 
        static_cast<HsContextAction>(action->data().toInt());

    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();

    switch (command) {
        case AddToHomeScreenContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createAddToHomeScreenEvent(
                    itemId, mMenuMode->getHsMenuMode(), mMenuMode->getHsToken()));
            break;
        case RenameContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createRenameCollectionEvent(itemId));
            break;
        case DeleteContextAction:
             machine()->postEvent(
                HsMenuEventFactory::createDeleteCollectionEvent(itemId));
            break;      
        default:
            break;
    }
                                    
    mMenuView->setSearchPanelVisible(false);
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
    int topItemId(0);

    const QList<HbAbstractViewItem *> array =
        mMenuView->listView()->visibleItems();

    if (array.count() >= 1) {
        QModelIndex idx = array[0]->modelIndex();
        topItemId = idx.data(CaItemModel::IdRole).toInt();
    }

    machine()->postEvent(
        HsMenuEventFactory::createArrangeAllCollectionsEvent(topItemId));
}
