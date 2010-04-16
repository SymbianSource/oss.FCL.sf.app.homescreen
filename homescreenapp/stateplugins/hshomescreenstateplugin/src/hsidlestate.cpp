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

#include "hsidlestate.h"
#include "hsidlewidget.h"
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hsselectbackgroundstate.h"
#include "hstrashbinwidget.h"
#include "hspageindicator.h"
#include "hsapptranslator.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsmenueventfactory.h"
#include "hshomescreenstatecommon.h"

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
    const char gAddPageTextName[]            = "txt_homescreen_opt_add_page";
    const char gRemovePageTextName[]         = "txt_homescreen_opt_remove_page";
    const char gToOnlineTextName[]           = "txt_homescreen_opt_home_screen_to_online";
    const char gToOfflineTextName[]          = "txt_homescreen_opt_home_screen_to_offline";
    const char gChangeWallpaperTextName[]    = "txt_homescreen_list_change_wallpaper";
    const char gAddContentTextName[]         = "txt_homescreen_list_add_content";
    //const char gTitleOfflineTextName[]       = "txt_homescreen_title_offline";
}

/*!
    \class HsIdleState
    \ingroup group_hshomescreenstateplugin
    \brief Controller part of the home screen idle state.

    Controls the home screen idle state execution. See the
    state chart below for the state structure.

    \image html hsidlestate_statechart.png
*/

/*!
    Constructs a new idle state with the given \a parent.
*/
HsIdleState::HsIdleState(QState *parent)
  : QState(parent),
    mView(0), mNavigationAction(0), mUiWidget(0),
    mTapAndHoldDistance(16),
    mPageChangeZoneWidth(60)
{
    setupStates();
    mTimer.setSingleShot(true);
}

/*!
    Destroys this idle state.
*/
HsIdleState::~HsIdleState()
{
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
    \fn HsIdleState::event_selectSceneWallpaper()  

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
    HsSelectBackgroundState *state_selectSceneWallpaper = 
        new HsSelectBackgroundState(this);
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

    state_widgetInteraction->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);
    state_widgetInteraction->addTransition(
        this, SIGNAL(event_moveWidget()), state_moveWidget);

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
        this, SIGNAL(event_selectSceneWallpaper()), state_selectSceneWallpaper);

    state_selectSceneWallpaper->addTransition(
        state_selectSceneWallpaper, SIGNAL(event_waitInput()), state_waitInput);

    state_addPage->addTransition(state_waitInput);

    state_removePage->addTransition(state_waitInput);

    state_toggleConnection->addTransition(state_waitInput);

    // Actions

    ENTRY_ACTION(this, action_idle_setupView)
    ENTRY_ACTION(this, action_idle_layoutNewWidgets)
    ENTRY_ACTION(this, action_idle_showActivePage)
    ENTRY_ACTION(this, action_idle_connectOrientationChangeEventHandler)
    EXIT_ACTION(this, action_idle_cleanupView)
    EXIT_ACTION(this, action_idle_disconnectOrientationChangeEventHandler)

    ENTRY_ACTION(state_waitInput, action_waitInput_updateOptionsMenu)
    ENTRY_ACTION(state_waitInput, action_waitInput_connectMouseEventHandlers)
    EXIT_ACTION(state_waitInput, action_waitInput_disconnectMouseEventHandlers)

    ENTRY_ACTION(state_widgetInteraction, action_widgetInteraction_connectMouseEventHandlers)
    ENTRY_ACTION(state_widgetInteraction, action_widgetInteraction_connectGestureTimers)
    EXIT_ACTION(state_widgetInteraction, action_widgetInteraction_disconnectMouseEventHandlers)
    EXIT_ACTION(state_widgetInteraction, action_widgetInteraction_disconnectGestureTimers)

    ENTRY_ACTION(state_sceneInteraction, action_sceneInteraction_connectMouseEventHandlers)
    ENTRY_ACTION(state_sceneInteraction, action_sceneInteraction_connectGestureTimers)
    EXIT_ACTION(state_sceneInteraction, action_sceneInteraction_disconnectMouseEventHandlers)
    EXIT_ACTION(state_sceneInteraction, action_sceneInteraction_disconnectGestureTimers)

    ENTRY_ACTION(state_moveWidget, action_moveWidget_reparentToControlLayer)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_startWidgetDragAnimation)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectMouseEventHandlers)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectGestureTimers)

    EXIT_ACTION(state_moveWidget, action_moveWidget_reparentToPage)
    EXIT_ACTION(state_moveWidget, action_moveWidget_startWidgetDropAnimation)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectMouseEventHandlers)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectGestureTimers)

    ENTRY_ACTION(state_moveScene, action_moveScene_connectMouseEventHandlers)
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
    return -pageIndex * HsScene::mainWindow()->layoutRect().width() - 0.5;
}

