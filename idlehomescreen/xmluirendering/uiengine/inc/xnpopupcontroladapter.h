/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Popup control adapter
*
*/

#ifndef __XNPOPUPCONTROLADAPTER_H__
#define __XNPOPUPCONTROLADAPTER_H__

// System includes
#include <e32base.h>
#include <eikmobs.h>

// User includes
#include "xncontroladapter.h"
#include "xnmenu.h"

// Forward declarations
class CXnNodePluginIf;
class CXnNode;
class CXnUiEngine;
class CAknStylusPopUpMenu;

// Class declaration

/**
 *  Popup Control adapter interface class
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xnlayoutengine.lib
 *  @since Series 60 5.0
 */
NONSHARABLE_CLASS( CXnPopupControlAdapter ) : public CXnControlAdapter,
    public MEikMenuObserver
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     *
     * @param aNode Node
     */
    static CXnPopupControlAdapter* NewL( CXnNodePluginIf& aNode );

    /**
     * Destructor.
     */
    ~CXnPopupControlAdapter();

private:
    // Constructors
    
    /**
     * C++ default constructor.
     */
    CXnPopupControlAdapter();

    /**
     * Two-phased constructor.        
     */
    void ConstructL( CXnNodePluginIf& aNode  );

private:    
    // From MEikMenuObserver
    void ProcessCommandL( TInt aCommandId );
    void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );
    
public:
    // New functions
    
    /*
     * Sets dynamic menuitem observer
     * 
     * @param aObserver Observer
     */
    void SetObserver( XnMenuInterface::MXnMenuObserver& aObserver );
    
    /*
     * Makes stylus popup menu visible
     * 
     * @param aPlugin Plugin which displays this popup
     */
    void TryDisplayingStylusPopupL( CXnNode& aPlugin );
    
    /*
     * Screen device change notification
     * 
     * @see CXnControlAdapter documentation
     */
    void HandleScreenDeviceChangedL();
    
private:
    // New functions
    void PopulateMenuL( CXnNode* aItem, CXnNode& aPlugin );                                  
    void ShowMenuL( CXnNode& aPlugin, TPoint aPosition );
    void HideMenuL();
                           
private:
    // Data
    
    /** UiEngine, Not owned */
    CXnUiEngine*                        iUiEngine;
    /** Node - received in constructor, Not owned  */
    CXnNode*                            iNode;
    /** Observer, Not owned */
    XnMenuInterface::MXnMenuObserver*   iObserver;
    /** Stylus popup menu, Owned */
    CAknStylusPopUpMenu*                iStylusPopupMenu;    
    /** Stylus popup menu items, Owned */
    RPointerArray< CXnNode >            iMenuItems;
    /** Stylus popup menu command ids */
    TInt                                iCommandId;    
    /** Flag to indicate whether popup menu is shown */
    TBool                               iMenuShown;
    };

#endif // __XNPOPUPCONTROLADAPTER_H__

