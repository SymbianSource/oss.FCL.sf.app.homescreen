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
* Xuikon layout engine plugin interface.
*
*/

#ifndef XNUIENGINEPLUGINIF_H
#define XNUIENGINEPLUGINIF_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CXnUiEngine;
class CXnProperty;
class CXnNodePluginIf;
class CXnDomStringPool;
class CXnResource;
class CXnControlAdapter;
struct TPointerEvent;

// Class declaration
/**
 * Xuikon layout engine plugin interface.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
class TXnUiEnginePluginIf
    {
public:
    /**
     * C++ default constructor.
     */
    TXnUiEnginePluginIf( CXnUiEngine& aUiEngine );

public:
    // New functions

    /**
     * Calculates the layout of the UI
     *
     * @since Series 60 3.1
     * @param aNode Starting point for the layout
     * @exception KXnErrLoadLayoutFileFailed Loading the layout file failed.
     */
    IMPORT_C void RenderUIL( CXnNodePluginIf* aNode = NULL );

    /**
     * Gets the root node of the UI. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Node
     */
    IMPORT_C CXnNodePluginIf& RootNodeL();

    /**
     * Find a node by its id. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aNodeId Area id
     * @param aNamespace Namespace
     * @return Node with the given id
     */
    IMPORT_C CXnNodePluginIf* FindNodeByIdL(
        const TDesC& aNodeId,
        const TDesC& aNamespace = KNullDesC );

    /**
     * Find a node by its id. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aNodeId Area id
     * @param aNamespace Namespace
     * @return Node with the given id
     */
    IMPORT_C CXnNodePluginIf* FindNodeByIdL(
        const TDesC8& aNodeId,
        const TDesC8& aNamespace = KNullDesC8 );

    /**
     * Find nodes by class. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes belonging to the class
     */
    IMPORT_C RPointerArray< CXnNodePluginIf > FindNodeByClassL(
        const TDesC& aClassId );

    /**
     * Find resources (images etc.) of the UI. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Resources of the UI
     */
    IMPORT_C CArrayPtrSeg< CXnResource >& Resources();

    /**
     * Get currently focused node. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Focused node
     */
    IMPORT_C CXnNodePluginIf* FocusedNodeL();

    /**
     * Set currently focused node
     *
     * @since Series 60 3.1
     * @param aFocusedNode Focused node
     */
    IMPORT_C void SetFocusedNode( CXnNodePluginIf* aFocusedNode );


    /**
     * Get the string pool
     *
     * @since Series 60 3.1
     * @return String pool
     */
    IMPORT_C CXnDomStringPool& StringPool();

    /**
     * Get a numeric value of a property in vertical pixels
     *
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in vertical pixels
     */
    IMPORT_C TInt VerticalPixelValueL(
        CXnProperty* aValue,
        TInt aReferenceValue );

    /**
     * Get a numeric value of a property in horizontal pixels
     *
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in horizontal pixels
     */
    IMPORT_C TInt HorizontalPixelValueL(
        CXnProperty* aValue,
        TInt aReferenceValue );

    /**
     * Get a numeric value of a property in vertical twips
     *
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in vertical twips
     */
    IMPORT_C TInt VerticalTwipValueL(
        CXnProperty* aValue,
        TInt aReferenceValue );

    /**
     * Get a numeric value of a property in horizontal twips
     *
     * @since Series 60 3.1
     * @param aValue Property
     * @param aReferenceValue Value used for percentage calculation
     * @return Value in horizontal pixels
     */
    IMPORT_C TInt HorizontalTwipValueL(
        CXnProperty* aValue,
        TInt aReferenceValue );

    /**
     * Get the size of the current screen device
     *
     * @since Series 60 3.1
     * @return Size of the current screen device
     */
    IMPORT_C TSize ScreenDeviceSize();

    /**
     * Checks whether the dialog is displaying.
     *
     * @since Series 60 3.1
     * @return ETrue if dialog is displaying.
     */
    IMPORT_C TBool IsDialogDisplaying();

    /**
     * Sets the given node dirty.
     *
     * @since Series 60 3.1
     * @param aNode A node to be set dirty.
     */
    IMPORT_C void AddDirtyNodeL( CXnNodePluginIf& aNode );

    /**
     * Checks whether the menu is displaying.
     *
     * @since Series 60 3.1
     * @return ETrue if menu is displaying.
     */
    IMPORT_C TBool IsMenuDisplaying();

    /**
     * Calculates the layout of the UI.
     *
     * @since Series 60 3.1
     * @param aNode Starting point for the layout
     */
    IMPORT_C void LayoutUIL( CXnNodePluginIf* aNode = NULL );

    /**
     * Gets the edit mode state from UI Engine.
     *
     * @since Series 60 5.0
     * @return A state of edit mode.
     */
    IMPORT_C TInt EditMode();

    /**
     * Gets widgets visibilty state
     *
     * @since Series 60 5.0
     * @return ETrue if widgets visible, EFalse otherwise
     */
    IMPORT_C TBool WidgetsVisible() const;
    
    /**
     * Gets a plugin node which the given node is.
     *
     * @since Series 60 5.0
     * @param aNode Node to start.
     * @return A Plugin node.
     */
    IMPORT_C CXnNodePluginIf& PluginNodeL( CXnNodePluginIf* aNode );
                                    
    /**
     * Disables layoyt and redraw, places cleanup item to cleanup stack
     * for enabling layot and redraw
     * 
     * @since Series 60 5.0     
     */                        
    IMPORT_C void DisableRenderUiLC();
    
    /**
     * Enables partial touch input
     * 
     * @since Series 60 5.2
     * @param aNode Editor Node
     * @param TBool True if partial input is open      
     */      
    IMPORT_C void EnablePartialTouchInput( CXnNodePluginIf& aNode, TBool aEnable );

    /**
     * Checks if text editor is focused or partioal touch input open.
     * 
     * @since Series 60 5.2
     * @return TBool True if partial input is open or editor focused
     */      
    IMPORT_C TBool IsTextEditorActive();

private:
    // Data

    /**
     * The layout calculation and ui rendering implementation.
     * Not own.
     */
    CXnUiEngine* iUiEngine;
    };

#endif
