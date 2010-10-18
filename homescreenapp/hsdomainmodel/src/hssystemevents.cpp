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
* Description:
*
*/

#include "hssystemevents.h"

/*!
    Constructor
*/
HsSystemEvents::HsSystemEvents(QObject *parent) : QObject(parent)
{

}

/*!
    Destructor
*/
HsSystemEvents::~HsSystemEvents()
{

}

/*!
    Returns the current instance.
*/
HsSystemEvents *HsSystemEvents::instance()
{
    if (!mInstance)
        mInstance = new HsSystemEvents();
    return mInstance;
}

void HsSystemEvents::setInstance(HsSystemEvents *instance)
{
    delete mInstance;
    mInstance = instance;
}

/*!
    Stores the current instance.
*/
HsSystemEvents *HsSystemEvents::mInstance = 0;
