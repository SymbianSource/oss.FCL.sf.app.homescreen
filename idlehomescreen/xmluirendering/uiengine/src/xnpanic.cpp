/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic and error codes
*
*/

// INCLUDE FILES
#include "xnpanic.h"

// CONSTANTS
_LIT( KXnPanic, "UIEngine" );

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Panic
// Implementation of Xuikon UI Engine panic mechanism
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic( TXnPanic aPanic )
    {
    User::Panic( KXnPanic, aPanic );
    }
