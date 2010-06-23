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
* Description:
*
*/

#include <QStateMachine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDir>

#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbAction>
#include <HbIcon>
#include <HbMessageBox>
#include <HbLabel>
#include <HbInstantFeedback>
#include <HbContinuousFeedback>
#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <startupdomainpskeys.h>
#endif
#include "hsidlestate.h"
#include "hsidlewidget.h"
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hswallpaperselectionstate.h"
#include "hstrashbinwidget.h"
#include "hspageindicator.h"
#include "hsmenueventfactory.h"
#include "hshomescreenstatecommon.h"
#include "hstitleresolver.h"
#include "hsmenuservice.h"
#include "hsgui.h"
#include "hsconfiguration.h"

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));

// Helper macros for connecting and disconnecting mouse event handlers.
#define CONNECT_MOUSE_EVENT_HANDLER(signal, slot) \
    connect(mUiWidget, SIGNAL(signal(QGraphicsItem*,QGraphicsSceneMouseEvent*,bool&)), \
        SLOT(slot(QGraphicsItem*,QGraphicsSceneMouseEvent*,bool&)));
#define DISCONNECT_MOUSE_EVENT_HANDLER(signal, slot) \
    disconnect(mUiWidget, SIGNAL(signal(QGraphicsItem*,QGraphicsSceneMouseEvent*,bool&)), \
        this, SLOT(slot(QGraphicsItem*,QGraphicsSceneMouseEvent*,bool&)));


namespace
{
    const char gApplicationLibraryIconName[] = "qtg_mono_applications_all";

    //User adds a new page to home screen
    const char hsLocTextId_OptionsMenu_AddPage[] = "txt_homescreen_opt_add_page";

    //User removes page from home screen
    const char hsLocTextId_OptionsMenu_RemovePage[] = "txt_homescreen_opt_remove_page";

    //Sends request to all widgets to enable data connections
    const char hsLocTextId_OptionsMenu_HsToOnline[] = "txt_homescreen_opt_home_screen_to_online";

    //Sends request to all widgets to disable data connections
    const char hsLocTextId_OptionsMenu_HsToOffline[] = "txt_homescreen_opt_home_screen_to_offline";

    //Home screen canvas menu item for opening picture gallery
    const char hsLocTextId_ContextMenu_ChangeWallpaper[] = "txt_homescreen_list_change_wallpaper";

    //Home screen canvas menu item for opening Application library
    const char hsLocTextId_ContextMenu_AddContent[] = "txt_homescreen_list_add_content";

    //Home screen options menu item for opening Task switcher
    const char hsLocTextId_OptionsMenu_TaskSwitcher[] = "txt_homescreen_opt_task_switcher";

    //Heading text in confirmation dialog while removing page with content
    const char hsLocTextId_Title_RemovePage[] = "txt_homescreen_title_remove_page";

    //Text in confirmation dialog while removing page with content
    const char hsLocTextId_Confirmation_RemovePage[] = "txt_homescreen_info_page_and_content_will_be_remov";

    //Button in confirmation dialog while removing page with content
    const char hsLocTextId_ConfirmationButton_Ok[] = "txt_homescreen_button_ok";

    //Button in confirmation dialog while removing page with content
    const char hsLocTextId_ConfirmationButton_Cancel[] = "txt_homescreen_button_cancel";
}

/*!
    \class HsIdleState
    \ingroup group_hshomescreenstateplugin
    \brief Controller part of the home screen idle state.

    Controls the home screen idle state execution. See the
    state chart below for the state structure.

    \imageDEPRECATED html hsidlestate_statechart.png
*/

/*!
    Constructs a new idle state with the given \a parent.
*/
HsIdleState::HsIdleState(QState *parent)
  : QState(parent),
    mNavigationAction(0), mUiWidget(0),
    mTitleResolver(0),
    mZoneAnimation(0),
    mAllowZoneAnimation(false),
    mPageChangeAnimation(0),
    mContinuousFeedback(0),
    mTrashBinFeedbackAlreadyPlayed(false),
    mDeltaX(0),
    mSceneMenu(0)
#ifdef Q_OS_SYMBIAN
    ,mSettingsMgr(0)
#endif
{
    setupStates();
    mTimer.setSingleShot(true);
    mTitleResolver = new HsTitleResolver(this);
	
    // TODO: Uncomment when updated API available
    //mContinuousFeedback = new HbContinuousFeedback;
    //mContinuousFeedback->setContinuousEffect(HbFeedback::ContinuousSmooth);
    //mContinuousFeedback->setIntensity(HbFeedback::IntensityFull );
}

/*!
    Destroys this idle state.
*/
HsIdleState::~HsIdleState()
{
    delete mZoneAnimation;
    // TODO: Uncomment when updated API available
    //delete mContinuousFeedback;
}

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsIdleState::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
        case QEvent::ApplicationActivate:
            action_idle_layoutNewWidgets();
            break;
        case QEvent::ApplicationDeactivate:
            mUiWidget->clearDelayedPress();
            emit event_waitInput();
            break;
        default:
            break;
    }
    return QState::eventFilter(watched, event);
}

/*!
    \fn HsIdleState::event_applicationLibrary()

    This signal initiates a transition to application library.
*/

/*!
    \fn HsIdleState::event_waitInput()

    This signal initiates a transition to the waitInput state.
*/

/*!
    \fn HsIdleState::event_widgetInteraction()

    This signal initiates a transition to the widgetInteraction state.
*/

/*!
    \fn HsIdleState::event_sceneInteraction()

    This signal initiates a transition to the sceneInteraction state.
*/

/*!
    \fn HsIdleState::event_moveWidget()

    This signal initiates a transition to the moveWidget state.
*/

/*!
    \fn HsIdleState::event_moveScene()

    This signal initiates a transition to the moveScene state.
*/

