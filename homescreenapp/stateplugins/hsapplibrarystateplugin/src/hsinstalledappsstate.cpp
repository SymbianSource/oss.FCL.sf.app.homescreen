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
#include "hsinstalledappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"
#include "hsmenumodetransition.h"

/*!
 \class HsInstalledAppsState
 \ingroup group_hsapplibrarystateplugin
 \brief Manages displaying installed applications
 */

/*!
 Constructor.
 \param menuView menu view.
 \param parent Pwner.
 */
HsInstalledAppsState::HsInstalledAppsState(HsMenuView &menuView, QState *parent) :
    QState(parent), mMenuView(menuView), mInstalledAppsModel(0)
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

    mSecondarySoftkeyAction = new HbAction(Hb::BackAction, this);

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

    if (!mInstalledAppsModel) {
        mInstalledAppsModel
        = HsMenuService::getInstalledModel(AscendingNameHsSortAttribute);
    }
    mMenuView.setModel(mInstalledAppsModel);
    mMenuView.scrollTo(mInstalledAppsModel->index(0));
    setMenuOptions();

    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SLOT(backAction()));
    mMenuView.hideToolBar();
    mOldNavigationAction = mMenuView.view()->navigationAction();
    mMenuView.view()->setNavigationAction(mSecondarySoftkeyAction);
    mMenuView.setLabelVisible(true);
    mMenuView.setLabel(hbTrId("txt_applib_subtitle_installed"));
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsInstalledAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateExited");
    mMenuView.setSearchPanelVisible(false);
    // revert navigation action
    mMenuView.view()->setNavigationAction(mOldNavigationAction);
    mMenuView.showToolBar();
    mMenuView.setLabelVisible(false);
    mMenuView.disconnect(this);
    mMenuView.view()->setMenu(NULL);

    disconnect(mSecondarySoftkeyAction, SIGNAL(triggered()),
               this, SLOT(backAction()));

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
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
    HbMenu *menu = new HbMenu();

    HbAction *uninstallAction = menu->addAction(
                                    hbTrId("txt_common_menu_delete"));

    // choose proper action
    if (HbAction *selectedAction = menu->exec(coords)) {
        if (selectedAction == uninstallAction) {
            const int itemId = item->modelIndex().data(
                                   CaItemModel::IdRole).toInt();
            HsMenuService::executeAction(itemId, removeActionIdentifier());
        }
        mMenuView.setSearchPanelVisible(false);
    }
    delete menu;
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::listItemLongPressed");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

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
