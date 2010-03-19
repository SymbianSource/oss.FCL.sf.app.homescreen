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
 * Description: Hs Ovi Store Handler.
 *
 */

#ifndef HSOVISTOREHANDLER_H_
#define HSOVISTOREHANDLER_H_

#include <QObject>
#include "hbicon.h"
#include "hsmenustates_global.h"

class HsOviStoreHandlerPrivate;

class HsOviStoreHandler: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    HsOviStoreHandler(QObject *parent = 0);
    virtual ~HsOviStoreHandler();
    HbIcon icon();

public slots:
    void action();

private:
    HsOviStoreHandlerPrivate *const m_q;
};

#endif /* HSOVISTOREHANDLER_H_ */