/*!
    \fn HsIdleState::event_sceneMenu()

    This signal initiates a transition to the sceneMenu state.
*/

/*!
    \fn HsIdleState::event_selectWallpaper()

    This signal initiates a transition to the selectSceneWallpaper state.
*/

/*!
    \fn HsIdleState::event_addPage()

    This signal initiates a transition to the addPage state.
*/

/*!
    \fn HsIdleState::event_removePage()

    This signal initiates a transition to the removePage state.
*/

/*!
    \fn HsIdleState::event_toggleConnection()

    This signal initiates a transition to the toggleConnection state.
*/

/*!
    Creates the internal state structure and connects
    state entry and exit actions.
*/
void HsIdleState::setupStates()
{
    // States

    QState *state_waitInput = new QState(this);
    setInitialState(state_waitInput);
    QState *state_widgetInteraction = new QState(this);
    QState *state_sceneInteraction = new QState(this);
    QState *state_moveWidget = new QState(this);
    QState *state_moveScene = new QState(this);
    QState *state_sceneMenu = new QState(this);
    HsWallpaperSelectionState *state_wallpaperSelectionState =
        new HsWallpaperSelectionState(this);
    QState *state_addPage = new QState(this);
    QState *state_removePage = new QState(this);
    QState *state_toggleConnection = new QState(this);

    // Transitions

    state_waitInput->addTransition(
        this, SIGNAL(event_widgetInteraction()), state_widgetInteraction);
    state_waitInput->addTransition(
        this, SIGNAL(event_sceneInteraction()), state_sceneInteraction);
    state_waitInput->addTransition(
        this, SIGNAL(event_addPage()), state_addPage);
    state_waitInput->addTransition(
        this, SIGNAL(event_removePage()), state_removePage);
    state_waitInput->addTransition(
        this, SIGNAL(event_toggleConnection()), state_toggleConnection);
    state_waitInput->addTransition(
        this, SIGNAL(event_selectWallpaper()), state_wallpaperSelectionState);

    state_widgetInteraction->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);
    state_widgetInteraction->addTransition(
        this, SIGNAL(event_moveWidget()), state_moveWidget);
    state_widgetInteraction->addTransition(
        this, SIGNAL(event_moveScene()), state_moveScene);

    state_sceneInteraction->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);
    state_sceneInteraction->addTransition(
        this, SIGNAL(event_moveScene()), state_moveScene);
    state_sceneInteraction->addTransition(
        this, SIGNAL(event_sceneMenu()), state_sceneMenu);

    state_moveWidget->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);

    state_moveScene->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);

    state_sceneMenu->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);
    state_sceneMenu->addTransition(
        this, SIGNAL(event_selectWallpaper()), state_wallpaperSelectionState);
    state_sceneMenu->addTransition(
        this, SIGNAL(event_addPage()), state_addPage);

    state_wallpaperSelectionState->addTransition(
        state_wallpaperSelectionState, SIGNAL(event_waitInput()), state_waitInput);

    state_addPage->addTransition(state_waitInput);

    state_removePage->addTransition(state_waitInput);

    state_toggleConnection->addTransition(state_waitInput);

    // Actions

    ENTRY_ACTION(this, action_idle_setupView)
    ENTRY_ACTION(this, action_idle_layoutNewWidgets)
    ENTRY_ACTION(this, action_idle_showActivePage)
    ENTRY_ACTION(this, action_idle_connectOrientationChangeEventHandler)
    ENTRY_ACTION(this, action_idle_setupTitle)
    ENTRY_ACTION(this, action_idle_installEventFilter)
    EXIT_ACTION(this, action_idle_cleanupTitle)
    EXIT_ACTION(this, action_idle_cleanupView)
    EXIT_ACTION(this, action_idle_disconnectOrientationChangeEventHandler)
    EXIT_ACTION(this, action_idle_uninstallEventFilter)

    ENTRY_ACTION(state_waitInput, action_waitInput_updateOptionsMenu)
    ENTRY_ACTION(state_waitInput, action_waitInput_connectMouseEventHandlers)
    ENTRY_ACTION(state_waitInput, action_waitInput_publishIdleKey)
    EXIT_ACTION(state_waitInput, action_waitInput_disconnectMouseEventHandlers)
    EXIT_ACTION(state_waitInput, action_waitInput_resetNewWidgets)

    ENTRY_ACTION(state_widgetInteraction, action_widgetInteraction_connectMouseEventHandlers)
    ENTRY_ACTION(state_widgetInteraction, action_widgetInteraction_connectGestureTimers)
    EXIT_ACTION(state_widgetInteraction, action_widgetInteraction_disconnectMouseEventHandlers)
    EXIT_ACTION(state_widgetInteraction, action_widgetInteraction_disconnectGestureTimers)

    ENTRY_ACTION(state_sceneInteraction, action_sceneInteraction_connectMouseEventHandlers)
    ENTRY_ACTION(state_sceneInteraction, action_sceneInteraction_connectGestureTimers)
    EXIT_ACTION(state_sceneInteraction, action_sceneInteraction_disconnectMouseEventHandlers)
    EXIT_ACTION(state_sceneInteraction, action_sceneInteraction_disconnectGestureTimers)

    ENTRY_ACTION(state_moveWidget, action_moveWidget_reparentToControlLayer)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_startWidgetDragEffect)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectMouseEventHandlers)

    EXIT_ACTION(state_moveWidget, action_moveWidget_reparentToPage)
    EXIT_ACTION(state_moveWidget, action_moveWidget_startWidgetDropEffect)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectMouseEventHandlers)
    EXIT_ACTION(state_moveWidget, action_moveWidget_preventZoneAnimation)

    ENTRY_ACTION(state_moveScene, action_moveScene_connectMouseEventHandlers)
    EXIT_ACTION(state_moveScene, action_moveScene_moveToNearestPage)
    EXIT_ACTION(state_moveScene, action_moveScene_disconnectMouseEventHandlers)

    ENTRY_ACTION(state_sceneMenu, action_sceneMenu_showMenu)

    ENTRY_ACTION(state_addPage, action_disableUserInteraction)
    ENTRY_ACTION(state_addPage, action_addPage_addPage)
    EXIT_ACTION(state_addPage, action_enableUserInteraction)

    ENTRY_ACTION(state_removePage, action_disableUserInteraction)
    ENTRY_ACTION(state_removePage, action_removePage_removePage)
    EXIT_ACTION(state_removePage, action_enableUserInteraction)

    ENTRY_ACTION(state_toggleConnection, action_disableUserInteraction)
    ENTRY_ACTION(state_toggleConnection, action_toggleConnection_toggleConnection)
    EXIT_ACTION(state_toggleConnection, action_enableUserInteraction)
}

