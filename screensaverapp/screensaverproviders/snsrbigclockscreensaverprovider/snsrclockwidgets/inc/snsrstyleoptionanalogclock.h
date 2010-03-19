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

#ifndef SNSRSTYLEOPTIONANALOGCLOCK_H
#define SNSRSTYLEOPTIONANALOGCLOCK_H

#include <hbstyleoption.h>

class SnsrStyleOptionAnalogClock : public HbStyleOption
{

public:

    enum ItemIndex
    {
        BackgroundItemIndex = 0,
        HourHandItemIndex,
        MinuteHandItemIndex,
        LastItemIndex
    };

public:
    SnsrStyleOptionAnalogClock();
    SnsrStyleOptionAnalogClock(const SnsrStyleOptionAnalogClock &other);
    ~SnsrStyleOptionAnalogClock();

    enum StyleOptionType { Type = HbSO_Widget+1000};
    enum StyleOptionVersion { Version = 1 };

    qreal mM;
    qreal mH;
};

#endif // SNSRSTYLEOPTIONANALOGCLOCK_H
