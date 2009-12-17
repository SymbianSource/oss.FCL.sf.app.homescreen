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
* Defines an ordered list of widget id's with persistence capability
*
*/

#ifndef WMPERSISTENTWIDGETORDER_H
#define WMPERSISTENTWIDGETORDER_H

// INCLUDES
#include <e32base.h>
#include "wmwidgetdata.h"

// FORWARD DECLARATIONS
class RFs;

// CLASS DECLARATION
/**
 *  CWmPersistentWidgetOrder
 * This class is an ordered list of widget identifiers. The list can be
 * serialised and extract from stream and persisted. The list can be used
 * in sorting a pointer array of CWmWidgetData.
 */
NONSHARABLE_CLASS( CWmPersistentWidgetOrder ) : public CBase
    {

public: // Constructors and destructor
    /** 
     * static constructor.
     */
    static CWmPersistentWidgetOrder* NewL( RFs& aFs );

    /** Destructor. */
    ~CWmPersistentWidgetOrder();

private: // private construction

    /**
     * constructor
     */
    CWmPersistentWidgetOrder( RFs& aFs );

    /**
     * 2nd phase constructor
     */
    void ConstructL();


public: // API

    /**
     * Loads the persistent widget list from storage.
     * This is typically done once after construction.
     */
    void LoadL();
  
    /**
     * Stores a sorted array of WmWidgetData types as the persistent list.
     * The instance can now be used to sort widget data.
     * 
     * @param aArray the array of widget daya types
     */
    void StoreL( const RWidgetDataValues& aArray );

    /**
     * Retrieves an index of given widget data object within the list.
     * This index can be used to sort and organise.
     * 
     * @param aWidgetData data to seek index of in the sorted order
     * @return the index found, or KErrNotFound if widget is new
     */
    TInt IndexOf( const CWmWidgetData& aWidgetData ) const;

    /**
     * checks if the persistent widget order is empty. In this case it does
     * not make any sense to use the object to sort anything.
     * Note that before loading the object is always empty.
     */
    TBool IsEmpty() const;

protected:

    /**
     * Cleans up the array
     */
    void CleanupArray();

    /**
     * build the storage file path and name. The storage file will be located
     * in host process private directory.
     * @param aPathBuf a buffer to receive the complete file path
     */
    void GetStoreFileNameL( TDes& aPathBuf );

    /**
     * Unique tag of a widget
     */
    class Tag
        {
        public:
            /** constructs a tag from parameters */
            Tag( TInt32 aUid, HBufC16* aPublisherId );

            /** compares a tag agains a widget data */
            TBool Matches( const CWmWidgetData& aWidgetData ) const;

            /** widget uid */
            TInt32 iUid;

            /** widget publisher id */
            HBufC16* iPublisherId;
        };

private: // data members

    /** the file session */
    RFs& iFs;

    /** array of widget tags for sorting */
    RArray<Tag> iTagArray;

    };


#endif // WMPERSISTENTWIDGETORDER_H