/*!
    Computes the page layer x position based on the given \a pageIndex.
*/
qreal HsIdleState::pageLayerXPos(int pageIndex) const
{
    return -pageIndex * HsScene::mainWindow()->layoutRect().width();
}

/*!
    Starts the page change animation based on the given \a targetPageIndex
    and \a duration.
*/
void HsIdleState::startPageChangeAnimation(int targetPageIndex, int duration)
{
    if (!mPageChangeAnimation) {
        mPageChangeAnimation = new QPropertyAnimation(mUiWidget, "sceneX");        
    }
    else if (mPageChangeAnimation->state() != QAbstractAnimation::Stopped) {
        mPageChangeAnimation->stop();
    }        
    
    mPageChangeAnimation->setEndValue(pageLayerXPos(targetPageIndex));
    mPageChangeAnimation->setDuration(duration);
    connect(mPageChangeAnimation,
            SIGNAL(finished()),
            SLOT(pageChangeAnimationFinished()));
    mPageChangeAnimation->start();
    
    HbInstantFeedback::play(HSCONFIGURATION_GET(pageChangeFeedbackEffect));
    
    mUiWidget->showPageIndicator();
    mUiWidget->setActivePage(targetPageIndex);
}

/*!
    Starts the page change zone animation (crawling) based on the given a\ duration
*/
void HsIdleState::startPageChangeZoneAnimation(int duration)
{
    HsScene *scene = HsScene::instance();

    int targetPageIndex = scene->activePageIndex();

    if (isInLeftPageChangeZone() && 0 < targetPageIndex) {
        --targetPageIndex;
    } else if (isInRightPageChangeZone() &&
        targetPageIndex < HSCONFIGURATION_GET(maximumPageCount) - 1) {
        ++targetPageIndex;
    } else {
        return;
    }

    if (targetPageIndex >= HSCONFIGURATION_GET(maximumPageCount)){
        return;
    }

    if (!mZoneAnimation) {
        mZoneAnimation = new QPropertyAnimation(mUiWidget, "sceneX");
    }

    int bounceEffect = HSCONFIGURATION_GET(bounceEffect);

    if (isInLeftPageChangeZone()) {
        mZoneAnimation->setEndValue(pageLayerXPos(scene->activePageIndex()) + bounceEffect);
    } else {
        mZoneAnimation->setEndValue(pageLayerXPos(scene->activePageIndex()) - bounceEffect);
    }
    mZoneAnimation->setDuration(duration);
    mZoneAnimation->setDirection(QAbstractAnimation::Forward);

    connect(mZoneAnimation,
            SIGNAL(finished()),
            SLOT(zoneAnimationFinished()));
    mZoneAnimation->start();
}

/*!
    Checks if the active widget is located inside the left or right
    page change zone.
*/
bool HsIdleState::isInPageChangeZone()
{
    return isInLeftPageChangeZone() ||
           isInRightPageChangeZone();
}

/*!
    Checks if touch point in the active widget is located inside the left
    page change zone.
*/
bool HsIdleState::isInLeftPageChangeZone()
{
    return mTouchScenePos.x() < HSCONFIGURATION_GET(pageChangeZoneWidth);
}

/*!
    Checks if touch point in the active widget is located inside the right
    page change zone.
*/
bool HsIdleState::isInRightPageChangeZone()
{
    qreal pageWidth = HsScene::mainWindow()->layoutRect().width();
    return mTouchScenePos.x() > pageWidth - HSCONFIGURATION_GET(pageChangeZoneWidth);
}

/*!
    Inserts new page at index position \a pageIndex in the scene.
*/
void HsIdleState::addPageToScene(int pageIndex)
{
    HsPageData data;
    data.indexPosition = pageIndex;
    HsPage *page = HsPage::createInstance(data);
    page->load();
    HsScene::instance()->addPage(page);
    mUiWidget->insertPage(pageIndex, page);
}

/*!
    Checks if page change zone animation needs to be started or stopped
*/
void HsIdleState::updateZoneAnimation()
{
    if (isInPageChangeZone() && mAllowZoneAnimation) {
        if (!mZoneAnimation
            && ( !mPageChangeAnimation
                 || mPageChangeAnimation->state() == QAbstractAnimation::Stopped )) {
            startPageChangeZoneAnimation(HSCONFIGURATION_GET(pageChangeZoneAnimationDuration));
        }
    }
    else if (mZoneAnimation && !isInPageChangeZone()) {
        if (mZoneAnimation->state() == QAbstractAnimation::Running) {
            if (mZoneAnimation->direction() == QAbstractAnimation::Forward) {
                    mZoneAnimation->setDuration(HSCONFIGURATION_GET(pageChangeZoneReverseAnimationDuration));
                    mZoneAnimation->setDirection(QAbstractAnimation::Backward);
            }
        }
        else {
            // Out of the page change zone. Delete animation.
            deleteZoneAnimation();
        }
    }
    else if (!isInPageChangeZone()) {
        if (mZoneAnimation) {
            mZoneAnimation->stop();
        }
    }
}

