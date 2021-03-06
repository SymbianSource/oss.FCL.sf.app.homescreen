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
#include <HbMenu>
#include <HbAction>
#include <HbAbstractViewItem>
#include <HbIndexFeedback>
#include <HbListView>
#include <HbView>
#include <HbGroupBox>
#include <HbInstance>
#include <HbMainWindow>

#include <cadefs.h>
#include <caentry.h>
#include <caitemmodel.h>
#include <caservice.h>

#include "hsapp_defs.h"
#include "hsmenueventfactory.h"
#include "hsmenuitemmodel.h"
#include "hsallappsstate.h"
#include "hsmenumodewrapper.h"
#include "hsmenuviewbuilder.h"
#include "hsmainwindow.h"
#include "hsaddmodeproxymodel.h"
#include "hsapplibstateutils.h"

/*!
 \class HsAllAppsState
 \ingroup group_hsmenustateplugin
 \brief All Applications State.
 Displays all applications present on the device in a sorted list.
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsAllAppsState::mSortAttribute
 Sort order
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode Menu mode object(add mode on/add mode off).
 \param mainWindow Main window wrapper.
 \param parent Owner.
 */
HsAllAppsState::HsAllAppsState(HsMenuViewBuilder &menuViewBuilder,
                               HsMenuModeWrapper &menuMode,
                               HsMainWindow &mainWindow,
                               QState *parent) :
    HsBaseViewState(mainWindow, menuMode, parent),
    mAddModeProxyModel(0),
    mSortAttribute(Hs::AscendingNameHsSortAttribute),
    mAscendingMenuAction(0),
    mDescendingMenuAction(0),
    mListMenuAction(0),
    mGridMenuAction(0),
    mPublisher(NULL),
    mSubscriber(NULL)
{
    mPublisher = new QValueSpacePublisher(QValueSpace::PermanentLayer, Hs::KViewAllAppsPath, this);
    mSubscriber = new QValueSpaceSubscriber(QString("%1/%2").arg(Hs::KViewAllAppsPath).arg(Hs::KGridPath), this);
    mGrid = mSubscriber->value().toBool();
    mMainWindow = HbInstance::instance()->allMainWindows().value(0);
    if (mGrid && mMainWindow) {
        if (mMainWindow->orientation() == Qt::Vertical) {
            initialize(menuViewBuilder, HsAllAppsContext, HsGridPortraitContext);
        } else {
            initialize(menuViewBuilder, HsAllAppsContext, HsGridLandscapeContext);
        }
    } else {
        initialize(menuViewBuilder, HsAllAppsContext);
    }
    
    construct();
    mAddModeProxyModel = new HsAddModeProxyModel(this);
    mAddModeProxyModel->setSourceModel(mModel);    
}

/*!
 Constructs contained objects.
 */
void HsAllAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::construct");

    defineTransitions();

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/allappsstate");

    connect(mBackKeyAction, SIGNAL(triggered()), SIGNAL(toAppLibraryState()));

    mModel = HsMenuService::getAllApplicationsModel(mSortAttribute);
    if (mGrid) {
        mModel->setSecondLineVisibility(false);
    }
    mMenuView->setModel(mModel);
    updateLabel();
    connect(mModel, SIGNAL(countChange()), SLOT(updateLabel()));
    
    mModel->preloadIcons();

    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mMenuView->view());
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mMenuView->itemView());

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::construct");
}


/*!
 Creates and installs menu options for the view
 */
void HsAllAppsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::setMenuOptions");
    mViewOptions->clearActions();
    mViewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
        static_cast<HsBaseViewState*>(this), SLOT(openTaskSwitcher()));
    mViewOptions->addAction(hbTrId("txt_applib_opt_add_to_collection"),
        this, SLOT(addToCollection()));
    mAscendingMenuAction =
        mViewOptions->addAction(hbTrId("txt_applib_menu_sort_by_ascending"),
                            this, SLOT(ascendingMenuAction()));
    mDescendingMenuAction =
        mViewOptions->addAction(hbTrId("txt_applib_menu_sort_by_descending"),
            this, SLOT(descendingMenuAction()));
    
    mListMenuAction =
        mViewOptions->addAction(hbTrId("txt_applib_opt_list_view"),
                            this, SLOT(listMenuAction()));
    mGridMenuAction =
        mViewOptions->addAction(hbTrId("txt_applib_opt_grid_view"),
            this, SLOT(gridMenuAction()));
    
    mViewOptions->addAction(hbTrId("txt_applib_opt_check_software_updates"),
        static_cast<HsBaseViewState*>(this), SLOT(checkSoftwareUpdates()));
    mViewOptions->addAction(
        hbTrId("txt_applib_opt_view_installed_applications"),
        static_cast<HsBaseViewState*>(this), SLOT(openInstalledView()));

    if (mSortAttribute == Hs::AscendingNameHsSortAttribute) {
        mAscendingMenuAction->setVisible(false);
    } else {
        mDescendingMenuAction->setVisible(false);
    }
    
    if (mGrid) {
        mGridMenuAction->setVisible(false);
    } else {
        mListMenuAction->setVisible(false);
    }

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::setMenuOptions");
}

/*!
 Destructor.
 */

HsAllAppsState::~HsAllAppsState()
{
    delete mPublisher;
    delete mSubscriber;
}


/*!
 Slot invoked when normal mode entered.
 */
void HsAllAppsState::stateEntered()
{
    orientationChanged(mMainWindow->orientation(), false);
    HsBaseViewState::stateEntered();
    connect(mMainWindow,
            SIGNAL(aboutToChangeOrientation()),
            this,
            SLOT(orientationGoingToBeChanged())
            );    
    connect(mMainWindow,
            SIGNAL(orientationChanged(Qt::Orientation)),
            this,
            SLOT(orientationChanged(Qt::Orientation))
            );
}

/*!
 Slot invoked when normal mode entered.
 */
void HsAllAppsState::normalModeEntered()
{
    HsBaseViewState::normalModeEntered();
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            static_cast<HsBaseViewState*>(this), SLOT(launchItem(QModelIndex)));
}

/*!
 Sets model for this view in depends on menuMode.
 \param menuMode menu mode
 */
void HsAllAppsState::setModel(Hs::HsMenuMode menuMode)
{
    if (menuMode == Hs::AddHsMenuMode) {
        if (mMenuView->model() != mAddModeProxyModel) {
            mMenuView->setModel(mAddModeProxyModel);
        }
    } else {
        if (mMenuView->model() != mModel) {
            mMenuView->setModel(mModel);
        }
    }
}


/*!
 Scrolls view to first item at top
 */
void HsAllAppsState::scrollToBeginning()
{
    QAbstractItemModel* model = mMenuView->model();
    if (model != NULL ) {
        mMenuView->itemView()->scrollTo(
                model->index(0,0), HbAbstractItemView::PositionAtTop);
    }
}

/*!
 Slot invoked when a state is exited.
 */
void HsAllAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::stateExited");
    
    disconnect(mMainWindow,
            SIGNAL(orientationChanged(Qt::Orientation)),
            this,
            SLOT(orientationChanged(Qt::Orientation))
            );
    
    disconnect(mMainWindow,
            SIGNAL(aboutToChangeOrientation()),
            this,
            SLOT(orientationGoingToBeChanged())
            );
    
    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Slot invoked label has to be changed.
 */
void HsAllAppsState::updateLabel()
{
    mMenuView->viewLabel()->setHeading(
            hbTrId("txt_applib_subtitle_applications_widgets_l1").arg(
                    mModel->rowCount()));    
}
/*!
  Slot called when application is adding to collection.
 */
void HsAllAppsState::addToCollection()
{
    // Add one/many applications to
    // a new/an existing collection via the All view
    machine()->postEvent(
        HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
            mSortAttribute,0,
            mMenuView->currentScrollPosition().row()));
}

/*!
 Method seting context menu options.
 */
