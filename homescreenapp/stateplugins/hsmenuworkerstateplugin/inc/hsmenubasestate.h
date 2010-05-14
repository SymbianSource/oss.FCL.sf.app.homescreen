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

class HsMenuBaseState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsMenuBaseState(const QString &objectName, QState *parent = 0);

    virtual ~HsMenuBaseState();

protected:

    void construct(const QString &objectName);

    void requestServices(const QVariantList &services);

    void requestService(const QVariant &service);

    HsShortcutService *shortcutService() const;

    HsContentService *contentService() const;

private:

    QVariant propertyWithChecking(const char *propertyName) const;


};

#endif //HSMENUBASESTATE_H
