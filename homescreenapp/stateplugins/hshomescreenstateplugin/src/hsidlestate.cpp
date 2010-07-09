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
#include <QPropertyAnimation>
#include <QApplication>
#include <QVariantHash>

#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbAction>
#include <HbMessageBox>
#include <HbLabel>
#include <HbInstantFeedback>
#include <HbContinuousFeedback>
#include <HbPanGesture>
#include <HbTapAndHoldGesture>

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
#include "hsmessageboxwrapper.h"
#include "hspropertyanimationwrapper.h"

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));

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
    //const char hsLocTextId_ConfirmationButton_Ok[] = "txt_homescreen_button_ok";

    //Button in confirmation dialog while removing page with content
    //const char hsLocTextId_ConfirmationButton_Cancel[] = "txt_homescreen_button_cancel";
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
    mNavigationAction(0), 
    mUiWidget(0),
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
    ,mMessageBoxWrapper(0)
    
{
    setupStates();
    mTitleResolver = new HsTitleResolver(this);
     
    // TODO: Uncomment when updated API available
    //mContinuousFeedback = new HbContinuousFeedback;
    //mContinuousFeedback->setContinuousEffect(HbFeedback::ContinuousSmooth);
    //mContinuousFeedback->setIntensity(HbFeedback::IntensityFull );

    mVerticalSnapLineTimer.setSingleShot(true);
    mHorizontalSnapLineTimer.setSingleShot(true);
}

