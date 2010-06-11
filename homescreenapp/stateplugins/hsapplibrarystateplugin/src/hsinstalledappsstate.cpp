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
#include <HbGroupBox>
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
#include "hsmenuviewbuilder.h"
#include "cadefs.h"
#include "hsinstalledappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"
#include "hsmenumodetransition.h"
#include "hsmainwindow.h"

/*!
 \class HsInstalledAppsState
 \ingroup group_hsapplibrarystateplugin
 \brief Manages displaying installed applications
 Displays all installed packages present on the device.
 \see StateMachine
 \lib ?library
 */

/*!
 \var HsInstalledAppsState::mMenuView
 The View widget.
 Own.
 */

/*!
 \var HsInstalledAppsState::mInstalledAppsModel
 Item Model for the List.
 Own.
 */

/*!
 \var HsInstalledAppsState::mSecondarySoftkeyAction
 Secondary Softkey action.
 Backstepping functionality.
 Own.
 */

/*!
 \var HsInstalledAppsState::mOldNavigationAction
 Old navigation icon.
 Not own.
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param parent Owner.
 */
HsInstalledAppsState::HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
        HsMainWindow &mainWindow,
        QState *parent):
    QState(parent),
    mMenuView(menuViewBuilder, HsInstalledAppsContext),
    mInstalledAppsModel(0),
    mSecondarySoftkeyAction(new HbAction(Hb::BackNaviAction, this)),
    mMainWindow(mainWindow),
    mContextModelIndex(),mContextMenu(0)
{
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
    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));

    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SLOT(backAction()));
    mMenuView.view()->
    setNavigationAction(mSecondarySoftkeyAction);


    setMenuOptions();

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

    mMenuView.view()->setMenu(mOptions);
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::setMenuOptions");
}

/*!
 Destructor.
 */
HsInstalledAppsState::~HsInstalledAppsState()
{
    mMenuView.inactivate();
    mMenuView.setModel(NULL);
    mMenuView.view()->setNavigationAction(NULL);

    delete mInstalledAppsModel;
}

/*!
 Slot connected to entered() signal.
 Invoked when a state is entered.
 */
void HsInstalledAppsState::stateEntered()
{
    qDebug("AllAppsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateEntered");

    mMainWindow.setCurrentView(mMenuView);
    mMenuView.activate();

    if (!mInstalledAppsModel) {
        mInstalledAppsModel
        = HsMenuService::getInstalledModel(AscendingNameHsSortAttribute);
        mMenuView.setModel(mInstalledAppsModel);
    }

    if (mInstalledAppsModel->rowCount() == 0){
        mMenuView.setContext(HsInstalledAppsContext,HsEmptyLabelContext);
        }

    mMenuView.listView()->scrollTo(
        mInstalledAppsModel->index(0));

    connect(&mMenuView,
            SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));

    connect(mInstalledAppsModel, SIGNAL(empty(bool)),this,
            SLOT(setEmptyLabelVisibility(bool)));

    mMenuView.viewLabel()->setHeading(
        hbTrId("txt_applib_subtitle_installed"));

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsInstalledAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateExited");

    disconnect(mInstalledAppsModel, SIGNAL(empty(bool)),this,
               SLOT(setEmptyLabelVisibility(bool)));

    mMenuView.setSearchPanelVisible(false);
    mMenuView.inactivate();

    if (mContextMenu)
        mContextMenu->close();
    
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Handles button visibility
 */
void HsInstalledAppsState::setEmptyLabelVisibility(bool visibility)
{
    if(visibility){
        mMenuView.setContext(HsInstalledAppsContext,HsEmptyLabelContext);
    } else {
        mMenuView.setContext(HsInstalledAppsContext,HsItemViewContext);
    }
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsInstalledAppsState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Method invoked when a list item is activated.
 \param index indec of activated item.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsInstalledAppsState::listItemActivated(const QModelIndex &/*index*/)
{
    //no implementation yet
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Handles long-item-pressed event in all apps view by showing context menu
 \param item the event pertains to
 \param coords press point coordinates.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsInstalledAppsState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::listItemLongPressed");

    // create context menu
    mContextMenu = new HbMenu();

    HbAction *uninstallAction = mContextMenu->addAction(
                                    hbTrId("txt_common_menu_delete"));
    HbAction *appDetailsAction(NULL);
    uninstallAction->setData(UninstallContextAction);
    
    QSharedPointer<const CaEntry> entry = mInstalledAppsModel->entry(item->modelIndex());
    EntryFlags flags = item->modelIndex().data(
                           CaItemModel::FlagsRole).value<EntryFlags> ();
    
    if (!(entry->attribute(componentIdAttributeName()).isEmpty()) && 
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_details"));
        appDetailsAction->setData(AppDetailsContextAction);
    }      

    mContextModelIndex = item->modelIndex();
    mContextMenu->setPreferredPos(coords);
    mContextMenu->setAttribute(Qt::WA_DeleteOnClose);
    mContextMenu->open(this, SLOT(contextMenuAction(HbAction*)));
    
    
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::listItemLongPressed");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Handles context menu actions
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
    mMenuView.setSearchPanelVisible(false);
}

/*!
 Slot invoked when a back action is triggered.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsInstalledAppsState::backAction()
{
    machine()->postEvent(HsMenuEventFactory::createOpenAppLibraryEvent());
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
