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

#include <QApplication>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QGraphicsLinearLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneResizeEvent>
#include <QGesture>
#include <QGraphicsScene>

#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstantFeedback>
#include <HbTouchArea>

#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswidgettoucharea.h"
#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include "hsconfiguration.h"
#include "hscontentservice.h"

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));

QTM_USE_NAMESPACE


/*!
    \class HsWidgetHost
    \ingroup group_hsdomainmodel
    \brief Each widget is controlled by the home screen framework through a widget host.
*/

/*!
    Constructs a new widget host with given \a databaseId and
    \a parent item.
*/
HsWidgetHost::HsWidgetHost(int databaseId, QGraphicsItem *parent)
  : HbWidget(parent),
    mDatabaseId(databaseId),
    mStateMachine(0),
    mWidget(0),
    mPage(0),
    mComponent(0),
    mIsFinishing(false)
{
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    setFlag(QGraphicsItem::ItemHasNoContents);

    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
    grabGesture(Qt::CustomGesture);
    
    setupTouchArea();
    setupEffects();
    setupStates();
}

/*!
    Destructor.
*/
HsWidgetHost::~HsWidgetHost()
{
}

/*!
    Creates a new widget host instance based on the give
    \a widgetData and \a preferences. Returns the created
    instance. Return 0 in failure cases.
*/
HsWidgetHost *HsWidgetHost::createInstance(HsWidgetData &widgetData, 
                                           const QVariantHash &preferences)
{
    HsDatabase *db = HsDatabase::instance();

    if (db->insertWidget(widgetData)) {
        db->setWidgetPreferences(widgetData.id, preferences);
        return new HsWidgetHost(widgetData.id);
    } else {
        return 0;
    }
}

/*!
    Returns the databaseId.
*/
int HsWidgetHost::databaseId() const
{
    return mDatabaseId;
}

/*!
    Sets the containing \a page for this widget host.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::setPage(HsPage *page)
{
    HsDatabase *db = HsDatabase::instance();
    
    HsWidgetData data;
    data.id = mDatabaseId;
    if (db->widget(data)) {
        if (!page) {
            data.pageId = -1;
        } else {
            data.pageId = page->databaseId();
        }
        if (!db->updateWidget(data)) {
            return false;
        }
    } else {
        return false;
    }

    mPage = page;
    return true;
}
 
/*!
    Returns the containing page for this widget. Returns 0 
    if this widget has no containg page.
*/
HsPage *HsWidgetHost::page() const
{
    return mPage;
}

/*!
    Loads presentation based on the current orientation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::loadPresentation()
{
    return loadPresentation(HsScene::orientation());
}

/*!
    Loads presentation based on the given \a orientation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::loadPresentation(Qt::Orientation orientation)
{
    HsDatabase *db = HsDatabase::instance();
        
    HsWidgetPresentationData data;
    data.orientation = orientation;
    data.widgetId = mDatabaseId;
    if (!db->widgetPresentation(data)) {
        return false;
    }
    setPos(data.x, data.y);
    setZValue(data.zValue);
    return true;
}

/*!
    Saves the current presentation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::savePresentation()
{
    return savePresentation(HsScene::orientation());
}

/*!
    Saves the current presentation for the given \a orientation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::savePresentation(Qt::Orientation orientation)
{
    HsDatabase *db = HsDatabase::instance();
        
    HsWidgetPresentationData data;
    data.orientation = orientation;
    data.setPos(pos());
    data.zValue = zValue();
    data.widgetId = mDatabaseId;
    return db->setWidgetPresentation(data);
}

/*!
    Saves the given presentation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::savePresentation(HsWidgetPresentationData &presentation)
{
    HsDatabase *db = HsDatabase::instance();
    
    presentation.widgetId = mDatabaseId;
    return db->setWidgetPresentation(presentation);
}

/*!
    Fills the \a presentation based on the orientation field in 
    the given \a presentation. Returns true on success, otherwise 
    returns false.
*/
bool HsWidgetHost::getPresentation(HsWidgetPresentationData &presentation)
{
    HsDatabase *db = HsDatabase::instance();
        
    presentation.widgetId = mDatabaseId;
    return db->widgetPresentation(presentation);
}