/*!
    Selects which trash bin icon is shown or not shown at all
*/
void HsIdleState::showTrashBin()
{
    if (mUiWidget->trashBin()->isUnderMouse()) {
        if (!mTrashBinFeedbackAlreadyPlayed) {
            HbInstantFeedback::play(HSCONFIGURATION_GET(widgetOverTrashbinFeedbackEffect));
            mTrashBinFeedbackAlreadyPlayed = true;
        }
        mUiWidget->trashBin()->activate();
    } else {
        mUiWidget->trashBin()->deactivate();
        mTrashBinFeedbackAlreadyPlayed = false;
    }

    if (!mUiWidget->pageIndicator()->isAnimationRunning()) {
        mUiWidget->showTrashBin();
    }
}

/*!
    Removes currently active page.
*/
void HsIdleState::removeActivePage()
{
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();

    int pageIndex = scene->activePageIndex();

    mUiWidget->removePage(pageIndex);
    scene->removePage(page);
    
    if (page->wallpaper()) {
        page->wallpaper()->remove();        
    }
    delete page;

    pageIndex = pageIndex == 0 ? 0 : pageIndex - 1;
    scene->setActivePageIndex(pageIndex);

    startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(pageRemovedAnimationDuration));

    mUiWidget->pageIndicator()->removeItem(pageIndex);
    mUiWidget->setActivePage(pageIndex);
    mUiWidget->showPageIndicator();
}

/*!
    Deletes page change zone animation.
*/
void HsIdleState::deleteZoneAnimation()
{
    delete mZoneAnimation;
    mZoneAnimation = NULL;
}

/*!
    Disables the main window user interaction.
*/
void HsIdleState::action_disableUserInteraction()
{
    HsScene::mainWindow()->setInteractive(false);
}

/*!
    Enables the main window user interaction.
*/
void HsIdleState::action_enableUserInteraction()
{
    HsScene::mainWindow()->setInteractive(true);
}

/*!
    If called for the first time, setups the idle view.
    Updates the soft key action and sets the idle view
    as the current view to the main window.
*/
void HsIdleState::action_idle_setupView()
{
    HbView *idleView = HsGui::idleView();
    if (!idleView) {
        mUiWidget = new HsIdleWidget;
        idleView = HsScene::mainWindow()->addView(mUiWidget);
        idleView->setContentFullScreen();

        mNavigationAction = new HbAction(this);
        mNavigationAction->setIcon(HbIcon(gApplicationLibraryIconName));
        connect(mNavigationAction, SIGNAL(triggered()), SIGNAL(event_applicationLibrary()));
        idleView->setNavigationAction(mNavigationAction);

        HsGui::setIdleView(idleView);

        if (mPageChangeAnimation) {
            delete mPageChangeAnimation;
            mPageChangeAnimation = NULL;
        }
    }

    HsScene::mainWindow()->setCurrentView(idleView);
}

/*!
    Sets the idle view's title.
*/
void HsIdleState::action_idle_setupTitle()
{
    qDebug() << "HsIdleState::action_idle_setupTitle() - ENTRY";
    onTitleChanged(mTitleResolver->title());
    connect(mTitleResolver, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
    qDebug() << "HsIdleState::action_idle_setupTitle() - EXIT";
}

/*!
    Updates the idle view's title.
*/
void HsIdleState::onTitleChanged(QString title)
{
    qDebug() << "HsIdleState::onTitleChanged() to title: " << title;
    HsGui::idleView()->setTitle(title);
}

/*!

*/
void HsIdleState::onAddContentActionTriggered()
{
    machine()->postEvent(
        HsMenuEventFactory::createOpenAppLibraryEvent(AddHsMenuMode));
}

/*!
    Disconnects the idle view's title.
*/
void HsIdleState::action_idle_cleanupTitle()
{
    mTitleResolver->disconnect(this);
}

/*!
    Lays out the active page's new widgets that were added
    from the application library.
*/
void HsIdleState::action_idle_layoutNewWidgets()
{
    HsPage *page = HsScene::instance()->activePage();
    if (!page) {
        return;
    }

    QList<HsWidgetHost *> widgets = page->newWidgets();
    if (widgets.isEmpty()) {
        return;
    }

    foreach (HsWidgetHost *widget, widgets) {
        widget->startWidget();
    }

    page->layoutNewWidgets();

}

/*!
    Moves the scene and page layers so that the active
    page is shown.
*/
void HsIdleState::action_idle_showActivePage()
{    
    HsScene *scene = HsScene::instance();
    QApplication::instance()->installEventFilter(scene);
    scene->activePage()->showWidgets();
    qreal x = pageLayerXPos(scene->activePageIndex());
    mUiWidget->setSceneX(x);
}

/*!
    Connects the orientation change event handler.
*/
void HsIdleState::action_idle_connectOrientationChangeEventHandler()
{
    connect(HsScene::mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(action_idle_showActivePage()));
}

/*!
    Installs the event filter.
*/
void HsIdleState::action_idle_installEventFilter()
{
    QCoreApplication::instance()->installEventFilter(this);
}

/*!
    Publishes the idle key
*/
void HsIdleState::action_waitInput_publishIdleKey()
{
#ifdef Q_OS_SYMBIAN
    qDebug() << "HsIdleState::action_waitInput_publishIdleKey";
    if (!mSettingsMgr){
        // All done, let others know it too
        qDebug() << "Setting the idle key";
        mSettingsMgr = new XQSettingsManager(this);
        XQPublishAndSubscribeSettingsKey key(KPSUidStartup.iUid, KPSIdlePhase1Ok);
        mSettingsMgr->writeItemValue(key, EIdlePhase1Ok);

        // Done, we can disconnect this signal.
        disconnect(sender(), SIGNAL(entered()), this, SLOT(action_waitInput_publishIdleKey()));
    }
#endif
}

/*!
    Cleans up the idle view.
*/
void HsIdleState::action_idle_cleanupView()
{
    HsScene *scene = HsScene::instance();
    QApplication::instance()->removeEventFilter(scene);
    scene->activePage()->hideWidgets();

    if (mUiWidget) {
        mUiWidget->clearDelayedPress();
    }

    //Close options menu on view change
    HsGui::idleView()->setMenu(NULL);

    //Close context menu on view change
    if (mSceneMenu) {
        disconnect(mSceneMenu, SIGNAL(aboutToClose()), this, SLOT(onSceneMenuAboutToClose()));
        mSceneMenu->close();
    }
}

/*!
    Disconnects the orientation change event handler.
*/
void HsIdleState::action_idle_disconnectOrientationChangeEventHandler()
{
    disconnect(HsScene::mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(action_idle_showActivePage()));
}

/*!
    Disconnects the event filter.
*/
void HsIdleState::action_idle_uninstallEventFilter()
{
    QCoreApplication::instance()->removeEventFilter(this);
}

/*!
    Updates the options menu content.
*/
void HsIdleState::action_waitInput_updateOptionsMenu()
{
    HsScene *scene = HsScene::instance();

    HbMenu *menu = new HbMenu();

    // Task switcher
    menu->addAction(hbTrId(hsLocTextId_OptionsMenu_TaskSwitcher),
        this, SLOT(openTaskSwitcher()));

    // Add content
    menu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent),
        this, SLOT(onAddContentActionTriggered()));

    // Add page
    if (scene->pages().count() < HSCONFIGURATION_GET(maximumPageCount)) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_AddPage),
            this, SIGNAL(event_addPage()));
    }

    // Change wallpaper
    menu->addAction(hbTrId(hsLocTextId_ContextMenu_ChangeWallpaper),
        this, SIGNAL(event_selectWallpaper()));

    // Remove page
    if (scene->activePage()->isRemovable()) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_RemovePage),
            this, SIGNAL(event_removePage()));
    }

    // Online / Offline
    if (scene->isOnline()) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_HsToOffline),
            this, SIGNAL(event_toggleConnection()));
    } else {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_HsToOnline),
            this, SIGNAL(event_toggleConnection()));
    }

    HsGui::idleView()->setMenu(menu);
}

