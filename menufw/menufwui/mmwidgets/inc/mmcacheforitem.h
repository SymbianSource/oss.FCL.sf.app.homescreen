/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*  Version     : 2 << Don't touch! Updated by Synergy at check-out.
*
*/

#ifndef MMCACHEFORITEM_H
#define MMCACHEFORITEM_H

#include <e32base.h>
#include <e32cmn.h>
#include "mmsubcellssetupcode.h"

class CGulIcon;
class CHnIconHolder;
class CMmItemsDataCache;

/**
 * Stores cached information needed to draw a single item.
 */
NONSHARABLE_CLASS( CMmCacheForItem ): public CBase
    {
public:
    /**
     * Creates a new instance of CMmCacheForItem object.
     * The newly created object will be left on the cleanup stack.
     * 
     * @param aParent The owner of the newly created object.
     */
    static CMmCacheForItem* NewLC( CMmItemsDataCache& aParent );
    
    /**
     * Creates a new instance of CMmCacheForItem object.
     * The newly created object will be left on the cleanup stack.
     * 
     * @param aParent The owner of the newly created object. 
     */
    static CMmCacheForItem* NewL( CMmItemsDataCache& aParent );
    
    /**
     * Standard C++ virtual destructor.
     */
    ~CMmCacheForItem();
    
    /**
     * Sets the subcell setup code that denoted valid subcell setup for this
     * item.
     * 
     * @param aSubcellsSetupCode Subcells setup code for this item. 
     */
    inline void SetSubcellsSetupCode( TMmSubcellsSetupCode aSubcellsSetupCode );
    
    /**
     * Returns the code of the subcells setup required by this item to be
     * properly drawn using cached data.
     */
    inline TMmSubcellsSetupCode GetSubcellsSetupCode() const;
    
    /**
     * Returns information whether the data in this cache is valid.
     * Invalid cache should never be used.
     * 
     * @return ETrue if valid, EFalse otherwise.
     */
    inline TBool IsValid() const;
    
    /**
	 * Marks this cache object as valid.
	 */
    inline void MarkAsValidL();
    
    /**
	 * Marks this cache object as invalid.
	 */
    inline void MarkAsInvalid();
    
    /**
     * Returns information whether at the moment of updating this cache object
     * the item was current (highlighted).
     * 
     * @return ETrue if current, EFalse otherwise.
     */
    inline TBool IsCurrent();
    
    /**
     * Sets the information whether the item was current (highlighted). 
     */
    inline void SetCurrent( TBool aCurrent );
    
    /**
     * Retrieves the cached item text.
     * This is the complete string needed by AVKON methods to draw the item
     * including tab characters and icon indices.
     * 
     * @return Cached item text.
     */
    inline const TDesC& GetItemText() const;
    
    /**
     * Returns reference to the text buffer owned by this cache.
     * This methods allows for updating the cached text.
     * 
     * @return Reference to the text buffer owned by this cache.
     */
    inline RBuf& GetItemTextForModifications();
    
    /**
     * Retrives cached template name.
     * 
     * @return Cached template name. 
     */
    const TDesC8& GetTemplate() const;
    
    /**
     * Sets template name for the item.
     * 
     * @param aItemTemplate Template name.
     */
    void SetTemplateL( const TDesC8& aItemTemplate );
    
    /**
     * Returns cached item size.
     *
     * @return Cached item size. 
     */
    inline TSize GetSize() const;
    
    /**
     * Updates item size stored in the cache.
     * 
     * @param aItemSize Item size.
     */
    inline void SetSize( TSize aItemSize );

    /**
     * Produces icon list, which can be then passed as parameter to
     * CEikFormattedCellListBoxData::SetIconArray.
     * It is up to the caller to delete the returned array. However,
     * the icons stored in the array must not be deleted.
     */
    CArrayPtr<CGulIcon>* GetIconListL();

    /**
     * Appends an icon (specifically an icon holder) to the cached array of icon holders.
     * The icon holder is then co-owned by this CMmCacheForItem object so that the
     * icon contained in the icon holder can be safely stored.
     * 
     * @param aIconHolder An icon holder (NULL value not allowed).
     * @return Index of the just appended icon holder.
     */
    TInt AppendIconL( CHnIconHolder* aIconHolder );

    /**
     * Clears the array of icon holders stored in this item cache.
     * It ceases co-owning the icon holders and resets the array
     * that stores them.
     */
    void ClearIconArray();
    
    /**
     * Based on the information about how the item must be drawn (with
     * hightlight or not) and what currect subcells setup it, this methods
     * invalidates the cache for this item to prevent problems such as:
     * * cache information for highlighted item being used for to draw
     *   the item without highlight or vice versa.
     * * relying on cache and not performing the full subcells setup while
     *   the current subcells setup does not fit this particular item.
     */
    void InvalidateIfCacheMayNotBeUsed( TBool aIsItemCurrent,
            TMmSubcellsSetupCode aSubcellsSetupCode );
    
protected:
    /**
     * Stardard C++ constructor.
     * 
     * @param aParent CMmItemsDataCache object owning this object.
     */
    CMmCacheForItem( CMmItemsDataCache& aParent );
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Updates the iIconList so that it reflects the contents of
     * iIconHolderList.
     */
    void UpdateIconListL();

protected:
    /**
     * Stores information whether this cache object is valid.
     */
    TBool iIsValid;
    
    /**
     * Stores information the item was highlighted at the moment
     * of last cache update.
     */
    TBool iIsCurrent;
    
    /**
     * Stores subcells setup code that denotes the subcells setup
     * needed to draw the item using the cached data.
     */
    TMmSubcellsSetupCode iSubcellsSetupCode;
    
    /**
     * Own.
     * Stores the item text in the format that
     * CFormattedCellListBoxData::Draw accepts.
     * This text includes icon indices separated with tab characters.
     */
    RBuf iItemText;
    
    /**
     * Stores the item size.
     */
    TSize iItemSize;
    
    /**
     * Template identifier obtained from @c aParent using
     * @c CMmItemsDataCache::GetTemplateIdentifierL.
     */
    TInt iTemplateIdentifier;
    
    /**
     * Own. 
     * The icon holders in the array are co-owned.
     */
    RArray<CHnIconHolder*> iIconHolderList;
    
    /**
     * Object that represents the whole data cache as opposed
     * to single item cache represented by this object.
     */
    CMmItemsDataCache& iParent;

    /**
     * Array of icons stored in the icon holders which are in iIconHolderList.
     * This member is only valid when this cache object is marked as valid
     * using the MarkAsValidL method.
     * Own.
     */
    CArrayPtr<CGulIcon>* iIconList;

    };

#include "mmcacheforitem.inl"

#endif // MMCACHEFORITEM_H