/*!
    Removes the presentation for the given \a orientation.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::removePresentation(Qt::Orientation orientation)
{
    HsDatabase *db = HsDatabase::instance();
    return db->deleteWidgetPresentation(mDatabaseId, orientation);
}

/*!
    Reimplemented from QGraphicsItem. Returns the shape of the
    this widget host. The shape is computed based on the contained
    widget.
*/
QPainterPath HsWidgetHost::shape() const
{
    QPainterPath path;

    if (mWidget) {
        QRectF currRect = rect();
        path = mWidget->shape();

        QRectF pathRect(path.boundingRect());

        if (pathRect.width() > currRect.width()
            || pathRect.height() > currRect.height()) {
            QPainterPath newPath(currRect.topLeft());
            newPath.addRect(currRect);
            path = path.intersected(newPath);
        }
    }
    return path;
}

/*!
    \fn HsWidgetHost::event_startAndShow()
    Initiates a transition to show state.
*/

/*!
    \fn HsWidgetHost::event_startAndHide()
    Initiates a transition to hide state.
*/

/*!
    \fn HsWidgetHost::event_unload()
    Initiates a transition to unloaded state.
*/

/*!
    \fn HsWidgetHost::event_show()
    Initiates a transition to show state.
*/

/*!
    \fn HsWidgetHost::event_hide()
    Initiates a transition to hide state.
*/

/*!
    \fn HsWidgetHost::event_remove()
    Initiates a transition to remove state.
*/

/*!
    \fn HsWidgetHost::event_close()
    Initiates a transition to final state.
*/

/*!
    \fn HsWidgetHost::event_finished()
    Initiates a transition to finished state.
*/

/*!
    \fn HsWidgetHost::event_faulted()
    Initiates a transition to faulted state.
*/

/*!
    \fn HsWidgetHost::finished()
    Notifies the home screen framework that this widget
    host has moved to finished state.
*/

/*!
    \fn HsWidgetHost::faulted()
    Notifies the home screen framework that this widget
    host has moved to faulted state.
*/

/*!
    \fn HsWidgetHost::resized()
    Notifies the home screen framework that this widget
    host has resized itself.
*/

/*!
    \fn HsWidgetHost::available()
    Notifies the home screen framework that this widget
    is now available.
*/

/*!
    \fn HsWidgetHost::unavailable()
    Notifies the home screen framework that this widget
    is temporarily unavailable.
*/


/*!
    Starts the widget. The \a show parameter defines the
    visibility of the widget.
*/
void HsWidgetHost::startWidget(bool show)
{
    if (!mStateMachine->isRunning()) {
        mStateMachine->start();
        // This is needed because QStateMachine::start() starts
        // the state machine asynchronously via the eventloop. 
        // Here we want the machine to start synchronously.
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    if (show) {
        emit event_startAndShow();
    } else {
        emit event_startAndHide();
    }
}
 
/*!
    Puts the contained widget into show state.
*/
void HsWidgetHost::showWidget()
{
    emit event_show();
}
  
/*!
    Puts the contained widget into hidden state.
*/
void HsWidgetHost::hideWidget()
{
    emit event_hide();
}

/*!
    Notifies the conained widget about \a online
    status changes.
*/
void HsWidgetHost::setOnline(bool online)
{
    mIsOnlineProperty.write(mWidget, online);
}

/*!
    Deletes this widget instance, including the 
    database entries.
*/
void HsWidgetHost::remove()
{
    if (mStateMachine->isRunning()) {
        emit event_remove();
    } else {
        action_remove();
        deleteLater();
    }
}
 
/*!
    Deletes this widget instance, leaving the 
    database entries untouched.
*/
void HsWidgetHost::close()
{
    if (mStateMachine->isRunning()) {
        emit event_close();
    } else {
        deleteLater();
    }
}

/*!
    Starts the drag effect.
*/
void HsWidgetHost::startDragEffect()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */
    HbInstantFeedback::play(HbFeedback::ItemPick);

    setTransformOriginPoint(rect().center());

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(HSCONFIGURATION_GET(widgetDragEffectDuration));
    animation->setEndValue(1.1);
    animationGroup->addAnimation(animation);

    /* TODO: Uncomment after the Qt bug has been fixed.
    animation = new QPropertyAnimation(effect, "offset");
    animation->setDuration(200);
    animation->setEndValue(QPointF(8 ,8));
    animationGroup->addAnimation(animation);
    */

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
    Starts the drop effect.
*/
void HsWidgetHost::startDropEffect()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */
    HbInstantFeedback::play(HbFeedback::ItemDrop);

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(HSCONFIGURATION_GET(widgetDropEffectDuration));
    animation->setEndValue(1);
    animationGroup->addAnimation(animation);

    /* TODO: Uncomment after the Qt bug has been fixed.
    animation = new QPropertyAnimation(effect, "offset");
    animation->setDuration(200);
    animation->setEndValue(QPointF(3, 3));
    animationGroup->addAnimation(animation);
    */

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
    Reimplemented from QObject for monitoring changes in 
    contained widget's size.
*/
bool HsWidgetHost::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneResize ) {
        QGraphicsSceneResizeEvent *resizeEvent = 
            static_cast<QGraphicsSceneResizeEvent *>(event);
        setNewSize(resizeEvent->newSize());
        emit resized();
    }
    return HbWidget::eventFilter(watched, event);
}

