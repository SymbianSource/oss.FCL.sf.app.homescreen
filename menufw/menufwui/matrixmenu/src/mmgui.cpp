/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application startpoint
*
*/


// INCLUDE FILES
#include <mw/memorymanager.h>
#include <eikstart.h>
#include "mmapplication.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
	{
	return new CMmApplication;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
	{
	RAllocator* iAllocator = MemoryManager::SwitchToFastAllocator();
	TInt err = EikStart::RunApplication( NewApplication );
	MemoryManager::CloseFastAllocator(iAllocator);
	return err;
	}
