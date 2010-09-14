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

#ifndef __MCSEXAMPLE_PAN__
#define __MCSEXAMPLE_PAN__

/** MCSExample application panic codes */
enum TMCSExamplePanics
    {
    EMCSExampleUi = 1
    // add further panics here
    };

inline void Panic(TMCSExamplePanics aReason)
    {
    _LIT(applicationName, "MCSExample");
    User::Panic(applicationName, aReason);
    }

#endif // __MCSEXAMPLE_PAN__