/*!
    Connects the waitInput state's mouse event handlers.
*/
void HsIdleState::action_waitInput_connectMouseEventHandlers()
{
    CONNECT_MOUSE_EVENT_HANDLER(mousePressed, waitInput_onMousePressed)
}

/*!
    Disconnects the waitInput state's mouse event handlers.
*/
void HsIdleState::action_waitInput_disconnectMouseEventHandlers()
{
    DISCONNECT_MOUSE_EVENT_HANDLER(mousePressed, waitInput_onMousePressed)
}

/*!
    Resets new widget layout.
*/
void HsIdleState::action_waitInput_resetNewWidgets()
{
    HsScene::instance()->activePage()->resetNewWidgets();
}

/*!
    Connects the widgetInteraction state's mouse event handlers.
*/
void HsIdleState::action_widgetInteraction_connectMouseEventHandlers()
{
    CONNECT_MOUSE_EVENT_HANDLER(mouseMoved, widgetInteraction_onMouseMoved)
    CONNECT_MOUSE_EVENT_HANDLER(mouseReleased, widgetInteraction_onMouseReleased)
}

/*!
    Connects the widgetInteraction state's gesture timers.
*/
void HsIdleState::action_widgetInteraction_connectGestureTimers()
{
    mTimer.setInterval(HSCONFIGURATION_GET(widgetTapAndHoldTimeout));
    connect(&mTimer, SIGNAL(timeout()),
        SLOT(widgetInteraction_onTapAndHoldTimeout()));
    mTimer.start();
}

/*!
    Disconnects the widgetInteraction state's mouse event handlers.
*/
void HsIdleState::action_widgetInteraction_disconnectMouseEventHandlers()
{
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseMoved, widgetInteraction_onMouseMoved)
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseReleased, widgetInteraction_onMouseReleased)
}

/*!
    Disconnects the widgetInteraction state's gesture timers.
*/
void HsIdleState::action_widgetInteraction_disconnectGestureTimers()
{
    disconnect(&mTimer, SIGNAL(timeout()),
        this, SLOT(widgetInteraction_onTapAndHoldTimeout()));
}

/*!
    Connects the sceneInteraction state's mouse event handlers.
*/
void HsIdleState::action_sceneInteraction_connectMouseEventHandlers()
{
    CONNECT_MOUSE_EVENT_HANDLER(mouseMoved, sceneInteraction_onMouseMoved)
    CONNECT_MOUSE_EVENT_HANDLER(mouseReleased, sceneInteraction_onMouseReleased)
}

/*!
    Connects the sceneInteraction state's gesture timers.
*/
void HsIdleState::action_sceneInteraction_connectGestureTimers()
{
    mTimer.setInterval(HSCONFIGURATION_GET(sceneTapAndHoldTimeout));
    connect(&mTimer, SIGNAL(timeout()),
        SLOT(sceneInteraction_onTapAndHoldTimeout()));
    mTimer.start();
}

/*!
    Disconnects the sceneInteraction state's mouse event handlers.
*/
void HsIdleState::action_sceneInteraction_disconnectMouseEventHandlers()
{
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseMoved, sceneInteraction_onMouseMoved)
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseReleased, sceneInteraction_onMouseReleased)
}

