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
 * Description: Hs Operator Handler Private.
 *
 */

#ifndef HSOPERATORHANDLER_P_H_
#define HSOPERATORHANDLER_P_H_

#ifndef NO_QT_EXTENSIONS

#include <QObject>
#include <HbIcon>
#include "hsmenustates_global.h"

class CaEntry;
class XQSettingsManager;

class HsOperatorHandlerPrivate: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    HsOperatorHandlerPrivate(QObject *parent = 0);
    virtual ~HsOperatorHandlerPrivate();
    HbIcon icon();
    QString text();
    void action();

private:
    void readCentralRepository();
    HbIcon createIcon(XQSettingsManager *crManager);
    QString readText(XQSettingsManager *crManager);
    CaEntry *createAppEntry(XQSettingsManager *crManager);
    CaEntry *createUrlEntry(XQSettingsManager *crManager);

private:
    CaEntry *mOperatorEntry;
    HbIcon mIcon;
    QString mText;
};

#endif

#endif /* HSOPERATORHANDLER_P_H_ */

