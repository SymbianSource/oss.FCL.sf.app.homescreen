/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*  Version     : %version: 3 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef HNXMLMODELCACHE_H_
#define HNXMLMODELCACHE_H_

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengdom.h>
#include <bautils.h>


/**
 * Bitmaps and masks ids cache.
 * 
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnBitmapIdCache ) : public CBase
    {

private:
    
    /**
     * Default constructor.
     * 
     * @since S60 5.0
     */
    CHnBitmapIdCache();
    
    /**
     * Standard second phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();

public:
    
    /**
     * Two-phase constructor.
     * 
     * @since S60 5.0
     */
    static CHnBitmapIdCache* NewLC();
    
    /**
     * Two-phase constructor.
     * 
     * 
     */
    static CHnBitmapIdCache* NewL();
    
    /**
     * Adds a new entry into the cache.
     * @param aKey A key identifying a pair: key - value.
     * @param aValue A value.
     * 
     * @since S60 5.0
     */
    TInt AddL( const TDesC8& aKey, TInt aValue );
    
    /**
     * Checks if the cache contains an entry identifies by a given key.
     * 
     * @param aSuiteName The name of the suite to be checked.
     * @return ETrue if a suite of a given name is stored in the cache,
     *         otherwise - EFalse.
     *         
     * @since S60 5.0
     */
    TBool ExistsL( const TDesC8& aKey );
    
    /**
     * Gets an integer value by a given name.
     * 
     * @param aSuiteName The name corresponding to the xml document to be
     *                   retrieved from the cache.
     * @param aDocument The reference to the xml document.
     * 
     * @since S60 5.0
     */
    void GetL( const TDesC8& aKey, TInt& aValue );
    
    /**
     * Resets the cache - removes all stored xml documents.
     * 
     * @since S60 5.0
     */
    void Reset();
    
    /**
     * Standard C++ virtual destructor.
     * 
     * @since S60 5.0
     */
    virtual ~CHnBitmapIdCache();

private:
    
    /**
     * The hash map storing pairs: and an integer value.
     * 
     * @since S60 5.0
     */
    RHashMap< HBufC8*, TInt > iEntries;
    
    };

#endif /*HNXMLMODELCACHE_H_*/