/*!
    Reimplemented from HbWidget for pan gesture handling.
*/
void HsWidgetHost::gestureEvent(QGestureEvent *event)
{
    HsScene *scene = HsScene::instance();    
    QGesture *gesture = event->gesture(Qt::PanGesture);
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureStarted:
                grabMouse();
                emit scene->pagePanStarted(event);
                break;
            case Qt::GestureUpdated:
                emit scene->pagePanUpdated(event);
                break;
            case Qt::GestureFinished:
            case Qt::GestureCanceled:
                ungrabMouse();
                emit scene->pagePanFinished(event);
                break;
            default:
                break;
        }
    }
}

/*!
    \fn HsWidgetHost::mousePressEvent(QGraphicsSceneMouseEvent *)
    Reimplemented from QGraphicsItem for eating all mouse presses.
*/

/*!
    Configures the touch are for this widget host.
*/
void HsWidgetHost::setupTouchArea()
{
    mTouchArea = new HsWidgetTouchArea(this);
    mTouchArea->setZValue(1);
}

/*!
    Configures the effects for this widget host.
*/
void HsWidgetHost::setupEffects()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        new QGraphicsDropShadowEffect(this);
    effect->setColor(QColor(0, 0, 0, 150));
    effect->setBlurRadius(5);
    effect->setOffset(3);
    setGraphicsEffect(effect);
    */
}

/*!
    Configures the state machine for this widget host.
*/
void HsWidgetHost::setupStates()
{
    // State machine

    mStateMachine = new QStateMachine(this);
    mStateMachine->setAnimated(false);
    
    // States

    QState *state_component = new QState;
    QState *state_unloaded = new QState(state_component);
    QState *state_running = new QState(state_component);
    QState *state_show = new QState(state_running);
    QState *state_hide = new QState(state_running);
    QState *state_finished = new QState;
    QState *state_faulted = new QState;
    QState *state_remove = new QState;
    QFinalState *state_final = new QFinalState;

    mStateMachine->addState(state_component);
    mStateMachine->addState(state_finished);
    mStateMachine->addState(state_faulted);
    mStateMachine->addState(state_remove);
    mStateMachine->addState(state_final);

    mStateMachine->setInitialState(state_component);
    state_component->setInitialState(state_unloaded);
    state_running->setInitialState(state_hide);

    // Transitions

    state_component->addTransition(
        this, SIGNAL(event_close()), state_final);
    state_component->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_component->addTransition(
        this, SIGNAL(event_finished()), state_finished);
    state_component->addTransition(
        this, SIGNAL(event_faulted()), state_faulted);

    state_unloaded->addTransition(
        this, SIGNAL(event_startAndShow()), state_show);
    state_unloaded->addTransition(
        this, SIGNAL(event_startAndHide()), state_hide);

    state_running->addTransition(
        this, SIGNAL(event_unload()), state_unloaded);
    
    state_show->addTransition(
        this, SIGNAL(event_hide()), state_hide);

    state_hide->addTransition(
        this, SIGNAL(event_show()), state_show);

    state_finished->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_finished->addTransition(
        this, SIGNAL(event_close()), state_final);

    state_faulted->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_faulted->addTransition(
        this, SIGNAL(event_close()), state_final);

    state_remove->addTransition(state_final);

    // Actions

    ENTRY_ACTION(state_component, action_connectComponent)
    EXIT_ACTION(state_component, action_disconnectComponent)

    ENTRY_ACTION(state_running, action_load)
    ENTRY_ACTION(state_running, action_initialize)
    EXIT_ACTION(state_running, action_uninitialize)
    EXIT_ACTION(state_running, action_unload)

    ENTRY_ACTION(state_show, action_show)

    ENTRY_ACTION(state_hide, action_hide)

    ENTRY_ACTION(state_finished, action_finished)

    ENTRY_ACTION(state_faulted, action_faulted)

    ENTRY_ACTION(state_remove, action_notifyRemove)
    ENTRY_ACTION(state_remove, action_remove)

    // Delete on finish.

    connect(mStateMachine, SIGNAL(finished()), SLOT(deleteLater()), 
            Qt::QueuedConnection);
}

