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



#ifndef C_HN_ITEM_FILTER_H
#define C_HN_ITEM_FILTER_H


#include <e32base.h>
#include <s32file.h>
#include <s32mem.h>


/**
 * Item Id.
 * 
 * This class represtnets the id of the item.
 * It comprises a file name in which the item was defined
 * and the numerical identifier. 
 *  
 * @lib hnmetadatamodel
 * @since S60 v3.2
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnItemId) :
        public CBase
    {
public:
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aFile File name.
     * @param aID Item's numerical identifier.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnItemId* NewL( const TDesC& aFile,
        const TDesC& aID );
    
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aFile File name.
     * @param aID Item's numerical identifier.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnItemId* NewLC(const TDesC& aFile,
        const TDesC& aID );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aItemId Item id.
     * @return Fully constructed object.
     */
    IMPORT_C void UpdateL(const CHnItemId & aItemId );
    
    /**
     * Default destructor
     */
    ~CHnItemId();

    /**
     * Get suite name.
     * 
     * @return Suite's name.
     */
    IMPORT_C const TDesC& SuiteName() const;

    /**
     * Returns Id.
     *
     * @return Textual identifier.
     */
    IMPORT_C const TDesC& Id() const;

    /**
     * Compares fileName to filter.
     *
     * @param aItemId Item id.
     * @return True if aItemId is equal to current item id (this).
     */
    IMPORT_C TBool Compare( const CHnItemId& aItemId ) const;

private:

    /**
     * default constructor
     */
    CHnItemId();

    /**
     * EPOC default constructor for performing 2nd stage construction
     * 
    * @param aFile File name.
    * @param aID Textual identifier of an item.
     */
    void ConstructL( const TDesC& aFile, const TDesC& aID );

private: // data

    /**
     * Own - Filename.
     */
    RBuf iSuiteName;

    /**
     * Own - Id.
     */
    RBuf iId;

    };

#endif //C_HN_ITEM_FILTER_H