/*!
    Destroys this idle state.
*/
HsIdleState::~HsIdleState()
{
    if (mZoneAnimation) {
        mZoneAnimation->stop();
        delete mZoneAnimation;
    }
    if (mPageChangeAnimation) {
        mPageChangeAnimation->stop();
        delete mPageChangeAnimation;
    }
    
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
    \fn HsIdleState::event_moveWidget()

    This signal initiates a transition to the moveWidget state.
*/

/*!
    \fn HsIdleState::event_moveScene()

    This signal initiates a transition to the moveScene state.
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
    QState *state_moveWidget = new QState(this);
    QState *state_moveScene = new QState(this);
    HsWallpaperSelectionState *state_wallpaperSelectionState =
        new HsWallpaperSelectionState(this);
    QState *state_addPage = new QState(this);
    QState *state_preRemovePage = new QState(this);
    QState *state_removePage = new QState(this);
    QState *state_toggleConnection = new QState(this);

    // Transitions

    state_waitInput->addTransition(
        this, SIGNAL(event_addPage()), state_addPage);
    state_waitInput->addTransition(
        this, SIGNAL(event_preRemovePage()), state_preRemovePage);
    state_waitInput->addTransition(
        this, SIGNAL(event_toggleConnection()), state_toggleConnection);
    state_waitInput->addTransition(
        this, SIGNAL(event_selectWallpaper()), state_wallpaperSelectionState);
    state_waitInput->addTransition(
        this, SIGNAL(event_moveScene()), state_moveScene);
    state_waitInput->addTransition(
        this, SIGNAL(event_moveWidget()), state_moveWidget);

    state_moveWidget->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);

    state_moveScene->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);

    state_wallpaperSelectionState->addTransition(
        state_wallpaperSelectionState, SIGNAL(event_waitInput()), state_waitInput);

    state_addPage->addTransition(state_waitInput);
    
    state_preRemovePage->addTransition(
        this,SIGNAL(event_removePage()),state_removePage);
    
    state_preRemovePage->addTransition(
        this,SIGNAL(event_waitInput()),state_waitInput);

    state_removePage->addTransition(
        this,SIGNAL(event_waitInput()),state_waitInput);

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
    ENTRY_ACTION(state_waitInput, action_waitInput_connectGestureHandlers)
    ENTRY_ACTION(state_waitInput, action_waitInput_publishIdleKey)
    EXIT_ACTION(state_waitInput, action_waitInput_disconnectGestureHandlers)
    EXIT_ACTION(state_waitInput, action_waitInput_resetNewWidgets)

    ENTRY_ACTION(state_moveWidget, action_moveWidget_reparentToControlLayer)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_startWidgetDragEffect)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectGestureHandlers)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_setWidgetSnap)

    EXIT_ACTION(state_moveWidget, action_moveWidget_reparentToPage)
    EXIT_ACTION(state_moveWidget, action_moveWidget_startWidgetDropEffect)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectGestureHandlers)
    EXIT_ACTION(state_moveWidget, action_moveWidget_preventZoneAnimation)
    EXIT_ACTION(state_moveWidget, action_moveWidget_deleteWidgetSnap)

    ENTRY_ACTION(state_moveScene, action_moveScene_connectGestureHandlers)
    EXIT_ACTION(state_moveScene, action_moveScene_moveToNearestPage)
    EXIT_ACTION(state_moveScene, action_moveScene_disconnectGestureHandlers)
    
    ENTRY_ACTION(state_addPage, action_addPage_addPage)
    
    ENTRY_ACTION(state_preRemovePage, action_preRemovePage_showQuery)

    ENTRY_ACTION(state_removePage, action_removePage_startRemovePageAnimation)
    EXIT_ACTION(state_removePage, action_removePage_removePage)

    ENTRY_ACTION(state_toggleConnection, action_toggleConnection_toggleConnection)
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
    if (mPageChangeAnimation->isRunning()) {
        mPageChangeAnimation->stop();
    }        
    mPageChangeAnimation->disconnect(this);
    mPageChangeAnimation->setEndValue(pageLayerXPos(targetPageIndex));
    mPageChangeAnimation->setDuration(duration);
    connect(mPageChangeAnimation,
            SIGNAL(finished()),
            SLOT(pageChangeAnimationFinished()), 
            Qt::UniqueConnection);
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
            SLOT(zoneAnimationFinished()),
            Qt::UniqueConnection);
    
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
    return mWidgetHotSpot.x() < HSCONFIGURATION_GET(pageChangeZoneWidth);
}

/*!
    Checks if touch point in the active widget is located inside the right
    page change zone.
*/
bool HsIdleState::isInRightPageChangeZone()
{
    qreal pageWidth = HsScene::mainWindow()->layoutRect().width();
    return mWidgetHotSpot.x() > pageWidth - HSCONFIGURATION_GET(pageChangeZoneWidth);
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
    if (!mAllowZoneAnimation) {
        mZoneAnimation->stop();
    } else if (isInPageChangeZone()) {
        // should we start it
        if (mZoneAnimation->state() == QAbstractAnimation::Stopped &&
            !mPageChangeAnimation->isRunning()) {
            startPageChangeZoneAnimation(HSCONFIGURATION_GET(pageChangeZoneAnimationDuration));
        } 
    } else if (mZoneAnimation->state() == QAbstractAnimation::Running) {
        // Not in zone, but still running     
        if (mZoneAnimation->direction() == QAbstractAnimation::Forward){
            // reverse
            mZoneAnimation->setDuration(HSCONFIGURATION_GET(pageChangeZoneReverseAnimationDuration));
            mZoneAnimation->setDirection(QAbstractAnimation::Backward);
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
        mNavigationAction->setObjectName("applib_navigation_action");
        connect(mNavigationAction, SIGNAL(triggered()), SIGNAL(event_applicationLibrary()));
        idleView->setNavigationAction(mNavigationAction);

        HsGui::setIdleView(idleView);
        
        delete mPageChangeAnimation;
        mPageChangeAnimation = NULL;
        mPageChangeAnimation = new HsPropertyAnimationWrapper;
        mPageChangeAnimation->setTargetObject(mUiWidget);
        mPageChangeAnimation->setPropertyName("sceneX"); 
        delete mZoneAnimation;
        mZoneAnimation = NULL;
        mZoneAnimation = new QPropertyAnimation(mUiWidget, "sceneX");
       
    }

    HsScene::mainWindow()->setCurrentView(idleView);
}

/*!
    Sets the idle view's title.
*/
void HsIdleState::action_idle_setupTitle()
{    
    onTitleChanged(mTitleResolver->title());
    connect(mTitleResolver, 
        SIGNAL(titleChanged(QString)), 
        SLOT(onTitleChanged(QString)));
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
void HsIdleState::onAddContentFromContextMenuActionTriggered()
{
    /* include position information in homescreen data,
       which passed around in applib and returned when adding content
    */
    QVariant homescreenData(mPageHotSpot);
    machine()->postEvent(
        HsMenuEventFactory::createOpenAppLibraryEvent(AddHsMenuMode, homescreenData));
    mPageHotSpot = QPointF();
}

/*!

*/
void HsIdleState::onAddContentFromOptionsMenuActionTriggered()
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

*/
void HsIdleState::onPageTapAndHoldFinished(QGestureEvent *event)
{
    mPageHotSpot = qobject_cast<HbTapAndHoldGesture *>(
        event->gesture(Qt::TapAndHoldGesture))->scenePosition();

    mSceneMenu = new HbMenu();
    mSceneMenu->setObjectName("hs_ctx_menu");
    mSceneMenu->setAttribute(Qt::WA_DeleteOnClose);

    HbAction *action = mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent), this,
	                      SLOT(onAddContentFromContextMenuActionTriggered()));
    action->setObjectName("action_add_content_ctx_menu");

    HsScene *scene = HsScene::instance();
    if (scene->pages().count() < HSCONFIGURATION_GET(maximumPageCount)) {
        action = mSceneMenu->addAction(hbTrId(hsLocTextId_OptionsMenu_AddPage), this, SIGNAL(event_addPage()));
        action->setObjectName("action_add_page_ctx_menu");
    }

    action = mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_ChangeWallpaper), this, SIGNAL(event_selectWallpaper()));
    action->setObjectName("action_change_wallpaper_ctx_menu");
    mSceneMenu->setPreferredPos(mPageHotSpot);
    mSceneMenu->open();
}

