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
* Layout calculation and UI rendering mechanism.
*
*/

#ifndef XNUIENGINEAPPIF_H
#define XNUIENGINEAPPIF_H

// System inlcudes
#include <e32base.h>

// Forward declarations
class CXnUiEngine;
class CXnNodeAppIf;
class CXnDomStringPool;
class RFile;

// Class declaration
/**
 * Layout engine.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
class TXnUiEngineAppIf
    {
public:
    /**
     * C++ default constructor.
     */
    TXnUiEngineAppIf( CXnUiEngine& aUiEngine );

public:
    // New functions

    /**
     * Calculates the layout of the UI
     *
     * @since Series 60 3.1
     * @param aNode Starting point for the layout
     * @exception KXnErrLoadLayoutFileFailed Loading the layout file failed.
     */
    IMPORT_C void RenderUIL( CXnNodeAppIf* aNode = NULL );

    /**
     * Gets the root node of the UI. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return Node
     */
    IMPORT_C CXnNodeAppIf& RootNodeL();

    /**
     * Find a node by its id. Ownership is not transferred.
     *
     * @since Series 60 3.1
     * @param aNodeId Area id
     * @param aNamespace Namespace
     * @return Node with the given id
     */
    IMPORT_C CXnNodeAppIf* FindNodeByIdL(
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
    IMPORT_C CXnNodeAppIf* FindNodeByIdL(
        const TDesC8& aNodeId,
        const TDesC8& aNamespace = KNullDesC8 );

    /**
     * Find nodes by class. Ownership of the array is transferred, ownership of
     * the nodes is not transferred.
     *
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes belonging to the class
     */
    IMPORT_C RPointerArray< CXnNodeAppIf > FindNodeByClassL(
        const TDesC& aClassId,
        const TDesC& aNamespace = KNullDesC );

    /**
     * Find nodes by class. Ownership of the array is transferred, ownership of
     * the nodes is not transferred.
     *
     * @since Series 60 3.1
     * @param aClassId Class id
     * @return Nodes belonging to the class
     */
    IMPORT_C RPointerArray< CXnNodeAppIf > FindNodeByClassL(
        const TDesC8& aClassId,
        const TDesC8& aNamespace = KNullDesC8 );

    /**
     * Find content source nodes. Ownership of the array is transferred,
     * ownership of the nodes is not transferred.
     *
     * @since S60 5.0
     * @param aNamespace namespace where to find content source nodes
     * @return Content source nodes from the namespace
     */
    IMPORT_C RPointerArray< CXnNodeAppIf > FindContentSourceNodesL(
        const TDesC8& aNamespace );

    /**
     * Get the string pool
     *
     * @since Series 60 3.1
     * @return String pool
     */
    IMPORT_C CXnDomStringPool& StringPool();

    /**
     * Refresh current menu
     *
     * @since Series 60 3.1
     */
    IMPORT_C void RefreshMenuL();

    /**
     * Activate a view
     *
     * @since Series 60 3.1
     * @param aViewNode View node
     */
    IMPORT_C void ActivateViewL( CXnNodeAppIf& aViewNode );

    /**
     * Returns the active view
     *
     * @since Series 60 3.1
     * @return The active view node
     */
    IMPORT_C CXnNodeAppIf* ActiveView();

    /**
     * Checks whether the menu is displaying.
     *
     * @since Series 60 3.2
     * @return ETrue if menu is displaying.
     */
    IMPORT_C TBool IsMenuDisplaying();

    /**
     * Populate given array with plugin nodes.
     *
     * @since Series 60 5.0
     * @param aArray Array to populate
     * @return Number of added plugin nodes
     */
    IMPORT_C TInt GetPluginNodeArrayL(
        RPointerArray< CXnNodeAppIf >& aArray );

    /**
     * Gets the currently focused node
     *
     * @since Series 60 5.0
     * @return Pointer to focused node, NULL if no focus
     */
    IMPORT_C CXnNodeAppIf* FocusedNode();

    /**
     * Checks if the UI is in Edit mode.
     *
     * @since Series 60 5.0
     * @return ETrue if the current mode is edit, otherwise EFalse.
     */
    IMPORT_C TBool IsEditMode();

    /**
     * Changes internal ui engine pointer
     *
     * @since Series 60 5.0
     */
    IMPORT_C void SetUiEngine( CXnUiEngine* aUiEngine );

    /**
     * Gets theme resource file
     *
     * @since S60 5.1
     * @param aPath path from which file is found
     * @param aFile file to which resource is set
     * @return errorcode
     */
    IMPORT_C TInt GetThemeResource( const TDesC& aPath, RFile& aFile );

    /**
     * Gets widgets visibilty state
     *
     * @since Series 60 5.0
     * @return ETrue if widgets visible, EFalse otherwise
     */
    IMPORT_C TBool WidgetsVisible() const;

private:
    // Data

    /**
     * The layout calculation and ui rendering implementation.
     * Not own.
     */
    CXnUiEngine* iUiEngine;
    };

#endif
