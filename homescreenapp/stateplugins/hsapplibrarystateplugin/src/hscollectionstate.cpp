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
 * Description: Menu collection state.
 *
 */

#include <qstatemachine.h>

#include <HbGroupBox>
#include <HbListView>
#include <HbPushButton>
#include <hbview.h>
#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <hsmenueventfactory.h>
#include <hbmenu.h>
#include <hbinstance.h>
#include <hbsearchpanel.h>

#include "hsmenuevent.h"
#include "hsmenuservice.h"
#include "hsmenumodewrapper.h"
#include "hsmenuitemmodel.h"
#include "hsmenuviewbuilder.h"
#include "cadefs.h"
#include "caentry.h"
#include "hscollectionstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"
#include "hsmainwindow.h"

/*!
 \class HsCollectionState
 \ingroup group_hsmenustateplugin
 \brief Collection State.
 Displays a concrete collection.
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsCollectionState::mSortAttribute
 Sort order.
 */

/*!
 \var HsCollectionState::mCollectionsSortAttribute
 All collections sort order.
 */

/*!
 \var HsCollectionState::mCollectionId
 The id of the current collection.
 */

/*!
 \var HsCollectionState::mCollectionType
 The type of the current collection.
 */

/*!
 \var HsCollectionState::mMenuView
 The List View widget.
 */

/*!
 \var HsCollectionState::mSecondarySoftkeyAction
 Secondary Softkey action.
 Backstepping functionality.
 Own.
 */

/*!
 \var HsCollectionState::mOldNavigationAction
 Old navigation icon.
 Not own.
 */

/*!
 \var HsCollectionState::mCollectionModel
 Item Model for the List.
 Own.
 */

/*!
 \var HsCollectionState::mOptions
 Options menu.
 Own.
 */

/*!
  \fn void HsCollectionState::sortOrderChanged(SHsSortAttribute sortOrder);
  This signal is emitted when sort order is changed to \a sortOrder.
  \param sortAttribute sort attribute
 */

/*!
  \fn void HsCollectionState::constructMenu(bool isDynamic);
  Set up Menu.
  \param isDynamic Decides which options should be added to menu.
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode reference to object representing menu mode (add mode on/add mode off).
 \param parent Owner.
*/
HsCollectionState::HsCollectionState(HsMenuViewBuilder &menuViewBuilder,
                                     HsMenuModeWrapper &menuMode,
                                     HsMainWindow &mainWindow,
                                     QState *parent) :
    HsBaseViewState(parent),
    mSortAttribute(LatestOnTopHsSortAttribute),
    mCollectionsSortAttribute(CustomHsSortAttribute),
    mCollectionId(-1),
    mMenuView(menuViewBuilder, HsCollectionContext),
    mMenuMode(menuMode),
    mSecondarySoftkeyAction(new HbAction(Hb::BackNaviAction, this)),
    mCollectionModel(0),
    mOptions(0), mContextModelIndex(), mContextMenu(0),
    mMainWindow(mainWindow)
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsCollectionState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::construct");

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/collectionstate");

    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    
    mMenuView.collectionButton()->setCheckable(true);   
    makeConnect();
    mMenuView.view()->setNavigationAction(mSecondarySoftkeyAction);

    HSMENUTEST_FUNC_EXIT("HsCollectionState::construct");
}

/*!
 Destructor.
 */
HsCollectionState::~HsCollectionState()
{
    makeDisconnect();
    mMenuView.inactivate();
    mMenuView.setModel(NULL);
    mMenuView.view()->setNavigationAction(NULL);
    delete mCollectionModel;
}


/*!
 Sets entry event.
 \param event entry event.
 */
void HsCollectionState::onEntry(QEvent *event)
{
    qDebug("HsCollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::onEntry");
    QState::onEntry(event);

    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();
        mCollectionId = data.value(itemIdKey()).toInt();
        mCollectionType = data.value(entryTypeNameKey()).toString();
    }

    HSMENUTEST_FUNC_EXIT("HsCollectionState::onEntry");
}

/*!
 Slot invoked when a state is entered.
 */


