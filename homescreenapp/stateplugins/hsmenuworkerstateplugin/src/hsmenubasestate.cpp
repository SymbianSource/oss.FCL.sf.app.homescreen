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

/*!
 \class HsMenuBaseState
 \ingroup group_hsworkerstateplugin
 \brief Menu Base State
 Menu Base state is the base class for states used in Menu.
 It provides basic functionality that makes implemenatation state easier.
 Derived implementation can request specific service and access it using convience function.
 \lib ?library
 */

/*!
 Constructor.
 \param objectName object name, used for debug purpose
 \param parent Owner.
 */
HsMenuBaseState::HsMenuBaseState(const QString &objectName, QState *parent) :
    QState(parent)
{
    construct(objectName);
}

/*!
 Destructor.
 */
HsMenuBaseState::~HsMenuBaseState()
{
}

/*!
 Constructs contained objects.
 \param objectName object name, used for debug purpose
 */
void HsMenuBaseState::construct(const QString &objectName)
{
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + "/" + objectName);
    } else {
        setObjectName(objectName);
    }
}

/*!
 Convenience method that request single service.
 \param service service to be requested.
 \see requestServices
 */
void HsMenuBaseState::requestService(const QVariant &service)
{
    requestServices(QList<QVariant> () << service);
}

/*!
 Request for services
 notice that services are appended to the list,
 so this fuction can be called more than once.
 \param services list of requested services
 this method must be called during construction phased of derived class
 */
void HsMenuBaseState::requestServices(const QVariantList &services)
{
    // if value is not list, it returns empty list
    QVariantList value = property(HS_SERVICES_REGISTRATION_KEY).toList();
    // apppend to list
    value.append(services);
    setProperty(HS_SERVICES_REGISTRATION_KEY, value);
}

/*!
 Convenience method that returns the shortcut serviceif was requested
 \return Shortcut service if exists.
 */
HsShortcutService *HsMenuBaseState::shortcutService() const
{
    return propertyWithChecking(SHORTCUT_SERVICE_KEY).value<
           HsShortcutService *> ();
}

/*!
 Convenient method that returns the content serviceif was requested
 \return Content service if exists.
 */
HsContentService *HsMenuBaseState::contentService() const
{
    return propertyWithChecking(CONTENT_SERVICE_KEY).value<
           HsContentService *> ();
}

/*!
 Function return value for specified property and
 if it is not valid add warning message on console.
 \param propertyName Property name.
 \return Property after validation.
 */
QVariant HsMenuBaseState::propertyWithChecking(const char *propertyName) const
{
    QVariant val = property(propertyName);
    if (!val.isValid()) {
        qWarning() << "Missing value for property: " << propertyName;
    }
    return val;
}
