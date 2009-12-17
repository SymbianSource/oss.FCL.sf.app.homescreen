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
* Description:  Class represents an ODT (Object Description Tree) of Xuikon.
*
*/


#ifndef XNODT_H
#define XNODT_H

#include <e32base.h>

class RWriteStream;
class RReadStream;
class CXnDomDocument;
class CXnResource;
class CXnDomNode;

/**
*  @ingroup group_xnodt
*  Class represents an Object Description Tree of Xuikon.
*  ODT is cabable of marshaling itself into HBufC8 descriptor and unmarshaling
*  from descriptor stream.
*
*  @lib XnODT.lib
*  @since S60 3.1
*/
class CXnODT : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnODT* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnODT();
            
    public: // New functions
                        
        /**
        * Internalizes the ODT header.
        * @since S60 5.1
        * @param aStream The read stream
        * @return void
        */ 
        IMPORT_C static void InternalizeHeaderL( RReadStream& aStream );
                
        /**
        * Internalizes the Resource List.
        * @since S60 5.1
        * @param aStream The read stream
        * @return resource list, ownership is passed to the caller
        */ 
        IMPORT_C static CArrayPtrSeg< CXnResource >* InternalizeResourceListL( 
                RReadStream& aStream );

        /**
        * Internalizes the dom document.
        * @since S60 5.1
        * @param aStream The read stream
        * @return dom document's root node
        */         
        IMPORT_C CXnDomNode* InternalizeDomDocumentL( 
                RReadStream& aStream );
        
        /**
        * Get the DomDocument.
        * @since S60 3.1
        * @return CXnDomDocument& Dom document
        */ 
        IMPORT_C CXnDomDocument& DomDocument() const;
        
    private:

        /**
        * C++ default constructor.
        */
        CXnODT();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data        
        CXnDomDocument* iDomDocument;
    };

#endif      // XNODT_H   
            
// End of File
