/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for blank screen.
*
*/

#include "snsrblankcontainer.h"

/*!
    \class SnsrBlankContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for blank screen.
 */

/*!
    Constructs a new SnsrBlankContainer.
 */
SnsrBlankContainer::SnsrBlankContainer() :
    SnsrBigClockContainer()
{
}

/*!
    Destructs the class.
 */
SnsrBlankContainer::~SnsrBlankContainer()
{
}

void SnsrBlankContainer::update()
{
    // nothing to do
}

int SnsrBlankContainer::updateIntervalInMilliseconds()
{
    // don't start timer
    return -1;
}

void SnsrBlankContainer::loadWidgets()
{
    // nothing to do as we have no visual components
}

// end of file
