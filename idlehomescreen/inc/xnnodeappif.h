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
* Application interface to node of layout tree
*
*/

#ifndef XNNODEAPPIF_H
#define XNNODEAPPIF_H

// System includes
#include <coedef.h>
#include <e32base.h>
#include <w32std.h>
#include <coecobs.h>

// Forward declarations
class CXnType;
class CXnProperty;
class CXnNode;
class TXnUiEngineAppIf;
class CXnComponent;

namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Class declaration
/**
 * Application interface to node of layout tree
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
class CXnNodeAppIf : public CBase
    {
public:
    /**
     * C++ default constructor.
     */
    CXnNodeAppIf( CXnNode& aNode );

public:
    // New functions

    /**
     * Get the UI engine
     *
     * @since Series 60 3.1
     * @return UI engine
     */
    IMPORT_C TXnUiEngineAppIf* UiEngineL();

    /**
     * Set the node's characted data
     *
     * @since Series 60 3.1
     * @param aData Character data
     */
    IMPORT_C void SetPCDataL( const TDesC8& aData );

    /**
     * Get the node's characted data
     *
     * @since Series 60 3.1
     * @return Character data
     */
    IMPORT_C const TDesC8& GetPCData() const;

    /**
     * Set a property.
     *
     * The list will assume ownership of the property.
     * If an equal property is already in the list, it will be
     * replaced with the new property and the old property will be destroyed.
     * @since Series 60 3.1
     * @param aProperty Property to set.
     */
    IMPORT_C void SetPropertyL( CXnProperty* aProperty );

    /**
     * Set an array of properties.
     *
     * Improves the performance when many properties need to be
     * set e.g. in construction phase.
     * The list will assume ownership of the property.
     * If an equal property is already in the list, it will be
     * replaced with the new property and the old property will be destroyed.
     * @since Series 60 3.1
     * @param aPropertyArray Properties to set.
     */
    IMPORT_C void SetPropertyArrayL(
        RPointerArray< CXnProperty >* aPropertyArray );

    /**
     * Gets a property.
     *
     * The list will retain ownership of the property,
     * the caller must not destroy the property. The returned pointer will
     * become invalid if the property is replaced in the list by a subsequent
     * SetL().
     * @since Series 60 3.1
     * @param aProperty Property key
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GetPropertyL( CXnProperty& aProperty ) const;

    /**
     * Gets a property.
     *
     * The list will retain ownership of the property,
     * the caller must not destroy the property. The returned pointer will
     * become invalid if the property is replaced in the list by a subsequent
     * SetL().
     * @since Series 60 3.1
     * @param aKey Name of the property
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GetPropertyL( const TDesC8& aKey ) const;

    /**
     * Set a state
     *
     * @since Series 60 3.1
     * @param aState State to set.
     */
    IMPORT_C void SetStateL( const TDesC8& aState );

    /**
     * Check whether a state is set
     *
     * @since Series 60 3.1
     * @return ETrue if set, EFalse if not
     */
    IMPORT_C TBool IsStateSet( const TDesC8& aState );

    /**
     * Unset a state
     *
     * @since Series 60 3.1
     * @param aState State to unset.
     */
    IMPORT_C void UnsetStateL( const TDesC8& aState );

    /**
     * Get node type
     *
     * @since Series 60 3.1
     * @return Node type
     */
    IMPORT_C CXnType* Type();

    /**
     * Get node's content rect
     *
     * @since Series 60 3.1
     * @return Content rect
     */
    IMPORT_C TRect Rect();

    /**
     * Get node's border rect
     *
     * @since Series 60 3.1
     * @return Border rect
     */
    IMPORT_C TRect BorderRect();

    /**
     * Get node's margin rect
     *
     * @since Series 60 3.1
     * @return Margin rect
     */
    IMPORT_C TRect MarginRect();

    /**
     * Get node's padding rect
     *
     * @since Series 60 3.1
     * @return Padding rect
     */
    IMPORT_C TRect PaddingRect();

    /**
     * Get the internal node
     * @since Series 60 3.1
     * @return Internal node
     */
    CXnNode& Node();

    /**
     * Create a component interface according to the given type.
     *
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */
    IMPORT_C XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
        const TDesC8& aType );

    /**
     * Get the child nodes
     *
     * @since Series 60 3.1
     * @return Child nodes
     */
    IMPORT_C RPointerArray< CXnNodeAppIf > ChildrenL();

    /**
     * Get the parent node
     *
     * @since Series 60 3.1
     * @return parent node
     */
    IMPORT_C CXnNodeAppIf* ParentL() const;

    /**
     * Get internal dom node type
     *
     * @return Reference to node type
     */
    IMPORT_C const TDesC8& InternalDomNodeType() const;

    /**
     * Shows all tooltip nodes associated to this node
     */
    IMPORT_C void ShowTooltipsL();

    /**
     * Hides all tooltip nodes associated to this node
     */
    IMPORT_C void HideTooltipsL();

    /**
     * Get namespace of this node
     */
    IMPORT_C const TDesC8& Namespace();
    
    /**
     * Get the component object of the node.
     *
     * @since Series 60 5.2
     * @return component object
     */
    IMPORT_C CXnComponent& Component();

private:
    // Data

    /**
     * Pointer to internal node object.
     * Not own.
     */
    CXnNode* iNode;
    };

#endif