/*!
    Assigns the meta \a property based on the given property \a name.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::setProperty(const char *name, QMetaProperty &property)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfProperty(name);
    property = object->property(index);
    return index >= 0;
}

/*!
    Assigns the meta \a method based on the given method \a signature.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::setMethod(const char *signature, QMetaMethod &method)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfMethod(
        QMetaObject::normalizedSignature(signature));
    method = object->method(index);
    return index >= 0;
}

/*!
    Checks if the contained widget has implemented the
    signal with the given \a signature.
*/
bool HsWidgetHost::hasSignal(const char *signature)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfSignal(
        QMetaObject::normalizedSignature(signature));
    return index >= 0;
}

/*!
    Resizes this widget host to the given \a size.
*/
void HsWidgetHost::setNewSize(const QSizeF &size)
{
    mTouchArea->resize(size);
    resize(size);
    setPreferredSize(size);
}

/*!
    Assigns preferences for the contained widget.
    Returns true on success, otherwise returns false.
*/
bool HsWidgetHost::setPreferencesToWidget()
{
    HsDatabase *db = HsDatabase::instance();
    
    QVariantHash preferences;
    if (!db->widgetPreferences(mDatabaseId, preferences)) {
        return false;
    }

    QStringList names = preferences.keys();
    foreach (QString name, names) {
        mWidget->setProperty(name.toLatin1(),
                             preferences.value(name));
    }

    return true;
}

/*!
    Connects this widget host for monitoring changes
    in its widget component.
*/
void HsWidgetHost::action_connectComponent()
{
    HsDatabase *db = HsDatabase::instance();
    
    HsWidgetData data;
    data.id = mDatabaseId;
    if (!db->widget(data)) {
        emit event_faulted();
        return;
    }
    
    mComponent = HsWidgetComponentRegistry::instance()->component(data.uri);
    
    connect(mComponent, SIGNAL(aboutToUninstall()), SIGNAL(event_unload()));
    connect(mComponent, SIGNAL(uninstalled()), SIGNAL(event_finished()));
    connect(mComponent, SIGNAL(unavailable()), SIGNAL(event_unload()));
    connect(mComponent, SIGNAL(unavailable()), SIGNAL(unavailable()));
    connect(mComponent, SIGNAL(available()), SIGNAL(available()));
    connect(mComponent, SIGNAL(updated()), SIGNAL(available()));
}

/*!
    Disconnects this widget host from its widget component.
*/
void HsWidgetHost::action_disconnectComponent()
{
    mComponent->disconnect(this);
}

/*!
    Loads the contained widget from a widget plugin and 
    connects to it's meta interface.
*/
void HsWidgetHost::action_load()
{
    QServiceManager manager;
    QServiceFilter filter("com.nokia.symbian.IHomeScreenWidget");
    filter.setServiceName(mComponent->uri());
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
    if (interfaces.isEmpty()) {
        emit event_faulted();
        return;
    }

    QObject *widgetObject = manager.loadInterface(interfaces.first());
    mWidget = qobject_cast<QGraphicsWidget *>(widgetObject);

    if (!mWidget ||
        !setMethod("onShow()", mOnShowMethod) ||
        !setMethod("onHide()", mOnHideMethod)) {
        mWidget = 0;
        delete widgetObject;
        emit event_faulted();
        return;
    }

    setMethod("onInitialize()", mOnInitializeMethod);
    setMethod("onUninitialize()", mOnUninitializeMethod);
    
    setProperty("isOnline", mIsOnlineProperty);
	setProperty("rootPath", mRootPathProperty);
    
    if (hasSignal("setPreferences(const QStringList&)")) {
        connect(mWidget, SIGNAL(setPreferences(QStringList)),
            SLOT(onSetPreferences(QStringList)), Qt::QueuedConnection);
    }
    if (hasSignal("finished()")) {
        connect(mWidget, SIGNAL(finished()),
            SLOT(onFinished()), Qt::QueuedConnection);
    }
    if (hasSignal("error()")) {
        connect(mWidget, SIGNAL(error()),
            SLOT(onError()), Qt::QueuedConnection);
    }

    mWidget->installEventFilter(this);

    setMinimumSize(HSCONFIGURATION_GET(minimumWidgetSizeInPixels));
    setMaximumSize(HSCONFIGURATION_GET(maximumWidgetSizeInPixels));
    
    loadPresentation();

    mWidget->setParentItem(this);

    setNewSize(mWidget->size());

    QString objName(mComponent->uri());
    objName.append(":");
    objName.append(QString::number(mDatabaseId));
    setObjectName(objName);
}