void HsCollectionState::stateEntered()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::stateEntered");

    mMainWindow.setCurrentView(mMenuView);
    mMenuView.activate();

    if (!mCollectionModel) {
        mCollectionModel =
            HsMenuService::getCollectionModel(
                mCollectionId, mSortAttribute, mCollectionType);
    }
    
    EntryFlags flags =
        mCollectionModel->root().data(CaItemModel::FlagsRole).value<
        EntryFlags> ();

    if (mCollectionModel->rowCount() == 0){
        if (flags & RemovableEntryFlag){
            mMenuView.setContext(HsCollectionContext,HsButtonContext);
        } else {
            mMenuView.setContext(HsCollectionContext,HsEmptyLabelContext);
        }
        mMenuView.disableSearch(true);
    } else {
        mMenuView.setContext(HsCollectionContext,HsItemViewContext);
        mMenuView.disableSearch(false);
    }

    connect(mCollectionModel, SIGNAL(modelReset()), SLOT(updateLabel()));
    connect(mCollectionModel, SIGNAL(empty(bool)),this,
            SLOT(handleEmptyChange(bool)));
    connect(mCollectionModel, SIGNAL(empty(bool)),this,
            SLOT(lockSearchButton(bool)));

    mMenuView.setModel(mCollectionModel);

    mMenuView.listView()->scrollTo(
        mCollectionModel->index(0), HbAbstractItemView::PositionAtTop);

    mMenuView.viewLabel()->setHeading(
        mCollectionModel->root().data(Qt::DisplayRole).toString());

    if (!mOptions) {
        mOptions = new HbMenu();
        mOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                            this,
                            SLOT(openTaskSwitcher()));

        if (flags & RemovableEntryFlag) {
            mOptions->addAction(hbTrId("txt_applib_opt_add_content"), this,
                                SLOT(addAppsAction()));
        }

        mOptions->addAction(hbTrId("txt_applib_opt_add_to_home_screen"),
                            this, SLOT(addCollectionShortcutToHomeScreenAction()));

        if (flags & RemovableEntryFlag) {
            mOptions->addAction(
                hbTrId("txt_common_opt_rename_item"),
                this,
                SLOT(renameAction()));
            mOptions->addAction(
                hbTrId("txt_common_opt_delete"),
                this,
                SLOT(deleteAction()));
        }
        if (mCollectionType == collectionDownloadedTypeName()) {
            HbMenu *sortMenu = mOptions->addMenu(
                                   hbTrId("txt_applib_opt_sort_by"));
            //Grouped options are exclusive by default.
            QActionGroup *sortGroup = new QActionGroup(mOptions);
            sortGroup->addAction(
                sortMenu->addAction(
                    hbTrId("txt_applib_opt_sort_by_sub_latest_on_top"),
                    this,
                    SLOT(latestOnTopMenuAction())));
            sortGroup->addAction(
                sortMenu->addAction(
                    hbTrId("txt_applib_opt_sort_by_sub_oldest_on_top"),
                    this,
                    SLOT(oldestOnTopMenuAction())));
            foreach(QAction *action, sortMenu->actions()) {
                action->setCheckable(true);
            }
            static const int defaultSortingPosition = 0;
            sortGroup->actions().at(defaultSortingPosition)->setChecked(true);
        }

        mOptions->setParent(this);
        mMenuView.view()->setMenu(mOptions);
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsCollectionState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::stateExited");

    mMenuView.inactivate();
    mMenuView.setSearchPanelVisible(false);
    mMenuView.disableSearch(false);
    disconnect(mCollectionModel, SIGNAL(empty(bool)),this,
               SLOT(handleEmptyChange(bool)));

    disconnect(mCollectionModel, SIGNAL(empty(bool)),this,
               SLOT(lockSearchButton(bool)));

    disconnect(mCollectionModel, SIGNAL(modelReset()),
                   this, SLOT(updateLabel()));
    delete mCollectionModel;
    mCollectionModel = NULL;
    mOptions->close();
    delete mOptions;
    mOptions = NULL;
    if (mContextMenu)
        mContextMenu->close();
    this->mSortAttribute = NoHsSortAttribute;
    
    HsBaseViewState::stateExited();
    
    HSMENUTEST_FUNC_EXIT("HsCollectionState::stateExited");
    qDebug("CollectionState::stateExited()");
}

/*!
 Perform all signal connections.
 */
