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
* Implementation part of the bridge pattern of  CXnNode
*
*/

#ifndef XNNODEIMPL_H
#define XNNODEIMPL_H

// System includes
#include <e32base.h>
#include <w32std.h>

// User includes
#include "xndomproperty.h"
#include "xnnode.h"

// Forward declarations
class CXnType;
class CXnPropertyList;
class CXnProperty;
class CXnNode;
class CXnUiEngine;
class CLayoutPropertyCache;
class CXnDomNode;
class CXnScrollableControlAdapter;

namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Class declaration
/**
 * Implementation part of the bridge pattern in CXnNode.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnNodeImpl ) : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    static CXnNodeImpl* NewL( CXnType* aType );

    /**
     * Destructor.
     */
    virtual ~CXnNodeImpl();

public:
    // New functions

    /**
     * Sets wheter to handle tooltips
     * @since S60 5.1
     * @param aFlag ETrue to handle, EFalse otherwise
     */
    void SetHandleTooltip( TBool aFlag );

    /**
     * Hides all popups
     * @since S60 5.1
     */
    void HidePopupsL();

    /**
     * Shows all popups
     *
     * @param aRect content rect
     * @param aSource Event source
     * @since S60 5.1
     */
    void ShowPopupsL( TRect aRect, TInt aSource = XnEventSource::EUnknown );

    /**
     * Set content rect
     *
     * @since Series 60 3.1
     * @param aRect content rect
     */
    void SetRect( const TRect& aRect );

    /**
     * Get content rect
     *
     * @since Series 60 3.1
     * @return Content rect
     */
    TRect Rect();

    /**
     * Set border rect
     *
     * @since Series 60 3.1
     * @param aRect border rect
     */
    void SetBorderRect( const TRect& aRect );

    /**
     * Get border rect
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
     * Set margin rect
     *
     * @since Series 60 3.1
     * @param aRect margin rect
     */
    void SetMarginRect( const TRect& aRect );

    /**
     * Get margin rect
     *
     * @since Series 60 3.1
     * @return Margin rect
     */
    TRect MarginRect();

    /**
     * Set padding rect
     *
     * @since Series 60 3.1
     * @param aRect padding rect
     */
    void SetPaddingRect( const TRect& aRect );

    /**
     * Get padding rect
     *
     * @since Series 60 3.1
     * @return Padding rect
     */
    TRect PaddingRect();

    /**
     * Set node
     *
     * @since Series 60 3.1
     * @param aNode Node
     */
    void SetNode( CXnNode& aNode );

    /**
     * Get node
     *
     * @since Series 60 3.1
     * @return Node
     */
    CXnNode* Node();

    /**
     * Set UI engine
     *
     * @since Series 60 3.1
     * @param aEngine UI engine
     */
     void SetUiEngine( CXnUiEngine& aEngine );

    /**
     * Get UI engine
     *
     * @since Series 60 3.1
     * @return UI engine
     */
    CXnUiEngine* UiEngine();

    /**
     * Handle interaction events
     *
     * @since Series 60 3.1
     * @param aEventData Event data
     * @param aSource A source of event, e.g. XnEventSource::EStylus.
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
     * Set character data
     *
     * @since Series 60 3.1
     * @param aData Character data
     */
    void SetPCDataL( const TDesC8& aData );

    /**
     * Get character data
     *
     * @since Series 60 3.1
     * @return Character data
     */
    const TDesC8& GetPCData();

    /**
     * Set a property.
     *
     * The list will assume ownership of the property.
     * If an equal property is already in the list, it will be replaced
     * with the new property and the old property will be destroyed.
     * @since Series 60 3.1
     * @param aProperty Property to set.
     * @param aNotify Boolean flag indicating whether notifications should be
     *        sent
     */
    void SetPropertyL( CXnProperty* aProperty, TBool aNotify = ETrue );

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
    void SetPropertyArrayL( RPointerArray< CXnProperty >& aPropertyArray );

    /**
     * Gets a property.
     *
     * The list will retain ownership of the property,
     * the caller must not destroy the property. The returned pointer will
     * become invalid if the property is replaced in the list by a subsequent
     * SetL().
     * @since Series 60 3.1
     * @param aKey Property name
     * @return The property mapped to the key or NULL if not found.
     */
    CXnProperty* GetPropertyL( const TDesC8& aKey );

    /**
     * Set a state
     *
     * @since Series 60 3.1
     * @param aState State to set
     * @param aSource A source that triggered this event     
     */
    void SetStateL( const TDesC8& aState, TInt aSource );

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
     * Adds a child node. Assumes ownership of the node.
     *
     * @since Series 60 3.1
     * @param aChild Child to be added
     * @exception KXnErrAddChildToAreaFailed Adding child failed.
     */
    void AddChildL( CXnNode* aChild );

    /**
     * Get child nodes.
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
     * Set layout capability
     *
     * @since Series 60 3.1
     * @param aLayoutCapable Layout capability
     */
    void SetLayoutCapable( const TBool aLayoutCapable );

    /**
     * Get layout capability
     *
     * @since Series 60 3.1
     * @return Layout capability
     */
    TBool IsLayoutCapable() const;

    /**
     * Set node rendered, which means it will not be rendered again until its
     * properties, states or rects change
     *
     * @since Series 60 3.1
     */
    void SetRenderedL();

    /**
     * Set node laid out, which means it will not be laid out again until its
     * properties, states or rects change
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
     * Create a component interface according to the given type.
     *
     * This must be virtual because CXnComponentNodeImpl inherits this.
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
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
    TInt IsAdaptive(  TBool aIgnoreSizeFixed = EFalse ) const;

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
     * Find next node located aDirection from aNode.
     */

    /**
     * Gets namespace of node
     */
    const TDesC8& Namespace();

    /**
     * Sets scrollable control
     */
    void SetScrollableControl( CXnScrollableControlAdapter* aScrollableControl );

    /**
     * Gets scrollable control.
     */
    CXnScrollableControlAdapter* ScrollableControl();
            