/*!
    Unloads the contained widget. 
*/
void HsWidgetHost::action_unload()
{
    delete mWidget;
    mWidget = 0;
		// This is needed because QServicePluginCleanup is 
    // deleted asynchronously via the eventloop (deleteLater). 
    // Here we want the plugin to unload synchronously.
    QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    	
    mOnInitializeMethod = QMetaMethod();
    mOnShowMethod = QMetaMethod();
    mOnHideMethod = QMetaMethod();
    mOnUninitializeMethod = QMetaMethod();    
    mIsOnlineProperty = QMetaProperty();
	mRootPathProperty = QMetaProperty();    
}

/*!
    Initializes the contained widget.
*/
void HsWidgetHost::action_initialize()
{    
    mRootPathProperty.write(mWidget, mComponent->rootPath());
    setPreferencesToWidget();
    setOnline(HsScene::instance()->isOnline());
    mOnInitializeMethod.invoke(mWidget);
}

/*!
    Uninitializes the contained widget.
*/
void HsWidgetHost::action_uninitialize()
{
    mOnUninitializeMethod.invoke(mWidget);
}

/*!
    Puts the contained widget into show state.
*/
void HsWidgetHost::action_show()
{
    if (!mIsFinishing) {
        mOnShowMethod.invoke(mWidget);
    }
}

/*!
    Puts the contained widget into hidden state.
*/
void HsWidgetHost::action_hide()
{
    if (!mIsFinishing) {
        mOnHideMethod.invoke(mWidget);
    }
}

/*!
    Notifies the home screen framework that this widget
    host has moved to finished state.
*/
void HsWidgetHost::action_finished()
{
    emit finished();
}

/*!
    Notifies the home screen framework that this widget
    host has moved to faulted state.
*/
void HsWidgetHost::action_faulted()
{
    emit faulted();
}

/*!
    Removes the contained widget from the home screen
    database.
*/
void HsWidgetHost::action_remove()
{
    HsDatabase *db = HsDatabase::instance();
    
    db->deleteWidget(mDatabaseId);
    mDatabaseId = -1;
}

/*!
    Notifies the widget removal through the content service.
*/
void HsWidgetHost::action_notifyRemove()
{
    HsDatabase *db = HsDatabase::instance();
    QVariantHash preferences;
    db->widgetPreferences(mDatabaseId, preferences);
    HsContentService::instance()->emitWidgetRemoved(mComponent->uri(), preferences);
}

/*!
    Handles contained widget's finished event.
    Moves this widget host to finished state.
*/
void HsWidgetHost::onFinished()
{
    mIsFinishing = true;
    emit event_finished();
}
 
/*!
    Handles contained widget's error event.
    Moves this widget host to faulted state.
*/
void HsWidgetHost::onError()
{
    mIsFinishing = true;
    emit event_faulted();
}

/*!
    Handles contained widget's setPreferences event.
    Stores the preferences for the given \a names to
    the home screen database.
*/
void HsWidgetHost::onSetPreferences(const QStringList &names)
{
    if (names.isEmpty()) {
        return;
    }

    QVariantHash preferences;

    foreach (QString name, names) {
        QVariant value = mWidget->property(name.toLatin1());
        preferences.insert(name, value);
    }

    HsDatabase *db = HsDatabase::instance();    
    if (!db->setWidgetPreferences(mDatabaseId, preferences)) {
        onError();
    }
}
