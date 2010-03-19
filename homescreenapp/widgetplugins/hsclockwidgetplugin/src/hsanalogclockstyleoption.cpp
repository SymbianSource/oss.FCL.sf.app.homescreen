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

#include "hsanalogclockstyleoption.h"

HsAnalogClockStyleOption::HsAnalogClockStyleOption():
    HbStyleOption(), mM(0), mH(0)
{
    type = Type;
    version = Version;
}

HsAnalogClockStyleOption::HsAnalogClockStyleOption(const HsAnalogClockStyleOption &other) :
    HbStyleOption(other), mM(other.mM), mH(other.mH)
{
    type = Type;
    version = Version;
}

HsAnalogClockStyleOption::~HsAnalogClockStyleOption()
{
}
