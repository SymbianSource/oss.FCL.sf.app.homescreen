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
* Node of layout tree
*
*/

#ifndef XNNODE_H
#define XNNODE_H

// System includes
#include <e32base.h>
#include <w32std.h>

// User includes

// Forward declarations
class CXnType;
class CXnProperty;
class CXnNodeImpl;
class CXnViewsNodeImpl;
class CXnViewNodeImpl;
class CXnComponentNodeImpl;
class CXnUiEngine;
class CXnNodePluginIf;
class CXnNodeAppIf;
class CXnDomNode;
class CXnControlAdapter;
class CXnScrollableControlAdapter;
namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Constants
namespace XnDirtyLevel
    {
    /** Not dirty */
    const TInt ENone = 0;
    /** Draw node's appearance */
    const TInt ERender = 1;
    /** Layout and draw node's appearance */
    const TInt ELayoutAndRender = 2;
    /** Layout and draw node's and its siblings appearance */
    const TInt ELayoutAndRenderSiblings = 3;
    }

namespace XnNodeLayout
    {
    const TInt ENone = 0;
    const TInt EDropped = 1;
    const TInt ENotLayouted = 2;
    const TInt EDroppedAndNotLayouted = 3;
    }

namespace XnAdaptive
    {
    const TInt ENone = 0x00;
    const TInt EWidth = 0x01;
    const TInt EHeight = 0x02;
    const TInt EMeasure = 0x04;
    const TInt ESizeFixed = 0x08;
    const TInt EIgnoreDirty = 0x10;
    }

namespace XnEventSource
    {
    const TInt EUnknown = 0x00;
    const TInt EStylus = 0x01;
    const TInt EKey = 0x02;
    }

namespace XnHoldProperty
    {
    const TInt ENoHold = 0x00;
    const TInt EHold = 0x01;
    }

// Class declaration
/**
 * Node of the layout tree
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnNode ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
    static CXnNode* NewL();

    /**
     * Destructor.
     */
    virtual ~CXnNode();

