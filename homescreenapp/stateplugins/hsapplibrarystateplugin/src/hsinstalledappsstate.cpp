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

#include <QStateMachine>
#include <HbView>
#include <HbGroupBox>
#include <HbListView>
#include <HbAction>
#include <HbMenu>
#include <HbAbstractViewItem>

#include <cadefs.h>
#include <caentry.h>

#include "hsapp_defs.h"
#include "hsmenueventfactory.h"
#include "hsmenuitemmodel.h"
#include "hsinstalledappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsmenumodetransition.h"

/*!
 \class HsInstalledAppsState
 \ingroup group_hsapplibrarystateplugin
 \brief Manages displaying installed applications
 Displays all installed packages present on the device.
 \see StateMachine
 \lib ?library
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param mainWindow main window wrapper.
 \param parent Owner.
 */
HsInstalledAppsState::HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
        HsMainWindow &mainWindow,
        QState *parent):
    HsBaseViewState(mainWindow, parent)
{
    initialize(menuViewBuilder, HsInstalledAppsContext);
    construct();
}

/*!
 Constructor part.
 */
void HsInstalledAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::construct");

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/installedappsstate");

    connect(mBackKeyAction, SIGNAL(triggered()), SLOT(openAppLibrary()));

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::construct");
}

/*!
 Method seting menu options.
 */
void HsInstalledAppsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::setMenuOptions");
    HbMenu *const mOptions = new HbMenu();
    mOptions->setParent(this);

    mOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                        this, SLOT(openTaskSwitcher()));
    mOptions->addAction(hbTrId("txt_applib_opt_installation_log"),
                        this, SLOT(openInstallationLog()));

    mMenuView->view()->setMenu(mOptions);
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::setMenuOptions");
}

/*!
 Method seting context menu options.
 \param item the context menu is built for.
 \param flags of the \item.
 */
void HsInstalledAppsState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    HbAction *uninstallAction = mContextMenu->addAction(
                                hbTrId("txt_common_menu_delete"));
    HbAction *appDetailsAction(NULL);
    uninstallAction->setData(UninstallContextAction);

    QSharedPointer<const CaEntry> entry = mModel->entry(item->modelIndex());

    if (!(entry->attribute(componentIdAttributeName()).isEmpty()) &&
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_details"));
        appDetailsAction->setData(AppDetailsContextAction);
    }
}


/*!
 Destructor.
 */
HsInstalledAppsState::~HsInstalledAppsState()
{
    mMenuView->inactivate();
    mMenuView->view()->setNavigationAction(NULL);
}

/*!
 Slot connected to entered() signal.
 Invoked when a state is entered.
 */
void HsInstalledAppsState::stateEntered()
{
    qDebug("AllAppsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateEntered");

    HsBaseViewState::stateEntered();
    setMenuOptions();
    if (!mModel) {
        mModel
        = HsMenuService::getInstalledModel(AscendingNameHsSortAttribute);
        mMenuView->setModel(mModel);
    }

    if (mModel->rowCount() == 0){
        mMenuView->setContext(HsInstalledAppsContext,HsEmptyLabelContext);
        }

    mMenuView->listView()->scrollTo(
        mModel->index(0));

    connect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));

    connect(mModel, SIGNAL(empty(bool)),this,
            SLOT(setEmptyLabelVisibility(bool)));

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsInstalledAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateExited");

    disconnect(mModel, SIGNAL(empty(bool)),this,
               SLOT(setEmptyLabelVisibility(bool)));

    disconnect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)), this,
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));

    HsBaseViewState::stateExited();
    
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Handles button visibility.
 \param visibility indicates whether show or not to show 'empty' label.
 */
void HsInstalledAppsState::setEmptyLabelVisibility(bool visibility)
{
    if(visibility){
        mMenuView->setContext(HsInstalledAppsContext,HsEmptyLabelContext);
    } else {
        mMenuView->setContext(HsInstalledAppsContext,HsItemViewContext);
    }
}

/*!
 Open installation log.
 */
void HsInstalledAppsState::openInstallationLog()
{
    machine()->postEvent(
        HsMenuEventFactory::createInstallationLogEvent());
}

/*!
 Handles context menu actions.
 \param action to handle.
 */
void HsInstalledAppsState::contextMenuAction(HbAction *action)
{    
    HsContextAction command = 
            static_cast<HsContextAction>(action->data().toInt());
    
    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();
    
    switch (command) {
        case UninstallContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createUninstallApplicationEvent(
                    itemId));
            break;
        case AppDetailsContextAction: 
            machine()->postEvent(
                HsMenuEventFactory::createAppDetailsViewEvent(itemId));
            break;            
        default:
            break;            
    }
    mMenuView->setSearchPanelVisible(false);
}
