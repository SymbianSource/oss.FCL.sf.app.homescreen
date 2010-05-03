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
 * Description: Menu Base state
 *
 */

#ifndef HSMENUBASESTATE_H
#define HSMENUBASESTATE_H

#include <qstate.h>
#include <QVariant>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HsShortcutService;
class HsContentService;

/**
 * @ingroup group_hsworkerstateplugin
 * @brief Menu Base State
 *
 * Menu Base state is the base class for states used in Menu.
 * It provides basic functionality that makes implemenatation state easier.
 * Derived implementation can request specific service and access it using convience function.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsMenuBaseState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param objectName object name, used for debug purpose
     * @param parent Owner.
     */
    HsMenuBaseState(const QString &objectName, QState *parent = 0);

    /**

     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsMenuBaseState();

protected:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     * @param objectName object name, used for debug purpose
     */
    void construct(const QString &objectName);

    /**
     * Request for services
     *
     * notice that services are appended to the list,
     * so this fuction can be called more than once.
     * @since S60 ?S60_version.
     * @param services list of requested services
     * this method must be called during construction phased of derived class
     */
    void requestServices(const QVariantList &services);

    /**
     * Convenience method that request single service.
     *
     * @param service service to be requested.
     * @see requestServices
     */
    void requestService(const QVariant &service);

    /**
     * Convenience method that returns the shortcut serviceif was requested
     *
     * @since S60 ?S60_version.
     * @return Shortcut service if exists.
     */
    HsShortcutService *shortcutService() const;

    /**
     * Convenient method that returns the content serviceif was requested
     *
     * @since S60 ?S60_version.
     * @return Content service if exists.
     */
    HsContentService *contentService() const;

private:
    /**
     * Function return value for specified property and
     * if it is not valid add warning message on console.
     *
     * @since S60 ?S60_version.
     * @param propertyName Property name.
     * @return Property after validation.
     */
    QVariant propertyWithChecking(const char *propertyName) const;


};

#endif //HSMENUBASESTATE_H
