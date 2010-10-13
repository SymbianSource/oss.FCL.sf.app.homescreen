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
* Description:  Layout calculation and UI rendering mechanism implementations.
*
*/


#ifndef _CXNUIENGINEIMPL_H
#define _CXNUIENGINEIMPL_H

// System includes

// User includes
#include "xnuistatelistener.h"
#include "xnviewmanager.h"
#include "xnmenu.h"

// Forward declarations
class CCoeControl;
class CXnControlAdapter;
class CXnNode;
class CXnPointerArray;
class CXnODT;
class CGraphicsDevice;
class CXnUiEngine;
class CXnKeyEventDispatcher;
class CXnEditor;
class CXnResource;
class CXnEditMode;
class CXnViewManager;
class CXnViewData;
class CXnPluginData;

// Constants
namespace XnLayoutPhase
    {
    const TInt ENone = 0x00;
    const TInt ELayout = 0x01;
    const TInt EMeasure = 0x02;
    }

namespace XnLayoutControl
    {
    const TInt ELayoutUI = 0x01;
    const TInt ERenderUI = 0x02;
    const TInt ERefreshMenu = 0x04;
    const TInt EIgnoreState = 0x08;
    const TInt EViewDirty = 0x10;
    const TInt EEffectStarted = 0x20;    
    }
	
NONSHARABLE_STRUCT( TXnDirtyRegion )
    {
    RRegion         iRegion;      
    CCoeControl*    iControl; // Not own.
    
    ~TXnDirtyRegion()
        {
        iRegion.Close();
        }
    };


NONSHARABLE_STRUCT( TXnSplitScreenState )
    {
    /** Partial screen editor node. Not own */
    CXnNode* iPartialScreenEditorNode;
    /** Is partial screen input open */
    TBool iPartialScreenOpen;
    /** Partial screen block progression. Own. */
    const TDesC8* iPartialScreenBlock;
    
    // ctor
    TXnSplitScreenState() : 
        iPartialScreenEditorNode( NULL ),
        iPartialScreenOpen( EFalse ),
        iPartialScreenBlock( NULL ) {}
    };


// Class declaration

