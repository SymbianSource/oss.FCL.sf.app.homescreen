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
* Description:  Style option for analog clock widget.
*
*/

#ifndef HSANALOGCLOCKSTYLEOPTION_H
#define HSANALOGCLOCKSTYLEOPTION_H

#include <hbstyleoption.h>

const int backgroundItemIndex = 0;
const int hourHandItemIndex = 1;
const int minuteHandItemIndex = 2;

class HsAnalogClockStyleOption : public HbStyleOption
{
public:
    HsAnalogClockStyleOption();
    HsAnalogClockStyleOption(const HsAnalogClockStyleOption &other);
    ~HsAnalogClockStyleOption();

    enum StyleOptionType { Type = HbSO_Widget+1000};
    enum StyleOptionVersion { Version = 1 };

    qreal mM;
    qreal mH;
};

#endif // HSANALOGCLOCKSTYLEOPTION_H
