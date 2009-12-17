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



#ifndef XN_DOM_PROPERTY_VALUE_H
#define XN_DOM_PROPERTY_VALUE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "mxndomlistitem.h"


//CLASS FORWARD
class CXnDomStringPool;
class TRgb;
// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Serializable class to hold property's value.
*
*  @lib XnDomDocument.lib
*  @since Series 60 3.1
*/
class CXnDomPropertyValue : public CBase, public MXnDomListItem
    {
    public:  //Enumerations
        
        /**
        * CSS Primitive value type
        */
        enum TPrimitiveValueType
            {
            EUnknown = 0,
            ENumber,
            EPercentage,
            EEms,
            EExs,
            EPx,
            ECm,
            EMm,
            EIn,
            EPt,
            EPc,
            EDeg,
            ERad,
            EGrad,
            EMs,
            ES,
            EHz,
            EKHz,
            EDimension,
            EString,
            EUri,
            EIdent,
            EAttr,
            ECounter,
            ERect,
            ERgbColor,
            ERgbaColor,
            EFunction,
            EUnitValue
            };
        
        /**
        * Ident type
        */
        enum TIdentType
            {
            ENotSet = 0,
            EAuto,
            ENone,
            EInherit
            };
                
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnDomPropertyValue* NewL( CXnDomStringPool& aStringPool );
        
        /**
        * Two-phased stream constructor.
        */
        static CXnDomPropertyValue* NewL( 
            RReadStream& aStream, 
            CXnDomStringPool& aStringPool );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXnDomPropertyValue();

    public: // New functions
        /**
        * Clone property value.
        * @since Series 60 3.1
        * @return Pointer to a property value. Ownership is transferred to a caller 
        */
        IMPORT_C CXnDomPropertyValue* CloneL();
       
        /**
        * Clone property value.
        * @since Series 60 3.1
        * @param aStringPool StringPool clone which holds actual strings.
        * @return Pointer to a property value. Ownership is transferred to a caller 
        */
        CXnDomPropertyValue* CloneL( CXnDomStringPool& aStringPool );
        
        /**
        * Release object's data.
        * @since Series 60 3.1 
        */
        IMPORT_C void ReleaseData();        
        
        /**
        * Get the property's value. If value type is not recognized,
        * function will leave with KErrNotSupported. 
        * @since Series 60 3.1
        * @return Property's value
        */
        IMPORT_C TReal FloatValueL() const;

        /**
        * Set the property's value based on a given type. If type is not recognized,
        * function will leave with KErrNotSupported. 
        * @since Series 60 3.1
        * @param aValueType Type of a value.
        * @param aFloatValue Property's value.
        */
        IMPORT_C void SetFloatValueL( TPrimitiveValueType aValueType, TReal aFloatValue );

        /**
        * Get the property's string value. If value is not a string type,
        * function will leave with KErrNotSupported.
        * @since Series 60 3.1
        * @return Pointer to the property's string value.
        */
        IMPORT_C const TDesC8& StringValueL() ;
        
        /**
        * Get the property's string value. If value is not a string type,
        * function will return KNullDesC8.
        * @since Series 60 5.0
        * @return Reference to string.
        */
        IMPORT_C const TDesC8& StringValue() ;

        /**
        * Set the property's string value based on a given type. If type is not recognized,
        * function will leave with KErrNotSupported. 
        * @since Series 60 3.1
        * @param aValueType Type of a value.
        * @param aStringValue Property's string value.
        */
        IMPORT_C void SetStringValueL(
            TPrimitiveValueType aValueType,
            const TDesC8& aStringValue );
        
        /**
        * Get the property's color value.
        * @since Series 60 3.1
        * @return Property's color value (TRgb). 
        */
        IMPORT_C const TRgb& RgbColorValueL() const;
        
        /**
        * Sets the RGB Color value
        * @since Series 60 3.1
        * @param aRgb Instance of TRgb.
        */
        IMPORT_C void SetRgbColorL( const TRgb& aRgb );
        
        /**
        * Returns the primitive value type
        * @since Series 60 3.1
        * @return Primitive value type
        */
        IMPORT_C TPrimitiveValueType PrimitiveValueType() const;
        
        /**
        * Checks if string value is "inherit" 
        * @since Series 60 3.1
        * @return ETrue if TPrimitiveValueType::EIdent and string value is "inherit".
        */
        IMPORT_C TBool IsInheritIdent();
        
        /**
        * Checks if string value is "auto" 
        * @since Series 60 3.1
        * @return ETrue if TPrimitiveValueType::EIdent and string value is "auto".
        */
        IMPORT_C TBool IsAutoIdent();
        
        /**
        * Checks if string value is "none" 
        * @since Series 60 3.1
        * @return ETrue if TPrimitiveValueType::EIdent and string value is "none".
        */
        IMPORT_C TBool IsNoneIdent();
        
        /**
        * Set the property's string pool index
        * @since Series 60 3.1
        * @param aValueType Type of a value.
        * @param aStringPoolIndex String pool index.
        */
        IMPORT_C void SetStringPoolIndexL( 
            TPrimitiveValueType aValueType,
            TInt16 aStringPoolIndex );
        /**
        * Get the property's string pool index. If type is not string,
        * function will leave with KErrNotSupported. 
        * @since Series 60 3.1
        * @return String pool index.
        */
        IMPORT_C TInt16 StringPoolIndexL()const; 
    
    public: //From MXnDomListItem    
        /**
        * Documented in MXnDomListItem::Size
        */
        TInt Size()const;
        
        /**
        * Documented in MXnDomListItem::ExternalizeL
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Documented in MXnDomListItem::InternalizeL
        */
        void InternalizeL( RReadStream& aStream );
        
        
        /**
        * Documented in MXnDomListItem::Name
        */
        const TDesC8& Name(){ return KNullDesC8; }   

    private:

        /**
        * C++ default constructor.
        */
        CXnDomPropertyValue( CXnDomStringPool& aStringPool );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
                
    private:    // Data
        
        // union containing the primitive value data, 
        // this is an union for memory consumption optimization reasons
        union
            {
            // rgba color
            TRgb*   iRgbColor;

            // Real value
            TReal*  iRealValue;

            // String reference
            TInt16  iStringRef;
           
            // Constant string value
            TPtrC*  iUnknownValue;
            
            } iData;    
       
       //String pool
       CXnDomStringPool&    iStringPool;
       
       // primitive value type
       TPrimitiveValueType  iPrimitiveValueType;
       
       //Flag to identify ident
       TIdentType           iIdent;
       
    };

#endif      // XN_DOM_PROPERTY_VALUE_H  
            
// End of File