/**
 *  Layout calculation and UI rendering mechanism
 *  implementations.
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnUiEngineImpl ) : public CBase,     
    public MXnViewObserver, 
    public MXnUiResourceChangeObserver,
    public XnMenuInterface::MXnMenuObserver
    {
    
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * 
     * @param aUiEngine Ui engine
     * @param aViewManager View manager
     */
    static CXnUiEngineImpl* NewL( CXnUiEngine& aUiEngine,
        CXnAppUiAdapter& aAdapter );

    /**
     * 2nd phase constructor 
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CXnUiEngineImpl();

public:
    // New functions

    /**
     * Gets AppUi
     *
     * @since S60 5.0     
     * @return AppUi
     */    
    CXnAppUiAdapter& AppUiAdapter() const;
    
    /**
     * Lays out the UI
     * 
     * @since Series 60 3.1
     * @param aNode Starting point of the layout
     */
    void LayoutUIL( CXnNode* aNode = NULL );

    /**
     * Creates UI implementations for the UI, adjusts control coordinates 
     * according to the layout
     * 
     * @since Series 60 3.1
     * @param aNode Starting point
     */
    void RenderUIL( CXnNode* aNode = NULL );
    
    /**
     * Gets the root node of the UI
     * 
     * @since Series 60 3.1
     * @return Root node
     */
    CXnNode* RootNode();
    
    /**
     * Gets the ODT
     * 
     * @since Series 60 3.1
     * @return ODT
     */
    CXnODT* ODT();
    
    /**
     * Finds a node by id
     * 
     * @since Series 60 3.1
     * @param aNode Node id
     * @param aNamespace Namespace
     * @return Node with given id
     */
    CXnNode* FindNodeByIdL( const TDesC& aNodeId,
        const TDesC& aNamespace = KNullDesC );
    /**
     * Finds a node by id, b-bit version
     * 
     * @since Series 60 3.1
     * @param aNode Node id
     * @param aNamespace Namespace
     * @return Node with given id
     */
    CXnNode* FindNodeByIdL( const TDesC8& aNodeId, 
        const TDesC8& aNamespace = KNullDesC8 );
    /**
     * Finds nodes by class
     * 
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes with given class id
     */
    CXnPointerArray* FindNodeByClassL( const TDesC& aClassId,
        const TDesC& aNamespace = KNullDesC );
    
    /**
     * Finds nodes by class
     * 
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes with given class id
     */
    CXnPointerArray* FindNodeByClassL( const TDesC8& aClassId,
        const TDesC8& aNamespace = KNullDesC8 );
    
    /**
     * Find content source nodes from namespace. Ownership is not transferred.
     *
     * @since S60 5.2
     * @param aNamespace Namespace
     * @return Content source nodes from namespace
     */    
    CXnPointerArray* CXnUiEngineImpl::FindContentSourceNodesL(
        const TDesC8& aNamespace );
    
    /**
     * Gets resources of the UI
     * 
     * @since Series 60 3.1
     * @return List of resources
     */
    CArrayPtrSeg< CXnResource >& Resources();
    
    /**
     * Sets currently focused node
     * 
     * @since Series 60 3.1
     * @param aFocusedNode Currently focused node
     * @param aSource A source of event that triggered focus change.
     */
    void SetFocusedNodeL( CXnNode* aFocusedNode, TInt aSource = 0 );
    
    /**
     * Get currently focused node
     * 
     * @since Series 60 3.1
     * @return Focused node.
     */
    CXnNode* FocusedNode();

    /**
     * Checks if the UI is in Edit mode.
     * 
     * @since Series 60 5.0        
     * @return ETrue if the current mode is edit, otherwise EFalse.
     */
    TBool IsEditMode();
           
    /**
     * Get a numeric value of a property in vertical pixels
     * 
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in vertical pixels
     */
    TInt VerticalPixelValueL( CXnProperty* aValue, TInt aReferenceValue );
    
    /**
     * Get a numeric value of a property in horizontal pixels
     * 
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in horizontal pixels
     */
    TInt HorizontalPixelValueL( CXnProperty* aValue, TInt aReferenceValue );
    
    /**
     * Get a numeric value of a property in vertical twips
     * 
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in vertical twips
     */
    TInt VerticalTwipValueL( CXnProperty* aValue, TInt aReferenceValue );
    
    /**
     * Get a numeric value of a property in horizontal twips
     * 
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in horizontal pixels
     */
    TInt HorizontalTwipValueL( CXnProperty* aValue, TInt aReferenceValue );

    /**
     * Refresh current menu
     *
     * @since Series 60 3.1
     */
    void RefreshMenuL();

    /**
     * Returns view manager
     *
     * @since S60 5.0
     * @return view manager
     */
    CXnViewManager* ViewManager();

    /**
     * Returns the active view
     
     * @since Series 60 3.1
     * @return Active view node
     */
    CXnNode* ActiveView();

    /**
     * Add a dirty node. LayoutUiL and RenderUiL will only perform operations
     * on the set of dirty nodes.
     *
     * @since Series 60 3.1
     * @param aNode Dirty node.
     * @param aLayoutPhaseHint, a hint where to start next layout phase
     */
    void AddDirtyNodeL( CXnNode* aNode, TInt aLevel );

    /**
     * Get the size of the current screen device
     *
     * @since Series 60 3.1
     * @return Size of the current screen device
     */
    TSize ScreenDeviceSize() const;

    /**
     * Checks whether the dialog is displaying.
     *
     * @since Series 60 3.1
     * @return ETrue if dialog is displaying.
     */
    TBool IsDialogDisplaying();

    /**
     * Handles a change to the control's resources of type aType
     *
     * @since Series 60 3.1
     * @param aType Changed resource type
     */
    void HandleResourceChangeL( TInt aType );

    /**
     * Checks whether the menu is displaying.
     * 
     * @since Series 60 3.1
     * @return ETrue if menu is displaying.
     */
    TBool IsMenuDisplaying();

    /**
     * Returns menubar node
     * 
     * @since Series 60 3.2
     * @return Menubar node
     */
    CXnNode* MenuBarNode() const;

    /**
     * Returns stylus popup node
     * 
     * @since S60 60 5.0
     * @return stylus popup node
     */    
    CXnNode* StylusPopupNode() const;
    
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
     * Sets node's dropped state
     * 
     * @since Series 60 3.2
     * @param aNode Node
     * @param aDropped Dropped
     */
    void SetNodeDroppedL( CXnNode& aNode, TInt aDropped ) const;

    /**
     * Checks if node is adaptive, but doesn't have any content
     * 
     * @since Series 60 3.2
     * @param aNode node
     * @param aAvailableSize size to be checked
     */
    void CheckAdaptiveContentL( CXnNode& aNode, TSize aAvailableSize ) const;

    /**
     * Adds a node which can be focused
     * 
     * @since Series 60 3.2
     * @param aNode Node
     */
    void AddFocusCandidateL( CXnNode* aNode );

    /**
     * Sets client rect
     * 
     * @since Series 60 3.2
     * @param aRect area to be set
     * @param aDrawNow Call rendering routines if ETrue
     */
    void SetClientRectL( TRect aRect, TBool aDrawNow = ETrue );
    
    /**
     * Fetch editor to operate on AI3 widgets.
     * 
     * @since Series 60 5.0
     * @return Pointer to Editor
     */
    CXnEditor* Editor() const;
    
    /**
     * Gets client rect
     * 
     * @since Series 60 3.2
     * @return client rect
     */
    TRect ClientRect() const;

    /**
     * Gets pointer to edit mode storage.
     * 
     * @since Series 60 5.0
     * @return edit mode storage
     */
    CXnEditMode* EditMode();

    /**
     * Get current view plugins nodes.
     * 
     * @since Series 60 5.0
     * @return Pointer to plugin node array
     */
    RPointerArray< CXnNode >* Plugins();

    /**
     * Gets theme resource file
     * 
     * @since S60 5.1         
     * @param aPath path from which file is found
     * @param aFile file to which resource is set
     * @return errorcode
     */
    TInt GetThemeResource( const TDesC& aPath, RFile& aFile );

    /**
     * Analyse added widget
     * 
     * @since S60 5.1
     * @param aNode Widget to be analyzed
     * @return ETrue if fits, EFalse otherwise
     */
    TBool AnalyseAddedWidgetL( CXnNode& aNode );
        
    /**
     * Disables layoyt and redraw, places cleanup item to cleanup stack
     * for enabling layot and redraw
     * 
     * @since Series 60 5.0     
     */                        
    void DisableRenderUiLC();
    
    /**
     * Set event dispatcher
     * 
     * @since Series 60 5.0
     * @param aDispather Event dispatcher     
     */                    
    void SetEventDispatcher( CXnKeyEventDispatcher* aDispatcher );

    /**
     * Enables partial touch input
     * 
     * @since Series 60 5.2
     * @param aNode Editor Node
     * @param TBool Partial input is enabled      
     */      
    void EnablePartialTouchInput( CXnNode& aNode, TBool aEnable );

    /**
     * Is partial input active
     * 
     * @since Series 60 5.2
     * @return TBool is partial input active      
     */      
    TBool IsPartialInputActive();

    /**
      * Checks if text editor is focused or partioal touch input open.
      * 
      * @since Series 60 5.2
      * @return TBool True if partial input is open or editor focused
      */      
    TBool IsTextEditorActive();

