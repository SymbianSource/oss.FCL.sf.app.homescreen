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
* Description:  Represents the entire xmluiml specific xml and css data. 
*
*/



#ifndef XN_DOM_DOCUMENT_H
#define XN_DOM_DOCUMENT_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>


//FORWARD DECLARATIONS
class CXnDomStringPool;
class CXnDomNode;
// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Provides the primary access to the document's data.
*  
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomDocument: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnDomDocument* NewL();
        /**
        * Two-phased stream constructor.
        */
        IMPORT_C static CXnDomDocument* NewL( RReadStream& aStream );
        /**
        * Construct CXnDomDocument from streamed HBufC8.
        */
        IMPORT_C static CXnDomDocument* NewL( const HBufC8* aBufStream );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXnDomDocument();

    public: // New functions
        /**
        * Makes a clone of this document.
        * @since Series 60 3.1
        * @return Pointer to a document. Caller has the ownership.
        */
        IMPORT_C CXnDomDocument* CloneL();
        
        /**
        * Factory method to create node elements, because elements cannot exist outside 
        * the context of a document.
        * @since Series 60 3.1
        * @param aName UTF-8 encoded Element name
        * @param aNamespace UTF-8 encoded Element namespace
        * @return A new CXnDomNode type instance. Ownership is transferred to caller. 
        */
        IMPORT_C CXnDomNode* CreateElementNSL( const TDesC8& aName, const TDesC8& aNamespace );
        
        /**
        * Set the root of the document tree. Root node provides primary access to 
        * document data. This class owns root node.
        * @since Series 60 3.1
        * @param aRootNode Documents's root node. Document takes ownership.
        */
        IMPORT_C void SetRootNode( CXnDomNode* aRootNode );

        /**
        * Get the root node of the document.
        * @since Series 60 3.1
        * @return Pointer to CXnDomNode type node
        */
        IMPORT_C CXnDomNode* RootNode() const;

        /**
        * Get the last node of the document.
        * @since Series 60 3.1
        * @return Pointer to CXnDomNode type node
        */
        IMPORT_C CXnDomNode* LastNode() const;

        /**
        * Get the size of the document's data. 
        * @since Series 60 3.1
        * @return Document's data size
        */
        IMPORT_C TInt Size() const;
        
        /**
        * Get document node count.
        * @since Series 60 3.1
        * @return Count of all nodes in a document
        */
        IMPORT_C TInt DomNodeCount() const;
        
        /**
        * Externalize document's data.
        * @since Series 60 3.1
        * @param aStream Output stream
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalize document's data.
        * @since Series 60 3.1
        * @param aStream Input stream
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /**
        * Returns document's data as a streamed buffer.
        * @since Series 60 3.1
        * @return Pointer to a new HBufC8 buffer which holds streamed document. 
        * Ownership is transferred to a caller.
        */
        IMPORT_C HBufC8* MarshallL();
        
        /**
        * Get reference to string pool
        * @return Reference to string pool implementation
        */
        IMPORT_C CXnDomStringPool& StringPool() const;

        /**
        * Read contents from a stream.
        * @param aStream Input stream
        * @return void
        */
        IMPORT_C CXnDomNode* ReadL( RReadStream& aStream );

    private:

        /**
        * C++ default constructor.
        */
        CXnDomDocument();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        //Document owns the root node.
        CXnDomNode*         iRootNode;
        
        //Namespace map stores namespaces used in dom
        CXnDomStringPool*   iDomStringPool;
        
        
    };

#endif      // XN_DOM_DOCUMENT_H  
            
// End of File
