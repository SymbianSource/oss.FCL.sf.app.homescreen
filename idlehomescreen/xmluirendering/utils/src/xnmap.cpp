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
* Description:  Associative container.
*
*/



// INCLUDE FILES
#include    "xnmap.h"
#include	"xncomparator.h"
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
// CXnMap::CXnMap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnMap::CXnMap() : iContainer(4)
    {
    }

// -----------------------------------------------------------------------------
// CXnMap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnMap::ConstructL(MXnComparator* aComparatorFunctor)
    {
    iComparator = aComparatorFunctor;
    }

// -----------------------------------------------------------------------------
// CXnMap::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnMap* CXnMap::NewL(MXnComparator* aComparatorFunctor)
    {
    CXnMap* self = new( ELeave ) CXnMap;
    
    CleanupStack::PushL( self );
    self->ConstructL(aComparatorFunctor);
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CXnMap::~CXnMap()
    {
    delete iComparator;
    iContainer.ResetAndDestroy();
    }
// -----------------------------------------------------------------------------
// CXnMap::PutL
// Finds an equal object in the map. If found, destroys it and 
// replaces it
// with the object given as a parameter. If not found, inserts
// the parameter object to the map.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnMap::PutL(CBase* aObject)
	{	
	CBase* tmp = Get(*aObject);
	if (tmp == aObject)
	    {
	    // object already in the map, do nothing
	    return;
	    }
	if (tmp != NULL)
		{
        // if an equal object is already in the map, replace it
		TInt index = iContainer.Find(tmp);
		// add new object
		TInt error = iContainer.Append(aObject);
		if (error != KErrNone)
		    {
			User::Leave(KXnErrAppendingMapItemFailed_1);
			}
	    // remove old object
		delete tmp;
		iContainer.Remove(index);
		return;
		}
    // object is not in the map, insert it
	TInt err = iContainer.Append(aObject);
	if (err != KErrNone)
	    {
		User::Leave(KXnErrAppendingMapItemFailed_2);
		}
	}
// -----------------------------------------------------------------------------
// CXnMap::Get
// Finds an object that is equal to the parameter. If found,
// returns the object. If not found, returns NULL.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CBase* CXnMap::Get(CBase& aObject) const
	{
	TUint containerCount = iContainer.Count();
	CBase* returnValue = NULL;
	for (TUint i = 0; i < containerCount; ++i)
		{
		CBase* tmp = iContainer[i];
		if (iComparator->Equals(aObject, *tmp))
			{
			returnValue = tmp;
			break;
			}
		}
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CXnMap::Find
// Finds an object that is equal to the parameter. If found,
// returns the object. If not found, returns NULL.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CBase* CXnMap::Find(CBase& aObject, MXnComparator& aComparator) const
	{
	TUint containerCount = iContainer.Count();
	CBase* returnValue = NULL;
	for (TUint i = 0; i < containerCount; ++i)
		{
		CBase* tmp = iContainer[i];
		if (aComparator.Equals(aObject, *tmp))
			{
			returnValue = tmp;
			break;
			}
		}
	return returnValue;
	}


// -----------------------------------------------------------------------------
// CXnMap::Container
// Gets access to the underlying container
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CBase>& CXnMap::Container()
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
