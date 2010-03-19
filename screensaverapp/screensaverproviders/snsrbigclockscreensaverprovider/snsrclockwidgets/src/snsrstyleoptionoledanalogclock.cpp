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

#include "snsrstyleoptionoledanalogclock.h"

/*!
    \class SnsrStyleOptionOledAnalogClock
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Style option for analog clock widget.
 */

/*!
    Constructs a new SnsrStyleOptionOledAnalogClock.
 */
SnsrStyleOptionOledAnalogClock::SnsrStyleOptionOledAnalogClock():
    HbStyleOption(), mM(0), mH(0), mShortDate(QString())
{
    type = Type;
    version = Version;
}

/*!
    Copy constructor - constructs a new SnsrStyleOptionOledAnalogClock.
 */
SnsrStyleOptionOledAnalogClock::SnsrStyleOptionOledAnalogClock(const SnsrStyleOptionOledAnalogClock &other) :
    HbStyleOption(other), mM(other.mM), mH(other.mH), mShortDate(other.mShortDate)
{
    type = Type;
    version = Version;
}

/*!
    Destructs the class.
 */
SnsrStyleOptionOledAnalogClock::~SnsrStyleOptionOledAnalogClock()
{
}
