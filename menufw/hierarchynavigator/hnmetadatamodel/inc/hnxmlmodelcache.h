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
*
*/


#ifndef HNXMLMODELCACHE_H_
#define HNXMLMODELCACHE_H_

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengdom.h>
#include <bautils.h>


/**
 * Suites cache.
 * 
 * This class is designed to support caching xml documents which contain
 * suites definitions.
 * 
 * The cache is directlty used by the xml model provider (CHnXmlModelProvider)
 * and is transparent for other data model's classes.
 * 
 * The cache has a maksimum capacity which is equal to KXmlModelCacheMaxLength.
 * The cache works similarily to a shift register. When the maksimum size of
 * the cache is exceeded then the last element in the cache is removed.
 * On contrary, most often used xml documents are moved to the beggining
 * of the cache.
 * 
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnXmlModelCache ) : public CBase
    {

private:
    
    /**
     * Default constructor.
     * 
     * @since S60 5.0
     */
    CHnXmlModelCache();
    
    /**
     * Standard second phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();
    
    /**
     * Removes a xml document from the cache if the size of the cache
     * is exceeded.
     * 
     * @since S60 5.0
     */
    void RemoveUnused();
    
    /**
     * Moves the xml document (identified by a given name) to the top of the
     * cache.
     * 
     * @param aSuiteName A name of the xml document to be moved to the top
     *                   of the cache.
     * @since S60 5.0
     */
    void MoveToTopL( const TDesC & aSuiteName );

public:
    
    /**
     * Two-phase constructor.
     * 
     * @since S60 5.0
     */
    static CHnXmlModelCache* NewLC();
    
    /**
     * Two-phase constructor.
     * 
     * 
     */
    static CHnXmlModelCache* NewL();
    
    /**
     * Adds a new xml document into the cache.
     * @param aSuiteName A name of the suite.
     * @param aDocument A xml document to be added to the cache.
     * 
     * @since S60 5.0
     */
    TInt AddL( const TDesC& aSuiteName, RXmlEngDocument& aDocument );
    
    /**
     * Checks if the xml document is stored in the cache.
     * 
     * @param aSuiteName The name of the suite to be checked.
     * @return ETrue if a suite of a given name is stored in the cache,
     *         otherwise - EFalse.
     *         
     * @since S60 5.0
     */
    TBool IsCachedL( const TDesC& aSuiteName );
    
    /**
     * Gets a xml document by a given name.
     * 
     * @param aSuiteName The name corresponding to the xml document to be
     *                   retrieved from the cache.
     * @param aDocument The reference to the xml document.
     * 
     * @since S60 5.0
     */
    void GetL( const TDesC& aSuiteName, RXmlEngDocument& aDocument );
    
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
    virtual ~CHnXmlModelCache();

private:
    
    /**
     * The hash map storing pairs: name of the suite, corresponding xml
     * document.
     * 
     * @since S60 5.0
     */
    RHashMap< HBufC*, RXmlEngDocument > iDocuments;
    
    /**
     * An auxilliary array storing the order of the xml document
     * in the cache.
     * 
     * @since S60 5.0
     */
    RPointerArray< HBufC > iOrder;
    
    };

#endif /*HNXMLMODELCACHE_H_*/
