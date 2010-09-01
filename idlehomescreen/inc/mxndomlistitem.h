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
* Description:  Interface class for dom list items.
*
*/



#ifndef MXN_DOM_LIST_ITEM_H
#define MXN_DOM_LIST_ITEM_H

//  INCLUDES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  @ingroup group_domdocument
*  Class represents a list item interface. Items are stored into
*  a CXnDomList as a instance of this class.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class MXnDomListItem
    {
    public:  // Destructor
        
        /**
        * Destructor.
        */
        virtual ~MXnDomListItem(){}

    public: // Serializing
                        
        /**
        * This list item's data size in bytes.
        * @since Series 60 3.1
        * @return Data size in bytes.
        */
        virtual TInt Size() const=0;
        
        /**
        * Externalize list item.
        * @since Series 60 3.1
        * @param aStream Output stream
        */
        virtual void ExternalizeL( RWriteStream& aStream )const=0;
        
        /**
        * Internalize list item.
        * @since Series 60 3.1
        * @param aStream Input stream
        */
        virtual void InternalizeL( RReadStream& aStream )=0;
        
        
        
    public: //Accessing     
        /**
        * Get the name of this item as UTF8 encoded, if any. 
        * @since Series 60 3.1
        * @return Name buffer or KNullDesC8
        */
        virtual const TDesC8& Name()=0;     
 
    };

#endif      // MXN_DOM_LIST_ITEM_H 
            
// End of File
