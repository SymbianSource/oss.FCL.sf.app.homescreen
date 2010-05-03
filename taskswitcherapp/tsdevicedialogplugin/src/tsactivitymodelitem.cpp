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
* Description: tsactivitymodelitem.cpp
*
*/
#include "tsactivitymodelitem.h"
#include "tsdataroles.h"
#include <hsactivitydbclientinterface.h>
#include <hbicon.h>

/*!
    \class TsActivityModelItem
    \ingroup group_tsdevicedialogplugin
    \brief Item presenting activity in the grid.
*/

/*!
    Standard C++ constructor
    /param model - reference to parent model
    /param service - reference to initialized ActivityManager instance
    /param entry - Activity data
*/
TsActivityModelItem::TsActivityModelItem(QAbstractListModel &model,
                                         QObject& service, 
                                         const QVariantHash & activity)
    :
    mService(service),
    mActivity(activity),
    mIcon(),
    mRequetPending(false)
{
    connect(this,
            SIGNAL(entryChanged(TsModelItem *)),
            &model,
            SLOT(entryChanged(TsModelItem *)));
    connect(&mService,
            SIGNAL(thumbnailReady(QPixmap, void *)),
            this,
            SLOT(thumbnailReady(QPixmap, void *)));
}

/*!
    Standard C++ destructor
    /param service - reference to initialized ActivityManager instance
    /param entry - Activity data
*/
TsActivityModelItem::~TsActivityModelItem()
{
}

/*!
    Returns the data stored under the given role.
    /param role - requested data role
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::data(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return displayRole();
        case Qt::DecorationRole:
            return decorationRole();
        case TsDataRoles::Closable:
            return QVariant(false);
        case TsDataRoles::Visible:
            return visibilityRole();
        default:
            return QVariant(QVariant::Invalid);
    }
}

/*!
    Empty interface implementation
*/
void TsActivityModelItem::close()
{
    //no implementation required
}

/*!
    Open or move to foreground activity repesented by entry
*/
void TsActivityModelItem::open()
{
    QVariantHash::const_iterator
    app(mActivity.constFind(ActivityApplicationKeyword)),
        act(mActivity.constFind(ActivityActivityKeyword));

    if (mActivity.end() != app && mActivity.end() != act) {
        QMetaObject::invokeMethod(&mService,
                                  "launchActivity",
                                  Q_ARG(int, app.value().toInt()),
                                  Q_ARG(QString, act.value().toString()));
    }
}

/*!
    Return application name keyword
*/
QString TsActivityModelItem::applicationKeyword()
{
    static QString applciationName(":ApplicationName");
    return applciationName;
}

/*!
    Return activity visibility status keyword
*/
QString TsActivityModelItem::visibilityKeyword()
{
    static QString visibility("visible");
    return visibility;
}

/*!
    Return activity screenshot keyword
*/
QString TsActivityModelItem::screenshotKeyword()
{
    static QString screenshot("screenshot");
    return screenshot;
}

/*!
    Returns the data stored under decoration role.
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::decorationRole() const
{
    if (!mRequetPending) {
        const_cast<TsActivityModelItem *>(this)->mRequetPending = true;
        QVariantHash::const_iterator node(mActivity.constFind(screenshotKeyword()));
        if (mActivity.end() != node) {
            QMetaObject::invokeMethod(&mService,
                                      "getThumbnail",
                                      Q_ARG(QString, node.value().toString()),
                                      Q_ARG(void *, const_cast<TsActivityModelItem *>(this)));
        }
    }
    return QVariant::fromValue<HbIcon>(HbIcon(mIcon));
}

/*!
    Returns the data stored under display role.
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::displayRole() const
{
    QVariantHash::const_iterator node(mActivity.find(applicationKeyword()));
    return mActivity.end() != node ? node.value() : QVariant(QString::null);
}

/*!
    Returns the data stored under visibility role.
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::visibilityRole() const
{
    QVariantHash::const_iterator node(mActivity.find(visibilityKeyword()));
    return mActivity.end() != node ? node.value() : QVariant(true);
}

/*!
    Slot retrieve thumbnail form TM, update content and notify observers
    /param pixmap - thumbnail content
    /ptr - user data provided to request
*/
void TsActivityModelItem::thumbnailReady(QPixmap pixmap, void *ptr)
{
    if (ptr == this) {
        mIcon = pixmap;
        emit entryChanged(this);
    }
}
