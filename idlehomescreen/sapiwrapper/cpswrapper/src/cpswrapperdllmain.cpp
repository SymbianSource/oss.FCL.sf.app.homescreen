/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Item id, name and properties
 *
 */

//  Include Files
#include <e32std.h>

//  Global Functions

//  Exported Functions

#ifndef EKA2 // for EKA1 only
/**
 * Called when the DLL is loaded and unloaded. Note: have to define
 * epoccalldllentrypoints in MMP file to get this called in THUMB.
 * @param aReason not used
 */
EXPORT_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return KErrNone;
    }
#endif

// End of file
