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
* Description:  
*
*/


#include "hnmdbasekey.h"
#include "hnmdstring8key.h"
#include "hnmdintegerkey.h"
#include "hnmdinteger64key.h"
#include "hnmduintegerkey.h"
#include "hnmduidkey.h"
#include "hnmdbooleankey.h"
#include "hnmdstring16key.h"
#include "hnmdmapkey.h"
#include "hnmdlistkey.h"
#include "hnmdundefkey.h"
#include "hnglobals.h"
#include "hnmdkeyfactory.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdBaseKey* HnMdKeyFactory::CreateLC( TXmlEngElement aElement )
    {
    CHnMdBaseKey* self = NULL;
    
    // First we are testing if the key contains any
    // runtime alterable parameters. If so, they have to be constructed 
    // differently
//    if ( aElement.HasAttributeL( KValueAttrName8) && 
//            aElement.HasAttributeL( KTypeAttrName8 ) ) 
//        {
//        TXmlEngAttr keyValue = aElement.AttributeNodeL( KValueAttrName8 );
//        TXmlEngAttr keyType = aElement.AttributeNodeL( KTypeAttrName8 );
//        if ( keyValue.Value().Find( HnEvent::KEventArgNamespace) 
//                != KErrNotFound)
//            {
//            self = CHnMdDynamicKey::NewL( keyType.Value() );
//            }
//        }
    
    if ( aElement.HasAttributeL( KTypeAttrName8 ) )
        {
        TXmlEngAttr type = aElement.AttributeNodeL( KTypeAttrName8 );
        const TPtrC8 &name = type.Value();
        self = HnMdKeyFactory::CreateL( name );
        }
    else
        {
        self = CHnMdMapKey::NewL();
        }
    
    CleanupStack::PushL( self );
    self->BaseConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdBaseKey* HnMdKeyFactory::CreateL( TXmlEngElement aElement )
    {
    CHnMdBaseKey* self = HnMdKeyFactory::CreateLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdBaseKey* HnMdKeyFactory::CreateL(
    const TDesC8& aName,
    const TDesC8& aType,
    const TDesC8& aValue )
    {
    CHnMdBaseKey* self = HnMdKeyFactory::CreateL( aType );
    CleanupStack::PushL( self );
    self->SetValueL( aValue );
    self->SetKeyNameL( aName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdBaseKey* HnMdKeyFactory::CreateL(
    const TDesC8& aName,
    const TDesC8& aType,
    const TDesC& aValue )
    {
    CHnMdBaseKey* self = HnMdKeyFactory::CreateL( aType );
    CleanupStack::PushL( self );
    self->SetValueL( aValue );
    self->SetKeyNameL( aName );
    CleanupStack::Pop( self );
    return self;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* HnMdKeyFactory::CreateL( const TDesC8& aType )
    {
    CHnMdBaseKey* key = NULL;
    
    if ( !aType.Compare( KKeyTypeString ) )
        {
        key = CHnMdString16Key::NewL(); //String
        }
    else if ( !aType.Compare( KKeyTypeString8 ) )
        {
        key = CHnMdString8Key::NewL(); //String8
        }
    else if ( !aType.Compare( KKeyTypeInteger ) )
        {
        key = CHnMdIntegerKey::NewL(); //Integer
        }
    else if ( !aType.Compare( KKeyTypeInteger64 ) )
        {
        key = CHnMdInteger64Key::NewL(); //Integer64
        }
    else if ( !aType.Compare( KKeyTypeUInteger ) )
        {
        key = CHnMdUIntegerKey::NewL(); //Unsigned Integer
        }
    else if ( !aType.Compare( KKeyTypeUid ) )
        {
        key = CHnMdUidKey::NewL(); //Uid
        }
    else if ( !aType.Compare( KKeyTypeBoolean ) )
        {
        key = CHnMdBooleanKey::NewL(); //Boolean
        }
    else if ( !aType.Compare( KKeyTypeString16 ) )
        {
        key = CHnMdString16Key::NewL(); //String16
        }
    else if ( !aType.Compare( KKeyTypeMap ) )
        {
        key = CHnMdMapKey::NewL(); //Map
        }
    else if ( !aType.Compare( KKeyTypeList ) )
        {
        key = CHnMdListKey::NewL(); //List
        }
    else
        {
        key = CHnMdUndefKey::NewL(); //Undefined
        }

    return key;
    }

