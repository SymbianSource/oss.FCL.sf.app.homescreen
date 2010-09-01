/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DOM property map
*
*/


#include "xnproperty.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"

#include "csspropertymap.h"
#include "aixmluiutils.h"

using namespace AiXmlUiController;

// ======== MEMBER FUNCTIONS ========

CCssPropertyMap::CCssPropertyMap()
    {
    }

CCssPropertyMap* CCssPropertyMap::NewL()
    {
    CCssPropertyMap* self = new( ELeave ) CCssPropertyMap;
    return self;
    }

CCssPropertyMap::~CCssPropertyMap()
    {
    Reset();
    iElementMap.Close();
    }


void CCssPropertyMap::StorePropertyValueL( const TDesC8& aElementId,
                                           CXnProperty& aProperty )
    {
    RDomPropertyValueMap* propertyMap = iElementMap.Find( aElementId );
    
    if ( !propertyMap )
        {
        // Create new entry
        propertyMap = new( ELeave ) RDomPropertyValueMap;
        
        CleanupStack::PushL( propertyMap );
        
        iElementMap.InsertL( aElementId.AllocLC(), propertyMap );
        
        CleanupStack::Pop( 2, propertyMap );
        }
        
    // Clone property value
    CXnDomProperty* property = aProperty.Property();
    CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                                            property->PropertyValueList().First() );
    LeaveIfNull( value, KErrArgument );
    
    value = value->CloneL();
    CleanupStack::PushL( value );
    
    // Store value
    propertyMap->InsertL( property->Name().AllocLC(), value );
    
    CleanupStack::Pop( 2, value );
    
    }
    
CXnDomPropertyValue* CCssPropertyMap::FindPropertyValue( const TDesC8& aElementId,
                                                         const TDesC8& aName )
    {
    RDomPropertyValueMap* propertyMap = iElementMap.Find( aElementId );
    
    if ( !propertyMap )
        {
        return NULL;
        }
        
    return propertyMap->Find( aName );
    }

void CCssPropertyMap::Reset()
    {
    TPtrHashMapIter< TDesC8, RDomPropertyValueMap > mapIter( iElementMap );
    mapIter.Reset();
    
    while ( mapIter.NextKey() )
        {
        // Reset map for current key
        RDomPropertyValueMap* map = const_cast< RDomPropertyValueMap* >( 
                                                    mapIter.CurrentValue() );
        map->ResetAndDestroy();
        
        // Delete current key and map
        delete mapIter.CurrentKey();
        delete map;
        
        // Remove map (pointer) for current key
        mapIter.RemoveCurrent();
        }
    
    }