private:

    TInt DoSetPropertyL( CXnProperty* aPropery );
    TRect AdjustRectIfNeeded( TRect aRect );

protected:
    /**
     * C++ default constructor.
     */
    CXnNodeImpl();

    /**
     * Two-phased constructor.
     */
    void ConstructL( CXnType* aType );

private:
    // Data

    /**
     * Component type.
     * Own.
     */
    CXnType* iType;

    /**
     * Pointer to property list.
     * Own.
     */
    CXnPropertyList* iPropertyList;

    /**
     * Pointer to parent node.
     * Not own.
     */
    CXnNode* iParent;

    /**
     * Pointer to node.
     * Not own.
     */
    CXnNode* iNode;

    /**
     * Content rect.
     */
    TRect iRect;

    /**
     * Border rect.
     */
    TRect iBorderRect;

    /**
     * Border rect according to normal flow
     */
    TRect iNormalFlowBorderRect;

    /**
     * Margin rect
     */
    TRect iMarginRect;

    /**
     * Padding rect
     */
    TRect iPaddingRect;

    /**
     * Is node dropped from layout
     */
    TInt iDropped;

    /**
     * Does node support layout
     */
    TBool iLayoutCapable;

    /**
     * Children of this node
     */
    RPointerArray< CXnNode > iChildren;

    /**
     * UI engine.
     * Not own.
     */
    CXnUiEngine* iUiEngine;

    /**
     * Flag indicating render status
     */
    TBool iRendered;

    /**
     * Flag indicating layout status
     */
    TBool iLaidOut;

    /**
     * Array containing list of pseudo classes that have class-specific properties
     */
    RArray< CXnDomProperty::TPseudoClass > iPropertyPseudoClasses;

    /**
     * Array containing list of pseudo classes that have a state set
     *
     * @see SetStateL
     */
    RArray< CXnDomProperty::TPseudoClass > iStates;

    /**
     * Local cache for property with name KName.
     * Not own.
     */
    CXnProperty* iName;

    /**
     * Local cache for property with name KValue.
     * Not own.
     */
    CXnProperty* iValue;

    /**
     * Local cache for property with name KLabel.
     * Not own.
     */
    CXnProperty* iLabel;

    /**
     * Local cache for property with name KInitialFocus.
     * Not own.
     */
    CXnProperty* iInitialFocus;

    /**
     * Local cache for property with name KClass.
     * Not own.
     */
    CXnProperty* iClass;

    /**
     * Local cache for property with name KId.
     * Not own.
     */
    CXnProperty* iId;

    /**
     * Local cache for property with name KPath.
     * Not own.
     */
    CXnProperty* iPath;

    /**
     * Local cache for property with name KMaskPath.
     * Not own.
     */
    CXnProperty* iMaskPath;

    /**
     * Local property cache.
     * Own.
     */
    CLayoutPropertyCache* iLayoutPropertyCache;

    /**
     * Flag indicating tooltip handling status
     */
    TBool iHandleTooltip;

    /**
     * Pointer to corresponding node in the DOM tree.
     * Not own.
     */
    CXnDomNode* iDomNode;

    /**
     * Pointer to scrollable control adapter
     * Not own.
     */
    CXnScrollableControlAdapter* iScrollableControl;
    
    /**    
     * States, if the nodes width, height or both are content based.
     */
    TInt iAdaptive;

    };

#endif
