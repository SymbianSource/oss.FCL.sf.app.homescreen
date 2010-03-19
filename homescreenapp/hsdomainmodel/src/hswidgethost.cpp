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

#include <QMetaObject>
#include <QGraphicsLinearLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstance>

#include "hswidgethost.h"
#include "hsdatabase.h"
#include "hswidgetdata.h"
#include "hspage.h"
#include "hsapp_defs.h"
#include "hsscene.h"
#include "cadefs.h"
#include "canotifier.h"
#include "canotifierfilter.h"
#include "caservice.h"
#include "caquery.h"
#include "caentry.h"

QTM_USE_NAMESPACE

/*!
    \class HsWidgetHost
    \ingroup group_hsutils
    \brief Homescreen widget runner.
    Is responsible of running a homescreen widget. Each 
    homescreen widget has its own host.
*/

HsWidgetHost* HsWidgetHost::createInstance(const HsWidgetData &widgetData, 
                                           const QVariantMap &preferences)
{
    HsWidgetHost *host = NULL;
    
    HsDatabase* db = HsDatabase::instance();
    Q_ASSERT(db);

    int databaseId = -1;
    if (db->insertWidget(widgetData, databaseId)) {
        db->setWidgetPreferences(databaseId, preferences);
        host = new HsWidgetHost(databaseId);
    }

    return host;                
}
/*!
    Construct a widget host for the given \a databaseId. 
    \a parent becomes the parent item for the host.
*/
HsWidgetHost::HsWidgetHost(int databaseId, QGraphicsItem *parent)
    : HbWidget(parent),
      mWidget(0),
      mPage(0),
      mDatabaseId(databaseId)
{
    CaQuery query;
    query.setEntryTypeNames(QStringList(widgetTypeName()));
    CaNotifierFilter filter(query);    
    CaNotifier *notifier = CaService::instance()->createNotifier(filter);
    notifier->setParent(this);
    connect(notifier,
            SIGNAL(entryChanged(const CaEntry&, ChangeType)),
            SLOT(onEntryChanged(const CaEntry&, ChangeType)), Qt::QueuedConnection);

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
    Destructor.
*/
HsWidgetHost::~HsWidgetHost()
{
}
/*!
    Load hosted widget from plugin and validate it.
    Returns true if widget construction is successfull.
*/
bool HsWidgetHost::load()
{
    if (mWidget) {
        return false;
    }

    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    // Find the widget data.
    HsWidgetData widgetData;
    if (!db->widget(mDatabaseId, widgetData, false)) {
        return false;
    }

    mUri = widgetData.uri();

    // Create the hosted widget.
    QServiceManager manager;
    QServiceFilter filter("com.nokia.IHomeScreenWidget");
    filter.setServiceName(widgetData.uri());
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
    if(interfaces.isEmpty()) {
        return false;
    }

    QObject *widgetObject = manager.loadInterface(interfaces.first());
    mWidget = qobject_cast<QGraphicsWidget *>(widgetObject);

    if (!mWidget || 
        !setMethod("onShow()", mOnShowMethod) ||
        !setMethod("onHide()", mOnHideMethod)) {
        mWidget = 0;
        delete widgetObject;
        return false;
    }
    
    setProperty("isOnline", mIsOnlineProperty);
    setMethod("onInitialize()", mOnInitializeMethod);    
    setMethod("onUninitialize()", mOnUninitializeMethod);
       
    if (hasSignal("setPreferences(const QStringList&)")) {
        connect(mWidget, SIGNAL(setPreferences(QStringList)),
                SLOT(onSetPreferences(QStringList)));
    }
    if (hasSignal("finished()")) {
        connect(mWidget, SIGNAL(finished()),
                SLOT(onFinished()));
    }
    if (hasSignal("error()")) {
        connect(mWidget, SIGNAL(error()),
                SLOT(onError()));
    }

    loadWidgetPresentation();

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mWidget);
    setLayout(layout);
    
    return true;
}

