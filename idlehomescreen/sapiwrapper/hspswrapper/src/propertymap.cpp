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
* Description:  Encapsulates name and value
*
*/



#include "propertymap.h"

namespace hspswrapper{


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPropertyMap::CPropertyMap()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPropertyMap::ConstructL()
    {

    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPropertyMap* CPropertyMap::NewL()
    {
    CPropertyMap* self = CPropertyMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPropertyMap* CPropertyMap::NewLC()
    {
    CPropertyMap* self = new( ELeave ) CPropertyMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPropertyMap::~CPropertyMap()
    {
    delete iName;
    delete iValue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPropertyMap& CPropertyMap::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPropertyMap::Name()const
    {
    if(iName)
       {
       return *iName;
       }
    else
       {
       return KNullDesC8;
       }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPropertyMap& CPropertyMap::SetValueL( const TDesC8& aValue )
    {
    delete iValue;
    iValue = NULL;
    iValue = aValue.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPropertyMap::Value()const
    {
    if(iValue)
       {
       return *iValue;
       }
    else
       {
       return KNullDesC8;
       }
    }

}
