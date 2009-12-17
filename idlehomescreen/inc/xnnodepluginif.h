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
* Node of layout tree, plugin interface
*
*/

#ifndef XNNODEPLUGINIF_H
#define XNNODEPLUGINIF_H

// System includes
#include <e32base.h>

// Forward declarations
class CXnType;
class CXnProperty;
class CXnNode;
class CXnViewNodeImpl;
class CXnComponentNodeImpl;
class TXnUiEnginePluginIf;
class CXnNodeAppIf;
class CXnControlAdapter;

namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Class declaration
/**
 * Node of layout tree, plugin interface
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
class CXnNodePluginIf : public CBase
    {
public:
    /**
     * C++ default constructor.
     */
    CXnNodePluginIf( CXnNode& aNode );

public:
    // New functions

    /**
     * Get the view node implementation
     *
     * @since Series 60 3.1
     * @return View node implementation
     */
    IMPORT_C CXnViewNodeImpl* ViewNodeImpl();

    /**
     * Get the component node implementation
     *
     * @since Series 60 3.1
     * @return Component node implementation
     */
    IMPORT_C CXnComponentNodeImpl* ComponentNodeImpl();

    /**
     * Get the UI engine
     *
     * @since Series 60 3.1
     * @return UI engine
     */
    IMPORT_C TXnUiEnginePluginIf* UiEngineL();

    /**
     * Set node's layout capability
     *
     * @since Series 60 3.1
     * @param aLayoutCapable Layout capability
     */
    IMPORT_C void SetLayoutCapable( const TBool aLayoutCapable );

    /**
     * Get node's layout capability
     *
     * @since Series 60 3.1
     * @return Layout capability
     */
    IMPORT_C TBool IsLayoutCapable() const;

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
     *
     * @since Series 60 3.1
     * @param aProperty Property to set.
     */
    IMPORT_C void SetPropertyL( CXnProperty* aProperty );

    /**
     * Set a property without notification being sent.
     *
     * @see SetPropertyL
     * @param aProperty Property to set.
     */
    IMPORT_C void SetPropertyWithoutNotificationL( CXnProperty* aProperty );

    /**
     * Gets a property.
     *
     * The list will retain ownership of the property,
     * the caller must not destroy the property. The returned pointer will
     * become invalid if the property is replaced in the list by a subsequent
     * SetL().
     *
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
     *
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
     * Unset a state
     *
     * @since Series 60 3.1
     * @param aState State to unset.
     */
    IMPORT_C void UnsetStateL( const TDesC8& aState );

    /**
     * Is the node in focused state
     *
     * @since Series 60 3.1
     * @return ETrue if node is focused.
     */
    IMPORT_C TBool IsFocusedState();

    /**
     * Get node type
     *
     * @since Series 60 3.1
     * @return Node type
     */
    IMPORT_C CXnType* Type();

    /**
     * Set node's content rect
     *
     * @since Series 60 3.1
     * @param aRect Content rect
     */
    IMPORT_C void SetRect( const TRect& aRect );

    /**
     * Get node's content rect
     *
     * @since Series 60 3.1
     * @return Content rect
     */
    IMPORT_C TRect Rect();

    /**
     * Set node's border rect
     *
     * @since Series 60 3.1
     * @param aRect Border rect
     */
    IMPORT_C void SetBorderRect( const TRect& aRect );

    /**
     * Get node's border rect
     *
     * @since Series 60 3.1
     * @return Border rect
     */
    IMPORT_C TRect BorderRect();

    /**
     * Set node's margin rect
     *
     * @since Series 60 3.1
     * @param aRect Margin rect
     */
    IMPORT_C void SetMarginRect( const TRect& aRect );

    /**
     * Get node's margin rect
     *
     * @since Series 60 3.1
     * @return Margin rect
     */
    IMPORT_C TRect MarginRect();

    /**
     * Set node's padding rect
     *
     * @since Series 60 3.1
     * @param aRect Padding rect
     */
    IMPORT_C void SetPaddingRect( const TRect& aRect );

    /**
     * Get node's padding rect
     *
     * @since Series 60 3.1
     * @return Padding rect
     */
    IMPORT_C TRect PaddingRect();

    /**
     * Returns the parent. The ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return The parent area, NULL if not set.
     */
    IMPORT_C CXnNodePluginIf* ParentL() const;

    /**
     * Adds a child node under this node. Assumes ownership of the node.
     *
     * @since Series 60 3.1
     * @param aChild Child to be added
     * @exception KXnErrAddChildToAreaFailed Adding child failed.
     */
    IMPORT_C void AddChildL( CXnNodePluginIf* aChild );

    /**
     * Get the child nodes
     *
     * @since Series 60 3.1
     * @return Child nodes
     */
    IMPORT_C RPointerArray< CXnNodePluginIf > ChildrenL();

    /**
     * Get whether the node is to be drawn
     *
     * @since Series 60 3.1
     * @return Flag indicating whether the node should be drawn or not
     */
    IMPORT_C TBool IsDrawingAllowed() const;

    /*
     * Get node application interface
     *
     * @return Node application interface
     */
    IMPORT_C CXnNodeAppIf& AppIfL();

    /**
     * Sets handle tooltip flag.
     *
     * If set to EFalse, tooltip
     * is not handled by node.
     *
     * @since S60 3.1
     * @return Pointer to DOM node
     */
    IMPORT_C void SetHandleTooltip( TBool aFlag );

    /**
     * Offers interface to report trigger event.
     *
     * @since S60 3.1
     * @param aTriggerName is a name of the trigger
     * @params aValueName and aValue delivers an attribute
     */
    IMPORT_C void ReportTriggerEventL(
        const TDesC8& aTriggerName,
        const TDesC8& aValueName,
        const TDesC8& aValue );

    /**
     * Gets namespace of node
     *
     * @since Series 60 5.0
     * @return namespace
     */
    IMPORT_C const TDesC8& Namespace();

    /**
     * Gets control of node
     *
     * @since Series 60 5.0
     * @return Control, NULL if not found
     */
    IMPORT_C CXnControlAdapter* Control() const;

    /**
     * Get the internal node
     *
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
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* DisplayL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* VisibilityL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* LabelL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* IdL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* VisibleRowsL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridColumnsL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridOrientationL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridVerDirectionL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridHorDirectionL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* FocusHorLoopingL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* FocusVerLoopingL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* RefL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridScrollBarWidthL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* GridScrollBarMarginL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* PathL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    IMPORT_C CXnProperty* MaskPathL();

    /**
     * Marks node as dirty, adds it to UiEngine's dirty list and
     * clears rendered and laidout from the node and its children.
     */
    IMPORT_C void SetDirtyL();

private:
    // Data

    /**
     * Pointer to internal node object.
     * Not own.
     */
    CXnNode* iNode;
    };

#endif