void HsCollectionState::makeConnect()
{
    connect(mSecondarySoftkeyAction, SIGNAL(triggered()),
            SLOT(backSteppingAction()));
    connect(&mMenuView,
            SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
    connect(mMenuView.collectionButton(),
            SIGNAL(toggled(bool)), this, SLOT(addAppsAction(bool)));
}

/*!
 Perform all signal disconnections.
 */
void HsCollectionState::makeDisconnect()
{
    disconnect(mMenuView.collectionButton(),
            SIGNAL(toggled(bool)), this, SLOT(addAppsAction(bool)));

    disconnect(mSecondarySoftkeyAction, SIGNAL(triggered()),
               this, SLOT(backSteppingAction()));

    disconnect(&mMenuView,
               SIGNAL(activated(QModelIndex)),
               this, SLOT(listItemActivated(QModelIndex)));

    disconnect(&mMenuView,
               SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
               this, SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
}

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsCollectionState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}

/*!
 Slot connected to List widget.
 \param index Model index of the activated item.
 */
void HsCollectionState::listItemActivated(const QModelIndex &index)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::listItemActivated");

    QSharedPointer<const CaEntry> entry = mCollectionModel->entry(index);

    if (entry->entryTypeName() == widgetTypeName()) {
        EntryFlags flags = index.data(CaItemModel::FlagsRole).value<
                               EntryFlags> ();
        if (!(flags & UninstallEntryFlag)) {
            machine()->postEvent(
                HsMenuEventFactory::createPreviewHSWidgetEvent(
                    entry->id(), entry->entryTypeName(), entry->attribute(
                        widgetUriAttributeName()), entry->attribute(
                        widgetLibraryAttributeName())));

            const int itemId = index.data(CaItemModel::IdRole).toInt();
            HsMenuService::touch(itemId);
        }
    } else {
        QVariant data = mCollectionModel->data(index, CaItemModel::IdRole);
        int errCode = HsMenuService::executeAction(data.toInt());
        if (errCode != 0) {
            createApplicationLaunchFailMessage(errCode,index.data(CaItemModel::IdRole).toInt());
        }
    }

    mMenuView.setSearchPanelVisible(false);
    HSMENUTEST_FUNC_EXIT("HsCollectionState::listItemActivated");
}

/*!
 Handles long-item-pressed event in all apps view by showing context menu
 \param item the event pertains to
 \param coords press point coordinates.
 \retval void
 */
void HsCollectionState::listItemLongPressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::listItemLongPressed");

    EntryFlags flags = item->modelIndex().data(CaItemModel::FlagsRole).value<
                       EntryFlags> ();

    if (!(flags & UninstallEntryFlag)) {
        // create context menu
        mContextMenu = new HbMenu();

        HbAction *addShortcutAction = mContextMenu->addAction(hbTrId(
                                          "txt_applib_menu_add_to_home_screen"));
        addShortcutAction->setData(AddToHomeScreenContextAction);
        HbAction *addToCollection = mContextMenu->addAction(hbTrId(
                                        "txt_applib_menu_add_to_collection"));
        addToCollection->setData(AddToCollectionContextAction);
        HbAction *removeAction(NULL);
        HbAction *uninstallAction(NULL);
        HbAction *appSettingsAction(NULL);
        HbAction *appDetailsAction(NULL);
        // we do not add remove option in locked collection
        // check conditions and hide irrelevant menu items
        EntryFlags rootFlags =
            mCollectionModel->root().data(CaItemModel::FlagsRole).value<
            EntryFlags> ();

        if (rootFlags & RemovableEntryFlag) {
            removeAction = mContextMenu->addAction(
                               hbTrId("txt_applib_menu_remove_from_collection"));
            removeAction->setData(RemoveFromCollectionContextAction);
        }

        if ((flags & RemovableEntryFlag)) {
            uninstallAction = mContextMenu->addAction(hbTrId("txt_common_menu_delete"));
            uninstallAction->setData(UninstallContextAction);
        }
        QSharedPointer<const CaEntry> entry = mCollectionModel->entry(item->modelIndex());

        if (!(entry->attribute(appSettingsPlugin()).isEmpty())) {
            appSettingsAction = mContextMenu->addAction(hbTrId(
                                                    "txt_common_menu_settings"));
            appSettingsAction->setData(AppSettingContextAction);
        }
    
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
    }

    HSMENUTEST_FUNC_EXIT("HsCollectionState::listItemLongPressed");
}

/*!
 Handles context menu actions
 */