void HsIdleState::onPagePanStarted(QGestureEvent *event)
{
    Q_UNUSED(event)
    emit event_moveScene();
}
 
void HsIdleState::onPagePanUpdated(QGestureEvent *event)
{
    HbPanGesture *gesture = qobject_cast<HbPanGesture *>(
        event->gesture(Qt::PanGesture));

    mDeltaX = gesture->sceneOffset().x();

    HsScene *scene = HsScene::instance();
    int bounceEffect = HSCONFIGURATION_GET(bounceEffect);
    qreal x = qBound(pageLayerXPos(scene->pages().count() - 1) - bounceEffect / 2 / mUiWidget->parallaxFactor(),
                     pageLayerXPos(scene->activePageIndex()) + mDeltaX,
                     pageLayerXPos(0) + (bounceEffect / 2 / mUiWidget->parallaxFactor()));

    mUiWidget->setSceneX(x);
}
 
void HsIdleState::onPagePanFinished(QGestureEvent *event)
{
    HbPanGesture *gesture = qobject_cast<HbPanGesture *>(
        event->gesture(Qt::PanGesture));
    mDeltaX = gesture->sceneOffset().x();
    emit event_waitInput();
}

void HsIdleState::onWidgetTapStarted(HsWidgetHost *widget)
{
    HsScene *scene = HsScene::instance();
    scene->setActiveWidget(widget);
    HsPage *page = scene->activePage();
    QMetaObject::invokeMethod(page, "updateZValues", Qt::QueuedConnection);
}
 
void HsIdleState::onWidgetTapAndHoldFinished(QGestureEvent *event, HsWidgetHost *widget)
{
    HsScene *scene = HsScene::instance();
    scene->setActiveWidget(widget);
    
    mWidgetHotSpot = qobject_cast<HbTapAndHoldGesture *>(
        event->gesture(Qt::TapAndHoldGesture))->scenePosition();
    mWidgetHotSpotOffset = mWidgetHotSpot - widget->pos();
    
    emit event_moveWidget();
}
 
