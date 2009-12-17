/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Item id, name and properties
*
*/



#ifndef C_CITEMMAP_H
#define C_CITEMMAP_H



#include <e32base.h>


namespace hspswrapper{

class CPropertyMap;

/**
 *  Item id, name and properties
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib xnlayoutengine.lib
 *  @since S60 v5.0
 */
class CItemMap : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CItemMap* NewL();
    IMPORT_C static CItemMap* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CItemMap();

    /**
     * Set item id
     *
     * @param aItemId Item id
     * @return Reference to this object
     */
    IMPORT_C CItemMap& SetItemIdL(const TDesC8& aItemId );
    
    /**
     * Get item id
     *
     * @return Reference to item id or KNullDesC8
     */
    IMPORT_C const TDesC8& ItemId()const;
    
    /**
     * Set item name
     *
     * @param aItemName Item name
     * @return Reference to this object
     */
    IMPORT_C CItemMap& SetItemNameL(const TDesC8& aItemName );
    
    /**
     * Get item name
     *
     * @return Reference to item name or KNullDesC8
     */
    IMPORT_C const TDesC8& ItemName()const;
    
    /**
     * Add item property map into properties array
     *
     * @param aPropertyMap Property map. Takes ownership
     * @return Reference to this object
     */
    IMPORT_C CItemMap& AddPropertyMapL(CPropertyMap* aPropertyMap );
    
    /**
     * Get property map array.
     *
     * @return Reference to proprty map array
     */
    IMPORT_C RPointerArray<CPropertyMap>& Properties()const;

private:

    CItemMap();

    void ConstructL();


private: // data

    /**
     * Own. Item id
     */
    HBufC8* iItemId;
    
    /**
     * Own. Item name
     */
    HBufC8* iItemName;
    
    /**
     * Own. Array of property maps
     */
    mutable RPointerArray<CPropertyMap> iProperties;
    };

}


#endif // C_CITEMMAP_H
