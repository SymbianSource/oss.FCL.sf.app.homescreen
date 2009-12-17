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


#ifndef C_HNFILTER_H
#define C_HNFILTER_H


#include <e32base.h>

class CHnItemId;

/**
 * Evaluation Filter
 * 
 * This class is a filter which is used during evaluation.
 * Due to the content of this filter the appropriate actions are taken.  
 *
 * @lib hnmetadatamodel
 * @since S60 v3.2
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnFilter ) : public CBase
    {
public:
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @return Filer object.
     */
    IMPORT_C static CHnFilter* NewL();
    
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @return Filter object.
     */
    IMPORT_C static CHnFilter* NewLC();
    
    /**
     * standard destructor
     */
    ~CHnFilter();

public:    

    /**
     * Returns iGenre
     *
     * @since S60 v5.0
     * @return Suite's name.
     */
    const TDesC& SuiteName() const;
    
    /**
     * Checks if has item ids.
     *
     * @since S60 v5.0
     * @return True if a filter has item ids.
     */    
    TBool HasItemIds() const;

    /**
     * Checks if has genre
     *
     * @since S60 v5.0
     * @return True if suite has name.
     */    
    TBool HasSuiteName() const;
    
    /**
     * Resets iterator.
     *
     * @since S60 v5.0
     */    
    void ResetIterator();
    
    /**
     * Gets next item id.
     *
     * @since S60 v5.0
     * @return Next item id.
     */    
    const CHnItemId* GetNextItemId() const;
    
    /**
     * Checks if has next item id.
     *
     * @since S60 v5.0
     * @return True if there is next item id.
     */    
    TBool HasNextItemId();

    /**
     * Resets genre.
     *
     * @since S60 v5.0
     */    
    void ResetSuiteName();
    
    /**
     * Resets item ids
     *
     * @since S60 v5.0
     */    
    void ResetItemIds();
    
    /**
     * Set genre.
     *
     * @since S60 v5.0
     * @param aGenre Suite's name.
     */    
    IMPORT_C void SetSuiteNameL( const TDesC& aGenre );
    
    /**
     * Append item id. 
     * NOTE: This class takes the ownership of the given CHnItemId.
     *
     * @since S60 v5.0
     * @param iItemId Item id.
     */    
    IMPORT_C void AppendItemIdL( CHnItemId* iItemId );
    
    /**
     * Marks suites for evaluation.
     *
     * @since S60 v5.0
     * @param aEvaluate ValueL of the flag
     */    
    IMPORT_C void SetEvaluateSuiteL( TBool aEvaluate );
    
    /**
     * Checks if the suite(s) should be evalauted.
     *
     * @since S60 v5.0
     */    
    TBool IsEvaluateSuite();
    
    /**
     * Set suite id.
     *
     * @since S60 v5.0
     * @param aSuiteId Suite's id.
     */    
    IMPORT_C void SetSuiteId( const TInt aSuiteId );
    
    /**
     * Returns iSuiteId
     *
     * @since S60 v5.0
     * @return Suite's id.
     */
    TInt SuiteId() const;
    
    /**
     * Checks if has suite id
     *
     * @since S60 v5.0
     * @return True if suite has id.
     */ 
    TBool HasSuiteId() const;

private:

    /**
     * standard constructor
     */
    CHnFilter();
    
    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    

private: // data
        
    /**
     * Suite's genre/name.
     */
    RBuf iGenre;
    
    /**
     * Items' ids.
     */
    RPointerArray< CHnItemId > iItemIds;
    
    /**
     * Flag determines if the suites should also be evaluated.
     * If set to false, just items are evaluated.
     * By default it is set to true.
     */
    TBool iEvaluateSuite;
    
    /**
     * Current index of item id -> for iterator support
     */
    mutable TInt iIter;
    
    /**
     * Suite Id
     */
    TInt iSuiteId;
    
    };

#endif //C_HNFILTER_H
