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
 * Description: hsmenucontentpublish.cpp
 *
 */

#include <QString>
#include <QDebug>

#include "hsapp_defs.h"
#include "caquery.h"
#include "caentry.h"
#include "caicondescription.h"
#include "caservice.h"

#include "hsmenucontentpublish.h"
#include "hsmenucontentpublish_p.h"

HsMenuContentPublish::HsMenuContentPublish(QObject *parent)
    :QObject(parent), m_d(new HsMenuContentPublishPrivate(this))
{
 
}

HsMenuContentPublish::~HsMenuContentPublish()
{
    delete m_d;
}

bool HsMenuContentPublish::add(const QVariantMap &entryPreference)
{    
    return m_d->add(entryPreference);
}
bool HsMenuContentPublish::remove(const QVariantMap &entry) const
{
    return m_d->remove(entry);
}

QList<QVariantMap > HsMenuContentPublish::getList(const QVariantMap &queryPreference)
{
    return m_d->getList(queryPreference);
}

/*!
 Constructor
 \param publishPublic pointer to public contentPublish
 */
HsMenuContentPublishPrivate::HsMenuContentPublishPrivate(HsMenuContentPublish *publishPublic) :
    m_q(publishPublic)
{

}

/*!
 destructor
 */
HsMenuContentPublishPrivate::~HsMenuContentPublishPrivate()
{
    
}

bool HsMenuContentPublishPrivate::add(const QVariantMap &entryPreference)
{
    bool result = false;
    QMap<QString, QVariant> pref = entryPreference;
    
    CaIconDescription iconDesc;
    if(pref.contains(hsItemId))
        {
            CaEntry *update_entry = CaService::instance()->getEntry(pref.take(hsItemId).toInt());
            
            update_entry->setText(pref.take(hsItemName).toString());
            
            update_entry->setDescription(pref.take(hsItemDescription).toString());
            
            iconDesc.setFilename(pref.take(hsIconFileName).toString());
            iconDesc.setSkinId(pref.take(hsIconName).toString());
            iconDesc.setApplicationId(pref.take(hsIconApplicationId).toString());
            update_entry->setIconDescription(iconDesc);
            QMapIterator<QString, QVariant> k(pref);
            while (k.hasNext()) {
                k.next();
                update_entry->setAttribute(k.key(),k.value().toString());
            }
            
            result = CaService::instance()->updateEntry(*update_entry);
        }
    else if (pref.contains(hsItemName) && pref.contains(hsitemLaunchUri) && pref.contains(hsitemPublisherId))
        {
            CaEntry add_entry(ItemEntryRole);
            add_entry.setEntryTypeName(templatedApplicationTypeName());
            // mandatory values
            add_entry.setText(pref.take(hsItemName).toString());
            
            iconDesc.setFilename(pref.take(hsIconFileName).toString());
            iconDesc.setSkinId(pref.take(hsIconName).toString());
            iconDesc.setApplicationId(pref.take(hsIconApplicationId).toString());            
            add_entry.setIconDescription(iconDesc);  
            
            add_entry.setDescription(pref.take(hsItemDescription).toString());
            QMapIterator<QString, QVariant> k(pref);
            while (k.hasNext()) {
                k.next();
                add_entry.setAttribute(k.key(),k.value().toString());
            }

            add_entry.setFlags(add_entry.flags() & ~RemovableEntryFlag);
            
            CaEntry *entry = CaService::instance()->createEntry(add_entry);
            if (entry) {
                result = true;
                delete entry;
            }      
        }
    else
        {
            return false;
        }
    return result;
    
  
}
bool HsMenuContentPublishPrivate::remove(const QVariantMap &entry) const
{
    bool result = false;
    if (entry.contains(hsItemId))
        {    
            result = CaService::instance()->removeEntry(entry.value(hsItemId).toInt());
        }
    return result;
}

QList<QVariantMap > HsMenuContentPublishPrivate::getList(const QVariantMap &queryPreference)
{
    QList<QVariantMap> list;
    QMap<QString, QVariant> map = queryPreference;
    
    map.remove(hsItemName);
    map.remove(hsItemDescription);
    
    CaQuery query;
    // define query
    query.setFlagsOn(VisibleEntryFlag);
    query.setEntryRoles(ItemEntryRole);
    query.setEntryTypeNames(QStringList(templatedApplicationTypeName()));
    
    
    // add atributes to caquery
    QMapIterator<QString, QVariant> k(map);
    while (k.hasNext()) {
        k.next();
        query.setAttribute(k.key(),k.value().toString());
    }  
    QList<CaEntry *> entries = CaService::instance()->getEntries(query);
    map.clear();
    if (entries.count()) {
        QMap<QString, QString> attrMap;
        QMapIterator<QString, QString> k(attrMap);
        //walk trought entrys and extract keys into map
        for (int i = 0; i < entries.count(); ++i) {
            map[hsItemId] = entries.at(i)->id();
            map[hsItemName] = entries.at(i)->text();
            map[hsItemDescription] = entries.at(i)->description();
            CaIconDescription iconDesc = entries.at(i)->iconDescription();
            map[hsIconFileName] = iconDesc.filename();
            map[hsIconName] = iconDesc.skinId();
            map[hsIconApplicationId] = iconDesc.applicationId();
            
            // extract entry atributes
            attrMap = entries.at(i)->attributes();
            QMapIterator<QString, QString> k(attrMap);
            // insert entrys attr into map
            while (k.hasNext()) {
                k.next();
                map.insert(k.key(),k.value());
            }
            list.append(map); //append entry map to list
            // clean used maps
            map.clear();
            attrMap.clear();
        }
    }
    qDeleteAll(entries);
    return list;
}
