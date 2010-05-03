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
#include <hbview.h>
#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <hsmenueventfactory.h>
#include <hbmenu.h>
#include <hbinstance.h>
#include <hbsearchpanel.h>

#include "hsmenuevent.h"
#include "hsmenuservice.h"
#include "hsmenuitemmodel.h"
#include "hsmenuview.h"
#include "cadefs.h"
#include "caentry.h"
#include "hscollectionstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapp_defs.h"

/*!
 Constructor.
 \param menuView Menu view.
 \param parent Owner.
*/
HsCollectionState::HsCollectionState(HsMenuView &menuView, QState *parent) :
    QState(parent),
    mSortAttribute(LatestOnTopHsSortAttribute),
    mCollectionsSortAttribute(CustomHsSortAttribute),
    mCollectionId(-1), mMenuView(menuView),
    mCollectionModel(0), mOptions(0)
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

    //new action for backstepping
    mSecondarySoftkeyAction = new HbAction(Hb::BackAction, this);
    connect(this, SIGNAL(entered()),SLOT(stateEntered()));
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
    HSMENUTEST_FUNC_EXIT("HsCollectionState::construct");
}

/*!
 Destructor.
 */
HsCollectionState::~HsCollectionState()
{
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
    if (!mCollectionModel) {
        mCollectionModel = HsMenuService::getCollectionModel(mCollectionId,
                           mSortAttribute, mCollectionType);
    } else {
        if (mCollectionType == collectionDownloadedTypeName()) {
            mCollectionModel->setFlagsOn(RemovableEntryFlag | VisibleEntryFlag);
        } else {
            mCollectionModel->setFlagsOn(VisibleEntryFlag);
        }

        mCollectionModel->setParentId(mCollectionId);
        mCollectionModel->setSort(
            LatestOnTopHsSortAttribute);
    }

    mMenuView.setModel(mCollectionModel);
    mMenuView.scrollTo(mCollectionModel->index(0),
                       HbAbstractItemView::PositionAtTop);

    mMenuView.setLabelVisible(true);
    mMenuView.setLabel(
        mCollectionModel->root().data(Qt::DisplayRole).toString());


    if (!mOptions) {
        mOptions = new HbMenu();
        mOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                            this,
                            SLOT(openTaskSwitcher()));

        EntryFlags flags =
            mCollectionModel->root().data(CaItemModel::FlagsRole).value<
            EntryFlags> ();

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
    mOldNavigationAction = mMenuView.view()->navigationAction();
    mMenuView.view()->setNavigationAction(mSecondarySoftkeyAction);

    makeConnect();
    HSMENUTEST_FUNC_EXIT("HsCollectionState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsCollectionState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::stateExited");
    mMenuView.setSearchPanelVisible(false);

    makeDisconnect();

    mOptions = NULL;

    mMenuView.view()->setMenu(NULL);
    mMenuView.setLabelVisible(false);

    // revert navigation action
    mMenuView.view()->setNavigationAction(mOldNavigationAction);

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
    connect(&mMenuView, SIGNAL(activated(QModelIndex)),
            SLOT(listItemActivated(QModelIndex)));
    connect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(listItemLongPressed(HbAbstractViewItem *, QPointF)));
    connect(mCollectionModel, SIGNAL(modelReset()),SLOT(updateLabel()));
}

/*!
 Perform all signal disconnections.
 */
void HsCollectionState::makeDisconnect()
{
    disconnect(mSecondarySoftkeyAction, SIGNAL(triggered()),
               this, SLOT(backSteppingAction()));

    disconnect(mCollectionModel, SIGNAL(modelReset()),
               this, SLOT(updateLabel()));

    disconnect(&mMenuView, SIGNAL(activated(QModelIndex)),
               this, SLOT(listItemActivated(QModelIndex)));

    disconnect(&mMenuView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
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
        machine()->postEvent(
            HsMenuEventFactory::createPreviewHSWidgetEvent(
                entry->id(), entry->entryTypeName(), entry->attribute(
                    widgetUriAttributeName()), entry->attribute(
                    widgetLibraryAttributeName())));

        const int itemId = index.data(CaItemModel::IdRole).toInt();
        HsMenuService::touch(itemId);
    } else {
        QVariant data = mCollectionModel->data(index, CaItemModel::IdRole);
        HsMenuService::executeAction(data.toInt());
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
    Q_UNUSED(item);
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::listItemLongPressed");
    int itemId = item->modelIndex().data(CaItemModel::IdRole).toInt();
    // create context menu
    HbMenu *menu = new HbMenu();

    HbAction *addShortcutAction = menu->addAction(hbTrId(
                                      "txt_applib_menu_add_to_home_screen"));
    HbAction *addToCollection = menu->addAction(hbTrId(
                                    "txt_applib_menu_add_to_collection"));
    HbAction *removeAction(NULL);
    HbAction *uninstallAction(NULL);
    HbAction *appSettingsAction(NULL);
    // we do not add remove option in collection download menu
    // check conditions and hide irrelevant menu items
    if (mCollectionType != collectionDownloadedTypeName()) {
        removeAction = menu->addAction(
                           hbTrId("txt_applib_menu_remove_from_collection"));
    }

    EntryFlags flags = item->modelIndex().data(CaItemModel::FlagsRole).value<
                       EntryFlags> ();

    if ((flags & RemovableEntryFlag)) {
        uninstallAction = menu->addAction(hbTrId("txt_common_menu_delete"));
    }
    QSharedPointer<const CaEntry> entry = mCollectionModel->entry(item->modelIndex());

    if (!(entry->attribute(appSettingsPlugin()).isEmpty())) {
        appSettingsAction = menu->addAction(hbTrId(
                                                "txt_common_menu_settings"));
    }
    // choose proper action
    if (HbAction *selectedAction = menu->exec(coords)) {

        if (selectedAction == addShortcutAction) {
            addElementToHomeScreen(item->modelIndex());
        } else if (selectedAction == addToCollection) {
            machine()->postEvent(
                HsMenuEventFactory::createAddAppsFromCallectionViewEvent(
                    mCollectionId, itemId, mCollectionsSortAttribute));
        } else if (selectedAction == uninstallAction) {
            HsMenuService::executeAction(itemId, removeActionIdentifier());
        } else if (selectedAction == removeAction) {
            machine()->postEvent(
                HsMenuEventFactory::createRemoveAppFromCollectionEvent(
                    itemId, mCollectionId));
        } else if (selectedAction == appSettingsAction) {
            const int itemId =
                item->modelIndex().data(CaItemModel::IdRole).toInt();
            QMap<QString, QString> attributes = entry->attributes();
            machine()->postEvent(
                HsMenuEventFactory::createAppSettingsViewEvent(itemId));
        }

        mMenuView.setSearchPanelVisible(false);
    }
    delete menu;
    HSMENUTEST_FUNC_EXIT("HsCollectionState::listItemLongPressed");
}

/*!
 Menu add applications action slot
 */
void HsCollectionState::addAppsAction()
{
    // Add applications
    machine()->postEvent(
        HsMenuEventFactory::createAddAppsFromCallectionViewEvent(
            mCollectionId));
}

/*!
 Menu add shortcut action slot
 */
void HsCollectionState::addCollectionShortcutToHomeScreenAction()
{
    machine()->postEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                             mCollectionId));
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
        mMenuView.setLabel(
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
            entry->id()));
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

/*!
  \fn void HsCollectionState::sortOrderChanged(SHsSortAttribute sortOrder);

  This signal is emitted when sort order is changed to \a sortOrder.
 */