/*!
    Starts the page change animation based on the given \a targetPageIndex
    and \a duration.
*/
void HsIdleState::startPageChangeAnimation(int targetPageIndex, int duration)
{
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;

    QPropertyAnimation *animation = new QPropertyAnimation(mUiWidget->pageLayer(), "x");
    animation->setEndValue(pageLayerXPos(targetPageIndex));
    animation->setDuration(duration);
    animationGroup->addAnimation(animation);

    animation = new QPropertyAnimation(mUiWidget->sceneLayer(), "x");
    animation->setEndValue((parallaxFactor() * pageLayerXPos(targetPageIndex)) - HSBOUNDARYEFFECT / 2);
    animation->setDuration(duration);
    animationGroup->addAnimation(animation);
        
    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);

    mUiWidget->showPageIndicator();
    mUiWidget->setActivePage(targetPageIndex);
}

/*!
    Checks if the active widget is located inside the left or right 
    page change zone.
*/
bool HsIdleState::isInPageChangeZone()
{
    qreal widgetXPos = HsScene::instance()->activeWidget()->geometry().center().x();
    qreal pageWidth = HsScene::mainWindow()->layoutRect().width();
    return widgetXPos < mPageChangeZoneWidth ||
           pageWidth - mPageChangeZoneWidth < widgetXPos;
}

/*!
    Checks if the active widget is located inside the left
    page change zone.
*/
bool HsIdleState::isInLeftPageChangeZone()
{
    qreal widgetXPos = HsScene::instance()->activeWidget()->geometry().center().x();
    return widgetXPos < mPageChangeZoneWidth;
}