/*!
    Disconnects the sceneInteraction state's gesture timers.
*/
void HsIdleState::action_sceneInteraction_disconnectGestureTimers()
{
    disconnect(&mTimer, SIGNAL(timeout()),
        this, SLOT(sceneInteraction_onTapAndHoldTimeout()));
}

/*!
    Reparents the active widget to the control layer.
*/
void HsIdleState::action_moveWidget_reparentToControlLayer()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);
    widget->setParentItem(mUiWidget->controlLayer());

    mUiWidget->showTrashBin();
}

/*!
    Starts the widget drag effect.
*/
void HsIdleState::action_moveWidget_startWidgetDragEffect()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);
    widget->startDragEffect();
    mAllowZoneAnimation = true;
}

/*!
    Connects the moveWidget state's mouse event handlers.
*/
void HsIdleState::action_moveWidget_connectMouseEventHandlers()
{
    CONNECT_MOUSE_EVENT_HANDLER(mouseMoved, moveWidget_onMouseMoved)
    CONNECT_MOUSE_EVENT_HANDLER(mouseReleased, moveWidget_onMouseReleased)
}

/*!
    Reparents the active widget to the active page.
*/
void HsIdleState::action_moveWidget_reparentToPage()
{
    mTimer.stop();

    if (mZoneAnimation
        && mZoneAnimation->state() == QAbstractAnimation::Running
        && mZoneAnimation->direction() == QAbstractAnimation::Forward) {
        mZoneAnimation->setDuration(HSCONFIGURATION_GET(pageChangeZoneReverseAnimationDuration));
        mZoneAnimation->setDirection(QAbstractAnimation::Backward);
    }

    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    HsWidgetHost *widget = scene->activeWidget();

    if (mUiWidget->trashBin()->isUnderMouse()) {
        HbInstantFeedback::play(HSCONFIGURATION_GET(widgetDropToTrashbinFeedbackEffect));
        widget->page()->removeWidget(widget);
        widget->remove();
        scene->setActiveWidget(0);
    } else {
        if (widget->page() != page) {
            widget->page()->removeWidget(widget);
            page->addExistingWidget(widget);
            if (HsScene::orientation() == Qt::Horizontal) {
                widget->removePresentation(Qt::Vertical);
            } else {
                widget->removePresentation(Qt::Horizontal);
            }
        }

        QRectF widgetRect = widget->geometry();
        QRectF pageRect = page->rect();
        qreal widgetX = qBound(qreal(0), widgetRect.x(), pageRect.width() - widgetRect.width());
        qreal widgetY = qBound(qreal(64), widgetRect.y(), pageRect.height() - widgetRect.height());

        // play feedback effect if user drops widget between pages and it needs to be repositioned back to original page
        if( widgetX != widgetRect.x() || widgetY != widgetRect.y()) {
            HbInstantFeedback::play(HSCONFIGURATION_GET(widgetRepositionFeedbackEffect));
        }
        widget->setPos(widgetX, widgetY);

        widget->savePresentation();
        page->updateZValues();
    }

    widget->setParentItem(HsScene::instance()->activePage());

    mUiWidget->showPageIndicator();
}

/*!
    Starts the widget drop effect.
*/
void HsIdleState::action_moveWidget_startWidgetDropEffect()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    if (widget) {
        widget->startDropEffect();
    }
}

/*!
    Disconnects the moveWidget state's mouse event handlers.
*/
void HsIdleState::action_moveWidget_disconnectMouseEventHandlers()
{
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseMoved, moveWidget_onMouseMoved)
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseReleased, moveWidget_onMouseReleased)
}

/*!
    Prevents page change zone animation.
*/
void HsIdleState::action_moveWidget_preventZoneAnimation()
{
    mAllowZoneAnimation = false;
}

/*!
    Connects the moveScene state's mouse event handlers.
*/
void HsIdleState::action_moveScene_connectMouseEventHandlers()
{
    CONNECT_MOUSE_EVENT_HANDLER(mouseMoved, moveScene_onMouseMoved)
    CONNECT_MOUSE_EVENT_HANDLER(mouseReleased, moveScene_onMouseReleased)
}

/*!
    Moves to the nearest page.
*/
void HsIdleState::action_moveScene_moveToNearestPage()
{
    QList<HsPage *> pages = HsScene::instance()->pages();
    QSizeF pageSize = pages.first()->size();

    int pageIndex = HsScene::instance()->activePageIndex();

    if (mDeltaX < -HSCONFIGURATION_GET(pageChangePanDistanceInPixels)) {
        pageIndex = qMin(pageIndex + 1, pages.count() - 1);
    } else if (HSCONFIGURATION_GET(pageChangePanDistanceInPixels) < mDeltaX) {
        pageIndex = qMax(pageIndex - 1, 0);
    }

    HsScene::instance()->setActivePageIndex(pageIndex);

    startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(pageChangeAnimationDuration));
}

/*!
    Disconnects the moveScene state's mouse event handlers.
*/
void HsIdleState::action_moveScene_disconnectMouseEventHandlers()
{
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseMoved, moveScene_onMouseMoved)
    DISCONNECT_MOUSE_EVENT_HANDLER(mouseReleased, moveScene_onMouseReleased)
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
    Shows the scene menu.
