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
* Description:  Data class to hold application configuration info
*
*/


#ifndef _XNROOTDATA_H
#define _XNROOTDATA_H

// System includes
#include <e32base.h>
#include <babitflags.h>

// User includes
#include "xnplugindata.h"

// Forward declarations
class CXnViewManager;
class CXnViewData;
class CXnEcomHandler;
class CXnODTParser;
class CXnODT;

/**
 * Holds root data in UiEngine
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnRootData ) : public CXnPluginData
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnRootData* NewL( CXnViewManager& aManager, TUid aApplicationUid );

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnRootData* NewLC( CXnViewManager& aManager, TUid aApplicationUid );

    /**
     * Destructor
     */
    ~CXnRootData();

protected:    
    /**
     * C++ constructor
     */
    CXnRootData( CXnViewManager& aManager, TUid aApplicationUid );

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();

public:
    // From CXnPluginData
    
    /**
     * @see CXnPluginData
     */
    TInt Load();

    /**
     * @see CXnPluginData
     */
    void Destroy();
    
public:
    // New functions
        
    /**
     * Loads remaining application views
     */    
    void LoadRemainingViewsL();

    /**
     * Cancels load remaining application views
     */        
    void CancelLoadRemainingViews();
    
    /**
     * Gets application active view data
     * 
     * @return Active view data
     */        
    CXnViewData& ActiveViewData() const;

    /**
     * Gets previous view data from currently active one
     * 
     * @return Previous view data
     */            
    CXnViewData& PreviousViewData() const;

    /**
     * Gets next view data from currently active one
     * 
     * @return Next view data
     */                
    CXnViewData& NextViewData() const;

    /**
     * Sets view data to destroyal list
     *
     * @param aViewData View data to destroy      
     */                    
    void DestroyViewData( CXnViewData* aViewData );
    
    /**
     * Gets ODT 
     * 
     * @return ODT
     */                
    CXnODT* ODT() const;
    
    /**
     * Gets root node from ODT
     * 
     * @return Root node
     */                
    CXnNode* RootNode() const;
              
    /**
     * Gets ODT Parser
     * 
     * @return ODT Parser
     */                
    CXnODTParser& Parser() const;
    
    /**
     * Gets Ecom handler
     * 
     * @return Ecom handler
     */                
    CXnEcomHandler& EcomHandler() const;
    
    /**
     * Sets maximum pages allowed
     * 
     * @param aPages maximum pages 
     */
    void SetMaxPages( TInt32 aPages );
    
    /**
     * Get maximum pages allowed
     * 
     * @return number of pages allowed
     */
    TInt32 MaxPages();
    
    /**
     * Returns template view's uid
     * 
     * @return Returns template view's uid, or if not set qhd default
     */
    const TDesC8& TemplateViewUid() const;
    
    /**
     * Sets template view's uid
     * 
     * @param Template view's uid
     */
    void SetTemplateViewUidL( const TDesC8& aTemplateViewUid );
    
    /**
     * Returns ETrue, if all views are loaded
     */
    TBool AllViewsLoaded() const;

    /**
     * Returns ETrue, if all views are destoyed
     */
    TBool AllViewsDestroyed() const;

private:
    // New functions
    static TInt RunLoadL( TAny* aAny );
    static TInt RunDestroyL( TAny *aAny );
               
private:
    // data

    /** Timer for loading views, Owned */    
    CPeriodic* iLoadTimer;
    /** Timer for deleting views, Owned */
    CPeriodic* iDestroyTimer;
    /** Application Uid */
    TUid iApplicationUid;
    /** ECom handler, Owned */
    CXnEcomHandler* iEcomHandler;    
    /** ODT Parser, Owned */
    CXnODTParser* iParser;
    /** ODT which describes application configuration, Owned */ 
    CXnODT* iODT;
    /** Array of views waiting destruction */
    RPointerArray< CXnViewData > iViewsToDestroy;    
    /** Flag to determine remaining view load direction */
    TBool iLoadForward;
    /** Maximum of views allowed */
    TInt32 iMaxPages;
    /** Template view uid is uid of the view when user selects New view */
    HBufC8* iTemplateViewUid;
    /** Load error code */
    TInt iLoadError;
    };

#endif // _XNROOTDATA_H