/*!
    Checks if the active widget is located inside the right
    page change zone.
*/
bool HsIdleState::isInRightPageChangeZone()
{
    qreal widgetXPos = HsScene::instance()->activeWidget()->geometry().center().x();
    qreal pageWidth = HsScene::mainWindow()->layoutRect().width();
    return pageWidth - mPageChangeZoneWidth < widgetXPos;
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
    Computes the parallax factor based on the current scene and
    page layer widths, and page count.
*/
qreal HsIdleState::parallaxFactor() const
{
    qreal clw = mUiWidget->controlLayer()->size().width();
    qreal slw = mUiWidget->sceneLayer()->size().width() - HSBOUNDARYEFFECT;
    int n = HsScene::instance()->pages().count();
    if (n < 2) {
        return 1;
    } else {
        return (slw - clw) / ((n - 1) * clw);
    }
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
    if (!mView) {
        mUiWidget = new HsIdleWidget;
        mView = HsScene::mainWindow()->addView(mUiWidget);
        mView->setContentFullScreen();
        mView->setTitle("Home Screen"/*hbTrId(gTitleOfflineTextName)*/);
        
        mNavigationAction = new HbAction(this);
        mNavigationAction->setIcon(HbIcon(gApplicationLibraryIconName));
        connect(mNavigationAction, SIGNAL(triggered()), SIGNAL(event_applicationLibrary()));
        mView->setNavigationAction(mNavigationAction);
        
#ifndef Q_OS_SYMBIAN
        connect(HsAppTranslator::instance(), 
            SIGNAL(languageChanged()), SLOT(translateUi()));
#endif
        // TODO: Workaround to Qt/Hb layouting bugs.
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    HsScene::mainWindow()->setCurrentView(mView);
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
        widget->initializeWidget();
        widget->showWidget();
    }

    page->layoutNewWidgets();

}

/*!
    Moves the scene and page layers so that the active
    page is shown.
*/
void HsIdleState::action_idle_showActivePage()
{
    qreal x = pageLayerXPos(HsScene::instance()->activePageIndex());
    mUiWidget->pageLayer()->setX(x);
    mUiWidget->sceneLayer()->setX((parallaxFactor() * x) - HSBOUNDARYEFFECT / 2);
}

/*!
    Connects the orientation change event handler.
*/
void HsIdleState::action_idle_connectOrientationChangeEventHandler()
{
    connect(HsScene::mainWindow(), 
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(onOrientationChanged(Qt::Orientation)));
}

/*!
    Cleans up the idle view.
*/
void HsIdleState::action_idle_cleanupView()
{
}

/*!
    Disconnects the orientation change event handler.
*/
void HsIdleState::action_idle_disconnectOrientationChangeEventHandler()
{
    disconnect(HsScene::mainWindow(), 
        SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(onOrientationChanged(Qt::Orientation)));
}

/*!
    Updates the options menu content.
*/
void HsIdleState::action_waitInput_updateOptionsMenu()
{
    HsScene *scene = HsScene::instance();

    HbMenu *menu = new HbMenu();
    
    // Add page
    if (scene->pages().count() < scene->maximumPageCount()) {
        menu->addAction(hbTrId(gAddPageTextName), 
            this, SIGNAL(event_addPage()));
    }

    // Remove page
    if (scene->activePage()->isRemovable()) {
        menu->addAction(hbTrId(gRemovePageTextName), 
            this, SIGNAL(event_removePage()));
    }
    
    // Online / Offline
    if (scene->isOnline()) {
        menu->addAction(hbTrId(gToOfflineTextName),
            this, SIGNAL(event_toggleConnection()));
    } else {
        menu->addAction(hbTrId(gToOnlineTextName),
            this, SIGNAL(event_toggleConnection())); 
    }

#ifndef Q_OS_SYMBIAN
    menu->addAction(hbTrId("txt_homescreen_opt_switch_language"),
        this, SLOT(switchLanguage()));
#endif

    mView->setMenu(menu);
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
    mTimer.setInterval(500);
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
    mTimer.setInterval(500);
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
    Starts the widget drag animation for the active widget.
*/
void HsIdleState::action_moveWidget_startWidgetDragAnimation()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);
    widget->startDragAnimation();
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
    Connects the moveWidget state's gesture timers.
*/
void HsIdleState::action_moveWidget_connectGestureTimers()
{
    mTimer.setInterval(800);
    connect(&mTimer, SIGNAL(timeout()), 
        SLOT(moveWidget_onHoldTimeout()));
}

/*!
    Reparents the active widget to the active page.
*/
void HsIdleState::action_moveWidget_reparentToPage()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    if (widget) {
        widget->setParentItem(HsScene::instance()->activePage());
    }
    
    mUiWidget->showPageIndicator();
}