bool HsWidgetHost::setPage(HsPage *page)
{
    HsDatabase* db = HsDatabase::instance();
    Q_ASSERT(db);

    HsWidgetData data;
    if (db->widget(mDatabaseId, data, false)) {
        if (!page) {
            data.setPageId(-1);
        } else {
            data.setPageId(page->databaseId());
        }
        if (!db->updateWidget(data, false)) {
            return false;
        }
    } else {
        return false;
    }

    mPage = page;
    return true;
}
 
HsPage *HsWidgetHost::page() const
{
    return mPage;
}

/*!
    Returns true if this host has a valid widget set.
    Otherwise, return false.
*/
bool HsWidgetHost::isValid() const
{
    return mWidget;
}

/*!
    Returns database id
*/
int HsWidgetHost::databaseId() const
{
    return mDatabaseId; 
}

/*!
    Returns true if this the database operation succeeds,
    false otherwise
*/
bool HsWidgetHost::deleteFromDatabase()
{
    if (HsDatabase::instance()->deleteWidget(mDatabaseId)) {
        mDatabaseId = -1;
        return true;
    }
    return false;
}

/*!
    Set widget presentation by using current values.
    Return true if successfull.
*/
bool HsWidgetHost::setWidgetPresentation()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    QString key = hbInstance->orientation() == Qt::Vertical ? 
        "portrait" : "landscape";
   
    HsWidgetPresentationData data;
    data.setWidgetId(databaseId());
    data.setKey(key);
    data.setPosition(pos());
    data.setSize(size());
    data.setZValue(zValue());
    
    HsWidgetPresentationData temp;
    if (!db->widgetPresentation(databaseId(), key, temp)) {        
        if (!db->insertWidgetPresentation(data)) {
            return false;
        }
    } else {
        data.setId(temp.id());
        if (!db->updateWidgetPresentation(data)) {
            return false;
        }
    }

    return true;
}

/*!
    Set widget presentation data. Return true if successfull

*/
bool HsWidgetHost::setWidgetPresentationData(HsWidgetPresentationData &presentationData)
{
    presentationData.setWidgetId(mDatabaseId);
    return HsDatabase::instance()->insertWidgetPresentation(presentationData);
}
/*!
    Get widget presentation data matching given \a key. 
    Data is returned on given empty \a presentationData. Return true if successfull 
*/
bool HsWidgetHost::widgetPresentationData(
    const QString &key,
    HsWidgetPresentationData &presentationData)
{
    return HsDatabase::instance()->widgetPresentation(mDatabaseId,key,presentationData);
}

/*!
    Return HsWidgetPresentationData for given \a orientation
*/
HsWidgetPresentationData HsWidgetHost::widgetPresentation(Qt::Orientation orientation)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    QString key = orientation == Qt::Vertical ? 
        "portrait" : "landscape";

    HsWidgetPresentationData data;
    if (db->widgetPresentation(databaseId(), key, data)) {
        return data;
    } else {
        return HsWidgetPresentationData();
    }

}

/*!
    Load HsWidgetPresentationData for current orientation
*/
bool HsWidgetHost::loadWidgetPresentation()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    QString key = hbInstance->orientation() == Qt::Vertical ? 
        "portrait" : "landscape";

    HsWidgetPresentationData data;
    if (!db->widgetPresentation(databaseId(), key, data)) {
        return false;
    }

    setGeometry(QRectF(data.position(), data.size()));
    setZValue(data.zValue());

    return true;
}

/*!
    Delete HsWidgetPresentationData for given \a orientation.
    Return true if successfull.
*/
bool HsWidgetHost::deleteWidgetPresentation(Qt::Orientation orientation)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    QString key = orientation == Qt::Vertical ? 
        "portrait" : "landscape";

    HsWidgetPresentationData data;
    if (!db->widgetPresentation(databaseId(), key, data)) {
        return true;
    } else {
        if (!db->deleteWidgetPresentation(data.id())) {
            return false;
        }
    }

    return true;
}

/*!
    \fn void HsWidgetHost::finished()
    This signal is emitten after the contained widget
    hs reported is completion.
*/

/*!
    \fn void HsWidgetHost::error()
    This signal is emitten after the contained widget
    hs reported an error.
*/

