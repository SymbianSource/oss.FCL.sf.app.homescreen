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
* Description:  Composer header
*
*/


#ifndef __XNCOMPOSER_H__
#define __XNCOMPOSER_H__

// System includes
#include <e32base.h>
#include <f32file.h>

// User includes
#include "hspswrapper.h"
#include "hspssapi.h"

// Forward declarations
class CXnODT;
class CXnDomNode;
class CXnViewManager;
class CXnRootData;
class CXnViewData;
class CXnPluginData;

using namespace hspswrapper;

/**
 *  This class loads views and plugins defined in HSPS, to the DOM tree.
 *  The feature of this class is to load views',and plugin's data from 
 *  storage.
 *  
 *  The class supports multiple views. It is mandatory to call one of methods,
 *  described below when a plugin/view is added to the Homscreen,
 *  or the Homescreen is being constructed.
 *  
 *  As the first ComposeInitialViewL function should be run. 
 *  It loads the first view and its plugins(via ComposeViewL and 
 *  ComposePluginL). ComposeViewL should be called when a view, and its 
 *  plugins, is composed. ComposePluginL should be called always when a plugin
 *  is added to the view. 
 *
 *  @ingroup group_xnlayoutengine
 *  Composer Implementation
 *  @lib xn3layoutengine.lib
 *  @since Series 60 5.0
 */
NONSHARABLE_CLASS( CXnComposer ) : public CBase
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     *
     * @param aUiEngineImpl Provides plugin resources store
     * @return Property list object.
     */
    static CXnComposer* NewL( CHspsWrapper& aWrapper );

    /**
     * Destructor.
     */
    ~CXnComposer();

protected:
    
    /**
     * C++ default constructor.
     */
    CXnComposer( CHspsWrapper& aWrapper );

    /**
     * Two-phased constructor.
     *
     * @param aComparator Propety comparator.
     */
    void ConstructL();

public:
    // new functions

    /**
     * Composes application root configuration.
     * 
     * @param aRootData Application root data.
     * @return ODT tree, ownership is granted to caller. NULL if failed.
     */
    CXnODT* ComposeRootL( CXnRootData& aRootData );
    
    /**
     * Composes a view.     
     * 
     * @param aViewData View data for the composed view.
     * @return KErrNone if succesful, error otherwise         
     */
    TInt ComposeViewL( CXnViewData& aViewData );

    /**
     * Composes a widget.
     *     
     * @param aPluginData Plugin data for the composed widget.
     * @return KErrNone if succesful, error otherwise     
     */
    TInt ComposeWidgetL( CXnPluginData& aPluginData );
    
private:
    // new functions     

    CXnDomNode* GetOdtL( const CObjectMap& aObject,              
                         CXnPluginData& aPluginData );                
                         
    /**
     * Finds an object from the provided array.
     *     
     * @param aResourceObjects An array of objects.
     * @param aTag A tag of an object to be found.
     * @return Object instance if succesful, NULL otherwise
     */
    const CObjectMap* FindObject( 
            RPointerArray<CObjectMap>& aResourceObjects,
            const TDesC8& aTag ) const; 
                         
private:
    // Data        
    
    /** 
     * HSPS Wrapper, not owned  
     */
    CHspsWrapper& iWrapper;
    
    /*
     * ODT, not owned
     */    
    CXnODT* iODT;
    
    /** 
     * Handle to file server session, owned 
     */
    RFs iFs;
    };

#endif // __XNCOMPOSER_H__
