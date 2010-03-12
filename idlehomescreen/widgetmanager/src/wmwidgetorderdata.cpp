/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmWidgetOrderData implementation.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include "wmwidgetorderdata.h"
#include "wmpersistentwidgetorder.h"

// ---------------------------------------------------------
// CWmWidgetOrderData::NewL
// ---------------------------------------------------------
//
CWmWidgetOrderData* CWmWidgetOrderData::NewL( 
        const TDesC& aPublisherId, 
        const TUid aUid, 
        const TDesC& aName,
        const CWmPersistentWidgetOrder* aPersistentWidgetOrder )
    {
    CWmWidgetOrderData* self = CWmWidgetOrderData::NewLC(
            aPublisherId, aUid, aName, aPersistentWidgetOrder );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::NewLC
// ---------------------------------------------------------
//
CWmWidgetOrderData* CWmWidgetOrderData::NewLC(
        const TDesC& aPublisherId, 
        const TUid aUid, 
        const TDesC& aName,
        const CWmPersistentWidgetOrder* aPersistentWidgetOrder )
    {
    CWmWidgetOrderData* self = new ( ELeave ) CWmWidgetOrderData( aUid );
    CleanupStack::PushL(self);
    self->ConstructL( aPublisherId, aName, aPersistentWidgetOrder );
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::CWmWidgetOrderData
// ---------------------------------------------------------
//
CWmWidgetOrderData::CWmWidgetOrderData( const TUid aUid )
    : iUid( aUid )
    {
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::ConstructL
// ---------------------------------------------------------
//
void CWmWidgetOrderData::ConstructL( 
        const TDesC& aPublisherId, 
        const TDesC& aName,
        const CWmPersistentWidgetOrder* aPersistentWidgetOrder )
    {
    iPublisherId = aPublisherId.AllocL();
    iName = aName.AllocL();
    iPersistentWidgetOrder = aPersistentWidgetOrder;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::~CWmWidgetOrderData
// ---------------------------------------------------------
//
CWmWidgetOrderData::~CWmWidgetOrderData() 
    {
    delete iPublisherId;
    delete iName;
    //delete iPersistentWidgetOrder // TODO: is needed???
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::PublisherId
// ---------------------------------------------------------
//
const TDesC& CWmWidgetOrderData::PublisherId() const
    {
    return ( iPublisherId ) ? *iPublisherId : KNullDesC();
    }
  
// ---------------------------------------------------------
// CWmWidgetOrderData::Uid
// ---------------------------------------------------------
//
const TUid CWmWidgetOrderData::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::Name
// ---------------------------------------------------------
//
const TDesC& CWmWidgetOrderData::Name() const
    {
    return ( iName ) ? *iName : KNullDesC();  
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::CompareByName
// ---------------------------------------------------------
//
TInt CWmWidgetOrderData::CompareByName( 
        const CWmWidgetOrderData& aDataOne, const CWmWidgetOrderData& aDataTwo )
    {
    // negate the result for ascending alphabetical order
    TInt result = aDataOne.Name().CompareC( aDataTwo.Name() );
    return result;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::CompareByPersistentWidgetOrder
// ---------------------------------------------------------
//
TInt CWmWidgetOrderData::CompareByPersistentWidgetOrder( 
        const CWmWidgetOrderData& aDataOne, const CWmWidgetOrderData& aDataTwo )
    {
    TInt result;
    const CWmPersistentWidgetOrder* order = aDataOne.iPersistentWidgetOrder;
    if ( order && !order->IsEmpty() )
        {
        result = order->IndexOf( aDataOne ) - order->IndexOf( aDataTwo );
        }
    else
        {
        // fallback: if persistent widget order is not available
        // or it is empty (this is the case on first start, or if the persistent
        // file is corrupted or deleted) -> order widgets by name.
        result = CompareByName( aDataOne, aDataTwo );
        }
    return result;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::EqualsTo
// ---------------------------------------------------------
//
TBool CWmWidgetOrderData::EqualsTo( const TUid aUid, const TDesC& aPublisherId )
    {
    return ( iUid == aUid && *iPublisherId == aPublisherId );
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::SetPersistentWidgetOrder
// ---------------------------------------------------------
//
void CWmWidgetOrderData::SetPersistentWidgetOrder( 
        CWmPersistentWidgetOrder* aPersistentWidgetOrder )
    {
    iPersistentWidgetOrder = aPersistentWidgetOrder;
    }

// ---------------------------------------------------------
// CWmWidgetOrderData::UpdateName
// ---------------------------------------------------------
//
void CWmWidgetOrderData::UpdateNameL( const TDesC& aName )
    {
    if ( iName->Compare( aName ) != KErrNone )
        {
        delete iName;
        iName = NULL;
        iName = aName.AllocL();
        }
    }

// End of file
