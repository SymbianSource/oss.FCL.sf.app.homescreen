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
* Description:  suite stack
*
*/


#include "hnsuitemodel.h"
#include "hnsuitesstack.h"
#include "hnsuitemodelcontainer.h"
#include "hncontrollerinterface.h"


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuitesStack* CHnSuitesStack::NewL( MHnControllerInterface& aControllerInterface )
    {
    CHnSuitesStack* self = new (ELeave) CHnSuitesStack( aControllerInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnSuitesStack::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnSuitesStack::~CHnSuitesStack()
    {
    iStack.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnSuitesStack::CHnSuitesStack( MHnControllerInterface& aControllerInterface ):
    iControllerInterface( aControllerInterface )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuitesStack::PushL( CHnSuiteModel* aSuiteModel )
    {
    iStack.AppendL( aSuiteModel );
    iControllerInterface.HandleSuiteEventL( ESuitePushedToStack, aSuiteModel );
    return iStack.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuitesStack::PopL()
    {
    CHnSuiteModel* retSuite = NULL;
    TInt pos = iStack.Count() - 1;
    if ( pos >= 0 )
    { 
        retSuite = iStack[ pos ];
        iStack.Remove( pos );
    }
    iControllerInterface.HandleSuiteEventL( ESuitePoppedFromStack, retSuite );
    
    return retSuite;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuitesStack::GetLast()
    {
    return ( iStack.Count() > 0 ) ? iStack[ iStack.Count() - 1 ] : NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuitesStack::Get( TInt aPosition )
    {
    ASSERT( aPosition >= 0 );
    ASSERT( aPosition < iStack.Count() );

    return iStack[ aPosition ];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnSuitesStack::IsEmpty()
    {
    return ( iStack.Count() > 0 ) ? EFalse : ETrue;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuitesStack::GetRootSuite()
    {
    CHnSuiteModel* retSuite = NULL;
    
    if ( iStack.Count() > 0 )
        {
        retSuite = iStack[ 0 ];
        }
        
    return retSuite;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuitesStack::Count()
    {
    return iStack.Count();
    }
//End of file.