private:
    
    IMPORT_C static void EnableRenderUi( TAny* aAny );
        
private:
    // constructors

    /**
     * C++ default constructor.
     */
    CXnUiEngineImpl( CXnUiEngine& aUiEngine, CXnAppUiAdapter& aAdapter );

private:
    // new functions
    
    /**
     * Prepares to the layout algorithm run
     * 
     * @since Series 60 3.2
     */
    void PrepareRunLayoutL();

    /**
     * Runs the layout algorithm        
     * 
     * @since Series 60 3.2
     */
    TInt RunLayoutL( CXnNode* aNode );

    /**
     * Checks if layout is currently disabled
     * 
     * @since Series 60 3.2
     */
    TBool IsLayoutDisabled();

    void AddToRedrawListL( CXnNode* aNode, TRect aRect =
        TRect::EUninitialized );

    void AddToDirtyListL( CXnNode* aNode );

    /**
     * Finds a node where to start layout
     *
     * @since Series 60 5.0
     */
    CXnNode* StartLayoutFromL();

    void ForceRenderUIL( TBool aLayoutOnly = EFalse );

    CCoeControl* WindowOwningControl( CXnNode& aNode );
    
    TXnDirtyRegion* FindDirtyRegionL( CXnNode& aNode );

    void AddRedrawRectL( TRect aRect, CXnNode& aNode );

    void ReportScreenDeviceChangeL();
    
    /**
      * Handle partial touch input
      * 
      * @since Series 60 5.2
      * @param TInt aType 
      */ 
    void HandlePartialTouchInputL( CXnNode& aNode, TBool aEnable );

    /**
      * Set node visible
      * 
      * @since Series 60 5.2
      * @param aNode node to hide/show
      * @param TBool aVisible boolean to set node visible      
      */      
    void SetNodeVisibleL( CXnNode* aNode, TBool aVisible );

    /**
      * Set partial screen block
      * 
      * @since Series 60 5.2
      * @param CXnNode aParent parent node 
      * @param TDesC8 aBlockProgression set layout direction
      */     
    void SetPartialScreenBlockProgressionL( 
            CXnNode* aParent, const TDesC8& aBlockProgression );
   
    /**
      * Handles skin change resource change
      * 
      * @since Series 60 5.2
      */
    void HandleSkinChangeL();

    /**
      * Handles KEikDynamicLayoutVariantSwitch resource change
      * 
      * @since Series 60 5.2
      */
    void HandleDynamicLayoutVariantSwitchL();
	
