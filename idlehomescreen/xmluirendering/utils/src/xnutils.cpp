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
* Description:  DLL entry point
*
*/


#include <e32base.h>
// -----------------------------------------------------------------------------
// E32Dll implements the Symbian OS dll entry point
// Needed for EKA1 kernel only
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt  E32Dll(
    TDllReason)  // ?description
    {

    return KErrNone;

    }
#endif
