/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMITEMSDATACACHE_H
#define MMITEMSDATACACHE_H

#include <e32base.h>
#include <e32cmn.h>

class CMmCacheForItem;

/**
 * Provides a cache of data needed to draw every item in grid or list view.
 * This class is only used by CMmListBoxItemDrawer.
 */
NONSHARABLE_CLASS( CMmItemsDataCache ): public CBase
    {
public:
    /**
     * Creates new CMmItemsDataCache object. 
     */
    static CMmItemsDataCache* NewLC();
    
    /**
     * Creates new CMmItemsDataCache object.
     */
    static CMmItemsDataCache* NewL();
    
    /**
     * Returns item cache object for an item at specified index.
     * 
     * @param aItemIndex Item index
     * @return Item cache object.
     */
    CMmCacheForItem* GetItemCacheL( TInt aItemIndex );
    
    /**
     * Gets an integer identifier of a template of given name.
     * The identifier is guaranteed to be unique among other
     * identifiers obtained from this instance of CMmItemsDataCache
     * object.
     * Such identifier can be used to perform fast template
     * comparisons in the CMmListBoxItemDrawer code.
     * @c TMmSubcellsSetupCode code assumes that the template
     * identifier will not exceed 255. Assumption will be met unless
     * there are more that 256 different templates used in one suite
     * which is extremely unlikely.
     * 
     * @param aTemplateName Name of a template.
     * @return Template identifier based on template name.
     */
    TInt GetTemplateIdentifierL( const TDesC8& aTemplateName );
    
    /**
     * Translates a template identifier obtained from @c GetTemplateIdentifierL
     * into a regular string containing template name.
     * 
     * @param aTemplateIdentifier Template identifier from GetTemplateIdentifierL.
     * @return Template name.
     */
    const TDesC8& GetTemplateNameByIdentifier( TInt aTemplateIdentifier ) const;
    
    /**
     * Marks the whole data in cache as outdated so that
     * it cannot possibly be used until it is updated by
     * setting up subcells in normal (non-cached) mode.
     */
    void Invalidate();
    
    /**
     * Reduces the number of cache entries to given number.
     * If there are less entries than aItemCount then nothing
     * is done.
     * 
     * @param aItemCount number of items to store cached data for
     */
    void Trim( TInt aItemCount );
    
    /**
     * Standard C++ virtual destructor. 
     */
    ~CMmItemsDataCache();
protected:
    
    /**
     * Standard C++ constructor.
     */
    CMmItemsDataCache();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();

protected: // data

    /**
     * Array of cache objects for individual items.
     * Own.
     */
    RPointerArray<CMmCacheForItem> iItemCacheArr;
    
    /**
     * Array of template names used by @c GetTemplateIdentifierL and
     * @c GetTemplateNameByIdentifier methods. Template identifiers
     * that @c GetTemplateIdentifierL returns are in fact indices in
     * this array.
     * Own. Contents of the array owned too.
     */
    RPointerArray<HBufC8> iTemplateNames;

    };

#endif // MMITEMSDATACACHE_H