private:
    //Derived functions
    
    /** 
     * from MXnViewObserver 
     */
    void NotifyViewActivatedL( const CXnViewData& aViewData );

    /** 
     * from MXnViewObserver
     */
    void NotifyViewDeactivatedL( const CXnViewData& /*aViewData*/ );

    /**
     * from MXnViewObserver 
     */
    void NotifyConfigureWidgetL( const CHsContentInfo& /*aContentInfo*/,
        CXnPluginData& /*aPluginData*/ );
    
    /** 
     * from MXnViewObserver 
     */
    void NotifyWidgetAdditionL( const CXnPluginData& aPluginData );

    /** 
     * from MXnViewObserver
     */
    void NotifyWidgetRemovalL( const CXnPluginData& /*aPluginData*/ );

    /** 
     * from MXnViewObserver 
     */
    void NotifyViewAdditionL( const CXnPluginData& /*aPluginData*/ ){};

    /** 
     * from MXnViewObserver
     */
    void NotifyViewRemovalL( const CXnPluginData& /*aPluginData*/ ){};

    /** 
     * from MXnViewObserver
     */
    void NotifyAllViewsLoadedL(){};

    /** 
     * from MXnViewObserver
     */
    void NotifyContainerActivatedL( const CXnViewData& /*aViewData*/ ){};

private:
    // from MXnMenuObserver

    TBool DynInitMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aPlugin = NULL );

private:
    // from MXnPropertyChangeObserver

    /**
     * Property changed
     * 
     * @since S60 5.1         
     * @param aKey key value
     * @param aValue property value         
     */
    void PropertyChangedL( const TUint32 aKey, const TInt aValue );

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
    
private:
    // Data               
    
    /** Current screen device, Owned */
    CGraphicsDevice* iCurrentGraphicsDevice;
    /** UI engine - received from constructor's input parameters, Not owned */
    CXnUiEngine* iUiEngine;
    /** AppUi, Not owned */
    CXnAppUiAdapter& iAppUiAdapter;    
    /** Not owned. View manager */
    CXnViewManager& iViewManager;
    /** Key event dispatcher, Not owned */
    CXnKeyEventDispatcher* iKeyEventDispatcher;
    /** ControlAdapters, not owned */
    const RPointerArray< CXnControlAdapter >* iControlAdapterList;
    /** Region pending redraw */
    RPointerArray<TXnDirtyRegion> iRedrawRegions;
    /** List of currently dirty nodes */
    RPointerArray< CXnNode > iDirtyList;
    /** Array of nodes which can be focused */
    RPointerArray< CXnNode > iFocusCandidateList;
    /** current view */
    CXnNode* iCurrentView;
    /** current view control adapter, not owned */
    CXnControlAdapter* iCurrentViewControlAdapter;
    /** Controls layouting */
    TInt iLayoutControl;
    /** Layout algo phase */
    TInt iLayoutPhase;
    /** Unit in pixels (width). */
    TReal iHorizontalUnitInPixels;
    /** Unit in pixels (height). */
    TReal iVerticalUnitInPixels;
    /** Cached menu node	*/	
    CXnNode* iMenuNode;
    /** Cached styluspopup node */
    CXnNode* iStylusPopupNode;
    /** Drawing area */
    TRect iClientRect;
    /** Owned. Edit mode storage; */
    CXnEditMode* iEditMode;
    /** Disable count */
    TInt iDisableCount;   
	/**Split screen states*/
    TXnSplitScreenState iSplitScreenState;
    };

#endif      // _CXNUIENGINEIMPL_H