void HsIdleState::onWidgetMoveUpdated(const QPointF &scenePos, HsWidgetHost *widget)
{
    QRectF widgetRect = widget->geometry();

    // Move widget to updated position.
    mWidgetHotSpot = scenePos;
    widgetRect.moveTopLeft(mWidgetHotSpot - mWidgetHotSpotOffset);

    // When moving widget can go over the pages left border.
    qreal lowerBoundX = -widgetRect.width();

    // When moving widget can go over the pages right border.
    QRectF pageRect = HsGui::idleView()->rect();
    qreal upperBoundX = pageRect.width();

    // When moving widget can go under the chrome at the pages upper border.
    // Notice that chrome height is 64 pixels.
    qreal lowerBoundY = qreal(64) - widgetRect.height();

    // When moving widget can go over the pages down border.
    qreal upperBoundY = pageRect.height();

    qreal widgetX = qBound(lowerBoundX, widgetRect.x(), upperBoundX);
    qreal widgetY = qBound(lowerBoundY, widgetRect.y(), upperBoundY);

    // If using ItemClipsChildrenToShape-flag in widgethost then
    // setPos does not update position here, however setGeometry does it, QT bug?
    widget->setGeometry(widgetX, widgetY, widgetRect.width(), widgetRect.height());
    
    if (HSCONFIGURATION_GET(isSnapEnabled)) {
        mSnapResult = HsWidgetPositioningOnWidgetMove::instance()->run(widget->sceneBoundingRect());

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            if (mSnapResult.hasHorizontalSnap) {
                showVerticalLine();
            }
            else {
                hideVerticalLine();
            }
            if (mSnapResult.hasVerticalSnap) {
                showHorizontalLine();
                }
            else {
                hideHorizontalLine();
            }
        }

        mPreviousSnapResult = mSnapResult;
    }

    // Handle effects:
    // User is indicated by a tactile feedback if he/she is trying to move
    // widget over the first or the last page.
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    int bounceFeedbackEffectDistance = HSCONFIGURATION_GET(bounceFeedbackEffectDistance);
    if( (page == scene->pages().first() && mWidgetHotSpot.x() < bounceFeedbackEffectDistance ) ||
        (page == scene->pages().last() && scene->pages().count() == HSCONFIGURATION_GET(maximumPageCount)
         && mWidgetHotSpot.x() > pageRect.width() - bounceFeedbackEffectDistance)) {
             HbInstantFeedback::play(HSCONFIGURATION_GET(widgetMoveBlockedFeedbackEffect));
            // TODO: use code below when Orbit has updated ContinuousFeedback API
            //if (!mContinuousFeedback->isPlaying()) {
            //    mContinuousFeedback->play();
            //}
        } else /*if (mContinuousFeedback->isPlaying())*/ {
            //mContinuousFeedback->stop();
        }

    updateZoneAnimation();
    showTrashBin();
}
 
void HsIdleState::onWidgetMoveFinished(const QPointF &scenePos, HsWidgetHost *widget)
{
    Q_UNUSED(scenePos)
    Q_UNUSED(widget)
    emit event_waitInput();
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
        SLOT(action_idle_orientationChanged()));
}

void HsIdleState::action_idle_orientationChanged()
{
    HsScene::instance()->activePage()->resetNewWidgets();
    action_idle_showActivePage();
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

    //Close options menu on view change
    HsGui::idleView()->setMenu(NULL);

    //Close context menu on view change
    if (mSceneMenu) {
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
        this, SLOT(action_idle_orientationChanged()));
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
    menu->setObjectName("hs_menu");
    // Task switcher
    menu->addAction(hbTrId(hsLocTextId_OptionsMenu_TaskSwitcher),
        this, SLOT(openTaskSwitcher()));
    
    // Add content
    menu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent),
        this, SLOT(onAddContentFromOptionsMenuActionTriggered()));
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
            this, SIGNAL(event_preRemovePage()));
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