/*!
    Starts the widget drop animation for the active widget.
*/
void HsIdleState::action_moveWidget_startWidgetDropAnimation()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    if (widget) {
        widget->startDropAnimation();
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
    Disconnects the moveWidget state's gesture timers.
*/
void HsIdleState::action_moveWidget_disconnectGestureTimers()
{
    disconnect(&mTimer, SIGNAL(timeout()), 
        this, SLOT(moveWidget_onHoldTimeout()));
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
    HbMenu menu;

    HbAction *changeWallpaperAction = 
        menu.addAction(hbTrId(gChangeWallpaperTextName));
    HbAction *addContentAction = 
        menu.addAction(hbTrId(gAddContentTextName));

    HbAction *action = menu.exec(mSceneMenuPos);
    if (action == changeWallpaperAction) {
        emit event_selectSceneWallpaper();
    } else if (action == addContentAction) {
        machine()->postEvent(
            HsMenuEventFactory::createOpenAppLibraryEvent(AddHsMenuMode));
    } else {
        emit event_waitInput();
    }
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
    int pageIndex = scene->pages().count();    
    addPageToScene(pageIndex);    
    scene->setActivePageIndex(pageIndex);
    startPageChangeAnimation(pageIndex, 700);
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
    int pageIndex = scene->activePageIndex();

    mUiWidget->removePage(pageIndex);
    scene->removePage(page);
    delete page;

    pageIndex = pageIndex == 0 ? 0 : pageIndex - 1;
    scene->setActivePageIndex(pageIndex);

    startPageChangeAnimation(pageIndex, 200);

    mUiWidget->pageIndicator()->removeItem(pageIndex);
    mUiWidget->setActivePage(pageIndex);
    mUiWidget->showPageIndicator();
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

    mSceneMenuPos = event->scenePos();
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

    QPointF point = 
        event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
    if (mTapAndHoldDistance < point.manhattanLength()) {
        mTimer.stop();
        mUiWidget->sendDelayedPress();
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

    HsScene::instance()->activePage()->updateZValues();

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

    mSceneMenuPos = event->scenePos();

    if (!mTimer.isActive()) {
        return;
    }

    QPointF point = 
        event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
    if (mTapAndHoldDistance < point.manhattanLength()) {
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

    if (mTimer.isActive()) {
        mTimer.stop();
        mUiWidget->sendDelayedPress();
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
    Q_UNUSED(filtered)

    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);

    QPointF delta(event->scenePos() - event->lastScenePos());
    QRectF region =  mView->rect().adjusted(10, 55, -10, -10);
    QPointF position = widget->geometry().center() + delta;
    if (!region.contains(position)) {
        position.setX(qBound(region.left(), position.x(), region.right()));
        position.setY(qBound(region.top(), position.y(), region.bottom()));
    }

    widget->setPos(position - widget->rect().center());

    if (mTimer.isActive()) {
        if (mTapAndHoldDistance < delta.manhattanLength()) {
            mTimer.stop();
        }
    } else {
        if (isInPageChangeZone()) {
            mTimer.start();
        }
    }

    if (mUiWidget->trashBin()->isUnderMouse()) {
        mUiWidget->trashBin()->activate();
    } else {
        mUiWidget->trashBin()->deactivate();
    }

    if (!mUiWidget->pageIndicator()->isAnimationRunning()) {
        mUiWidget->showTrashBin();
    }
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

    mTimer.stop();

    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    HsWidgetHost *widget = scene->activeWidget();

    if (mUiWidget->trashBin()->isUnderMouse()) {
        widget->page()->removeWidget(widget);
        widget->uninitializeWidget();
        widget->deleteFromDatabase();
        widget->deleteLater();
        scene->setActiveWidget(0);
    } else {
        if (widget->page() != page) {
            widget->page()->removeWidget(widget);
            page->addExistingWidget(widget);
            if (HsScene::orientation() == Qt::Horizontal) {
                widget->deleteWidgetPresentation(Qt::Vertical);
            } else {
                widget->deleteWidgetPresentation(Qt::Horizontal);
            }
        }
        widget->setWidgetPresentation();
        page->updateZValues();
    }

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
    
    HsScene *scene = HsScene::instance();
    qreal delta = 
        event->scenePos().x() - event->buttonDownScenePos(Qt::LeftButton).x();

    qreal x = qBound(pageLayerXPos(scene->pages().count() - 1) - HSBOUNDARYEFFECT / 2 / parallaxFactor(),
                     pageLayerXPos(scene->activePageIndex()) + delta,
                     pageLayerXPos(0) + (HSBOUNDARYEFFECT / 2 / parallaxFactor()) - qreal(0.5));

    mUiWidget->pageLayer()->setX(x);
    mUiWidget->sceneLayer()->setX((parallaxFactor() * x) - HSBOUNDARYEFFECT / 2);
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
    
    QList<HsPage *> pages = HsScene::instance()->pages();
    QSizeF pageSize = pages.first()->size();

    int pageIndex = HsScene::instance()->activePageIndex();

    QPointF delta(
        event->scenePos() - event->buttonDownScenePos(Qt::LeftButton));
    if (delta.x() < -pageSize.width() / 3) {
        pageIndex = qMin(pageIndex + 1, pages.count() - 1);
    } else if (pageSize.width() / 3 < delta.x()) {
        pageIndex = qMax(pageIndex - 1, 0);
    }

    HsScene::instance()->setActivePageIndex(pageIndex);
    
    startPageChangeAnimation(pageIndex, 200);

    emit event_waitInput();
}

/*!
    Handles orientation change events. \a orientation is the 
    new orientation.
*/
void HsIdleState::onOrientationChanged(Qt::Orientation orientation)
{
    QList<HsPage *> pages = HsScene::instance()->pages();
    QList<HsWidgetHost *> widgets;
    HsWidgetHost *widget = 0;
    
    const int KChromeHeight = 64; // TODO: get this somewhere
    const int KWidgetAdjust = 10; // TODO: get this somewhere
    
    for (int i = 0; i < pages.count(); ++i) {        
        widgets = pages[i]->widgets();
        for (int j = 0; j < widgets.count(); ++j) {
            widget = widgets[j];
            HsWidgetPresentationData presentation = widget->widgetPresentation(orientation);
            if (presentation.widgetId < 0) {
                QList<QRectF> geometries = 
                    HsWidgetPositioningOnOrientationChange::instance()->convert(
                        QRectF(0, KChromeHeight, pages[i]->rect().height(), pages[i]->rect().width()-KChromeHeight).adjusted(KWidgetAdjust, KWidgetAdjust, -KWidgetAdjust, -KWidgetAdjust),
                        QList<QRectF>() << widget->geometry(),
                        QRectF(0, KChromeHeight, pages[i]->rect().width(), pages[i]->rect().height()-KChromeHeight).adjusted(KWidgetAdjust, KWidgetAdjust, -KWidgetAdjust, -KWidgetAdjust));
                widget->setGeometry(geometries.first());
                widget->setWidgetPresentation();
            } else {
                widget->setGeometry(presentation.geometry());
                widget->setZValue(presentation.zValue);
            }
        }
    }
    
    action_idle_showActivePage();
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

/*!
    Handles page change zone hold events for the moveWidget state.
*/
void HsIdleState::moveWidget_onHoldTimeout()
{
    HsScene *scene = HsScene::instance();
    
    int pageIndex = scene->activePageIndex();

    if (isInLeftPageChangeZone() && 
        0 < pageIndex) {
        --pageIndex;
    } else if (isInRightPageChangeZone() && 
        pageIndex < scene->pages().count()) {
        ++pageIndex;
    } else {
        return;
    }

    if (pageIndex == scene->pages().count()) {
        if (scene->pages().last()->widgets().isEmpty()) {
            return;
        } else if (scene->pages().count() < scene->maximumPageCount()) {
            addPageToScene(pageIndex);
            mUiWidget->showPageIndicator();
            mUiWidget->pageIndicator()->addItem(pageIndex);
        } else {
            return; 
        }
    }

    scene->setActivePageIndex(pageIndex);
    startPageChangeAnimation(pageIndex, 200);
}

#ifndef Q_OS_SYMBIAN
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
    Switch the home screen language.
*/
void HsIdleState::switchLanguage()
{
    QString locale;
    QFile file("hslocale.txt");
    QTextStream stream(&file);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString word;
        stream >> word;
        if (!word.isEmpty()) {
            locale = word;
        } else {
            locale = "en_US";
        }
    } else {
        locale = QLocale::system().name();
    }

    if (locale == "en_US") {
        locale = "fi_FI";
    } else {
        locale = "en_US";
    }

    file.seek(0);
    stream << locale;
    file.close();
    
    QEvent event(QEvent::LocaleChange);
    QApplication::sendEvent(qApp, &event);
}

/*!
    Translates the home screen ui.
*/
void HsIdleState::translateUi()
{
    mView->setTitle("Home Screen"/*hbTrId(gTitleOfflineTextName)*/);
    action_waitInput_updateOptionsMenu();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
#endif // Q_OS_SYMBIAN

// Undefine the helper macros.
#undef ENTRY_ACTION
#undef EXIT_ACTION
#undef CONNECT_MOUSE_EVENT_HANDLER
#undef DISCONNECT_MOUSE_EVENT_HANDLER
