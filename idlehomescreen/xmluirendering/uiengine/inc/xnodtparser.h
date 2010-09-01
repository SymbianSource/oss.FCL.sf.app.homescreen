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
* Description:  Parser for parsing ODT (Object Description Tree) data
*
*/


#ifndef _CXNODTPARSER_H
#define _CXNODTPARSER_H

//  System includes
#include <e32base.h>

// Constants

// Forward declarations
class CXnViewManager;
class CXnUiEngine;
class CXnEcomHandler;
class CXnRootData;
class CXnViewData;
class CXnPluginData;
class CXnDomStringPool;
class CXnDomNode;
class CXnNode;

// Class declaration

/**
 *  Layout file parser for reading ODT data to create the UI tree.
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnODTParser ) : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * 
     * @param aManager View Manager
     * @param aXnEcomHandler Ecom handler
     */
    static CXnODTParser* NewL( CXnViewManager& aManager,
            CXnEcomHandler& aXnEcomHandler );

    /**
     * Destructor.
     */
    virtual ~CXnODTParser();

public:
    // New functions

    /**
     * Loads application root
     * 
     * @param aRootData Root data
     * @param aAppUid Application uid
     */    
    void LoadRootL( CXnRootData& aRootData, TUid aAppUid );

    /**
     * Loads view
     * 
     * @param aViewData View data
     */        
    void LoadViewL( CXnViewData& aViewData );

    /**
     * Destroys view
     * 
     * @param aViewData View data
     */            
    void DestroyView( CXnViewData& aViewData );

    /**
     * Loads widget
     * 
     * @param aPluginData Plugin data
     */                
    void LoadWidgetL( CXnPluginData& aPluginData );

    /**
     * Destroys widget
     * 
     * @param aPluginData Plugin data
     */                    
    void DestroyWidgetL( CXnPluginData& aPluginData );
    
private:
    
    /**
     * 2nd phase constructor. 
     */
    void ConstructL();

    /**
     * C++ default constructor. 
     */
    CXnODTParser( CXnViewManager& aManager,
            CXnEcomHandler& aXnEcomHandler );

private:
    // new functions
    void CreateNodesL( CXnDomNode* aSourceNode, CXnDomStringPool& aSp,
                          CXnPluginData& aPluginData );

    void ConstructNodeL( CXnDomNode& aSource,        
        CXnDomStringPool& aSp,    
        CXnPluginData& aPluginData );

    void CreateControlsL( CXnDomNode* aNode, CXnPluginData& aPluginData );
    
    void ConstructControlL( CXnNode* aNode, CXnPluginData& aPluginData );

    TBool CreateBuiltInControlL( CXnNode& aNode, const TDesC8& aName );
    
    TBool CreateExternalControlL( CXnNode& aNode, const TDesC8& aName );

    void CreateFactoryControlL( CXnNode& aNode, const TDesC8& aName );

    void HandleWidgetBackgroundL( CXnNode* aWidgetNode );

private:
    // Data               
    
    /** View manager, not owned */    
    CXnViewManager& iManager;
    /** UiEngine, not owned */    
    CXnUiEngine& iUiEngine;
    /** ECOM helper object, not owned */
    CXnEcomHandler& iEcomHandler;
    };

#endif // _CXNODTPARSER_H
