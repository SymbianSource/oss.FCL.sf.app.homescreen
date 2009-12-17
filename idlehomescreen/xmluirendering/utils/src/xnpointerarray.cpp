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
* Description:  Heap-allocated pointer array
*
*/



// INCLUDE FILES
#include    "xnpointerarray.h"
#include    "xnpanic.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
//?type ?function_name(
//    ?arg_type arg,  // ?description
//    ?arg_type arg)  // ?description
//    {

//    ?code  // ?comment

    // ?comment
//    ?code
//    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnPointerArray::CXnPointerArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnPointerArray::CXnPointerArray()
    {
    }

// -----------------------------------------------------------------------------
// CXnPointerArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnPointerArray::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnPointerArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnPointerArray* CXnPointerArray::NewL()
    {
    CXnPointerArray* self = new( ELeave ) CXnPointerArray;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CXnPointerArray::~CXnPointerArray()
    {
    iContainer.Reset();
    }
// -----------------------------------------------------------------------------
// CXnPointerArray::Container
// Get the underlying container
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CBase>& CXnPointerArray::Container()
	{	
    return iContainer;
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
//?type  ?function_name(
//    ?arg_type arg,  // ?description
//    ?arg_type arg )  // ?description
//    {

//    ?code

//    }


//  End of File  
