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
 * Description: Base for menu view states.
 *
 */

#include <QScopedPointer>
#include <QStateMachine>
#include <HbMessageBox>
#include <HbParameterLengthLimiter>
#include <HbAction>
#include <HbMenu>
#include <HbAbstractViewItem>
#include <HbView>
#include <HbListView>

#include <canotifier.h>

#include "hsbaseviewstate.h"
#include "hsmenueventfactory.h"
#include "hsmenudialogfactory.h"
#include "hsmenuitemmodel.h"
#include "hsmenumodetransition.h"
#include "hsmenuentryremovedhandler.h"
#include "hsmainwindow.h"


/*!
 Constructor.
 \param mainWindow main window wrapper object.
 \param parent Owner.
 */
HsBaseViewState::HsBaseViewState(HsMainWindow &mainWindow, QState *parent):
    QState(parent),
    mApplicationLaunchFailMessage(0),
    mModel(0),
    mContextMenu(0),
    mContextModelIndex(),
    mBackKeyAction(0),
    mMenuView(0),
    mMenuMode(0),
    mMainWindow(mainWindow)
{
    mBackKeyAction = new HbAction(Hb::BackNaviAction, this);
}

/*!
 Constructor.
 \param mainWindow main window wrapper object.
 \param menuMode menu mode object.
 \param parent Owner.
 */
HsBaseViewState::HsBaseViewState(HsMainWindow &mainWindow,
    HsMenuModeWrapper& menuMode,
    QState *parent):
    QState(parent),
    mApplicationLaunchFailMessage(0),
    mModel(0),
    mContextMenu(0),
    mContextModelIndex(),
    mBackKeyAction(0),
    mMenuView(0),
    mMenuMode(&menuMode),
    mMainWindow(mainWindow)
{
    mBackKeyAction = new HbAction(Hb::BackNaviAction, this);
}
/*!
 Initialize contained objects.
 \param menuViewBuilder object providing widgets for menu view.
 \param stateContext state context of the view the builder
     is to provide widgets for.
 */
void HsBaseViewState::initialize(HsMenuViewBuilder &menuViewBuilder,
    HsStateContext stateContext)
{
    mMenuView.reset(new HsMenuView(menuViewBuilder, stateContext, mMainWindow));
    mMenuView->view()->setNavigationAction(mBackKeyAction);

    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
}

/*!
 Creates and open application launch fail message.
 \param errorCode eroor code to display.
 \param itemId id of the launched item.
 */
void HsBaseViewState::createApplicationLaunchFailMessage(int errorCode,
    int itemId)
{
    QString message;
    message.append(
        HbParameterLengthLimiter("txt_applib_info_launching_the_application_failed").arg(
            errorCode));

    // create and show message box
    mApplicationLaunchFailMessage = HsMenuDialogFactory().create(
            message, HsMenuDialogFactory::Close);

    QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
        new HsMenuEntryRemovedHandler(itemId, mApplicationLaunchFailMessage.data(), SLOT(close())));

    entryObserver.take()->setParent(mApplicationLaunchFailMessage.data());

    mApplicationLaunchFailMessage->open();
}

/*!
 Slot invoked when a state is entered.
 */
void HsBaseViewState::stateEntered()
{
    qDebug("HsBaseViewState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::stateEntered");
    mMenuView->activate();
    connect(mMenuView.data(), SIGNAL(listViewChange()),
            this, SLOT(closeContextMenu()));
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsBaseViewState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::stateExited");

    mMenuView->hideSearchPanel();
    mMenuView->disconnect(this);
    mMenuView->view()->setMenu(NULL);
    mMenuView->inactivate();
    if (!mApplicationLaunchFailMessage.isNull()) {
        mApplicationLaunchFailMessage->close();
    }
    closeContextMenu();
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::stateExited");
}

/*!
 Add mode entered.
 */
void HsBaseViewState::addModeEntered()
{
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            SLOT(addActivated(QModelIndex)));
    connect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(addLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Slot invoked when add mode entered.
 */
void HsBaseViewState::normalModeEntered()
{
    setMenuOptions();
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            mMenuView.data(),
            SLOT(hideSearchPanel()));
    connect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));
}