void HsIdleState::action_waitInput_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();

    connect(scene, 
        SIGNAL(pageTapAndHoldFinished(QGestureEvent*)), 
        SLOT(onPageTapAndHoldFinished(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene,
        SIGNAL(pagePanStarted(QGestureEvent*)),
        SLOT(onPagePanStarted(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene, 
        SIGNAL(widgetTapStarted(HsWidgetHost*)), 
        SLOT(onWidgetTapStarted(HsWidgetHost*)),
        Qt::UniqueConnection);
    
    connect(scene, 
        SIGNAL(widgetTapAndHoldFinished(QGestureEvent*,HsWidgetHost*)), 
        SLOT(onWidgetTapAndHoldFinished(QGestureEvent*,HsWidgetHost*)),
        Qt::UniqueConnection);
}

void HsIdleState::action_waitInput_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

/*!
    Resets new widget layout.
*/
void HsIdleState::action_waitInput_resetNewWidgets()
{
    HsScene::instance()->activePage()->resetNewWidgets();
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

void HsIdleState::action_moveWidget_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
        
    connect(scene, 
        SIGNAL(widgetMoveUpdated(const QPointF&,HsWidgetHost*)), 
        SLOT(onWidgetMoveUpdated(const QPointF&,HsWidgetHost*)),
        Qt::UniqueConnection);
    
    connect(scene, 
        SIGNAL(widgetMoveFinished(const QPointF&,HsWidgetHost*)), 
        SLOT(onWidgetMoveFinished(const QPointF&,HsWidgetHost*)),
        Qt::UniqueConnection);
}

/*!
    Initializes the presentation to the snapping algorithm
    Connects the SIGNAL for changing the presentation on active page changed
    Connects the timer for showing the snap lines
*/
void HsIdleState::action_moveWidget_setWidgetSnap()
{
    if (HSCONFIGURATION_GET(isSnapEnabled)) {

        QVariantHash snapConfiguration;
        snapConfiguration[SNAPENABLED] = QString::number(HSCONFIGURATION_GET(isSnapEnabled));
        snapConfiguration[SNAPFORCE] = QString::number(HSCONFIGURATION_GET(snapForce));
        snapConfiguration[SNAPGAP] = QString::number(HSCONFIGURATION_GET(snapGap));
        HsWidgetPositioningOnWidgetMove::instance()->setConfiguration(snapConfiguration);

        updatePagePresentationToWidgetSnap();

        connect(HsScene::instance(), SIGNAL(activePageChanged()),
            SLOT(onActivePageChanged()));

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            mVerticalSnapLineTimer.setInterval(HSCONFIGURATION_GET(snapTimeout));
            mHorizontalSnapLineTimer.setInterval(HSCONFIGURATION_GET(snapTimeout));

            connect(&mVerticalSnapLineTimer, SIGNAL(timeout()),
                SLOT(onVerticalSnapLineTimerTimeout()));
            connect(&mHorizontalSnapLineTimer, SIGNAL(timeout()),
                SLOT(onHorizontalSnapLineTimerTimeout()));

        }
    }
}

/*!
    Create the list of inactive rectangles on the page.
*/
QList<QRectF> HsIdleState::createInactiveWidgetRects()
{
    HsScene *scene = HsScene::instance();
    HsPage *activePage = scene->activePage();
    HsWidgetHost *activeWidget = scene->activeWidget();

    QList<QRectF> incativeWidgetRects;

    foreach (HsWidgetHost* widget, activePage->widgets()) {
        //If the above function returns the active widget, skip over it as widget and mMovingRect represent same widget.
        if (widget == activeWidget) {
           continue;
        }
        QRectF widgetRect = widget->geometry();
        incativeWidgetRects.append(widgetRect);
    }
    return incativeWidgetRects;
}

/*!
    Reparents the active widget to the active page.
*/
void HsIdleState::action_moveWidget_reparentToPage()
{
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

        //Set the snap position of the widget and save the position
        QRectF widgetRect = widget->geometry();
        if (mSnapResult.hasHorizontalSnap) {
            widgetRect.moveLeft(mSnapResult.horizontalSnapPosition);
        }
        if (mSnapResult.hasVerticalSnap) {
            widgetRect.moveTop(mSnapResult.verticalSnapPosition);
        }

        QPointF adjustedWidgetPosition = page->adjustedWidgetPosition(widgetRect);
        widget->setPos(adjustedWidgetPosition);

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

void HsIdleState::action_moveWidget_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

/*!
    Resets the snap position
    Disconnect the Active Page Changed SIGNAL
    Disconnects the timers to show snap lines
*/
void HsIdleState::action_moveWidget_deleteWidgetSnap()
{
    if (HSCONFIGURATION_GET(isSnapEnabled)) {
        resetSnapPosition();

        disconnect(HsScene::instance(), SIGNAL(activePageChanged()),
                    this, SLOT(onActivePageChanged()));

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            disconnect(&mVerticalSnapLineTimer, SIGNAL(timeout()),
                this, SLOT(onVerticalSnapLineTimerTimeout()));
            disconnect(&mHorizontalSnapLineTimer, SIGNAL(timeout()),
                this, SLOT(onHorizontalSnapLineTimerTimeout()));
        }
    }
}

/*!
    Prevents page change zone animation.
*/
void HsIdleState::action_moveWidget_preventZoneAnimation()
{
    mAllowZoneAnimation = false;
}

void HsIdleState::action_moveScene_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();

    connect(scene,
        SIGNAL(pagePanUpdated(QGestureEvent*)),
        SLOT(onPagePanUpdated(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene,
        SIGNAL(pagePanFinished(QGestureEvent*)),
        SLOT(onPagePanFinished(QGestureEvent*)),
        Qt::UniqueConnection);
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

void HsIdleState::action_moveScene_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

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

void HsIdleState::action_preRemovePage_showQuery()
{
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();

    if (!page->widgets().isEmpty()) {
        if (!mMessageBoxWrapper) {
            mMessageBoxWrapper = new HsMessageBoxWrapper(this);
            mMessageBoxWrapper->setHeader(hbTrId(hsLocTextId_Title_RemovePage));
            mMessageBoxWrapper->setQueryText(hbTrId(hsLocTextId_Confirmation_RemovePage));
            connect(mMessageBoxWrapper,SIGNAL(accepted()),SIGNAL(event_removePage())); 
            connect(mMessageBoxWrapper,SIGNAL(rejected()),SIGNAL(event_waitInput())); 
        }
        mMessageBoxWrapper->show();
    } else {
        // Empty page can be removed without confirmation
        emit event_removePage();
    }
}

/*!
    Start remove page animation.
*/
void HsIdleState::action_removePage_startRemovePageAnimation()
{
    HsScene *scene = HsScene::instance();
   
    int pageToRemoveIndex = scene->activePageIndex();
    bool isLastPage(scene->activePage() == scene->pages().last());
    
    int nextPageIndex(pageToRemoveIndex);
    nextPageIndex++;
    if (isLastPage) {
        nextPageIndex--; 
    }
    
    if (mPageChangeAnimation->isRunning()) {
        mPageChangeAnimation->stop();
    }
    mPageChangeAnimation->disconnect(this);
    connect(mPageChangeAnimation,
            SIGNAL(finished()),
            SIGNAL(event_waitInput()), 
            Qt::UniqueConnection);
    mPageChangeAnimation->setEndValue(pageLayerXPos(nextPageIndex));
    mPageChangeAnimation->setDuration(HSCONFIGURATION_GET(pageRemovedAnimationDuration));
   
    mPageChangeAnimation->start();
    
    HbInstantFeedback::play(HSCONFIGURATION_GET(pageChangeFeedbackEffect));
    
}

/*!
    Removes an existing page from the scene.
*/
void HsIdleState::action_removePage_removePage()
{
    if (mPageChangeAnimation->isRunning()) {
        mPageChangeAnimation->stop();
    }
    HsScene *scene = HsScene::instance();
    HsPage *pageToRemove = scene->activePage();
    
    // remove from ui
    mUiWidget->removePage(pageToRemove->pageIndex());
    mUiWidget->showPageIndicator();
    // update data model
    HsScene::instance()->removePage(pageToRemove);
    // Take new active page (previous was removed) and move scene to right position
    qreal x = pageLayerXPos(scene->activePageIndex());
    mUiWidget->setSceneX(x);
    // delete it   
    pageToRemove->deleteLater();
   
}
/*!
    Toggles the homescreen online/offline state.
*/
void HsIdleState::action_toggleConnection_toggleConnection()
{
    HsScene *scene = HsScene::instance();
    scene->setOnline(!scene->isOnline());
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
        } else if (isInRightPageChangeZone() &&
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
    } else {
        scene->setActivePageIndex(pageIndex);
        mUiWidget->setActivePage(pageIndex);
    }
}

/*!
    Page change animation has been finished.
 */
void HsIdleState::pageChangeAnimationFinished()
{
    updateZoneAnimation();
}


/*!
    Provides the page presentation to the Widget Snap algorithm
*/
void HsIdleState::updatePagePresentationToWidgetSnap()
{
    QRectF containerRect = HsScene::instance()->activePage()->contentGeometry();
    HsWidgetHost *activeWidget = HsScene::instance()->activeWidget();
    HsWidgetPositioningOnWidgetMove::instance()->setPagePresentation(containerRect, createInactiveWidgetRects(), activeWidget->geometry());
}

/*!
    Reset Snap position, hide the snap lines
*/
void HsIdleState::resetSnapPosition()
{
    mSnapResult = HsWidgetPositioningOnWidgetMove::Result();
    mPreviousSnapResult = HsWidgetPositioningOnWidgetMove::Result();
    hideVerticalLine();
    hideHorizontalLine();
}

/*!
    Show the Vertical line for Snap guidance
*/
void HsIdleState::showVerticalLine()
{
    if (mPreviousSnapResult.verticalSnapLine.x1() != mSnapResult.verticalSnapLine.x1()) {
        hideVerticalLine();
        mVerticalSnapLineTimer.start();
    }
    else {
        //As the Vertical Line position is at the same place,
        //the timer was started when vertical line positions were at different locations
        //the line will be shown when the timer expires.
        //If timer has already expired, just show the line, which is redrawn to new geometry.
        if (!mVerticalSnapLineTimer.isActive()) {
            mUiWidget->showVerticalSnapLine(mSnapResult.verticalSnapLine);
        }
    }
}

/*!
    Timer for showing the Vertical line expires
*/
void HsIdleState::onVerticalSnapLineTimerTimeout()
{
    mUiWidget->showVerticalSnapLine(mSnapResult.verticalSnapLine);
}

/*!
    Hide the Vertical line for Snap guidance
*/
void HsIdleState::hideVerticalLine()
{
    mUiWidget->hideVerticalSnapLine();
    mVerticalSnapLineTimer.stop();
}

/*!
    Show the Horizontal line for Snap guidance
*/
void HsIdleState::showHorizontalLine()
{
    if (mPreviousSnapResult.horizontalSnapLine.y1() != mSnapResult.horizontalSnapLine.y1()) {
        hideHorizontalLine();
        mHorizontalSnapLineTimer.start();
    }
    else {
        if (!mHorizontalSnapLineTimer.isActive()) {
            mUiWidget->showHorizontalSnapLine(mSnapResult.horizontalSnapLine);
        }
    }
}

/*!
    Timer for showing the Horizontal line expires
*/
void HsIdleState::onHorizontalSnapLineTimerTimeout()
{
    mUiWidget->showHorizontalSnapLine(mSnapResult.horizontalSnapLine);
}

/*!
    Hide the Horizontal line for Snap guidance
*/
void HsIdleState::hideHorizontalLine()
{
    mUiWidget->hideHorizontalSnapLine();
    mHorizontalSnapLineTimer.stop();
}

/*!
    Handles updating the Snap algorithm with page presentation on page change
*/
void HsIdleState::onActivePageChanged()
{
    updatePagePresentationToWidgetSnap();
    resetSnapPosition();
}