*/
void HsIdleState::action_sceneMenu_showMenu()
{
    mSceneMenu = new HbMenu();
    mSceneMenu->setAttribute(Qt::WA_DeleteOnClose);
    mSceneMenu->setDismissPolicy(HbDialog::TapOutside);
    //This is used to check if any action is not triggered ( == menu is dismissed by user or timeout).
    connect(mSceneMenu, SIGNAL(aboutToClose()), this, SLOT(onSceneMenuAboutToClose()));

    mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent), this, SLOT(onAddContentActionTriggered()));

    HsScene *scene = HsScene::instance();
    if (scene->pages().count() < HSCONFIGURATION_GET(maximumPageCount)) {
        mSceneMenu->addAction(hbTrId(hsLocTextId_OptionsMenu_AddPage), this, SIGNAL(event_addPage()));
    }

    mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_ChangeWallpaper), this, SIGNAL(event_selectWallpaper()));
    mSceneMenu->setPreferredPos(mTouchScenePos);
    mSceneMenu->open(this, SLOT(onSceneMenuTriggered(HbAction *)));
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Adds a new page to the scene.
*/
void HsIdleState::action_addPage_addPage()
{
    HsScene *scene = HsScene::instance();
	int pageIndex = scene->activePageIndex() + 1;
    addPageToScene(pageIndex);
    scene->setActivePageIndex(pageIndex);
    startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(newPageAddedAnimationDuration));
    mUiWidget->pageIndicator()->addItem(pageIndex);
    mUiWidget->showPageIndicator();
}

/*!
    Removes an existing page from the scene.
*/
void HsIdleState::action_removePage_removePage()
{
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();

    if (!page->widgets().isEmpty()) {
#ifndef HOMESCREEN_TEST //We don't want to test message box.
        //Confirm removal of page having content
        HbMessageBox *box = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->setHeadingWidget(new HbLabel
                                (hbTrId(hsLocTextId_Title_RemovePage)));
        box->setText(hbTrId(hsLocTextId_Confirmation_RemovePage));

        QAction *buttonOk = box->actions().at(0);
        //We are keen only from OK button. Cancel is not connected to any slot.
        connect(buttonOk, SIGNAL(triggered()), SLOT(onRemovePageConfirmationOk()));

        HsScene::mainWindow()->setInteractive(true);

        box->open();
#endif //HOMESCREEN_TEST
    } else {
        //Empty page can be removed without confirmation
        removeActivePage();
    }
}

/*!
    Toggles the homescreen online/offline state.
*/
void HsIdleState::action_toggleConnection_toggleConnection()
{
    HsScene *scene = HsScene::instance();
    scene->setOnline(!scene->isOnline());
}

/*!
    Handles mouse press events for the waitInput state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::waitInput_onMousePressed(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    filtered = true;

    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    QList<HsWidgetHost *> widgets = page->widgets();

    if (mUiWidget->controlLayer() == watched ||
        mUiWidget->controlLayer()->isAncestorOf(watched)) {
        filtered = false;
        return;
    }

    mUiWidget->captureDelayedPress(event);

    foreach (HsWidgetHost *widget, widgets) {
        if (widget == watched || widget->isAncestorOf(watched)) {
            scene->setActiveWidget(widget);
            emit event_widgetInteraction();
            return;
        }
    }

    mTouchScenePos = event->scenePos();
    emit event_sceneInteraction();
}

/*!
    Handles mouse move events for the widgetInteraction state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::widgetInteraction_onMouseMoved(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(filtered)

    if (!mTimer.isActive()) {
        return;
    }

    filtered = true;

    QPointF point =
        event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
    if (HSCONFIGURATION_GET(tapAndHoldDistance) < point.manhattanLength()) {
        // Threshold area for tap exceeded. This is pan or swipe
        mTimer.stop();
        if (HsScene::instance()->activeWidget()->isPannable(event)) {
            // let widget get first press
            mUiWidget->sendDelayedPress();
        } else {
            // Widget doesn't consume pan or swipe gestures
            mUiWidget->clearDelayedPress();
            emit event_moveScene();
        }
    }
}

/*!
    Handles mouse release events for the widgetInteraction state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::widgetInteraction_onMouseReleased(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(event)
    Q_UNUSED(filtered)

    if (mTimer.isActive()) {
        mTimer.stop();
        mUiWidget->sendDelayedPress();
    }

    HsPage *page = HsScene::instance()->activePage();
    QMetaObject::invokeMethod(page, "updateZValues", Qt::QueuedConnection);

    emit event_waitInput();
}

/*!
    Handles mouse move events for the sceneInteraction state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::sceneInteraction_onMouseMoved(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(filtered)

    mTouchScenePos = event->scenePos();

    if (!mTimer.isActive()) {
        return;
    }

    filtered = true;

    QPointF point =
        event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
    if (HSCONFIGURATION_GET(tapAndHoldDistance) < point.manhattanLength()) {
        mTimer.stop();
        mUiWidget->clearDelayedPress();
        emit event_moveScene();
    }
}

/*!
    Handles mouse release events for the sceneInteraction state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::sceneInteraction_onMouseReleased(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(event)
    Q_UNUSED(filtered)

    filtered = true;

    if (mTimer.isActive()) {
        mTimer.stop();
        mUiWidget->clearDelayedPress();
    }

    emit event_waitInput();
}

/*!
    Handles mouse move events for the moveWidget state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::moveWidget_onMouseMoved(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)

    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activeWidget();
    QRectF widgetRect = widget->geometry();

    //Move widget to new position:
    mTouchScenePos = event->scenePos();
    QPointF delta(event->scenePos() - event->lastScenePos());
    widgetRect.moveTopLeft(widgetRect.topLeft() + delta);

    //Widget can be moved over the pages left border
    qreal lowerBoundX = -widgetRect.width();
    HsPage *page = scene->activePage();
    QRectF pageRect = HsGui::idleView()->rect();
    //Widget can be moved over the pages right border
    qreal upperBoundX = pageRect.width();

    //Notice that chrome height is 64 pixels
    qreal lowerBoundY = qreal(64) - widgetRect.height();
    //Widget can be moved over the pages down border
    qreal upperBoundY = pageRect.height();

    qreal widgetX = qBound(lowerBoundX, widgetRect.x(), upperBoundX);
    qreal widgetY = qBound(lowerBoundY, widgetRect.y(), upperBoundY);
    /* if using ItemClipsChildrenToShape-flag in widgethost then
       setPos does not update position here, however setGeometry does it, QT bug?
    */
    widget->setGeometry(widgetX, widgetY, widgetRect.width(), widgetRect.height());

    int bounceFeedbackEffectDistance = HSCONFIGURATION_GET(bounceFeedbackEffectDistance);
    //Handle effects:
    //User is indicated by a tactile feedback if he/she is trying to move
    //widget over the first or the last page.
    if( (page == scene->pages().first() && mTouchScenePos.x() < bounceFeedbackEffectDistance ) ||
        (page == scene->pages().last() && scene->pages().count() == HSCONFIGURATION_GET(maximumPageCount)
         && mTouchScenePos.x() > pageRect.width() - bounceFeedbackEffectDistance)) {
             HbInstantFeedback::play(HSCONFIGURATION_GET(widgetMoveBlockedFeedbackEffect));
            // TODO: use code below when Orbit has updated ContinuousFeedback API
            //if (!mContinuousFeedback->isPlaying()) {
            //    mContinuousFeedback->play();
            //}
        }
        else /*if (mContinuousFeedback->isPlaying())*/ {
            //mContinuousFeedback->stop();
        }

    updateZoneAnimation();
    showTrashBin();

    filtered = true;
}

