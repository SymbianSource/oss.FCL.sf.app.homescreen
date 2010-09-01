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
* Description:  Compares two objects.
*
*/

// INCLUDES
#include "xnpropertycomparator.h"
#include "xnproperty.h"
#include "xnpropertylist.h"
#include "xndomproperty.h"

// -----------------------------------------------------------------------------
// TXnPropertyComparator::TXnPropertyComparator()
// -----------------------------------------------------------------------------
//
TXnPropertyComparator::TXnPropertyComparator()
    {
    }

// -----------------------------------------------------------------------------
// TXnPropertyComparator::~TXnPropertyComparator()
// -----------------------------------------------------------------------------
//
TXnPropertyComparator::~TXnPropertyComparator()
    {
    }

// -----------------------------------------------------------------------------
// TXnPropertyComparator::Equals()
// -----------------------------------------------------------------------------
//
TBool TXnPropertyComparator::Equals( CBase& aItem1, CBase& aItem2 )
    {
    CXnPropertyKey& item1 = static_cast< CXnPropertyKey& >( aItem1 );
    CXnProperty& item2 = static_cast< CXnProperty& >( aItem2 );
    // return ETrue if keys match and pseudoclasses are not set
    // or if keys match and both pseudoclasses are set and match
    CXnDomProperty* property2 = item2.Property();
    if ( item1.iPseudoClass == property2->PseudoClass() &&
         *( item1.iString ) == property2->Name() )
        {
        // names match, pseudoclasses match or are both not set
        return ETrue;
        }
    // all other cases return false.
    return EFalse;
    }
