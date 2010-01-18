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
* Description:  Xuikon property source
*
*/

//  INCLUDE FILES
#include "xnproperty.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xndomstringpool.h"
#include <utf.h>
#include <gdi.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnProperty::NewL( CXnDomProperty* aProperty )
    {
    CXnProperty* self = new ( ELeave ) CXnProperty;

    CleanupStack::PushL( self );

    self->ConstructL( aProperty );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnProperty::NewL( const TDesC8& aName,
    CXnDomPropertyValue* aPropertyValue, CXnDomStringPool& aStringPool )
    {
    CXnProperty* self = new ( ELeave ) CXnProperty;

    CleanupStack::PushL( self );

    self->ConstructL( aName, aPropertyValue, aStringPool );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnProperty::NewL( const TDesC8& aPropertyName,
    const TDesC8& aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType,
    CXnDomStringPool& aStringPool )
    {
    CXnProperty* self = new ( ELeave ) CXnProperty;
    CleanupStack::PushL( self );

    self->ConstructL( aPropertyName, aPropertyValue, aValueType, aStringPool );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnProperty::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnProperty::NewL( const TDesC8& aPropertyName,
    TReal aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType,
    CXnDomStringPool& aStringPool )
    {
    CXnProperty* self = new ( ELeave ) CXnProperty;
    CleanupStack::PushL( self );

    self->ConstructL( aPropertyName, aPropertyValue, aValueType, aStringPool );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnProperty::NewSharedL
// -----------------------------------------------------------------------------
//
CXnProperty* CXnProperty::NewSharedL( CXnDomProperty* aProperty )
    {
    CXnProperty* self = new ( ELeave ) CXnProperty;

    CleanupStack::PushL( self );

    self->ConstructSharedL( aProperty );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnProperty::~CXnProperty
// Destructor
// -----------------------------------------------------------------------------
//
CXnProperty::~CXnProperty()
    {
    if ( iIsDomPropertyOwned )
        {
        delete iProperty;
        }
    }

// -----------------------------------------------------------------------------
// CXnProperty::CXnProperty
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnProperty::CXnProperty()
    {
    }

// -----------------------------------------------------------------------------
// CXnProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnProperty::ConstructL( CXnDomProperty* aProperty )
    {
    iProperty = aProperty;
    iIsDomPropertyOwned = ETrue;
    }

// -----------------------------------------------------------------------------
// CXnProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnProperty::ConstructL( const TDesC8& aName,
    CXnDomPropertyValue* aPropertyValue, CXnDomStringPool& aStringPool )
    {
    iProperty = CXnDomProperty::NewL( aName, &aStringPool );
    iIsDomPropertyOwned = ETrue;

    CXnDomList& propertyValueList = iProperty->PropertyValueList();
    propertyValueList.AddItemL( aPropertyValue );
    }

// -----------------------------------------------------------------------------
// CXnProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnProperty::ConstructL( const TDesC8& aPropertyName,
    const TDesC8& aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType,
    CXnDomStringPool& aStringPool )
    {
    iProperty = CXnDomProperty::NewL( aPropertyName, &aStringPool );
    iIsDomPropertyOwned = ETrue;

    CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( &aStringPool );
    CleanupStack::PushL( newValue );

    newValue->SetStringValueL( aValueType, aPropertyValue );

    CXnDomList& propertyValueList( iProperty->PropertyValueList() );

    propertyValueList.AddItemL( newValue );

    CleanupStack::Pop( newValue );
    }

// -----------------------------------------------------------------------------
// CXnProperty::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnProperty::ConstructL( const TDesC8& aPropertyName,
    TReal aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType,
    CXnDomStringPool& aStringPool )
    {
    iProperty = CXnDomProperty::NewL( aPropertyName, &aStringPool );
    iIsDomPropertyOwned = ETrue;

    CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( &aStringPool );
    CleanupStack::PushL( newValue );

    newValue->SetFloatValueL( aValueType, aPropertyValue );

    CXnDomList& propertyValueList( iProperty->PropertyValueList() );

    propertyValueList.AddItemL( newValue );

    CleanupStack::Pop( newValue );
    }

// -----------------------------------------------------------------------------
// CXnProperty::ConstructSharedL
// -----------------------------------------------------------------------------
//
void CXnProperty::ConstructSharedL( CXnDomProperty* aProperty )
    {
    iProperty = aProperty;
    iIsDomPropertyOwned = EFalse;
    }

// -----------------------------------------------------------------------------
// CXnProperty::EqualsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnProperty::EqualsL( CXnProperty& aProperty ) const
    {
    TBool equals( EFalse );

    if ( aProperty.Property()->PseudoClass() != iProperty->PseudoClass() )
        {
        return equals;
        }

    if ( aProperty.Property()->Name() != iProperty->Name() )
        {
        return equals;
        }

    CXnDomPropertyValue* value1 = static_cast< CXnDomPropertyValue* >(
        iProperty->PropertyValueList().Item( 0 ) );
    CXnDomPropertyValue::TPrimitiveValueType primitiveValueType1 =
        value1->PrimitiveValueType();

    CXnDomPropertyValue* value2 = static_cast< CXnDomPropertyValue* >(
        aProperty.Property()->PropertyValueList().Item( 0 ) );
    CXnDomPropertyValue::TPrimitiveValueType primitiveValueType2 =
        value2->PrimitiveValueType();

    switch ( primitiveValueType1 )
        {
        case CXnDomPropertyValue::ENumber:
        case CXnDomPropertyValue::EPercentage:
        case CXnDomPropertyValue::EEms:
        case CXnDomPropertyValue::EExs:
        case CXnDomPropertyValue::EPx:
        case CXnDomPropertyValue::ECm:
        case CXnDomPropertyValue::EMm:
        case CXnDomPropertyValue::EIn:
        case CXnDomPropertyValue::EPt:
        case CXnDomPropertyValue::EPc:
        case CXnDomPropertyValue::EDeg:
        case CXnDomPropertyValue::ERad:
        case CXnDomPropertyValue::EGrad:
        case CXnDomPropertyValue::EMs:
        case CXnDomPropertyValue::ES:
        case CXnDomPropertyValue::EHz:
        case CXnDomPropertyValue::EKHz:
        case CXnDomPropertyValue::EUnitValue:
            if ( primitiveValueType1 == primitiveValueType2 )
                {
                equals = ( value1->FloatValueL() == value2->FloatValueL() );
                }
            break;
        case CXnDomPropertyValue::EString:
        case CXnDomPropertyValue::EIdent:
        case CXnDomPropertyValue::EUri:
        case CXnDomPropertyValue::EAttr:
        case CXnDomPropertyValue::EUnknown:
            if ( primitiveValueType2 == CXnDomPropertyValue::EString ||
                 primitiveValueType2 == CXnDomPropertyValue::EIdent ||
                 primitiveValueType2 == CXnDomPropertyValue::EUri ||
                 primitiveValueType2 == CXnDomPropertyValue::EAttr ||
                 primitiveValueType2 == CXnDomPropertyValue::EUnknown )
                {
                equals = ( value1->StringValueL() == value2->StringValueL() );
                }
            break;
        case CXnDomPropertyValue::ERgbColor:
        case CXnDomPropertyValue::ERgbaColor:
            if ( primitiveValueType1 == primitiveValueType2 )
                {
                TRgb rgb1( value1->RgbColorValueL() );
                TRgb rgb2( value2->RgbColorValueL() );

                equals = ( rgb1 == rgb2 );
                }
            break;
        default:
            break;
        }

    return equals;
    }

// -----------------------------------------------------------------------------
// CXnProperty::Property
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomProperty* CXnProperty::Property()
    {
    return iProperty;
    }

// -----------------------------------------------------------------------------
// CXnProperty::StringValueL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CXnProperty::StringValueL()
    {
    CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
        iProperty->PropertyValueList().Item( 0 ) );
    const TDesC8& name = StringValue();
    HBufC* utfName = HBufC::NewL( name.Length() );
    TPtr16 ptr = utfName->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr, name );
    return utfName;
    }

// -----------------------------------------------------------------------------
// CXnProperty::StringValue
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnProperty::StringValue()
    {
    CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
        iProperty->PropertyValueList().Item( 0 ) );
    return value->StringValue();
    }

// -----------------------------------------------------------------------------
// CXnProperty::FloatValueL
// -----------------------------------------------------------------------------
//
EXPORT_C TReal CXnProperty::FloatValueL()
    {
    CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
        iProperty->PropertyValueList().Item( 0 ) );
    return value->FloatValueL();
    }

// -----------------------------------------------------------------------------
// CXnProperty::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnProperty::CloneL()
    {
    CXnDomProperty* property = iProperty->CloneL();
    CleanupStack::PushL( property );
    CXnProperty* returnValue = CXnProperty::NewL( property );
    CleanupStack::Pop( property );
    return returnValue;
    }