/*!
    Handles mouse release events for the moveWidget state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::moveWidget_onMouseReleased(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(event)

    filtered = true;
    emit event_waitInput();
}

/*!
    Handles mouse move events for the moveScene state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::moveScene_onMouseMoved(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(filtered)

    filtered = true;

    HsScene *scene = HsScene::instance();
    mDeltaX = event->scenePos().x() - event->buttonDownScenePos(Qt::LeftButton).x();

    int bounceEffect = HSCONFIGURATION_GET(bounceEffect);

    qreal x = qBound(pageLayerXPos(scene->pages().count() - 1) - bounceEffect / 2 / mUiWidget->parallaxFactor(),
                     pageLayerXPos(scene->activePageIndex()) + mDeltaX,
                     pageLayerXPos(0) + (bounceEffect / 2 / mUiWidget->parallaxFactor()));

    mUiWidget->setSceneX(x);
}

/*!
    Handles mouse release events for the moveScene state.
    Filters events for the item \a watched. \a event is the
    filtered event. Sets the \a filtered true if the event
    was filtered by this handler.
*/
void HsIdleState::moveScene_onMouseReleased(
    QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered)
{
    Q_UNUSED(watched)
    Q_UNUSED(filtered)
    filtered = true;

    mDeltaX = event->scenePos().x() - event->buttonDownScenePos(Qt::LeftButton).x();

    emit event_waitInput();
}

/*!
    Handles tap-and-hold events for the widgetInteraction state.
*/
void HsIdleState::widgetInteraction_onTapAndHoldTimeout()
{
    mUiWidget->clearDelayedPress();
    emit event_moveWidget();
}

/*!
    Handles tap-and-hold events for the sceneInteraction state.
*/
void HsIdleState::sceneInteraction_onTapAndHoldTimeout()
{
    mUiWidget->clearDelayedPress();
    emit event_sceneMenu();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsIdleState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Page change zone animation has been finished.
 */
void HsIdleState::zoneAnimationFinished()
{
    HsScene *scene = HsScene::instance();
    int pageIndex = scene->activePageIndex();

    if (mZoneAnimation->direction() == QAbstractAnimation::Forward) {
        if (isInLeftPageChangeZone() &&
            0 < pageIndex) {
            --pageIndex;
        }
        else if (isInRightPageChangeZone() &&
            pageIndex < scene->pages().count()) {
            ++pageIndex;
        }

        if (pageIndex == scene->pages().count()) {
            if (pageIndex < HSCONFIGURATION_GET(maximumPageCount)) {
                addPageToScene(pageIndex);
                mUiWidget->showPageIndicator();
                mUiWidget->pageIndicator()->addItem(pageIndex);
            }
        }
        scene->setActivePageIndex(pageIndex);
        startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(pageChangeAnimationDuration));
    }
    else {
        scene->setActivePageIndex(pageIndex);
        mUiWidget->setActivePage(pageIndex);
    }
    deleteZoneAnimation();
}

/*!
    Page change animation has been finished.
 */
void HsIdleState::pageChangeAnimationFinished()
{
    updateZoneAnimation();
}

/*!
    Handles the close of the scene menu when menu is dismissed.
    While dismissed menu should be triggered with NULL action.
    Actions on menu are connected stright to desired slot or signal.
*/
void HsIdleState::onSceneMenuTriggered(HbAction *action)
{
    //We check if menu was dismissed either by user tap in outside of the menu,
    //menu's dismiss timer timeouts or state_sceneMenu was exited by any other reason
    //than triggered action, e.g. applib activated while context menu is active.
    if(!action) {
        emit event_waitInput();
    }
}

/*!
    Handles the close of remove page confirmation dialog for page having content.
*/
void HsIdleState::onRemovePageConfirmationOk()
{
    removeActivePage();
}

/*!
    Handles the close of the scene menu.

    This workaround is needed since HbMenu does not trigger empty action in case of
    dismission of menu dialog. That functionality will be added in near future. Thereafter
    onSceneMenuTriggered() can handle dismissions (HbMenu::triggered() emitted with NULL
    action). This workaround can be removed at that time.
*/
void HsIdleState::onSceneMenuAboutToClose() {
    //We need to check if menu is dismissed either by user tap in outside of the menu or
    //menu's dismiss timer timeouts. There is active action if any action tapped otherwise
    //menu was dissmised.
    HbMenu *menu = static_cast<HbMenu*>(sender());
    if(!menu->activeAction()) {
        //mSceneMenu = 0;  //Menu deletes itself at the close
        emit event_waitInput();
    }
}
