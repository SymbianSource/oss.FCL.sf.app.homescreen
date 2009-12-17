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
* Description:  Represent single xmluiml attribute
*
*/



#ifndef XN_DOM_ATTRIBUTE_H
#define XN_DOM_ATTRIBUTE_H

//  INCLUDES
#include    <e32base.h>
#include    <s32strm.h>
#include    "mxndomlistitem.h"

//CLASS FORWARD

class CXnDomStringPool;

// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Represent attribute name and value as strings. 
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomAttribute : public CBase, public MXnDomListItem
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor 8 bit.
        */
        IMPORT_C static CXnDomAttribute* NewL( 
            const TDesC8& aName, 
            CXnDomStringPool& aStringPool );
        
        /**
        * Two-phased stream constructor.
        */
        static CXnDomAttribute* NewL( 
            RReadStream& aStream, 
            CXnDomStringPool& aStringPool );
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXnDomAttribute();

    public: // New functions
        /**
        * Makes a copy from original attribute.
        * @since Series 60 3.1
        * @return Pointer to attribute. Attribute ownership is transferred to a caller
        */
        IMPORT_C CXnDomAttribute* CloneL();        
        
        /**
        * Makes a copy from original property.
        * @since Series 60 3.1
        * @param Original string pool clone.
        * @return Pointer to an attribute. Ownership is transferred to a caller.
        */
        CXnDomAttribute* CloneL( CXnDomStringPool& aStringPool );        
        
        /**
        * Get the attribute value.
        * @since Series 60 3.1
        * @return Pointer to a attribute value string. If value is not set KNullDesC8 is returned. 
        */
        IMPORT_C const TDesC8& Value();
        
        /**
        * Set attribute value.
        * @since Series 60 3.1
        * @param aValue Attribute string value
        */
        IMPORT_C void SetValueL( const TDesC8& aValue );
        
        /**
        * Get the attributes name string pool index. 
        * @since Series 60 3.1
        * @return String pool index.
        */
        IMPORT_C TInt16 NameStringPoolIndex()const;
        
        /**
        * Get the attributes value string pool index. 
        * @since Series 60 3.1
        * @return String pool index.
        */
        IMPORT_C TInt16 ValueStringPoolIndex()const;
         
    public: //From CXnDomListItem
        
        /**
        * Documented in CXnDomListItem::Size
        */
        IMPORT_C const TDesC8& Name();
        
       
        /**
        * Documented in CXnDomListItem::Size
        */
        TInt Size() const;
        
        /**
        * Documented in CXnDomListItem::Size
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Internalize property.
        * @since Series 60 3.1
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );    
    private:
        
        /**
        * C++ default constructor.
        */
        CXnDomAttribute( CXnDomStringPool& aStringPool );
            
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aName );

     
    private:    // Data
        //String pool to get string for references, not owned
        CXnDomStringPool& iStringPool;
        
        //Attribute name reference
        TInt16         iNameRef;
        
        //Attribute value reference
        TInt16         iValueRef;

    };

#endif      // XN_DOM_ATTRIBUTE_H   
            
// End of File
