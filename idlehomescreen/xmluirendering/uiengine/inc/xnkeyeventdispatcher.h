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
* Description:  Key event dispatcher header
*
*/


#ifndef __XNKEYEVENTDISPATCHER_H__
#define __XNKEYEVENTDISPATCHER_H__

// System includes
#include <e32base.h>
#include <coecntrl.h>
#include <eikmobs.h>

// Uder includes
#include "xnviewmanager.h"
#include "xnuistatelistener.h"

// Forward declarations
class CXnNode;
class CXnUiEngine;
class CEikButtonGroupContainer;
class CXnViewData;
class CXnPluginData;

// Class declaration

/**
 *  Control adapter class implementation.
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnKeyEventDispatcher ) : public CCoeControl,   
    public MCoeMessageMonitorObserver,
    public MXnUiResourceChangeObserver,
    public MXnViewObserver
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnKeyEventDispatcher* NewL( CXnUiEngine& aUiEngine );

    /**
     * Destructor.
     */
    ~CXnKeyEventDispatcher();

private:
    // From CCoeControl

    /**
     * @see CCoeControl
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType );
            
private: 
    // from MCoeMessageMonitorObserver
    
    /**
    * @see MCoeMessageMonitorObserver 
    */
    void MonitorWsMessage( const TWsEvent& aEvent );
        
private:
    // from MXnUiResourceChangeObserver
    
    /**
     * @see MXnUiResourceChangeObserver
     */
    void NotifyStatusPaneSizeChanged();

    /**
     * @see MXnUiResourceChangeObserver
     */
    void NotifyResourceChanged( TInt aType );    
    
public:
    // New functions
   
    /**
     * Set the node to which key events are given
     * 
     * @since Series 60 3.1
     * @param aNode Node to set
     * @param aSource A source of event that triggered focus change.
     */
    void SetNodeL( CXnNode* aNode, TInt aSource = 0 );

    /**
     * Set the node to which key events are given, but do not notify.
     * 
     * @since Series 60 3.1
     * @param aNode Node to set
     */
    void SetNodeWithoutNotificationL( CXnNode* aNode );
    
    /**
     * Get the node to which holds focus
     * 
     * @since Series 60 3.1
     * @return Focused node, NULL if no focus.
     */
    CXnNode* FocusedNode() const;
    
    /**
     * Set the node to which menu events are given
     * 
     * @since Series 60 3.1
     * @param aNode Node to set
     */
    void SetMenuNodeL( CXnNode* aNode );

    /**
     * Removes the node to which menu events are given
     * 
     * @since Series 60 3.2        
     */
    void ResetMenuNodeL();

    /**
     * Refresh current menu
     * 
     * @since Series 60 3.1
     */
    void RefreshMenuL();

    /**
     * Checks whether the menu is focused.
     * 
     * @since S60 3.1
     * @return ETrue if menu is focused.
     */
    TBool IsMenuFocused() const;

    /**
     * Set node as passive, and add it to the passive focused node array.
     * 
     * @since Series 60 3.2
     * @param aNode Node to use.
     */
    void AddPassiveFocusedNodeL( CXnNode* aNode );

    /**
     * Unset the passive focus from the node, and remove it from the passive
     * focused node array.
     * 
     * @since Series 60 3.2
     * @param aNode Node to use.
     */
    void RemovePassiveFocusedNodeL( CXnNode* aNode );

    /**
     * Clear the passive focused node array, and unset the passive focus 
     * states.
     * 
     * @since Series 60 3.2
     */
    void ClearPassiveFocusedNodesL();

    /**
     * Clears dispatcher's internal state.
     * 
     * @since S60 5.2
     */
    void ClearStateL();
    
    /** 
     * Returns the last pointer event
     * 
     * @since S60 5.2
     * @return Last pointer event
     */
    const TPointerEvent& PointerEvent() const;
       
    /**
     * Sets activate text editor
     * 
     * @since S60 5.2
     * @param aNode Editor Node
     * @param TBool True if text editor should be activated
     */          
    void SetTextEditorActive( CXnNode* aNode, TBool aActivate );
    
    /**
     * Queries whether a text editor is active
     * 
     * @since S60 5.2
     * @return ETrue if text editor is active, EFalse otherwise
     */
    TBool IsTextEditorActive() const;
    
private:
    // new functions
    
    void SetNodeL( CXnNode* aToLose, CXnNode* aToGain, TBool aNotify,
        TInt aSource = 0 );
    
    void ResolveAndSetFocusL();       
    
protected:
    // New functions

    /**
     * C++ default constructor.
     */
    CXnKeyEventDispatcher( CXnUiEngine& aUiEngine );

    /**
     * Two-phased constructor.
     */
    void ConstructL();

private:
    // from MXnViewObserver
    
    void NotifyViewActivatedL( const CXnViewData& aViewData );
    void NotifyViewLoadedL( const CXnViewData& /*aViewData*/ ) {};

    void NotifyViewDeactivatedL( const CXnViewData& aViewData );

    void NotifyConfigureWidgetL( const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    void NotifyWidgetAdditionL( const CXnPluginData& aPluginData );
    void NotifyWidgetRemovalL( const CXnPluginData& aPluginData );
    void NotifyViewAdditionL( const CXnViewData& /*aViewData*/ ){};
    void NotifyViewRemovalL( const CXnViewData& /*aViewData*/ ){};
    void NotifyAllViewsLoadedL(){};
    void NotifyContainerActivatedL( const CXnViewData& /*aViewData*/ ){};

private:
    // Data

    /** Reference to uiengine */
    CXnUiEngine& iUiEngine;
    /** Losefocus trigger, owned */
    CXnNode* iLoseFocus;
    /** Gainfocus trigger, owned */
    CXnNode* iGainFocus;
    /** Node that receives key events */
    CXnNode* iNode;
    /** Previous key event receiver */
    CXnNode* iPreviousNode;
    /** Node that receives menu events */
    CXnNode* iMenuNode;
    /** Node that was focused when keyevent started */
    CXnNode* iKeyEventNode;
    /** Event code for keyevents */
    TEventCode iEventCode;
    /** Focused node is changed during keyevent process */
    TBool iFocusChanged;
    /** Command button array for softkeys, not owned */
    CEikButtonGroupContainer* iCbaContainer;
    /** For storing the nodes that have passive focus. */
    RPointerArray< CXnNode > iPassiveFocusedNodes;
    /** Last pointer event */
    TPointerEvent iPointerEvent;
    /** Active text editor node, not owned */
    CXnNode* iActiveTextEditor;
    };

#endif //__XNKEYEVENTDISPATCHER_H__
