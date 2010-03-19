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

#include <QDebug>
#include <QVariantList>

#include <hsmenuservice.h>
#include <hsshortcutservice.h>
#include <hscontentservice.h>

#include "hsmenubasestate.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsMenuBaseState::HsMenuBaseState(const QString &objectName, QState *parent) :
    QState(parent)
{
    construct(objectName);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

HsMenuBaseState::~HsMenuBaseState()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuBaseState::construct(const QString &objectName)
{
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + "/" + objectName);
    } else {
        setObjectName(objectName);
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuBaseState::requestService(const QVariant &service)
{
    requestServices(QList<QVariant> () << service);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuBaseState::requestServices(const QVariantList &services)
{
    // if value is not list, it returns empty list
    QVariantList value = property(HS_SERVICES_REGISTRATION_KEY).toList();
    // apppend to list
    value.append(services);
    setProperty(HS_SERVICES_REGISTRATION_KEY, value);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsShortcutService *HsMenuBaseState::shortcutService() const
{
    return propertyWithChecking(SHORTCUT_SERVICE_KEY).value<
           HsShortcutService *> ();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsContentService *HsMenuBaseState::contentService() const
{
    return propertyWithChecking(CONTENT_SERVICE_KEY).value<
           HsContentService *> ();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
QVariant HsMenuBaseState::propertyWithChecking(const char *propertyName) const
{
    QVariant val = property(propertyName);
    if (!val.isValid()) {
        qWarning() << "Missing value for property: " << propertyName;
    }
    return val;
}
