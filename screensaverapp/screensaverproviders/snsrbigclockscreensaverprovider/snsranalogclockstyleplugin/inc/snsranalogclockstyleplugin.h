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
* Description:  Analog clock style plugin.
*
*/

#ifndef SNSRANALOGCLOCKSTYLEPLUGIN_H
#define SNSRANALOGCLOCKSTYLEPLUGIN_H

#include <QtPlugin>

#include <hbstyleinterface.h>
#include <hbglobal.h>

#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrACStylePlugin)

class SnsrAnalogClockStylePlugin: public QObject, public HbStyleInterface
{
    Q_OBJECT
    Q_INTERFACES(HbStyleInterface)

public:
    int primitiveCount() const;
    HbWidgetBase *createPrimitive(HbStyle::Primitive primitive, QGraphicsItem *parent = 0) const;
    void updatePrimitive(QGraphicsItem *item, HbStyle::Primitive primitive, const QStyleOption *option) const;
    QString layoutPath() const;
	
	SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrACStylePlugin)

};

#endif // SNSRANALOGCLOCKSTYLEPLUGIN_H
