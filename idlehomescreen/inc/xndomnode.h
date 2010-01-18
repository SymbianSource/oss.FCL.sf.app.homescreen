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
* Description:  Primary datatype for the entire Document Object Model.
*
*/



#ifndef XN_DOM_NODE_H
#define XN_DOM_NODE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "mxndomlistitem.h"

// FORWARD DECLARATIONS
enum TContentType
    {
    EEmpty = 0,
    EAny,
    EPCData
    };

class CXnDomList;
class CXnDomStringPool;
class CXnNode;

// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Class represents a single node in the document tree.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomNode : public CBase, public MXnDomListItem
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnDomNode* NewL( 
            const TDesC8& aName, 
            const TDesC8& aNS,
            CXnDomStringPool* aStringPool );
              
        /**
        * Two-phased stream constructor.
        */
        static CXnDomNode* NewL( RReadStream& aStream, CXnDomStringPool* aStringPool );
        
        /**
        * Destructor.
        */
        virtual ~CXnDomNode();
      
    public: // New functions       
        /**
        * Makes a clone from this node and it's child nodes.
        * @since Series 60 3.1
        * @param aStringPool. A new string pool.
        * @return Pointer to a clone node. Caller has the ownership.
        */
        IMPORT_C CXnDomNode* CloneL( CXnDomStringPool& aStringPool );
        
        /**
        * Get the namespace of this node. 
        * @since Series 60 3.1
        * @return Pointer to the namespace buffer
        */
        IMPORT_C const TDesC8& Namespace();
        
        /**
        * Node's attributes exist in a list.
        * @since Series 60 3.1
        * @return Reference to a attribute list. 
        */
        IMPORT_C CXnDomList& AttributeList()const;
                
        /**
        * Node's properties exist in a list.
        * @since Series 60 3.1
        * @return Reference to a property list. 
        */
        IMPORT_C CXnDomList& PropertyList();
        
        /**
        * Node need to know it's parent.
        * @since Series 60 3.1
        * @param aParent Pointer to this node parent
        */
        IMPORT_C void SetParent( CXnDomNode* aParent );
        
        /**
        * Node's parent is enquered e.g to traverse the document.
        * @since Series 60 3.1
        * @return Pointer to a parent node
        */
        IMPORT_C CXnDomNode* Parent() const;
        
        /**
        * Node id is for a uiengine use.
        * @since Series 60 3.1
        * @param aNodeId Unique identifier
        */
        IMPORT_C void SetNodeId( const TInt aNodeId );

        /**
        * Get the node unique identifier.
        * @since Series 60 3.1
        * @return Node unique identifier
        */
        IMPORT_C TInt NodeId() const;

        /**
        * Get the node unique identifier.
        * @since Series 60 3.1
        * @return Node unique identifier
        */
        IMPORT_C TInt ItemIndex( const MXnDomListItem& aItem )const;

        /**
        * Access to child nodes.
        * @since Series 60 3.1
        * @return Pointer to a list of child nodes
        */
        IMPORT_C CXnDomList& ChildNodes();
        
        /**
        * Child nodes of this node are needed to be able to 
        * traverse the document.
        * @since Series 60 3.1
        * @param aNode Node owns it's childs
        */
        IMPORT_C void AddChildL( CXnDomNode* aNode );

         /**
        * Child nodes of this node are needed to be able to 
        * traverse the document.
        * @since Series 60 3.1
        * @param aNode Node owns it's childs
        * @param aIndex Index in list where the node is added
        */
        IMPORT_C void AddChildL( CXnDomNode* aNode, TInt aIndex );
       
        /**
        * Removes node from the child list and frees memory allocated  
        * for a child node and it descendants.
        * @since Series 60 3.1
        * @param aNode Node to be released
        */
        IMPORT_C void DeleteChild( CXnDomNode* aNode );
        
        /**
        * Replaces a child node with the other node. Memory allocated for 
        * the node is freed.   
        * @since Series 60 3.1
        * @param aNode Node to be replaced
        * @param aNewNode New node
        */
        IMPORT_C void ReplaceChildL( CXnDomNode* aNode, CXnDomNode* aNewNode );
        
        /**
        * Get the character data
        * @since Series 60 3.1
        * @return Reference to a buffer
        */
        IMPORT_C const TDesC8& PCData();
        
        /**
        * Append PCData into node. Thus it is possible to have
        * "<root>a <first/> c <second/> e </root>" PCData.
        * @since Series 60 3.1
        * @param aPCData UTF-8 encoded parsed character data
        */
        IMPORT_C void AppendPCDataL( const TDesC8& aPCData );
        
        /**
        * Set parsed character data.
        * @since Series 60 3.1
        * @param aPCData UTF-8 encoded parsed character data
        */
        IMPORT_C void SetPCDataL( const TDesC8& aPCData );
        
        /**
        * Set content type
        * @since Series 60 3.1
        * @return content type
        */
        IMPORT_C const TContentType& ContentType();
        
        /**
        * Get content type
        * @since Series 60 3.1
        * @param aContentType content type
        */
        IMPORT_C void SetContentType( const TContentType& aContentType );
        
        /**
        * Recursive function to count all child nodes belonging
        * to this node. 
        * @since Series 60 3.1
        * @return This node and it's all childs count
        */
        IMPORT_C TInt DescendantCount() const;
        
        /**
        * Get pointer to the dom's string pool.
        * @since Series 60 3.1
        * @return Reference to string pool
        */
        IMPORT_C CXnDomStringPool* StringPool() const;
        
        /**
        * Deletes attributes when they are not needed anymore.
        * I.e. UIEngine has copied them into property list.
        * @since Series 60 3.1
        */
        IMPORT_C void DeleteAttributeList();
        
        /**
        * Returns value of given attribute
        * @param aAttribute Name of the attribute
        * @since S60 3.1
        * @return Value of an attribute
        */
        IMPORT_C const TDesC8& AttributeValue(const TDesC8& aAttribute) const;
   
   public: //From MXnDomListItem
        
        
        /**
        * Documented in CXnDomListItem::Size
        */
        TInt Size() const;
        
        /**
        * Documented in CXnDomListItem::ExternalizeL
        * NOTE: This function is not implemented yet!
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Documented in CXnDomListItem::InternalizeL
        */
        void InternalizeL( RReadStream& aStream );
        
        /**
        * Documented in CXnDomListItem::Name
        */
        IMPORT_C const TDesC8& Name();        
    
    public:          
    
        /**
        * Sets pointer to associated layout node
        * @since S60 3.1
        * @param aNode Pointer to associated layout node
        */
        IMPORT_C void SetLayoutNode(CXnNode* aNode);
        
        /**
        * Returns pointer to associated layout node
        * @since S60 3.1
        * @return Pointer to associated layout node
        */
        IMPORT_C CXnNode* LayoutNode();
        
        /**
        * Set the namespace for this node. 
        * @since Series 60 3.1
        * @param aNS New namespace
        */
        IMPORT_C void SetNamespaceL(const TDesC8& aNS);
        
        /**
        * Set ownership of node and its children
        * @since Series 60 5.1
        * @param aNS. A new namespace.
        * @return void
        */
        IMPORT_C void SetOwnershipL( const TDesC8& aNS );        

        /**
         * Swap used string pool.
         * 
         * @param aStringPool   New string pool to be used.
         *                      Ownership not transferred!
         */
        IMPORT_C void SwapStringPoolL( CXnDomStringPool* aStringPool );
        
    private:
        /**
        * C++ default constructor.
        */
        CXnDomNode( CXnDomStringPool* aStringPool );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aName, const TDesC8& aNS );
        
    private:    // Data
        
        //iNameRef refers to a name string in a string pool 
        TInt                iNameRef;
        
        //iNSRef refers to a namespace string in a string pool 
        TInt                iNSRef;
        
        //iStringPool is used to resolve strings based on references
        CXnDomStringPool*   iStringPool;
        
        //Type of the elements content
        TContentType        iContentType;
        
        //Parsed character data
        HBufC8*             iPCData;
        
        //Node's parent pointer 
        CXnDomNode*         iParentNode;
        
        //Node unique identifier
        TInt                iNodeId;
        
        //Node's childs are collected into a list. Childs are owned by node
        CXnDomList*         iChildList;
        
        //Node is the owner of the attributes
        CXnDomList*         iAttributeList;
        
        //Node is the owner of the properties
        CXnDomList*         iPropertyList;
        
        // Not owned
        CXnNode*            iLayoutNode;
    };

#endif      // XN_DOM_NODE_H 
            
// End of File
