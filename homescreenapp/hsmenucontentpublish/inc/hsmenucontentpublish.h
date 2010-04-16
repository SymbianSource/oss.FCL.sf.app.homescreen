/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: hsmenucontentpublish.h
 *
 */

#ifndef HSMENUCONTENTPUBLISH_H
#define HSMENUCONTENTPUBLISH_H

#include <QObject>
#include <QVariant>
#include <qservicemanager.h>

QTM_USE_NAMESPACE

//mandatory keys
const QString hsItemName("item:name");
const QString hsitemLaunchUri("item:launchuri");
const QString hsitemPublisherId("item:publisherId");

//not mandatory
const QString hsItemWidgetUri("widget:uri");
const QString hsItemDescription("item:description");

//for icon - not mandatory
const QString hsIconFileName("icon:filename");//to display specific icon from file
const QString hsIconName("icon:name");// it is used in similar way in HbIcon
const QString hsIconApplicationId("icon:applicationid"); //publisherid makes rest


class HsMenuContentPublishPrivate;

class HsMenuContentPublish : public QObject
{
    Q_OBJECT

public:
    HsMenuContentPublish(QObject *parent = 0);
    ~HsMenuContentPublish();
public slots:
   bool add(const QVariantMap &entryPreference = QVariantMap());
   bool remove(const QVariantMap &queryPreference = QVariantMap()) const;
   
   QList<QVariantMap > getList(const QVariantMap &queryPreference = QVariantMap());
  
private:
   Q_DISABLE_COPY(HsMenuContentPublish)
    /**
     * Pointer to a private implementation.
     */
   HsMenuContentPublishPrivate * const m_d;
};



#endif // HSMENUCONTENTPUBLISH_H
