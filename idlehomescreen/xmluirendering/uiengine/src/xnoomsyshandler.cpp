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
* Description:
* Helper system for handling Out-Of-Memory cases.
*
*/

// System includes
#include <hal.h>
#include <e32std.h>
#include <e32cmn.h> 
#include <aknnotewrappers.h>
#include <e32svr.h> 
#include <StringLoader.h> 
#include <xnuiengine.rsg>

// User includes
#include "xnoomsyshandler.h"
#include "xnpanic.h"

//Constants
const TInt64 KMinInterval = 5000000;    // 5 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CXnOomSysHandler* CXnOomSysHandler::NewL()
    {
    CXnOomSysHandler* self = new ( ELeave ) CXnOomSysHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CXnOomSysHandler::~CXnOomSysHandler()
    {
    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// Checks if required amount of memory is available on heap
// -----------------------------------------------------------------------------
//
TBool CXnOomSysHandler::HeapAvailable( TInt aAmount )
    {
    if( aAmount == EMinNotDef )
        {
        aAmount = EMem512kB;
        }

    // Gather needed data...
        
    // Space already allocated.
    TInt allocated;
    User::Heap().AllocSize( allocated );    
    
    // Max space for heap. Heap can extend only up to this value.
    const TInt heapMax = User::Heap().MaxLength(); 
    
    // Available memory in heap's current state.
    TInt obsolete = 0;
    const TInt availableHeap = User::Heap().Available( obsolete );
    
    // Current heap size.
    const TInt heapSize = allocated + availableHeap;
    
    // Heap extension requirement. Can be negative.
    const TInt extensionRequirement = aAmount - availableHeap;
    
    // Max. heap extension possible limited by heap max.
    const TInt heapExtensionMax = heapMax - heapSize;
    
    // System free ram.
    TInt ramFree = 0;
    HAL::Get( HALData::EMemoryRAMFree, ramFree );
    
    // Checking...
    
    // Fits into current heap allocation.
    if( availableHeap >= aAmount )
        {
        return ETrue;
        }
    
    // Fits if current heap extends. heap size and therefore
    // it's possibilities to extend is limited by max heap
    // size and by available system memory.     
    if( ( heapExtensionMax ) >= extensionRequirement &&
        ( ramFree >= extensionRequirement ) )
        {
        return ETrue;
        }   
    
    // Does not fit.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Handle potential OOM failure.
// -----------------------------------------------------------------------------
//
void CXnOomSysHandler::HandlePotentialOomL()
    {
    if( AllowDialog() )
        {
        CAknErrorNote* dialog = new (ELeave) CAknErrorNote();
        dialog->ExecuteLD( *iMsg );
        dialog = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CXnOomSysHandler::ConstructL()
    {
    iMsg = StringLoader::LoadL( R_QTN_HS_HS_MEMORY_FULL );    
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CXnOomSysHandler::CXnOomSysHandler()
    {    
    iPreviousDialogPopupTime = 0;
    }

//------------------------------------------------------------------------------
// Check whether OOM dialog can be shown.
// Prevents OOM dialogs popping up more often than
// defined in KMinInterval.
// -----------------------------------------------------------------------------
//
TBool CXnOomSysHandler::AllowDialog()
    {          
    TTime now;
    now.HomeTime();
    
    TTimeIntervalMicroSeconds interval
        = now.MicroSecondsFrom( iPreviousDialogPopupTime ); 
    
    if( iPreviousDialogPopupTime == 0 ||    // Not set yet.
        interval.Int64() >= KMinInterval )  // Proper interval passed.
        {        
        iPreviousDialogPopupTime = now.Int64();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
