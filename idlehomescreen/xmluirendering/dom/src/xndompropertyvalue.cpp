/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Property's value encapsulation. 
*
*/



// INCLUDE FILES
#include    "xndompropertyvalue.h"
#include    "xndomstringpool.h"
#include    <gdi.h>
// LOCAL CONSTANTS AND MACROS
_LIT8(KInherit, "inherit");
_LIT8(KAuto,    "auto");
_LIT8(KNone,    "none");

// ============================ LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::CXnDomPropertyValue
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomPropertyValue::CXnDomPropertyValue( CXnDomStringPool* aStringPool ):
    iStringPool( aStringPool ),	
    iPrimitiveValueType( (TPrimitiveValueType)KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomPropertyValue::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomPropertyValue* CXnDomPropertyValue::NewL( 
    CXnDomStringPool* aStringPool )
    {
    CXnDomPropertyValue* self = new( ELeave ) CXnDomPropertyValue( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomPropertyValue* CXnDomPropertyValue::NewL( 
    RReadStream& aStream,
    CXnDomStringPool* aStringPool )
    {
    CXnDomPropertyValue* self = new( ELeave ) CXnDomPropertyValue( aStringPool );
    
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }
    
// Destructor
EXPORT_C CXnDomPropertyValue::~CXnDomPropertyValue()
    {
    ReleaseData();     
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::CloneL()
// Make a copy from this object
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomPropertyValue* CXnDomPropertyValue::CloneL()
    {
    CXnDomPropertyValue* clone = CXnDomPropertyValue::NewL( iStringPool );
    CleanupStack::PushL( clone );
    clone->iPrimitiveValueType = iPrimitiveValueType;
    
    
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        {
	        clone->iData.iRealValue = new (ELeave) TReal( *iData.iRealValue );
	        break;
	        }
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        {
	        clone->iData.iStringRef = iData.iStringRef;
	        clone->iIdent = iIdent;
	        break;
	        }

	    case ERgbColor:
	    case ERgbaColor:
	        {
	        clone->iData.iRgbColor = new (ELeave) TRgb( *iData.iRgbColor );
#ifdef SYMBIAN_GRAPHICS_ALPHA_SUPPORT_V1
	        clone->iData.iRgbColor->SetAlpha( iData.iRgbColor->Alpha() );
#endif	        	        
	        break;
	        }
        default:
            break;
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::CloneL( CXnDomStringPool& aStringPool )
// Make a copy from this object
// -----------------------------------------------------------------------------
//
CXnDomPropertyValue* CXnDomPropertyValue::CloneL( CXnDomStringPool& aStringPool )
    {
    CXnDomPropertyValue* clone = CXnDomPropertyValue::NewL( &aStringPool );
    CleanupStack::PushL( clone );
    clone->iPrimitiveValueType = iPrimitiveValueType;
    
    
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        {
	        clone->iData.iRealValue = new (ELeave) TReal( *iData.iRealValue );
	        break;
	        }
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        {
	        const TDesC8& value = iStringPool->String( iData.iStringRef );
            clone->iData.iStringRef = aStringPool.AddStringL( value );
	        clone->iIdent = iIdent;
	        break;
	        }

	    case ERgbColor:
	    case ERgbaColor:
	        {
	        clone->iData.iRgbColor = new (ELeave) TRgb( *iData.iRgbColor );
#ifdef SYMBIAN_GRAPHICS_ALPHA_SUPPORT_V1
	        clone->iData.iRgbColor->SetAlpha( iData.iRgbColor->Alpha() );
#endif	        	        
	        break;
	        }
        default:
            break;
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// TReal CXnDomPropertyValue::FloatValueL
// Returns the float value
// -----------------------------------------------------------------------------
//
EXPORT_C TReal CXnDomPropertyValue::FloatValueL() const
    {
    switch ( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        break;

	    
	    default:
	        User::Leave( KErrNotSupported );
	        break;
        }    
    
    return *iData.iRealValue;
    }

// -----------------------------------------------------------------------------
// TReal CXnDomPropertyValue::SetFloatValueL
// Sets the float value
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::SetFloatValueL( 
    TPrimitiveValueType aValueType,
    TReal aFloatValue )
    {
   
    switch ( aValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        break;

	    default:  // not supported real value
	        User::Leave( KErrNotSupported );
	        break;
        }
    
    ReleaseData();
    
    iPrimitiveValueType = aValueType;
    iData.iRealValue = new ( ELeave ) TReal( aFloatValue );
    }

// -----------------------------------------------------------------------------
// const TDesC& CXnDomPropertyValue::StringValueL
// Returns the string value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomPropertyValue::StringValueL() 
    {
    switch ( iPrimitiveValueType )
        {
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        break;

	    default:  // not supported string value
	        User::Leave( KErrNotSupported );
	        break;
        }
    
    
    return iStringPool->String( iData.iStringRef );
    }
// -----------------------------------------------------------------------------
// const TDesC& CXnDomPropertyValue::StringValue
// Returns the string value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomPropertyValue::StringValue() 
    {
    switch ( iPrimitiveValueType )
        {
        // allowed cases for string value
        case EString:
        case EIdent:
        case EUri:
        case EAttr:
        case EUnknown:
            break;

        default:  // not supported string value
            return KNullDesC8;
           
        }
    
    
    return iStringPool->String( iData.iStringRef );
    }
// -----------------------------------------------------------------------------
// TReal CXnDomPropertyValue::SetStringValueL
// Sets the string value
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::SetStringValueL(
    TPrimitiveValueType aValueType,
    const TDesC8& aStringValue )
    {
    
    switch ( aValueType )
        {
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        break;

	    default:  // not supported string value
	        User::Leave( KErrNotSupported );
	        break;
        }
    
    ReleaseData();
    
    iPrimitiveValueType = aValueType;
    TInt stringRef = iStringPool->AddStringL( aStringValue );
    iData.iStringRef = stringRef;
    
    if ( aStringValue.Compare( KAuto ) == 0 )
        {
        iIdent = EAuto;
        }
    else if ( aStringValue.Compare( KNone ) == 0 )
        {
        iIdent = ENone;
        }
    else if ( aStringValue.Compare( KInherit ) == 0 )
        {
        iIdent = EInherit;
        }
    }


// -----------------------------------------------------------------------------
// const TRgb& CXnDomPropertyValue::RgbColorValueL
// Returns the TRgb object if supported
// -----------------------------------------------------------------------------
//
EXPORT_C const TRgb& CXnDomPropertyValue::RgbColorValueL() const
    {
    if ( ERgbColor != iPrimitiveValueType &&
            ERgbaColor != iPrimitiveValueType )
        {
        User::Leave( KErrNotSupported );
        }
    return *iData.iRgbColor;
    }

// -----------------------------------------------------------------------------
// TCSSPrimitiveValueType CXnDomPropertyValue::PrimitiveValueType
// Returns the primitive value type of the object
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomPropertyValue::TPrimitiveValueType 
    CXnDomPropertyValue::PrimitiveValueType() const
    {
    return iPrimitiveValueType;
    }


    
// -----------------------------------------------------------------------------
// void CXnDomPropertyValue::SetRgbColor
// Sets the CSS rgba object
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::SetRgbColorL( const TRgb& aColor )
    {
    ReleaseData();
    iPrimitiveValueType =  ERgbColor;
    iData.iRgbColor = new (ELeave) TRgb(aColor);
    }
    
// -----------------------------------------------------------------------------
// void CXnDomPropertyValue::ReleaseData
// Releases the union loaded heap allocated data
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::ReleaseData()
    {
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	     case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        {
	        delete iData.iRealValue;
	        iData.iRealValue = NULL;
	        break;
	        }
	    

        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        {
	        iData.iStringRef = -1;
	        iData.iStringRef = NULL;
	        iIdent = ENotSet;
	        break;
	        }

	    case ERgbColor:
	    case ERgbaColor:
	        {
	        delete iData.iRgbColor;
	        iData.iRgbColor = NULL;
	        break;
	        }

	    
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
// void CXnDomPropertyValue::IsInheritIdent
// Compares if value is 'inherit' 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnDomPropertyValue::IsInheritIdent()
    {
    return (iIdent == EInherit? ETrue : EFalse); 
    }

// -----------------------------------------------------------------------------
// void CXnDomPropertyValue::IsAutoIdent
// Compares if value is 'auto' 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnDomPropertyValue::IsAutoIdent()
    {
    return (iIdent == EAuto? ETrue : EFalse); 
    }

// -----------------------------------------------------------------------------
// void CXnDomPropertyValue::IsNoneIdent
// Compares if value is 'none' 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnDomPropertyValue::IsNoneIdent()
    {
    return (iIdent == ENone? ETrue : EFalse); 
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::SetStringPoolIndexL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::SetStringPoolIndexL(
    TPrimitiveValueType aValueType,
    TInt16 aStringPoolIndex )
    {
    switch(aValueType)
        {
        case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        break;
	    default:
	         User::Leave( KErrNotSupported );
        }
    ReleaseData();    
    iPrimitiveValueType = aValueType;
    iData.iStringRef = aStringPoolIndex;    
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::SetStringPoolIndexL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt16 CXnDomPropertyValue::StringPoolIndexL()const
    {
    switch(iPrimitiveValueType)
        {
        case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
	        break;
	    default:
	         User::Leave( KErrNotSupported );
        }
    return iData.iStringRef;    
    }

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::SwapStringPoolL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomPropertyValue::SwapStringPoolL( CXnDomStringPool* aStringPool )
    {
    if( !aStringPool )
        {
        User::Leave( KErrArgument );
        }

    if( iPrimitiveValueType == EString ||
        iPrimitiveValueType == EIdent ||
        iPrimitiveValueType == EUri ||
        iPrimitiveValueType == EAttr ||
        iPrimitiveValueType == EUnknown )
        {
        if( iData.iStringRef != NULL &&
            iData.iStringRef != -1 )
            {
            iData.iStringRef =
                    aStringPool->AddStringL( iStringPool->String( iData.iStringRef ) );
            }
        }
    
    iStringPool = aStringPool;
    }

// -----------------------------------------------------------------------------
// CXnDomPropertyValue::Size()const
// Count size of this object's data.
// -----------------------------------------------------------------------------
//
TInt CXnDomPropertyValue::Size()const
    {
    TInt size( 0 );
    
    size += sizeof(TInt8);  //iPrimitiveValueType
    
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
        case EUnitValue:
            {
            size += sizeof(TReal);
            }
            break;
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
            {
            size += sizeof(TInt16);
            size += sizeof(TIdentType);
            }
            break;
        case ERgbColor:
        case ERgbaColor:
            {
            size += sizeof(TRgb);
            }
            break;
        default:
            break;
        }
    return size;
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::ExternalizeL
// Serializing data to stream.
// -----------------------------------------------------------------------------
//
void CXnDomPropertyValue::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iPrimitiveValueType );
    
    
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
            {
            aStream << *iData.iRealValue;
            }
            break;
        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
            {
            aStream << iData.iStringRef;
            aStream.WriteInt8L( iIdent ); 
            }
            break;
        case ERgbColor:
        case ERgbaColor:
            {
            aStream << *iData.iRgbColor;
            }
            break;
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
// CXnDomPropertyValue::InternalizeL
// Deserializing data from stream to object
// -----------------------------------------------------------------------------
//
void CXnDomPropertyValue::InternalizeL( RReadStream& aStream )
    {
    iPrimitiveValueType = static_cast<TPrimitiveValueType>( aStream.ReadInt8L() );
    
    ReleaseData();
    
    switch( iPrimitiveValueType )
        {
        // allowed cases for real value
	    case ENumber:
	    case EPercentage:
	    case EEms:
	    case EExs:
	    case EPx:
	    case ECm:
	    case EMm:
	    case EIn:
	    case EPt:
	    case EPc:
	    case EDeg:
	    case ERad:
	    case EGrad:
	    case EMs:
	    case ES:
	    case EHz:
	    case EKHz:
	    case EUnitValue:
	        {
	        iData.iRealValue  = new ( ELeave ) TReal();
	        aStream >> *iData.iRealValue;
	        }
	        break;

        // allowed cases for string value
	    case EString:
	    case EIdent:
	    case EUri:
	    case EAttr:
	    case EUnknown:
            {
            aStream >> iData.iStringRef;
            iIdent = static_cast<TIdentType>( aStream.ReadInt8L() );
            }
            break;
        case ERgbColor:
        case ERgbaColor:
            {
            iData.iRgbColor = new (ELeave) TRgb();
            aStream >> *iData.iRgbColor;
            }
            break;
        
        default:
            break;
        }
    }

 

//  End of File  
