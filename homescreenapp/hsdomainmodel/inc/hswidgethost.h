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

#ifndef HSWIDGETHOST_H
#define HSWIDGETHOST_H

#include <QVariantMap>
#include <QMetaMethod>
#include <QMetaProperty>

#include <HbWidget>

#include "hsdomainmodeldatastructures.h"
#include "hsdomainmodel_global.h"
#include "hstest_global.h"


class HbIconItem;
class HsPage;


HOMESCREEN_TEST_CLASS(TestRuntimeServices)

class HSDOMAINMODEL_EXPORT HsWidgetHost : public HbWidget
{
    Q_OBJECT

public:
    enum State {
        Constructed,
        Initialized,
        Visible,
        Hidden,
        Uninitialized,
        Finished,
        Faulted
    };

public:
    static HsWidgetHost *createInstance(HsWidgetData &widgetData, 
                                        const QVariantHash &preferences = QVariantHash());

    HsWidgetHost(int databaseId, QGraphicsItem *parent = 0);
    
    ~HsWidgetHost();
    
    bool load();
    
    bool setPage(HsPage *page);
    HsPage *page() const;

    bool isValid() const;

    int databaseId() const;

    State state() const { return mState; }

    bool deleteFromDatabase();
    
    bool setWidgetPresentation();
    bool setWidgetPresentationData(HsWidgetPresentationData &presentationData);
    bool widgetPresentationData(const QString &key, HsWidgetPresentationData &presentationData);

    HsWidgetPresentationData widgetPresentation(Qt::Orientation orientation);
    bool loadWidgetPresentation();
    bool deleteWidgetPresentation(Qt::Orientation orientation);

signals:
    void widgetFinished(HsWidgetHost *widget);
    void widgetError(HsWidgetHost *widget);

public slots:
    void initializeWidget();
    void showWidget();
    void hideWidget();
    void uninitializeWidget();
    void setOnline(bool online = true);

    void startDragAnimation();
    void startDropAnimation();
    
    void startTapAndHoldAnimation();
    void stopTapAndHoldAnimation();

protected:    
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }    
    bool eventFilter(QObject *obj, QEvent *event);
    
private:
    bool setProperty(const char *name, QMetaProperty &property); 
    bool setMethod(const char *signature, QMetaMethod &method);    
    bool hasSignal(const char *signature);
    bool setPreferencesToWidget();
    void setNewSize(const QSizeF &newSize);

private slots:
    void onSetPreferences(const QStringList &names);
    void onFinished();
    void onError();
    
private:
    Q_DISABLE_COPY(HsWidgetHost)
    QGraphicsWidget *mWidget;
    HsPage *mPage;
    QMetaMethod mOnInitializeMethod;
    QMetaMethod mOnShowMethod;
    QMetaMethod mOnHideMethod;
    QMetaMethod mOnUninitializeMethod;
    QMetaProperty mIsOnlineProperty;
	QMetaProperty mRootPathProperty;
    State mState;
    QString mUri;
    int mDatabaseId;

    HbIconItem *mTapAndHoldIcon;

    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};

#endif // HSWIDGETHOST_H
