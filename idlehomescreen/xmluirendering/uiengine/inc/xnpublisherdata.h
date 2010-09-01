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
* Description:  Publisher data
*
*/


#ifndef _XNPUBLISHERDATA_H
#define _XNPUBLISHERDATA_H

// System includes
#include <e32base.h>
#include <babitflags.h>

// User includes
#include <hspublisherinfo.h>

// Forward declarations
class CXnAppUiAdapter;
class CXnPluginData;
class CXnNode;

/**
 * Publisher data
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CXnPublisherData ) : public CBase
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnPublisherData* NewL( CXnPluginData& aOwner, CXnNode& aNode );        

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnPublisherData* NewLC( CXnPluginData& aOwner, CXnNode& aNode );         

    /**
     * Destructor
     */
    ~CXnPublisherData();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnPublisherData( CXnPluginData& aOwner, CXnNode& aNode );         
    
public: 
    // new functions
    
    /**
     * Equals operator
     *
     * @since S60 5.2 
     * @param aNode Node
     * @return ETrue if this equals to aNode, EFalse otherwise     
     */
    TBool operator==( CXnNode& aNode ) const;

    /**
     * Equals operator
     *
     * @since S60 5.2 
     * @param aInfo Publisher info
     * @return ETrue if this equals to aInfo, EFalse otherwise     
     */    
    TBool operator==( const THsPublisherInfo& aInfo ) const;
    
    /**
     * Returns publisher info
     * 
     * @since S60 5.2
     * @return Publisher info
     */
    const THsPublisherInfo& Info() const;
    
    /** 
     * Returns the <contentsource> element
     * 
     * @since S60 5.2
     * @return Node defining <contentsource> element
     */
    CXnNode* ContentSource() const;

    /**
     * Schedules publisher load with given reason
     * 
     * @since S60 5.2
     * @param aReason Load reason
     */    
    void Load( TInt aReason );
    
    /**
     * Schedules publisher destroy with given reason
     * 
     * @since S60 5.2
     * @param aReason Destroy reason
     */
    void Destroy( TInt aReason );
    
    /**
     * Queries wheter this publisher loading is failed
     * 
     * @since S60 5.2
     * @return ETrue if failed, EFalse otherwise
     */
    TBool IsFailed() const;
    
    /**
     * Queries wheter this publisher loading is ongoing
     * 
     * @since S60 5.2
     * @return ETrue if ongoing, EFalse otherwise
     */    
    TBool IsLoading() const;

    /**
     * Gets publisher owner
     * 
     * @since S60 5.2
     * @return Owner
     */        
    CXnPluginData* Owner() const;
    
private:
    // new functions
    
    static void NotifyLoaded( TAny* aAny, TInt aResult );        
    static void NotifyDestroyed( TAny* aAny, TInt aResult );
        
private:
    // data
    
    /** CXnPluginData, Not owned */
    CXnPluginData& iOwner;
    /** <contentsource> element, Not owned */
    CXnNode& iNode;
    /** AppUiAdapter, Not owned */
    CXnAppUiAdapter* iAppUi;
    /** Publisher info built from <contentsource> element */
    mutable THsPublisherInfo iInfo;
    /** Flags to define this plugin's state */
    TBitFlags32 iFlags;    
    };

#endif // _XNPUBLISHERDATA_H

// End of file
