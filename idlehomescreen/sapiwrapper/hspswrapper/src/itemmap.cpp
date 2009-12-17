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



#include "itemmap.h"
#include "propertymap.h"

namespace hspswrapper{


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CItemMap::CItemMap()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CItemMap::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap* CItemMap::NewL()
    {
    CItemMap* self = CItemMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap* CItemMap::NewLC()
    {
    CItemMap* self = new( ELeave ) CItemMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap::~CItemMap()
    {
    delete iItemId;
    delete iItemName;
    iProperties.ResetAndDestroy();
    }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap& CItemMap::SetItemIdL(const TDesC8& aItemId )
    {
    delete iItemId;
    iItemId = NULL;
    iItemId = aItemId.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CItemMap::ItemId()const
    {
    if(iItemId)
        {
        return *iItemId;
        }
    else
        {
        return KNullDesC8;
        }
    
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap& CItemMap::SetItemNameL(const TDesC8& aItemName )
    {
    delete iItemName;
    iItemName = NULL;
    iItemName = aItemName.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CItemMap::ItemName()const
    {
    if(iItemName)
       {
       return *iItemName;
       }
   else
       {
       return KNullDesC8;
       }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CItemMap& CItemMap::AddPropertyMapL(CPropertyMap* aPropertyMap )
    {
    iProperties.AppendL(aPropertyMap);
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CPropertyMap>& CItemMap::Properties()const
    {
    return iProperties;
    }
}

