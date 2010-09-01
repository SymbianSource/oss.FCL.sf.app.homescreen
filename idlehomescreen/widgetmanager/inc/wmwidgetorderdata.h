/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmWidgetOrderData declaration
*
*/


#ifndef WMWIDGETORDERDATA_H
#define WMWIDGETORDERDATA_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CWmPersistentWidgetOrder;


// CLASS DECLARATION
/**
 * CWmWidgetOrderData 
 */
NONSHARABLE_CLASS( CWmWidgetOrderData )
    : public CBase
    {

public: // constructions
    /**
     * @param aPublisherId 
     * @param aUid 
     * @param aUid 
     * @param aPersistentWidgetOrder
     */
    static CWmWidgetOrderData* NewL( 
            const TDesC& aPublisherId, 
            const TUid aUid, 
            const TDesC& aName,
            const CWmPersistentWidgetOrder* aPersistentWidgetOrder );

    static CWmWidgetOrderData* NewLC(
            const TDesC& aPublisherId, 
            const TUid aUid, 
            const TDesC& aName,
            const CWmPersistentWidgetOrder* aPersistentWidgetOrder );
        
    /** Destructor. */
    ~CWmWidgetOrderData();

private: // private construction
    /** Constructor for performing 1st stage construction */
    CWmWidgetOrderData( const TUid aUid );

    /** 2nd phase constructor */
    void ConstructL( 
            const TDesC& aPublisherId, 
            const TDesC& aName,
            const CWmPersistentWidgetOrder* aPersistentWidgetOrder );

public:

    /** Returns publisherId */
    const TDesC& PublisherId() const;
    
    /** Returns Uid */
    const TUid Uid() const;
    
    /** Returns Name */
    const TDesC& Name() const; 
            
    /** comparator function for sort by name (compare two names) */    
    static TInt CompareByName(
                        const CWmWidgetOrderData& aDataOne,
                        const CWmWidgetOrderData& aDataTwo );

    /** comparator function for sorting by stored order */
    static TInt CompareByPersistentWidgetOrder(
            const CWmWidgetOrderData& aDataOne,
            const CWmWidgetOrderData& aDataTwo );
    
    /** compare given values to member. Returns ETrue if matches*/
    TBool EqualsTo( const TUid aUid, const TDesC& aPublisherId );
    
    /** set persistentorder data*/
    void SetPersistentWidgetOrder( 
            CWmPersistentWidgetOrder* aPersistentWidgetOrder );
    
    /** Updates name if param aName is different than iName */
    void UpdateName( const TDesC& aName );
    
private: // data members
    /** uid */
    TUid iUid;
    
    /** publisherId */
    HBufC* iPublisherId;
    
    /** Name */
    HBufC* iName;
    
    /** persistent order of widgets (used in sorting) */
    const CWmPersistentWidgetOrder* iPersistentWidgetOrder;
    };

typedef RPointerArray<CWmWidgetOrderData> ROrderArray;

#endif // WMWIDGETORDERDATA_H
