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
* Description:  List class to hold MXnDomListItem objects.
*
*/



#ifndef XN_DOM_LIST_H
#define XN_DOM_LIST_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>


// FORWARD DECLARATIONS
class MXnDomListItem;
class CXnDomStringPool;

// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Represents streamable list object, which holds MXnDomListItem objects.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class CXnDomList : public CBase
    {
    public:
        enum TListType
    		{
    		ENodeList=0,
    		EAttributeList,
    		EPropertyList,
    		EPropertyValueList
    		};
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnDomList* NewL( TListType aListType, 
                CXnDomStringPool& aStringPool,
                TInt aGranularity=4);
        
        /**
        * Two-phased stream constructor.
        */
        static CXnDomList* NewL( 
            RReadStream& aStream, 
            CXnDomStringPool& aStringPool );
        
        /**
        * Destructor.
        */
        virtual ~CXnDomList();

    public: // Adding and removing
    	
    	/**
        * Frees memory allocated for all items in a list.
        * @since Series 60 3.1
        */
        IMPORT_C void Reset();
        
        /**
        * List owns items.
        * @since Series 60 3.1
        * @param aListItem to add
        */
        IMPORT_C void AddItemL( MXnDomListItem* aListItem );

        /**
        * List owns items.
        * @since Series 60 3.1
        * @param aListItem to add
        * @param aIndex Index in list where the item is added
        */
        IMPORT_C void AddItemL( MXnDomListItem* aListItem, TInt aIndex );
        
        /**
        * Remove item from list.
        * @since Series 60 3.1
        * @param Index to aList item to remove
        */
        IMPORT_C void RemoveItem( TInt aIndex );
        
        /**
        * Delete item from list.
        * @since Series 60 3.1
        * @param Index to aList item to delete
        */
        IMPORT_C void DeleteItem( TInt aIndex );
        
        /**
        * Remove item from list.
        * @since Series 60 3.1
        * @param aList item to remove
        */
        IMPORT_C void RemoveItem( MXnDomListItem* aItem );
        
        /**
        * Delete item from list.
        * @since Series 60 3.1
        * @param aList item to delete
        */
        IMPORT_C void DeleteItem( MXnDomListItem* aItem );
   
   public: // Accessing     
        /**
        * List items are accessed by index, i.e like arrays 
        * @since Series 60 3.1
        * @param aIndex list item index
        * @return Pointer to a a list item
        */
        IMPORT_C MXnDomListItem* Item( TInt aIndex )const;
        
        /**
        * Number of items in a list.
        * @since Series 60 3.1
        * @return Item count
        */
        IMPORT_C TInt Length() const;
        
        /**
        * Get the first item in a list.
        * @since Series 60 3.1
        * @return Pointer to the first item in a list
        */
        IMPORT_C MXnDomListItem* First();
        
        /**
        * The last item in a list
        * @since Series 60 3.1
        * @return Pointer to the last item in a list
        */
        IMPORT_C MXnDomListItem* Last();
        
        /**
        * Find list item by item name. Searches list items 
        * which have name attribute.
        * @since Series 60 3.1
        * @param aName UTF8 encoded item's name to be searched
        * @return Pointer to the list item, NULL if not exist.
        */
        IMPORT_C MXnDomListItem* FindByName( const TDesC8& aName );
        
        /**
        * Get item index
        * @since Series 60 3.1
        * @param aItem Item to be searched
        * @return Item index or KErrNotFound if not found
        */
        IMPORT_C TInt ItemIndex( const MXnDomListItem& aItem )const;
        
        /**
        * Get reference to the dom's string pool.
        * @since Series 60 3.1
        * @return Reference to string pool
        */
        IMPORT_C CXnDomStringPool& StringPool() const;
        
        /**
        * Count items in a list.
        */
        TInt Count() const;
  public: // Serializing      
        
        /**
        * Get list's data size in bytes.
        * @since Series 60 3.1
        * @return Data size in bytes
        */
        TInt Size() const;
    
        /**
        * Externalize list's data.
        * @since Series 60 3.1
        * @param aStream Output stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalize list's data.
        * @since Series 60 3.1
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );
        
   public: //Utility
   	   		
   		/**
        * Get list item type.
        * @since Series 60 3.1
        * @return List item type
        */
        TListType Type() const 
            {
            return iListType;
            }
            
        
   private:

        /**
        * C++ default constructor.
        */
        CXnDomList( TListType aListType, 
                    CXnDomStringPool& aStringPool, 
                    TInt aGranularity=4 );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    protected:
        //List type    	
    	TListType                       iListType;
        
        //Items are stored into a segmented array
        RPointerArray<MXnDomListItem>   iList;
    
    private:    // Data
        // StringPool, not owned
        CXnDomStringPool&   iStringPool;
        
        
    };

#endif      // XN_DOM_LIST_H  
            
// End of File
