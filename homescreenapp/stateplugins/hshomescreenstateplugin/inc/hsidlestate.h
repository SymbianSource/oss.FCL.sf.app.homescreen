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

#ifndef HSIDLESTATE_H
#define HSIDLESTATE_H

#include <QState>
#include <QTimer>
#include <QPointF>

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class QGraphicsItem;
class QGraphicsSceneMouseEvent;
class QPropertyAnimation;
class HbView;
class HbAction;
class HbContinuousFeedback;
class HsIdleWidget;
class HsTitleResolver;
class QParallelAnimationGroup;
class HbMenu;
#ifdef Q_OS_SYMBIAN
class XQSettingsManager;
#endif

class HsIdleState : public QState
{
    Q_OBJECT

public:
    HsIdleState(QState *parent = 0);
    ~HsIdleState();

signals:
    void event_applicationLibrary();
    void event_waitInput();
    void event_widgetInteraction();
    void event_sceneInteraction();
    void event_moveWidget();
    void event_moveScene();
    void event_sceneMenu();
    void event_selectSceneWallpaper();
    void event_addPage();
    void event_removePage();
    void event_toggleConnection();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(HsIdleState)
    void setupStates();
    qreal pageLayerXPos(int pageIndex) const;
    void startPageChangeAnimation(int targetPageIndex, int duration);
    void startPageChangeZoneAnimation(int duration);
    bool isInPageChangeZone();
    bool isInLeftPageChangeZone();
    bool isInRightPageChangeZone();
    void addPageToScene(int pageIndex);
    qreal parallaxFactor() const;
    void updateZoneAnimation();
    void removeActivePage();

private slots:
    void action_disableUserInteraction();
    void action_enableUserInteraction();
    void action_idle_setupView();
    void action_idle_layoutNewWidgets();
    void action_idle_showActivePage();
    void action_idle_connectOrientationChangeEventHandler();
    void action_idle_installEventFilter();
    void action_idle_cleanupView();
    void action_idle_disconnectOrientationChangeEventHandler();
    void action_idle_uninstallEventFilter();
    void action_waitInput_updateOptionsMenu();
    void action_waitInput_connectMouseEventHandlers();
    void action_waitInput_publishIdleKey();
    void action_waitInput_disconnectMouseEventHandlers();
    void action_widgetInteraction_connectMouseEventHandlers();
    void action_widgetInteraction_connectGestureTimers();
    void action_widgetInteraction_disconnectMouseEventHandlers();
    void action_widgetInteraction_disconnectGestureTimers();
    void action_sceneInteraction_connectMouseEventHandlers();
    void action_sceneInteraction_connectGestureTimers();
    void action_sceneInteraction_disconnectMouseEventHandlers();
    void action_sceneInteraction_disconnectGestureTimers();
    void action_moveWidget_reparentToControlLayer();
    void action_moveWidget_startWidgetDragEffect();
    void action_moveWidget_connectMouseEventHandlers();
    void action_moveWidget_reparentToPage();
    void action_moveWidget_startWidgetDropEffect();
    void action_moveWidget_disconnectMouseEventHandlers();
    void action_moveScene_connectMouseEventHandlers();
    void action_moveScene_moveToNearestPage();
    void action_moveScene_disconnectMouseEventHandlers();
    void action_sceneMenu_showMenu();
    void action_addPage_addPage();
    void action_removePage_removePage();
    void action_toggleConnection_toggleConnection();
    void action_idle_setupTitle();
    void action_idle_cleanupTitle();

    void waitInput_onMousePressed(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void widgetInteraction_onMouseMoved(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void widgetInteraction_onMouseReleased(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void sceneInteraction_onMouseMoved(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void sceneInteraction_onMouseReleased(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void moveWidget_onMouseMoved(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void moveWidget_onMouseReleased(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void moveScene_onMouseMoved(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void moveScene_onMouseReleased(
        QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);

    void onOrientationChanged(Qt::Orientation orientation);

    void widgetInteraction_onTapAndHoldTimeout();
    void sceneInteraction_onTapAndHoldTimeout();

    void onTitleChanged(QString title);

    void onAddContentActionTriggered();

    bool openTaskSwitcher();
    void zoneAnimationFinished();
    void onSceneMenuAboutToClose();
    void onRemovePageMessageBoxClosed(HbAction *action);

private:
    HbView *mView;
    HbAction *mNavigationAction;
    HsIdleWidget *mUiWidget;
    QTimer mTimer;
    qreal mTapAndHoldDistance;
    qreal mPageChangeZoneWidth;

    QPointF mSceneMenuPos;
    HsTitleResolver *mTitleResolver;
    QPropertyAnimation *mZoneAnimation;
    bool mPageChanged;
    bool mAllowZoneAnimation;
    QParallelAnimationGroup *mPageChangeAnimation;
    HbContinuousFeedback *mContinuousFeedback;
    bool mTrashBinFeedbackAlreadyPlayed;

    qreal mDeltaX;
#ifdef Q_OS_SYMBIAN    
    XQSettingsManager *mSettingsMgr;
#endif    
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSIDLESTATE_H