void HsAllAppsState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    HbAction *openAction = mContextMenu->addAction(hbTrId(
        "txt_common_menu_open"));
    openAction->setData(Hs::OpenAppContextAction);

    HbAction *addToHomeScreenAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_home_screen"));
    addToHomeScreenAction->setData(Hs::AddToHomeScreenContextAction);

    HbAction *addToCollectionAction = mContextMenu->addAction(
        hbTrId("txt_applib_menu_add_to_collection"));

    addToCollectionAction->setData(
        Hs::AddToCollectionFromApplicationsViewContextAction);

    HbAction *appSettingsAction(NULL);
    HbAction *appDetailsAction(NULL);

    // check conditions and hide irrelevant menu items
    QSharedPointer<const CaEntry> entry = mModel->entry(item->modelIndex());

    if (!(entry->attribute(Hs::appSettingsPlugin).isEmpty())) {
        appSettingsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_settings"));
        appSettingsAction->setData(Hs::AppSettingContextAction);
    }

    HbAction *uninstallAction = mContextMenu->addAction(
        hbTrId("txt_common_menu_delete"));
    uninstallAction->setData(Hs::UninstallContextAction);

    if (!(entry->attribute(Hs::componentIdAttributeName).isEmpty()) &&
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_details"));
        appDetailsAction->setData(Hs::AppDetailsContextAction);
    }

    if (!(flags & RemovableEntryFlag)) {
        uninstallAction->setVisible(false);
    }

    addToHomeScreenAction->setVisible(
        !HsAppLibStateUtils::isCWRTWidgetOnHomeScreen(entry.data()));
}

/*!
  Menu ascending sort action slot.
 */
void HsAllAppsState::ascendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::ascendingMenuAction");
    mSortAttribute = Hs::AscendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mAscendingMenuAction->setVisible(false);
    mDescendingMenuAction->setVisible(true);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::ascendingMenuAction");
}

/*!
 Menu descending sort action slot.
 */
void HsAllAppsState::descendingMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::descendingMenuAction");
    mSortAttribute = Hs::DescendingNameHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mAscendingMenuAction->setVisible(true);
    mDescendingMenuAction->setVisible(false);
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::descendingMenuAction");
}

/*!
 Menu list action slot.
 */
void HsAllAppsState::listMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::listMenuAction");
    mGrid = false;
    mPublisher->setValue(Hs::KGridPath, static_cast<int>(false));
    
    mListMenuAction->setVisible(false);
    mGridMenuAction->setVisible(true);
    scrollToBeginning();
    
    mMenuView->reset(HsItemViewContext);
    mModel->setSecondLineVisibility(true);
    mMenuView->setModel(mModel);
    
    if (mViewOptions) {
        mMenuView->view()->setMenu(mViewOptions);
    }
    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mMenuView->view());
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mMenuView->itemView());    
    mMenuView->activate();

    HSMENUTEST_FUNC_EXIT("HsAllAppsState::listMenuAction");
}

/*!
 Menu grid action slot.
 */
void HsAllAppsState::gridMenuAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::gridMenuAction");
    mGrid = true;
    mPublisher->setValue(Hs::KGridPath, static_cast<int>(true));
    scrollToBeginning();
    orientationChanged(mMainWindow->orientation());
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::gridMenuAction");
}

/*!
 Orientation changed slot.
 */
void HsAllAppsState::orientationChanged(Qt::Orientation orientation, 
        bool ifActivate)
{
    HSMENUTEST_FUNC_ENTRY("HsAllAppsState::orientationChanged");
    if (mGrid && mListMenuAction && mGridMenuAction) {
        mListMenuAction->setVisible(true);
        mGridMenuAction->setVisible(false);        
        if (orientation == Qt::Vertical) {
            mMenuView->reset(HsGridPortraitContext);
        } else {
            mMenuView->reset(HsGridLandscapeContext);
        }
        mModel->setSecondLineVisibility(false);        
        mMenuView->setModel(mModel);        
        mMenuView->itemView()->scrollTo(mCurrentVisibleItemIndex, HbAbstractItemView::PositionAtTop);
        mCurrentVisibleItemIndex = QPersistentModelIndex();
        if (mViewOptions) {
            mMenuView->view()->setMenu(mViewOptions);
        }
        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mMenuView->view());
        indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mMenuView->itemView());        

        if (ifActivate) {
            mMenuView->activate();
        }
    }
    HSMENUTEST_FUNC_EXIT("HsAllAppsState::orientationChanged");
}
/*!
 orientationAboutToBeChanged slot.
 getting current first visible item index
 */
void HsAllAppsState::orientationGoingToBeChanged()
    {
    mCurrentVisibleItemIndex = mMenuView->currentScrollPosition();
    }

