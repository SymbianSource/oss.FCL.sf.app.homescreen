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
* Description:
* Xuikon UI engine.
*
*/

#ifndef XNUIENGINE_H
#define XNUIENGINE_H

// System includes
#include <e32base.h>
#include <w32std.h>

// Forward declarations
class CXnAppUiAdapter;
class CXnViewManager;
class CXnUiEngineImpl;
class TXnUiEngineAppIf;
class TXnUiEnginePluginIf;
class CCoeControl;
class CXnProperty;
class CXnNode;
class CXnResource;
class CXnODT;
class CXnPointerArray;
class CXnEditor;
class CXnEditMode;
class CXnHitTest;
class CXnKeyEventDispatcher;

// Class declaration
/**
 * Xuikon UI engine.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnUiEngine ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CXnUiEngine* NewL( CXnAppUiAdapter& aAdapter );

    /**
     * Destructor.
     */
    virtual ~CXnUiEngine();

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
     * Calculates the layout of the UI and renders it.
     *
     * @since Series 60 3.1
     * @param aNode Starting point for the layout
     * @exception KXnErrLoadLayoutFileFailed Loading the layout file failed.
     */
    void RenderUIL( CXnNode* aNode = NULL );

    /**
     * Calculates the layout of the UI.
     *
     * @since Series 60 3.1
     * @param aNode Starting point for the layout
     */
    void LayoutUIL( CXnNode* aNode = NULL );

    /**
     * Gets the root node of the UI. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Node
     */
    CXnNode* RootNode();

    /**
     * Find a node by its id. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aAreaId Area id
     * @param aNamespace Namespace
     * @return Node with the given id
     */
    CXnNode* FindNodeByIdL(
        const TDesC& aAreaId,
        const TDesC& aNamespace = KNullDesC );

    /**
     * 8-bit version of Find a node by its id. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aAreaId Area id
     * @param aNamespace Namespace
     * @return Node with the given id
     */
    CXnNode* FindNodeByIdL(
        const TDesC8& aAreaId,
        const TDesC8& aNamespace = KNullDesC8 );

    /**
     * Find nodes by class. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes belonging to the class
     */
    CXnPointerArray* FindNodeByClassL(
        const TDesC& aClassId,
        const TDesC& aNamespace = KNullDesC );

    /**
     * Find nodes by class. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes belonging to the class
     */
    CXnPointerArray* FindNodeByClassL(
        const TDesC8& aClassId,
        const TDesC8& aNamespace = KNullDesC8 );

    /**
     * Find resources (images etc.) of the UI. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Resources of the UI
     */
    CArrayPtrSeg< CXnResource >& Resources();

    /**
     * Set currently focused node
     *
     * @since Series 60 3.1
     * @param aFocusedNode Focused node
     * @param aSource A source of event that triggered focus change.
     */
    void SetFocusedNodeL( CXnNode* aFocusedNode, TInt aSource = 0 );

    /**
     * Get currently focused node
     *
     * @since Series 60 3.2
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
     * Get the raw ODT
     *
     * @since Series 60 3.1
     * @return ODT
     */
    CXnODT* ODT();

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
     * Activate a view
     *
     * @since Series 60 3.1
     * @param aViewNode View node
     */
    void ActivateViewL( CXnNode& aViewNode );

    /**
     * Returns view manager
     *
     * @since S60 5.0
     * @return view manager
     */
    CXnViewManager* ViewManager();

    /**
     * Return the active view
     *
     * @since Series 60 3.1
     * @return The active view node
     */
    CXnNode* ActiveView();

    /**
     * Add a dirty node.
     *
     * LayoutUiL and RenderUiL will only perform operations on the set of dirty
     * nodes.
     *
     * @since Series 60 3.1
     * @param aNode Dirty node.
     */
    void AddDirtyNodeL( CXnNode* aNode, TInt aLevel );

    /**
     * Get application interface.
     *
     * @since Series 60 3.1
     * @return application interface
     */
    TXnUiEngineAppIf& AppIfL();

    /**
     * Get plugin interface.
     *
     * @since Series 60 3.1
     * @return plugin interface
     */
    TXnUiEnginePluginIf& PluginIfL();

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
     * Checks whether the menu is displaying.
     *
     * @since Series 60 3.1
     * @return ETrue if menu is displaying.
     */
    TBool IsMenuDisplaying();
       
    /**
     * Returns menubar node.
     *
     * @since Series 60 3.2
     */
    CXnNode* MenuBarNode() const;

    /**
     * Returns stylus popup node.
     *
     * @since S60 5.0
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
     * @since Series 60 3.2
     * @param aNode Node to use.
     */
    void RemovePassiveFocusedNodeL( CXnNode* aNode );

    /**
     * Clear the passive focused node array, and unset the passive focus states.
     *
     * @since Series 60 3.2
     */
    void ClearPassiveFocusedNodesL();

    /**
     * Sets the main pane rect
     *
     * @since Series 60 5.0
     */
    void SetClientRectL( TRect aRect, TBool aDrawNow = ETrue );

    /**
     * Gets the main pane rect
     *
     * @since Series 60 5.0
     */
    TRect ClientRect() const;

    /**
     * Sets nodes dropped state
     *
     * @since S60 3.2
     */
    void SetNodeDroppedL( CXnNode& aNode, TInt aDropped ) const;

    /**
     * Checks if node is adaptive, but doesn't have any content
     *
     * @since Series 60 3.2
     */
    void CheckAdaptiveContentL(
        CXnNode& aNode,
        TSize aAvailableSize = TSize::EUninitialized ) const;

    /**
     * Adds a node which can be focused.
     *
     * @since Series 60 3.2
     */
    void AddFocusCandidateL( CXnNode *aNode );

    /**
     * Fetch editor to operate on AI3 widgets.
     *
     * @since Series 60 5.0
     * @return Pointer to Editor
     */
    CXnEditor* Editor()const;

    /**
     * Gets pointer to edit mode storage.
     *
     * @since Series 60 5.0
     */
    CXnEditMode* EditMode();

    /**
     * Gets the array that contains plugin nodes for current view.
     *
     * @since Series 60 5.0
     */
    RPointerArray< CXnNode >* Plugins();

    /**
     * Gets hit test
     *
     * @since S60 5.1
     * @return the hittest
     */
    CXnHitTest& HitTest() const;

    /**
     * Positions stylus popup according given position
     *
     * @since S60 5.1
     * @param aNode stylus popup node
     * @param aReference a reference node where popup should fit
     * @param aPosition stylus popup position to set.
     */
    void PositionStylusPopupL(
        CXnNode& aNode,
        CXnNode& aReference,
        const TPoint& aPosition );

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
    
private:
    /**
     * C++ default constructor.
     */
    CXnUiEngine();

    /**
     * 2nd phase constructor.
     */
    void ConstructL( CXnAppUiAdapter& aAdapter );

private:
    // Data

    /**
     * The layout calculation and ui rendering implementation.
     * Own.
     */
    CXnUiEngineImpl* iUiEngineImpl;

    /**
     * Application proxy.
     * Own.
     */
    TXnUiEngineAppIf* iAppIf;

    /**
     * Plugin proxy.
     * Own.
     */
    TXnUiEnginePluginIf* iPluginIf;
    };

#endif