/*!
    Calls the widget's onInitialize() slot if the
    widget defines it.
*/
void HsWidgetHost::initializeWidget()
{    
    setPreferencesToWidget();
    setOnline(HsScene::instance()->isOnline());
    mOnInitializeMethod.invoke(mWidget);
}

/*!
    Calls the widget's onShow() slot if the
    widget defines it.
*/
void HsWidgetHost::showWidget()
{
    mOnShowMethod.invoke(mWidget);
}

/*!
    Calls the widget's onHide() slot if the
    widget defines it.
*/
void HsWidgetHost::hideWidget()
{
    mOnHideMethod.invoke(mWidget);
}

/*!
    Calls the widget's onUninitialize() slot if the
    widget defines it.
*/
void HsWidgetHost::uninitializeWidget()
{
    mOnUninitializeMethod.invoke(mWidget);
}

/*!
    Calls the widget's widgetOnlineState property if the
    widget defines it.
*/
void HsWidgetHost::setOnline(bool online)
{
    mIsOnlineProperty.write(mWidget, online);
}

/*!
    Starts the widget drag animation.
*/
void HsWidgetHost::startDragAnimation()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect = 
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */

    setTransformOriginPoint(rect().center());

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(200);
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
    Starts the widget drop animation.
*/
void HsWidgetHost::startDropAnimation()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect = 
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(200);
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
    Checks if a property with the given \a name
    in the contained widget. If the property exists the \a
    metaProperty is made to reference to it. Returns true if
    the property was found. Otherwise, returns false.
*/
bool HsWidgetHost::setProperty(const char *name, QMetaProperty &property)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfProperty(name);
    property = object->property(index);
    return index >= 0;
}

/*!
    Checks if a slot with the given \a signature exists
    in the contained widget. If the slot exists the \a
    method is made to reference to it. Returns true if
    the slot was found. Otherwise, returns false.
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
    Returns true if a signal with the given \a signature
    exists in the contained widget. Otherwise, returns 
    false.
*/
bool HsWidgetHost::hasSignal(const char *signature)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfSignal(
        QMetaObject::normalizedSignature(signature));
    return index >= 0;
}
/*!
    Returns true if fetching widget preferences from db and setting those
    to widget is successfull
*/
bool HsWidgetHost::setPreferencesToWidget()
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    QVariantMap preferences;
    if (!db->widgetPreferences(mDatabaseId, preferences)) {
        return false;
    }

    QStringList names = preferences.keys();
    foreach(QString name, names) {
        mWidget->setProperty(name.toLatin1(),
                             preferences.value(name));
    }

    return true;
}

/*!
    This slot is connected to the contained widget's
    setPreferences() signal, if it was defined for
    the widget. The widget emits the signal for persisting
    its preferences named with \a names. The given
    preferences are read, validated, and written to
    the database.
*/
void HsWidgetHost::onSetPreferences(const QStringList &names)
{
    if (names.isEmpty()) {
        return;
    }

    QVariantMap preferences;

    foreach (QString name, names) {
        QVariant value = mWidget->property(name.toLatin1());
        preferences.insert(name, value);
    }

    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    if (!db->setWidgetPreferences(mDatabaseId, preferences)) {
        onError();
    }
}

/*!
    This slot reacts to the widgets finished() signal, if 
    it was defined for the widget. The widget emits the signal
    when it has finished its execution and is ready for 
    removal from the homescreen.
*/
void HsWidgetHost::onFinished()
{
    emit widgetFinished(this);
}

/*!
    This slot reacts to the widgets error() signal, if it was 
    defined for the widget. The widget emits the signal in 
    failure cases.
*/
void HsWidgetHost::onError()
{
    emit widgetError(this);
}

/*!
    This slot reacts to \a entry change that is described with 
    \a changeType. On remove change type, onFinished() signal is
    emitted.
*/
void HsWidgetHost::onEntryChanged(const CaEntry &entry, 
                                  ChangeType changeType)
{
    if (changeType == RemoveChangeType) {
        QString uri = entry.attribute(widgetUriAttributeName());
        if (uri == mUri) {
            onFinished();
        }
    }
}
