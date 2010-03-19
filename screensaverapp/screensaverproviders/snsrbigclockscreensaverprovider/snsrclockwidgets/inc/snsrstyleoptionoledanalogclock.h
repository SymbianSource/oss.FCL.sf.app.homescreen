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
* Description:  Style option for oled analog clock widget.
*
*/

#ifndef SNSRSTYLEOPTIONOLEDANALOGCLOCK_H
#define SNSRSTYLEOPTIONOLEDANALOGCLOCK_H

#include <hbstyleoption.h>

class SnsrStyleOptionOledAnalogClock : public HbStyleOption
{

public:

    enum ItemIndex
    {
        BackgroundItemIndex = 0,
        HourHandItemIndex,
        MinuteHandItemIndex,
        DateFrameItemIndex,
        DateLabelItemIndex,
        LastItemIndex
    };

public:
    SnsrStyleOptionOledAnalogClock();
    SnsrStyleOptionOledAnalogClock(const SnsrStyleOptionOledAnalogClock &other);
    ~SnsrStyleOptionOledAnalogClock();

    enum StyleOptionType { Type = HbSO_Widget+1000};
    enum StyleOptionVersion { Version = 1 };

    qreal mM;
    qreal mH;
    QString mShortDate;
};

#endif // SNSRSTYLEOPTIONOLEDANALOGCLOCK_H