/*!
 Destructor.
 */
HsBaseViewState::~HsBaseViewState()
{
    delete mModel;
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsBaseViewState::launchItem(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::launchItem");

    QSharedPointer<const CaEntry> entry = mModel->entry(index);
    if (!entry.isNull()) {
        if (entry->entryTypeName() == widgetTypeName()) {
            EntryFlags flags = entry->flags();
            if (!(flags & UninstallEntryFlag)) {
                machine()->postEvent(HsMenuEventFactory::createPreviewHSWidgetEvent(entry->id(),
                    entry->entryTypeName(), entry->attribute(widgetUriAttributeName()),
                    entry->attribute(widgetLibraryAttributeName())));
                HsMenuService::touch(entry->id());
            }
        }
        else {
            int errCode = HsMenuService::executeAction(entry->id());
            if (errCode != 0) {
                createApplicationLaunchFailMessage(errCode, entry->id());
            }
        }
    }
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::launchItem");
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsBaseViewState::openCollection(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::openCollection");
    QVariant data = mModel->data(index, CaItemModel::IdRole);
    int id = data.toInt();
    QString collectionType = mModel->data(
            index, CaItemModel::TypeRole).toString();
    qDebug("HsBaseViewState::openCollection - MCS ID: %d", data.toInt());

    machine()->postEvent(
            HsMenuEventFactory::createOpenCollectionFromAppLibraryEvent(
                    id, collectionType));
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::openCollection");
}

/*!
 Slot connected to List widget in normal mode.
 \param index Model index of the activated item.
 */
void HsBaseViewState::showContextMenu(HbAbstractViewItem *item, const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsBaseViewState::showContextMenu");

    EntryFlags flags = item->modelIndex().data(
            CaItemModel::FlagsRole).value<EntryFlags> ();

    if (!(flags & UninstallEntryFlag)) {
        mContextMenu = new HbMenu;
        setContextMenuOptions(item,flags);
        mContextModelIndex = item->modelIndex();
        mContextMenu->setPreferredPos(coords);
        mContextMenu->setAttribute(Qt::WA_DeleteOnClose);
        mContextMenu->open(this, SLOT(contextMenuAction(HbAction*)));
    }
    HSMENUTEST_FUNC_EXIT("HsBaseViewState::showContextMenu");

}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsBaseViewState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Menu softkey back action slot
 */
void HsBaseViewState::openAppLibrary()
{
    machine()->postEvent(HsMenuEventFactory::createOpenAppLibraryEvent());
}

/*!
 Check software updates.
 \retval 0 if operation is successful.
 */
int HsBaseViewState::checkSoftwareUpdates()
{
    int errCode = HsMenuService::launchSoftwareUpdate();
    if (errCode != 0){
        createApplicationLaunchFailMessage(errCode,0);
    }
    return errCode;
}

/*!
 close context menu
 */
void HsBaseViewState::closeContextMenu()
{
    if (mContextMenu) {
        mContextMenu->close();
    }    
}
/*!
 Scrolls view to first item at top
 */
void HsBaseViewState::scrollToBeginning()
{
    mMenuView->listView()->scrollTo(
            mModel->index(0), HbAbstractItemView::PositionAtTop);
}

/*!
 Normal mode exited dummy implementation.
 */
void HsBaseViewState::normalModeExited()
{
}

/*!
 Defines transitions
 */
void HsBaseViewState::defineTransitions()
{
    QState *initialState = new QState(this);
    setInitialState(initialState);

    QState *addModeState = new QState(this);
    connect(addModeState, SIGNAL(entered()),SLOT(addModeEntered()));

    QState *normalModeState = new QState(this);
    connect(normalModeState, SIGNAL(entered()),SLOT(normalModeEntered()));
    connect(normalModeState, SIGNAL(exited()),SLOT(normalModeExited()));

    initialState->addTransition(new HsMenuModeTransition(
            *mMenuMode, NormalHsMenuMode, normalModeState));
    initialState->addTransition(new HsMenuModeTransition(
            *mMenuMode, AddHsMenuMode, addModeState));
}