void HsCollectionState::contextMenuAction(HbAction *action)
{
    HsContextAction command = 
        static_cast<HsContextAction>(action->data().toInt());

    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();

    switch (command) {
        case AddToHomeScreenContextAction:
            addElementToHomeScreen(mContextModelIndex);
            break;
        case AddToCollectionContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createAddAppsFromCallectionViewEvent(
                    mCollectionId, itemId, mCollectionsSortAttribute));
            break;
        case UninstallContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createUninstallApplicationEvent(
                    itemId));
            break;
        case RemoveFromCollectionContextAction:
            machine()->postEvent(
                HsMenuEventFactory::createRemoveAppFromCollectionEvent(
                    itemId, mCollectionId));
            break;
        case AppSettingContextAction: 
            machine()->postEvent(
                HsMenuEventFactory::createAppSettingsViewEvent(itemId));
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
 Handles button visibility
  \param empty if true set empty text label or button to add entries to collection
 */
void HsCollectionState::handleEmptyChange(bool empty)
{
    EntryFlags flags =
        mCollectionModel->root().data(CaItemModel::FlagsRole).value<
        EntryFlags> ();

    if (empty){
        if (flags & RemovableEntryFlag){
            mMenuView.setContext(HsCollectionContext,HsButtonContext);
        } else {
            mMenuView.setContext(HsCollectionContext,HsEmptyLabelContext);
        }
    } else {
        mMenuView.setContext(HsCollectionContext,HsItemViewContext);
    }
}


/*!
 Handles lock serch button
  \param lock if true lock search button
 */
void HsCollectionState::lockSearchButton(bool lock)
{
    mMenuView.disableSearch(lock);
}


/*!
 Menu add applications action slot
   \param addApps if true create event for add enties to collection. Parametr use by toggled from HbPushButton
 */
void HsCollectionState::addAppsAction(bool addApps)
{
    // Add applications
    if (addApps) {
        mMenuView.collectionButton()->setChecked(false);
        machine()->postEvent(
            HsMenuEventFactory::createAddAppsFromCallectionViewEvent(
                mCollectionId));
    }    
}

/*!
 Menu add shortcut action slot
 */
void HsCollectionState::addCollectionShortcutToHomeScreenAction()
{
    machine()->postEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                             mCollectionId, mMenuMode.getHsMenuMode(), 
                             mMenuMode.getHsToken()));
}

/*!
 Menu rename action slot
 */
void HsCollectionState::renameAction()
{
    machine()->postEvent(HsMenuEventFactory::createRenameCollectionEvent(
                             mCollectionId));
}

/*!
 Menu delete action slot
 */
void HsCollectionState::deleteAction()
{
    machine()->postEvent(HsMenuEventFactory::createDeleteCollectionEvent(
                             mCollectionId));
}

/*!
 Menu softkey back action slot
 */
void HsCollectionState::backSteppingAction()
{
    machine()->postEvent(HsMenuEventFactory::createOpenAppLibraryEvent());
}

/*!
 Updates label
 */
void HsCollectionState::updateLabel()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::updateLabel");
    if (mCollectionModel) {
        mMenuView.viewLabel()->setHeading(
            mCollectionModel->root().data(Qt::DisplayRole).toString());
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionState::updateLabel");
}

/*!
  Triggers event so that a state adding to Home Screen is reached
  \param index of an item to be added to homescreen
  \retval void
 */
void HsCollectionState::addElementToHomeScreen(const QModelIndex &index)
{
    QSharedPointer<const CaEntry> entry = mCollectionModel->entry(index);

    QMap<QString, QString> attributes = entry->attributes();

    machine()->postEvent(
        HsMenuEventFactory::createAddToHomeScreenEvent(
            entry->id(), mMenuMode.getHsMenuMode(), mMenuMode.getHsToken()));
}

/*!
  A Slot called when an action for sorting (latest
  on top) is invoked for Downloaded collection.
 */
void HsCollectionState::latestOnTopMenuAction()
{
    mSortAttribute = LatestOnTopHsSortAttribute;
    mCollectionModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
}

/*!
  A Slot called when an action for sorting (oldest
  on top) is invoked for Downloaded collection.
 */
void HsCollectionState::oldestOnTopMenuAction()
{
    mSortAttribute = OldestOnTopHsSortAttribute;
    mCollectionModel->setSort(mSortAttribute);
    emit sortOrderChanged(mSortAttribute);
}

/*!
  Slot for setting all collections sort order
  \param sortOrder all collections sort order
 */
void HsCollectionState::collectionsSortOrder(
    HsSortAttribute sortOrder)
{
    mCollectionsSortAttribute = sortOrder;
}