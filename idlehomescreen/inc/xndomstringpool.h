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
* Description:  Flyweigth pattern implementation for dom strings.
*
*/



#ifndef XN_DOM_STRING_POOL_H
#define XN_DOM_STRING_POOL_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Class utilize flyweight pattern. Dom strings are stored once 
*  and referred with index. Class can be serialized.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomStringPool : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnDomStringPool* NewL();
        /**
        * Two-phased stream constructor.
        */
        static CXnDomStringPool* NewL( RReadStream& aStream );
        /**
        * Destructor.
        */
        virtual ~CXnDomStringPool();

   public: 
        /**
        * Make a copy from original StringPool.
        * @since Series 60 3.1
        * @return Pointer to a string pool. Ownership is transferred to a caller.
        */
        CXnDomStringPool* CloneL(); 
   public: //Adding
        
        /**
        * Set dom string into string pool.
        * @param aString String to add to string pool
        * @return Index (reference) to string pool
        */
        IMPORT_C TInt AddStringL( const TDesC8& aString ); 
   
   public: //Accessing     
        /**
        * Get pointer to the node element name.
        * @param aMap Map object which has index to name string
        * @return Pointer to the name
        */
        const TDesC8& String( const TInt aStringRef ); 
        
        /**
        * Get object's data size in bytes.
        * @return Data size in bytes
        */
        TInt Size() const;
        
        /**
        * Externalize object
        * @param aStream Output stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Internalize object
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );          
        
        /**
        * Get index offset to string array for shared resources
        * @return An offset that caller must add to its indexes
        */
        TUint Offset() const;

   private:

        /**
        * C++ default constructor.
        */
        CXnDomStringPool();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
     
    private:    
        //String pool
        RPointerArray<HBufC8>       iStringPool;
       
        TUint iStringPoolOffsetCurrent; // Internalize uses 
        TUint iStringPoolOffsetNext;
    };

#endif      // XN_DOM_STRING_POOL_H  
            
// End of File
