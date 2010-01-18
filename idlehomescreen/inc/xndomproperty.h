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
* Description:  Represents a CSS property.
*
*/



#ifndef XN_DOM_PROPERTY_H
#define XN_DOM_PROPERTY_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "mxndomlistitem.h"


// FORWARD DECLARATIONS
class CXnDomPropertyValue;
class CXnDomList;
class CXnDomStringPool;

// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Represents a CSS property. 
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomProperty : public CBase, public MXnDomListItem
    {
    public:
        /**
        * Property value's pseudo class type
        */
        enum TPseudoClass
        	{
        	ENone=0,
        	EFocus,
        	EPassiveFocus,
        	EActive,
        	EHold,
        	EEnabled,
        	EDisabled,
        	EHover,
        	ELink,
        	EVisited,
            EEdit,
            EPressedDown
        	};
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnDomProperty* NewL( 
            const TDesC8& aName,
            CXnDomStringPool* aStringPool );
        
        /**
        * Two-phased stream constructor.
        */
        static CXnDomProperty* NewL( 
            RReadStream& aStream, 
            CXnDomStringPool* aStringPool );
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnDomProperty* NewL( 
            TInt16 aStringPoolIndex,
            CXnDomStringPool* aStringPool );
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXnDomProperty();

    public: // New functions
        /**
        * Make a copy from original property.
        * @since Series 60 3.1
        * @return Pointer to a property. Ownership is transferred to a caller.
        */
        IMPORT_C CXnDomProperty* CloneL();        
        
        /**
        * Make a copy from original property.
        * @since Series 60 3.1
        * @param Original string pool clone.
        * @return Pointer to a property. Ownership is transferred to a caller.
        */
        IMPORT_C CXnDomProperty* CloneL( CXnDomStringPool& aStringPool );   
        
        /**
        * Get property value list.
        * @since Series 60 3.1
        * @return Reference to property value list.
        */
        IMPORT_C CXnDomList& PropertyValueList();
        
        /**
        * Test if property has any value.
        * @since Series 60 3.1
        * @return ETrue if at least one value exist
        */
        IMPORT_C TBool IsValueSet();
        
        /**
        * Test property's capability to inherit value.
        * @since Series 60 3.1
        * @return ETrue if property's value can be inherited
        */
        IMPORT_C TBool Inherited();
        
        /**
        * Set property to be capable to inherit value.
        * @since Series 60 3.1
        * @param aInherited ETrue inherit capable
        */
        IMPORT_C void SetInherited( TBool aInherited );
        
        /**
        * Get pointer to the dom's string pool.
        * @since Series 60 3.1
        * @return Reference to string pool
        */
        IMPORT_C CXnDomStringPool* StringPool() const;
        
        /**
        * Get the property's string pool index. 
        * @since Series 60 3.1
        * @return String pool index.
        */
        IMPORT_C TInt16 StringPoolIndex()const; 
        
        /**
         * Swap used string pool.
         * 
         * @param aStringPool   New string pool to be used.
         *                      Ownership not transferred!
         */
        IMPORT_C void SwapStringPoolL( CXnDomStringPool* aStringPool );        
        
     public:  //Pseudo class utility   
        /**
        * Get value's pseudo class
        * @since Series 60 3.1
        * @return Value's pseudo class
        */
        IMPORT_C TPseudoClass PseudoClass()const;
        
        /**
        * Set value's pseudo class
        * @since Series 60 3.1
        * @param aPseudoClass Pseudo class enumeration
        */
        IMPORT_C void SetPseudoClass( TPseudoClass aPseudoClass ); 
     
    public: //From CXnDomListItem
               
        /**
        * Documented in CXnDomListItem::Name
        */
        IMPORT_C const TDesC8& Name(); 
        
        /**
        * Documented in CXnDomListItem::Size
        */
        TInt Size() const;
        
        /**
        * Documented in CXnDomListItem::ExternalizeL
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Documented in CXnDomListItem::InternalizeL
        */
        void InternalizeL( RReadStream& aStream );
    
    private:

        /**
        * C++ default constructor.
        */
        CXnDomProperty( CXnDomStringPool* aStringPool);
        
        /**
        * C++ constructor with string pool index.
        */
        CXnDomProperty( TInt16 aStringPoolIndex, CXnDomStringPool* aStringPool);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aName );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        //String pool to get string for references, not owned
        CXnDomStringPool* iStringPool;
        
        // Pseudo class type for value
        TPseudoClass    iPseudoClass;
       
        //Property's name reference to string pool
        TInt16          iNameRef;
        
        //Property owns a value list which has property's values
        CXnDomList* iPropValList;
     
        //Property is capable to inherit
        TBool       iInherited;
                
        
    };

#endif      // XN_DOM_PROPERTY_H   
            
// End of File