public:
    // New functions

    /**
     * Get the node implementation
     *
     * @since Series 60 3.1
     * @return Node implementation
     */
    CXnNodeImpl* Impl();

    /**
     * Get the root node implementation
     *
     * @since Series 60 3.1
     * @return Root node implementation
     */
    CXnNodeImpl* RootNodeImpl();

    /**
     * Get the view node implementation
     *
     * @since Series 60 3.1
     * @return View node implementation
     */
    CXnViewNodeImpl* ViewNodeImpl();

    /**
     * Get the component node implementation
     *
     * @since Series 60 3.1
     * @return Component node implementation
     */
    CXnComponentNodeImpl* ComponentNodeImpl();

    /**
     * Set UI engine
     *
     * @since Series 60 3.1
     * @param aEngine UI engine
     */
    void SetUiEngine( CXnUiEngine& aEngine );

    /**
     * Get the UI engine
     *
     * @since Series 60 3.1
     * @return UI engine
     */
    CXnUiEngine* UiEngine();

    /**
     * Set the node implementation
     *
     * @since Series 60 3.1
     * @param aImpl Node implementation
     */
    void SetImpl( CXnNodeImpl* aImpl );

    /**
     * Set the Root node implementation
     *
     * @since Series 60 3.1
     * @param aRootNodeImpl Root node implementation
     */
    void SetRootNodeImpl( CXnNodeImpl* aRootNodeImpl );

    /**
     * Set the views implementation
     *
     * @since Series 60 3.1
     * @param aViewsNodeImpl Views node implementation
     */
    void SetViewsNodeImpl( CXnViewsNodeImpl* aViewsNodeImpl );

    /**
     * Set the view node implementation
     *
     * @since Series 60 3.1
     * @param aViewNodeImpl View node implementation
     */
    void SetViewNodeImpl( CXnViewNodeImpl* aViewNodeImpl );

    /**
     * Handle interaction events
     *
     * @since Series 60 3.1
     * @param aEventData Event data
     * @param aSource A source of event, e.g. XnEventSource::EStylus.
     * @return Flag indicating whether the event was consumed
     */
    TBool ReportXuikonEventL( CXnNode& aEventData, TInt aSource = 0 );

    /**
     * Handle key event
     *
     * @since Series 60 3.1
     * @param aKeyEvent Key event data
     * @param aType Event type
     */
    void OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
     * Set the component node implementation
     *
     * @since Series 60 3.1
     * @param aComponentNodeImpl Component node implementation
     */
    void SetComponentNodeImpl( CXnComponentNodeImpl* aComponentNodeImpl );

    /**
     * Set node's layout capability
     *
     * @since Series 60 3.1
     * @param aLayoutCapable Layout capability
     */
    void SetLayoutCapable( const TBool aLayoutCapable );

    /**
     * Get node's layout capability
     *
     * @since Series 60 3.1
     * @return Layout capability
     */
    TBool IsLayoutCapable() const;

    /**
     * Set the node's characted data
     *
     * @since Series 60 3.1
     * @param aData Character data
     */
    void SetPCDataL( const TDesC8& aData );

    /**
     * Get the node's characted data
     *
     * @since Series 60 3.1
     * @return Character data
     */
    const TDesC8& GetPCData() const;

    /**
     * Set a property.
     *
     * The list will assume ownership of the property.
     * If an equal property is already in the list, it will be
     * replaced with the new property and the old property will be destroyed.
     * @since Series 60 3.1
     * @param aProperty Property to set.
     */
    void SetPropertyL( CXnProperty* aProperty );

    /**
     * Set a property without notification being sent.
     *
     * @see SetPropertyL
     * @param aProperty Property to set.
     */
    void SetPropertyWithoutNotificationL( CXnProperty* aProperty );

    /**
     * Set a property NOT replacing an equal property if already in the list.
     *
     * @see SetPropertyL
     * @param aProperty Property to set.
     */
    void InitializePropertyL( CXnProperty* aProperty );

    /**
     * Set an array of properties.
     *
     * Improves the performance when many properties need to be
     * set e.g. in construction phase.
     * The list will assume ownership of the property.
     * If an equal property is already in the list, it will be
     * replaced with the new property and the old property will be destroyed.
     * @see SetPropertyL
     */
    void SetPropertyArrayL( RPointerArray< CXnProperty >* aPropertyArray );

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
    CXnProperty* GetPropertyL( const TDesC8& aKey ) const;

    /**
     * Set a state
     *
     * @since Series 60 3.1
     * @param aState State to set.
     * @param aSource A source of the event.
     */
    void SetStateL(
        const TDesC8& aState,
        TInt aSource = XnEventSource::EUnknown );        

    /**
     * Check whether a state is set
     *
     * @since Series 60 3.1
     * @return ETrue if set, EFalse if not
     */
    TBool IsStateSet( const TDesC8& aState );

    /**
     * Set a state without performing notifications and re-rendering
     *
     * @since Series 60 3.1
     * @param aState State to set
     */
    void SetStateWithoutNotificationL( const TDesC8& aState );

    /**
     * Unset a state
     *
     * @since Series 60 3.1
     * @param aState State to unset.
     */
    void UnsetStateL( const TDesC8& aState );

    /**
     * Get node type
     *
     * @since Series 60 3.1
     * @return Node type
     */
    CXnType* Type();

    /**
     * Set node's content rect
     *
     * @since Series 60 3.1
     * @param aRect Content rect
     */
    void SetRect( const TRect& aRect );

    /**
     * Get node's content rect
     *
     * @since Series 60 3.1
     * @return Content rect
     */
    TRect Rect();

    /**
     * Set node's border rect
     *
     * @since Series 60 3.1
     * @param aRect Border rect
     */
    void SetBorderRect( const TRect& aRect );

    /**
     * Get node's border rect
     *
     * @since Series 60 3.1
     * @return Border rect
     */
    TRect BorderRect();

    /**
     * Set normal flow border rect
     *
     * @since Series 60 3.1
     * @param aRect border rect
     */
    void SetNormalFlowBorderRect( const TRect& aRect );

    /**
     * Get normal flow border rect
     *
     * @since Series 60 3.1
     * @return Border rect
     */
    TRect NormalFlowBorderRect();

    /**
     * Set node's margin rect
     *
     * @since Series 60 3.1
     * @param aRect Margin rect
     */
    void SetMarginRect( const TRect& aRect );

    /**
     * Get node's margin rect
     *
     * @since Series 60 3.1
     * @return Margin rect
     */
    TRect MarginRect();

    /**
     * Set node's padding rect
     *
     * @since Series 60 3.1
     * @param aRect Padding rect
     */
    void SetPaddingRect( const TRect& aRect );

    /**
     * Get node's padding rect
     *
     * @since Series 60 3.1
     * @return Padding rect
     */
    TRect PaddingRect();

    /**
     * Returns the parent. The ownership is not transferred.
     *
     * @since Series 60 3.1
     * @return The parent area, NULL if not set.
     */
    CXnNode* Parent() const;

    /**
     * Sets the parent.
     *
     * @since Series 60 3.1
     * @param aParent Parent area
     */
    void SetParent( CXnNode& aParent );

    /**
     * Adds a child node under this node. Assumes ownership of the node.
     *
     * @since Series 60 3.1
     * @param aChild Child to be added
     * @exception KXnErrAddChildToAreaFailed Adding child failed.
     */
    void AddChildL( CXnNode* aChild );

    /**
     * Get the child nodes
     *
     * @since Series 60 3.1
     * @return Child nodes
     */
    RPointerArray< CXnNode >& Children();

    /**
     * Set whether the node is dropped from layout
     *
     * @since Series 60 3.1
     * @param aDropped Flag indicating whether the node is dropped from layout
     */
    void SetDropped( const TInt aDropped );

    /**
     * Get whether the node is dropped from layout
     *
     * @since Series 60 3.1
     * @return Flag indicating whether the node is dropped from layout
     */
    TInt IsDropped() const;

    /**
     * Set node rendered, which means it will not be rendered again until
     * its properties, states or rects change
     *
     * @since Series 60 3.1
     */
    void SetRenderedL();

    /**
     * Set node laid out, which means it will not be laid out again until
     * its properties, states or rects change
     *
     * @since Series 60 3.1
     */
    void SetLaidOutL();

    /**
     * Get layout state
     *
     * @since Series 60 3.1
     * @return Layout state
     */
    TBool IsLaidOut() const;

    /**
     * Set node not rendered and not laid out
     *
     * @since Series 60 3.1
     */
    void ClearRenderedAndLaidOut();

    /**
     * Get node plugin interface
     *
     * @since Series 60 3.1
     * @return Node plugin interface
     */
    CXnNodePluginIf& PluginIfL();

    /**
     * Get node application interface
     *
     * @since Series 60 3.1
     * @return Node application interface
     */
    CXnNodeAppIf& AppIfL();

    /**
     * Create a component interface according to the given type.
     *
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */
    XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
        const TDesC8& aType );

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* WidthL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* HeightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MarginLeftL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MarginRightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderLeftL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderRightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PaddingLeftL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PaddingRightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MarginTopL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MarginBottomL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderTopL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderBottomL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PaddingTopL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PaddingBottomL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderWidthL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BlockProgressionL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* DirectionL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PositionL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MaxHeightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MinHeightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MaxWidthL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MinWidthL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* DisplayL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* VisibilityL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* LeftL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* RightL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* TopL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BottomL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderLeftStyleL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderRightStyleL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderTopStyleL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderBottomStyleL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderStyleL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BorderImageL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* DisplayPriorityL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* NameL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* ValueL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* LabelL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* InitialFocusL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* ClassL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* IdL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* PathL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* MaskPathL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* NavIndexL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* ZIndexL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BackgroundColorL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* FocusBackgroundL();

    /**
     * Get property as indicated by method name.
     *
     * @see GetPropertyL
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* BackgroundImageL();

    /**
     * Sets the pointer of DOM node
     *
     * @since S60 3.1
     * @param aDomNode Corresponding DOM node
     */
    void SetDomNode( CXnDomNode* aDomNode );

    /**
     * Returns pointer to associated DOM node.
     *
     * @since S60 3.1
     * @return Pointer to DOM node
     */
    CXnDomNode* DomNode();

    /**
     * Sets handle tooltip flag.
     *
     * @since S60 3.1
     * @return Pointer to DOM node
     */
    void SetHandleTooltip( TBool aFlag );

    /**
     * Measures the adaptive content dimensions.
     *
     * @since Series 60 3.2
     * @param aAvailableSize A size where adaptive content must fit
     * @return the content dimensions
     */
    TSize MeasureAdaptiveContentL( const TSize& aAvailableSize );

    /**
     * Checks if the node has adaptive content.
     *
     * @since Series 60 3.2
     * @return adaptive state (none, width/height or both)
     */
    TInt IsAdaptive( TBool aIgnoreSizeFixed = EFalse ) const;

    /**
     * Set node's content based state (none, width, height, or both).
     *
     * @since Series 60 3.2
     * @param aAdaptive state (none, width/height or both)
     */
    void SetAdaptiveL( const TInt aAdaptive );

    /**
     * Set fixed size to adaptive node
     *
     * @since Series 60 3.2
     * @param aFixedSize the node's size determined by layout algorithm
     */
    void FixAdaptiveSizeL( const TSize& aFixedSize );

    /**
     * Marks node as dirty, adds it to UiEngine's dirty list and
     * clears rendered and laidout from the node and its children.
     *
     * @since S60 3.2
     */
    void SetDirtyL( TInt aLevel = XnDirtyLevel::ELayoutAndRender,
        TBool aDisplayedState = EFalse );

    /**
     * Focuses first focusable node from the focus candidate array
     */
    TBool RunFocusChangeL( RPointerArray< CXnNode >& aFocusCandidates );

    /**
     * Change the order of the nodes. This is for drag and drop.
     */
    void ReorderNodesL( CXnNode* aSource, CXnNode* aTarget );

    /**
     * Loops through parents until a view node is found.
     * If view is not found (or this node is a view), returns
     * null.
     */
    CXnNode* FindViewNode();

    /**
     * Shows all tooltip nodes associated to this node
     */
    void ShowTooltipsL();

    /**
     * Hides all tooltip nodes associated to this node
     */
    void HideTooltipsL();

    /**
     * Gets namespace of node
     */
    const TDesC8& Namespace();

    /**
     * Gets control of node
     */
    CXnControlAdapter* Control() const;
        
    /**
     * Sets scrollable control
     */
    void SetScrollableControl( CXnScrollableControlAdapter* aScrollableControl );

    /**
     * Gets scrollable control.
     */
    CXnScrollableControlAdapter* ScrollableControl();
        
protected:
    /**
     * C++ default constructor.
     */
    CXnNode();

    /**
     * Two-phased constructor.
     */
    void ConstructL();

private:
    // Data

    /**
     * Node implementation.
     * Own.
     */
    CXnNodeImpl* iImpl;

    /**
     * Root node implementation.
     * Not own (owned via iImpl).
     */
    CXnNodeImpl* iRootNodeImpl;

    /**
     * Views node implementation.
     * Not own (owned via iImpl).
     */
    CXnViewsNodeImpl* iViewsNodeImpl;

    /**
     * View node implementation.
     * Not own (owned via iImpl).
     */
    CXnViewNodeImpl* iViewNodeImpl;

    /**
     * Component node implementation.
     * Not own (owned via iImpl).
     */
    CXnComponentNodeImpl* iComponentNodeImpl;

    /**
     * node plugin interface.
     * Own.
     */
    CXnNodePluginIf* iPluginIf;

    /**
     * node application interface.
     * Own.
     */
    CXnNodeAppIf* iAppIf;

    };

#endif
