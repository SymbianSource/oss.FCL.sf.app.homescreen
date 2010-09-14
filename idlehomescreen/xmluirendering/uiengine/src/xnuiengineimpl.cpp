/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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


// System includes
#include <utf.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <layoutmetadata.cdl.h>
#include <AknPriv.hrh> 
#include <debug.h>

// User includes
#include "xnuiengine.h"
#include "xnuiengineimpl.h"
#include "xnuistatelistener.h"
#include "xnpointerarray.h"
#include "xnodt.h"
#include "xndomlist.h"
#include "xndompropertyvalue.h"
#include "xnmenuadapter.h"
#include "xnpopupcontroladapter.h"
#include "xnviewcontroladapter.h"
#include "xnviewdata.h"
#include "xnnodebreadthfirstiterator.h"
#include "xntype.h"
#include "xnnodepluginif.h"
#include "xncomponentnodeimpl.h"
#include "xnproperty.h"
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnwidgetextensionadapter.h"
#include "xnscrollablecontroladapter.h"
#include "xnkeyeventdispatcher.h"
#include <aknlayoutscalable_avkon.cdl.h>
#include "xuikon_builds_cfg.hrh"
#include "xndomdocument.h"
#include "xndomnode.h"
#include "xneditmode.h"
#include "xnnode.h"
#include "xnpanic.h"
#include "xneffectmanager.h"
#include "xneditor.h"
#include "xnbackgroundmanager.h"
#include "xntexteditor.h"
#include "xnrootdata.h"
#include "xnviewswitcher.h"

#ifdef _XN_PERFORMANCE_TEST_
#include "xntimemon.h"
#endif

#ifdef _XN3_DEBUG_
#include "xndepthfirsttreeiterator.h" // for TraceTreeL
#endif


// CONSTANTS
const TInt KXnBorderThin = 1;
const TInt KXnBorderMedium = 3;
const TInt KXnBorderThick = 5;
const TInt KXnStackPriorityKeyEventDispatcher = 55;
const TInt KFocusGrowValue = 3;

_LIT8( KBoxNodeName, "box" );
_LIT8( KButtonNodeName, "button" );
_LIT8( KWidgetExtensionNodeName, "widgetextension" );
_LIT8( KScrollableBoxNodeName, "scrollablebox" );
_LIT8( KMenuBar, "menubar" );
_LIT8( KPopUpNodeName, "popup" );

_LIT8( KPlugin, "plugin" );

_LIT8( KSplitScreenEnabledTrigger , "splitscreenenabled" );
_LIT8( KSplitScreenDisabledTrigger, "splitscreendisabled" );


// LOCAL CONSTANTS AND MACROS
static const TReal KIntConversionConstant = 0.5;
static const TReal KIntPercentageConstant =
    static_cast< TReal >( 1 ) / static_cast< TReal >( 100 );
static const TReal KInchesAsTwips = 1440;
static const TReal KCentimetersAsInches = static_cast< TReal >( 1 ) / 2.54;
static const TReal KCentimetersAsTwips = KCentimetersAsInches * KInchesAsTwips;
static const TReal KMillimetersAsInches =
    KCentimetersAsInches / static_cast< TReal >( 10 );
static const TReal KMillimetersAsTwips = KMillimetersAsInches * KInchesAsTwips;
static const TReal KPointsAsInches =
    static_cast< TReal >( 1 ) / static_cast< TReal >( 72 );
static const TReal KPointsAsTwips = KPointsAsInches * KInchesAsTwips;
static const TReal KPicasAsInches =
    KPointsAsInches / static_cast< TReal >( 12 );
static const TReal KPicasAsTwips = KPicasAsInches * KInchesAsTwips;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

static TRect AddRect( const TRect& aOriginal, const TRect& aAdded );
static TRect SubtractRect( const TRect& aOriginal, const TRect& aSubtracter );
static TSize AutoStaticBTL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, RArray< TPtrC8 >& aAutoProperties,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static TSize AutoStaticLRL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, RArray< TPtrC8 >& aAutoProperties,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static TSize AutoStaticRLL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, RArray< TPtrC8 >& aAutoProperties,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static TSize AutoStaticTBL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, RArray< TPtrC8 >& aAutoProperties,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static void CalculateAbsolutePositionsL( CXnNode& aNode,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static void CalculateBorderL( CXnNode& aNode,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static void CalculateMarginL( CXnNode& aParent, CXnNode& aNode,
    CXnNode* aPreviousSibling, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels, TInt& aColumnWidth, TInt& aColumnMargin );
static void CalculatePaddingL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );

static void CalculateRectL( CXnNode& aNode, CXnNode& aParent,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );

static void CalculateRelativePositionsL( const TRect& aRect, CXnNode& aNode,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TSize CalculateSpaceUsedByChildrenL( RPointerArray< CXnNode >& aChildren,
    CXnNode& aNode, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, CGraphicsDevice& aScreenDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );

static TSize CalculateTotalDimensionsL( CXnNode& aNode, TBool aIgnoreAutoValues,
    TBool aIgnoreMinSizes, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TBool CutOnePixelFromPercentChildNodeL( CXnNode& aNode,
    const TDesC8& aParentBlockProgression );
static void DetermineBorderWidthsL( CXnProperty* aBorderWidthProperty,
    TInt& aBordervertical, TInt& aBorderhorizontal,
    TRect& aParentRect, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static TInt DisplayedChildrenCountL( CXnNode& aNode,
    RPointerArray< CXnNode >* aDisplayedChildren = NULL );
static CXnNode* DisplayedParentL( CXnNode& aNode );
static CXnDomPropertyValue* DomPropertyValueFromProperty(
    CXnProperty* aProperty );
static void DropExceedingRectL( const TRect& aParentRect, CXnNode& aNode,
    TBool aDropRelativeNode = EFalse );
static TBool CalculateExceedingArea( CXnNode& aParent, CXnNode& aChild,
    TInt& aLeftOffset, TInt& aRightOffset, TInt& aTopOffset,
    TInt& aBottomOffset );
static void AdjustRectsL( CXnNode& aNode, TInt aLeftOffset, TInt aRightOffset,
    TInt aTopOffset, TInt aBottomOffset );
static void ClearRects( CXnNode& aNode, TBool aRecurse = EFalse );
static void FitChildrenIntoParentL( CXnNode& aNode,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TInt GetPositionL( CXnNode& aNode, 
    const TDesC8& aParentBlockProgression, const TDesC8& aParentDirection );
static TInt HasNodeAutoValuesL( CXnNode& aNode );
static TInt HasNodeHorizontalAutoValuesL( CXnNode& aNode );
static TInt HasNodeVerticalAutoValuesL( CXnNode& aNode );
static TBool HasNodeMinSizesL( CXnNode& aNode );
static TBool HasNodeMaxSizesL( CXnNode& aNode );
static TInt HorizontalPixelValueL( CXnProperty* aValue, TInt aReferenceValue,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels );
static TInt HorizontalValueToPixelsL( CXnDomPropertyValue* aValue,
    TReal aReferenceValue, CGraphicsDevice& aScreenDevice,
    TReal aHorizontalUnitInPixels );
static void GrowToMinSizeL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static void ShrinkToMaxSizeL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TBool IsAbsoluteL( CXnNode& aNode );
static TBool IsLargerThanMaxSizeL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TBool IsNodeDisplayedL( CXnNode& aNode );

static TBool IsNodeTooltip( CXnNode& aNode );
static TBool IsPropertyAutoL( CXnProperty& aProperty );
static TBool IsPropertyNone( CXnProperty& aProperty );
static TBool IsRelativeL( CXnNode& aNode );
static TBool IsSmallerThanMinSizeL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static TBool IsValuePercentageL( CXnProperty& aProperty );
static TInt PercentageToPixelsL( TReal& aPercentage, TReal aReferenceValue );
static void PlaceAreasL( CXnNode& aNode,
    RPointerArray< CXnNode >& aLaidOutList,
    TInt aLayoutPhase, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static void PriorizeChildrenL( CXnNode& aNode );
static void CalculateAutoPropertiesOfNodesL(
    RPointerArray< CXnNode >* aAutoNodeArray, TInt& aWidthAutoCount,
    TInt& aHeightAutoCount );
static void ProcessAutoAreasL( CXnNode& aParentNode,
    RPointerArray< CXnNode >* aArray, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels );
static void CalculateCollapseMarginsL( RPointerArray< CXnNode >& aChildren,
    RArray< TInt >& aCollapsedMarginsWidth, 
    RArray< TInt >& aCollapsedMarginsHeight,
    const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection );
static TSize CalculateSpaceUsedByAutoChildrenL(
    RPointerArray< CXnNode >& aAutoChildren,
    RPointerArray< CXnNode >& aCalculatedAutoChildren,
    CXnNode& aParent,
    const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection,
    CGraphicsDevice& aScreenDevice );
static void MoveAutoAreaL( TRect& parentRect,
    RPointerArray< CXnNode >& aChildren,
    const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection );
static TSize ScaleAutoAreaL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );
static void FitAutoChildrenIntoParentL( RPointerArray< CXnNode >& aAutoChildren,
    RPointerArray< CXnNode >& aCalculatedAutoChildren, CXnNode& aParent,
    const TDesC8& aParentBlockProgression, const TDesC8& aParentDirection,
    CGraphicsDevice& aGraphicsDevice );
static TInt VerticalPixelValueL( CXnProperty* aValue, TInt aReferenceValue,
    CGraphicsDevice& aScreenDevice, TReal aVerticalUnitInPixels );
static TInt VerticalValueToPixelsL( CXnDomPropertyValue* aValue,
    TReal aReferenceValue, CGraphicsDevice& aScreenDevice,
    TReal aVerticalUnitInPixels );
static TBool IsStringValueL( CXnProperty& aProperty );
static TInt HorizontalUnitsToPixelsL( TReal aUnits,
    TReal aHorizontalUnitInPixels );
static TInt VerticalUnitsToPixelsL( TReal aUnits, TReal aVerticalUnitInPixels );
static void ProcessAdaptiveAreasL( CXnNode& aParent,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );

static void CalculateAdaptiveSizeL( CXnNode& aNode,
    const TDesC8& aParentBlockProgression, const TDesC8& aParentDirection,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels );

static CXnNode* BuildTriggerNodeLC( CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName );
static void UpdateInternalUnits( TReal&  aHorizontalUnitInPixels,
    TReal& aVerticalUnitInPixels, TRect aRect );
static CXnNode* BuildScreenDeviceChangeTriggerNodeLC(
    CXnUiEngine& aUiEngine );
static void FillFocusCandidatesL( CXnNode* aParent,
    RPointerArray< CXnNode >& aArray );
static TBool SetAdaptivesL( CXnNode& aNode );

static void FindNodeByClassL( CXnNode* aRootNode, const TDesC8& aClassId,
    CXnPointerArray& aArray, const TDesC8& aNamespace );
static CXnNode* FindNodeByIdL( CXnNode* aRootNode, const TDesC8& aAreaId,
    const TDesC8& aNamespace );

#ifdef _XN3_DEBUG_
static void TraceTreeL( CXnNode* aRootNode );
#endif

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GrowIfNeeded()
// When a plugin is focused, the focus is a bit bigger than its control 
// (this is a hack fix to get focus visible with WRT widgets).
// Therefore we need to check if focused node is a plugin, 
// or focused node has same size with its plugin, and grow dirty rect accordingly.
// -----------------------------------------------------------------------------
//
static void GrowIfNeeded(CXnNode* aNode, TRect& aRect)
    {
    CXnNode* focusedNode(NULL);
    for( CXnNode* node = aNode; node; node = node->Parent() )
        {
        if( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
            {
            if( node->DomNode()->Name() == KPlugin )
                {
                CXnControlAdapter* adapter( node->Control() );
                if(adapter)
                    {
                    aRect = adapter->Rect();
                    aRect.Grow( KFocusGrowValue, KFocusGrowValue );
                    }
                break;
                }
            focusedNode = node;
            }
        else if( node->DomNode()->Name() == KPlugin )
            {
            if( focusedNode && focusedNode->MarginRect() == node->Rect() )
                {
                CXnControlAdapter* adapter( focusedNode->Control() );
                if(adapter)
                    {
                    aRect = adapter->Rect();
                    aRect.Grow( KFocusGrowValue, KFocusGrowValue );
                    }
                }
            break;
            }
        }                            
    }      

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static TBool IsSrollableBox( CXnNode& aNode )
    {
    if( aNode.Type()->Type() == KScrollableBoxNodeName )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// FindNodeByClassL()
// Finds recursively a node from current view by class
// -----------------------------------------------------------------------------
//
static void FindNodeByClassL( CXnNode* aRootNode, const TDesC8& aClassId,
    CXnPointerArray& aArray, const TDesC8& aNamespace )
    {
    ASSERT( aRootNode );
    
    if ( aNamespace != KNullDesC8 && aNamespace != aRootNode->Namespace() )
        {
        // Recursed to wrong namespace
        return;
        }
            
    CXnProperty* prop( aRootNode->ClassL() );
    
    if ( prop && prop->StringValue() == aClassId )
        {
        // Correct id found
        if ( aNamespace == KNullDesC8 )
            {
            aArray.Container().AppendL( aRootNode );
            }
        else if ( aNamespace == aRootNode->DomNode()->Namespace() )
            {
            // Namespace is defined and this node is in that namespace 
            // -> this is perfect match
            aArray.Container().AppendL( aRootNode );
            }        
        }
    
    // Recurse children
    RPointerArray< CXnNode >& children( aRootNode->Children() );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        FindNodeByClassL( children[i], aClassId, aArray, aNamespace );
        }
    }

// -----------------------------------------------------------------------------
// FindNodeByIdL()
// Finds recursively a node from current view by id
// -----------------------------------------------------------------------------
//
static CXnNode* FindNodeByIdL( CXnNode* aRootNode, const TDesC8& aAreaId,        
    const TDesC8& aNamespace )
    {
    ASSERT( aRootNode );
    
    if ( aNamespace != KNullDesC8 && aNamespace != aRootNode->Namespace() )
        {
        // Recursed to wrong namespace
        return NULL;
        }
                
    CXnProperty* prop( aRootNode->IdL() );
    
    if ( prop && prop->StringValue() == aAreaId )
        {
        // Correct id found
        if ( aNamespace == KNullDesC8 )
            {
            // No namespace defined, so this is the first found best match
            return aRootNode;
            }
        else if ( aNamespace == aRootNode->DomNode()->Namespace() )
            {
            // Namespace is defined and this node is in that namespace 
            // -> this is perfect match
            return aRootNode;
            }        
        }
        
    // No match found, recurse children
    RPointerArray< CXnNode >& children( aRootNode->Children() );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* retval( FindNodeByIdL( children[i], aAreaId, aNamespace ) );
        
        if ( retval )
            {
            return retval;
            }
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// AutoStaticVerticalBTL()
// Calculate properties set to AUTO when positioning is static, layout vertical
// and direction right-to-left
// -----------------------------------------------------------------------------
//
static TSize AutoStaticBTL( CXnNode& aNode, TInt aWidthToFit, TInt aHeightToFit,
    RArray< TPtrC8 >& aAutoProperties, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // Static positioning, vertical, right to left
    TBool horizontalMarginProcessed = EFalse;
    TBool verticalMarginProcessed = EFalse;
    TBool allVerticalValuesProcessed = EFalse;
    TBool allHorizontalValuesProcessed = EFalse;

    CXnProperty* maxHeightProperty = aNode.MaxHeightL();
    CXnProperty* maxWidthProperty = aNode.MaxWidthL();
    CXnProperty* minHeightProperty = aNode.MinHeightL();
    CXnProperty* minWidthProperty = aNode.MinWidthL();

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );

    TSize size = CalculateTotalDimensionsL( aNode, EFalse, ETrue, aParentRect,
        aGraphicsDevice, aHorizontalUnitInPixels, aVerticalUnitInPixels );

    for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
        {
        // search for height property
        const TDesC8& attributeName = aAutoProperties[i];
        if ( attributeName == XnPropertyNames::style::common::KHeight )
            {
            // height was AUTO, all other vertical auto values are 0
            TInt newHeight = aHeightToFit - size.iHeight;

            //  Calculate the difference so that it can be returned
            TRect paddingRectDiff( SubtractRect( paddingRect, contentRect ) );
            TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
            TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
            TRect normalFlowBorderRectDiff( SubtractRect ( normalFlowBorderRect,
                contentRect ) );

            // grow height to fill the void
            contentRect.SetHeight( newHeight );
            paddingRect = AddRect( contentRect, paddingRectDiff );
            borderRect = AddRect( contentRect, borderRectDiff );
            normalFlowBorderRect = AddRect( contentRect,
                normalFlowBorderRectDiff );
            marginRect = AddRect( contentRect, marginRectDiff );

            size = TSize( size.iWidth, aHeightToFit );
            }
        }

    // if exactly one value is AUTO, value is computed to make equality true
    if ( aAutoProperties.Count() == 1 )
        {
        if ( !allHorizontalValuesProcessed )
            {
            const TDesC8& attributeName = aAutoProperties[0];
            if ( attributeName == XnPropertyNames::style::common::KWidth )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect( paddingRect,
                    contentRect ) );
                TRect borderRectDiff( SubtractRect( borderRect,
                    contentRect ) );
                TRect marginRectDiff( SubtractRect( marginRect,
                    contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow width to fill the void
                contentRect.SetWidth( remainingWidth );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect( contentRect,
                    normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );

                size = TSize( aWidthToFit, size.iHeight );
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginLeft )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                contentRect.Move( remainingWidth, 0 );
                borderRect.Move( remainingWidth, 0 );
                normalFlowBorderRect.Move( remainingWidth, 0 );
                marginRect.Resize( remainingWidth, 0 );
                paddingRect.Move( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                marginRect.Resize( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                marginRect.Resize( 0, remainingHeight );
                marginRect.Move( 0, -remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }

            if ( attributeName == 
                 XnPropertyNames::style::common::KMarginBottom )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                marginRect.Resize( 0, remainingHeight );
                marginRect.Move( 0, -remainingHeight );
                paddingRect.Move( 0, -remainingHeight );
                borderRect.Move( 0, -remainingHeight );
                normalFlowBorderRect.Move( 0, -remainingHeight );
                contentRect.Move( 0, -remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }
            }
        }
    else
        {
        for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
            {
            const TDesC8& attributeName = aAutoProperties[i];
            if ( attributeName == XnPropertyNames::style::common::KWidth )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect(
                    borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect(
                    marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow width to fill the void
                contentRect.SetWidth( remainingWidth );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect(
                    contentRect, normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );

                size = TSize( aWidthToFit, size.iHeight );
                }

            // several auto properties
            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is right margin also set to AUTO?
                    CXnProperty* rightMargin = aNode.MarginRightL();
                    if ( rightMargin )
                        {
                        if ( IsPropertyAutoL( *rightMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( ( remainingWidth / 2 ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( ( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move( (
                                remainingWidth / 2 ), 0 );
                            paddingRect.Move( remainingWidth / 2, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only left margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( ( remainingWidth ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( remainingWidth, 0 );
                            normalFlowBorderRect.Move( remainingWidth, 0 );
                            paddingRect.Move( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }
            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is left margin also set to AUTO?
                    CXnProperty* leftMargin = aNode.MarginLeftL();
                    if ( leftMargin )
                        {
                        if ( IsPropertyAutoL( *leftMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            contentRect.Move( remainingWidth / 2, 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( ( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move( ( 
                                remainingWidth / 2 ), 0 );
                            paddingRect.Move( remainingWidth / 2, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only right margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            marginRect.Resize( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }
            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                if ( !verticalMarginProcessed &&
                     !allVerticalValuesProcessed )
                    {
                    // is bottom margin also set to AUTO?
                    CXnProperty* bottomMargin = aNode.MarginBottomL();
                    if ( bottomMargin )
                        {
                        if ( IsPropertyAutoL( *bottomMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            contentRect.Move( 0, remainingHeight / 2 );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, ( remainingHeight / 2 ) );
                            normalFlowBorderRect.Move(
                                0, ( remainingHeight / 2 ) );
                            paddingRect.Move( 0, remainingHeight / 2 );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only top margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            marginRect.Resize( 0, remainingHeight );
                            marginRect.Move( 0, -remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }
            if ( attributeName == 
                 XnPropertyNames::style::common::KMarginBottom )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is top margin also set to AUTO?
                    CXnProperty* topMargin = aNode.MarginTopL();
                    if ( topMargin )
                        {
                        if ( IsPropertyAutoL( *topMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight =  aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            contentRect.Move( 0, -( remainingHeight / 2 ) );
                            marginRect.Resize( 0, remainingHeight );
                            marginRect.Move( 0, -remainingHeight );
                            borderRect.Move( 0, -( remainingHeight / 2 ) );
                            normalFlowBorderRect.Move(
                                0, -( remainingHeight / 2 ) );
                            paddingRect.Move( 0, -( remainingHeight / 2 ) );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only bottom margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            contentRect.Move( 0, -remainingHeight );
                            marginRect.Resize( 0, remainingHeight );
                            marginRect.Move( 0, -remainingHeight );
                            borderRect.Move( 0, -remainingHeight );
                            normalFlowBorderRect.Move(
                                0, -remainingHeight );
                            paddingRect.Move( 0, -remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }
            }
        }
    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    return contentRect.Size();
    }

// -----------------------------------------------------------------------------
// AutoStaticLRL()
// Calculate properties set to AUTO when positioning is static,
// layout horizontal
// and direction left-to-right
// -----------------------------------------------------------------------------
//
static TSize AutoStaticLRL( CXnNode& aNode, TInt aWidthToFit, TInt aHeightToFit,
    RArray< TPtrC8 >& aAutoProperties, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // Static positioning, horizontal, left to right
    TBool horizontalMarginProcessed = EFalse;
    TBool verticalMarginProcessed = EFalse;
    TBool allVerticalValuesProcessed = EFalse;
    TBool allHorizontalValuesProcessed = EFalse;

    CXnProperty* maxHeightProperty = aNode.MaxHeightL();
    CXnProperty* maxWidthProperty = aNode.MaxWidthL();
    CXnProperty* minHeightProperty = aNode.MinHeightL();
    CXnProperty* minWidthProperty = aNode.MinWidthL();

    TRect contentRect = aNode.Rect();
    TRect paddingRect = aNode.PaddingRect();
    TRect borderRect = aNode.BorderRect();
    TRect marginRect = aNode.MarginRect();
    TRect normalFlowBorderRect = aNode.NormalFlowBorderRect();
    TSize size = CalculateTotalDimensionsL( aNode, EFalse, ETrue, aParentRect,
        aGraphicsDevice, aHorizontalUnitInPixels, aVerticalUnitInPixels );

    for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
        {
        // search for width property
        const TDesC8& attributeName = aAutoProperties[i];
        if ( attributeName == XnPropertyNames::style::common::KWidth )
            {
            // width was AUTO, all other vertical auto values are 0
            TInt newWidth = aWidthToFit - size.iWidth;

            //  Calculate the difference so that it can be returned
            TRect paddingRectDiff( SubtractRect( paddingRect, contentRect ) );
            TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
            TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
            TRect normalFlowBorderRectDiff( SubtractRect( normalFlowBorderRect,
                contentRect ) );

            // grow width to fill the void
            contentRect.SetWidth( newWidth );
            paddingRect = AddRect( contentRect, paddingRectDiff );
            borderRect = AddRect( contentRect, borderRectDiff );
            normalFlowBorderRect = AddRect(
                contentRect, normalFlowBorderRectDiff );
            marginRect = AddRect( contentRect, marginRectDiff );

            size = TSize( aWidthToFit, size.iHeight );
            allHorizontalValuesProcessed = ETrue;
            }
        }

        // if exactly one value is AUTO, value is computed to make equality true
    if ( aAutoProperties.Count() == 1 )
        {
        if ( !allHorizontalValuesProcessed )
            {
            const TDesC8& attributeName = aAutoProperties[0];
            if ( attributeName == XnPropertyNames::style::common::KHeight )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect(
                    borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect(
                    marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow height to fill the void
                contentRect.SetHeight( remainingHeight );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect(
                    contentRect, normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );
                size = TSize( size.iWidth, aHeightToFit );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                // move rect right to fill the void
                contentRect.Move( remainingWidth, 0 );
                paddingRect.Move( remainingWidth, 0 );
                borderRect.Move( remainingWidth, 0 );
                normalFlowBorderRect.Move( remainingWidth, 0 );
                marginRect.Resize( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                marginRect.Resize( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                // move rect right to fill the void
                contentRect.Move( 0, remainingHeight / 2 );
                paddingRect.Move( 0, remainingHeight / 2 );
                borderRect.Move( 0, remainingHeight / 2 );
                normalFlowBorderRect.Move( 0, remainingHeight / 2 );
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }
            }
        }
    else
        {
        for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
            {
            const TDesC8& attributeName = aAutoProperties[i];
            if ( attributeName == XnPropertyNames::style::common::KHeight )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect(
                    borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect(
                    marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow height to fill the void
                contentRect.SetHeight( remainingHeight );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect(
                    contentRect, normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );
                size = TSize( size.iWidth, aHeightToFit );
                }

            // several auto properties
            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is right margin also set to AUTO?
                    CXnProperty* rightMargin = aNode.MarginRightL();
                    if ( rightMargin )
                        {
                        if ( IsPropertyAutoL( *rightMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( ( remainingWidth / 2 ), 0 );
                            horizontalMarginProcessed = ETrue;
                            marginRect.Resize( ( remainingWidth ), 0 );
                            borderRect.Move( ( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move( (
                                remainingWidth / 2 ), 0 );
                            paddingRect.Move( ( remainingWidth / 2 ), 0 );
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only left margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            marginRect.Move( remainingWidth, 0 );
                            borderRect.Move( remainingWidth, 0 );
                            normalFlowBorderRect.Move( remainingWidth, 0 );
                            paddingRect.Move( remainingWidth, 0 );
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is left margin also set to AUTO?
                    CXnProperty* leftMargin = aNode.MarginLeftL();
                    if ( leftMargin )
                        {
                        if ( IsPropertyAutoL( *leftMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( ( remainingWidth / 2 ), 0 );
                            horizontalMarginProcessed = ETrue;
                            marginRect.Resize( ( remainingWidth ), 0 );
                            borderRect.Move( ( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move( (
                                remainingWidth / 2 ), 0 );
                            paddingRect.Move( ( remainingWidth / 2 ), 0 );
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only right margin set to AUTO
                            horizontalMarginProcessed = ETrue;
                            }
                        }
                    }
                }
            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is bottom margin also set to AUTO?
                    CXnProperty* bottomMargin = aNode.MarginBottomL();
                    if ( bottomMargin )
                        {
                        if ( IsPropertyAutoL( *bottomMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight =  aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, ( remainingHeight / 2 ) );
                            verticalMarginProcessed = ETrue;
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, ( remainingHeight / 2 ) );
                            normalFlowBorderRect.Move(
                                0, ( remainingHeight / 2 ) );
                            paddingRect.Move( 0, ( remainingHeight / 2 ) );
                            size = TSize( size.iWidth, 
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only top margin set to AUTO
                            TInt remainingHeight =  aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight );
                            normalFlowBorderRect.Move( 0, remainingHeight );
                            paddingRect.Move( 0, remainingHeight );
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                 }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is top margin also set to AUTO?
                    CXnProperty* topMargin = aNode.MarginTopL();
                    if ( topMargin )
                        {
                        if ( IsPropertyAutoL( *topMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight =  aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, remainingHeight / 2 );
                            verticalMarginProcessed = ETrue;
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight / 2 );
                            normalFlowBorderRect.Move(
                                0, remainingHeight / 2 );
                            paddingRect.Move( 0, remainingHeight / 2 );
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only bottom margin set to AUTO
                            TInt remainingHeight =  aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            marginRect.Resize( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth, 
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }
            }
        }
    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    return contentRect.Size();
    }

// -----------------------------------------------------------------------------
// AutoStaticRLL()
// Calculate properties set to AUTO when positioning is static,
// layout horizontal
// and direction right-to-left.
// -----------------------------------------------------------------------------
//
static TSize AutoStaticRLL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, RArray< TPtrC8 >& aAutoProperties, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // Static positioning, horizontal, right to left
    TBool horizontalMarginProcessed = EFalse;
    TBool verticalMarginProcessed = EFalse;
    TBool allVerticalValuesProcessed = EFalse;
    TBool allHorizontalValuesProcessed = EFalse;

    CXnProperty* maxHeightProperty = aNode.MaxHeightL();
    CXnProperty* maxWidthProperty = aNode.MaxWidthL();
    CXnProperty* minHeightProperty = aNode.MinHeightL();
    CXnProperty* minWidthProperty = aNode.MinWidthL();

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );

    TSize size = CalculateTotalDimensionsL( aNode, EFalse, ETrue, aParentRect,
        aGraphicsDevice, aHorizontalUnitInPixels, aVerticalUnitInPixels );

    for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
        {
        // search for height property
        const TDesC8& attributeName = aAutoProperties[i];
        if ( attributeName == XnPropertyNames::style::common::KWidth )
            {
            TInt newWidth = aWidthToFit - size.iWidth;

            //  Calculate the difference so that it can be returned
            TRect paddingRectDiff( SubtractRect( paddingRect, contentRect ) );
            TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
            TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
            TRect normalFlowBorderRectDiff( SubtractRect( normalFlowBorderRect,
                contentRect ) );

            // grow width to fill the void
            contentRect.SetWidth( newWidth );
            paddingRect = AddRect( contentRect, paddingRectDiff );
            borderRect = AddRect( contentRect, borderRectDiff );
            normalFlowBorderRect = AddRect( contentRect,
                normalFlowBorderRectDiff );
            marginRect = AddRect( contentRect, marginRectDiff );

            size = TSize( aWidthToFit, size.iHeight );
            allHorizontalValuesProcessed = ETrue;
            }
        }

    // if exactly one value is AUTO, value is computed to make equality true
    if ( aAutoProperties.Count() == 1 )
        {
        if ( !allHorizontalValuesProcessed )
            {
            const TDesC8& attributeName = aAutoProperties[0];
            if ( attributeName == XnPropertyNames::style::common::KHeight )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow height to fill the void
                contentRect.SetHeight( remainingHeight );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect( contentRect,
                    normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );

                size = TSize( size.iWidth, aHeightToFit );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                // move rect right to fill the void
                contentRect.Move( remainingWidth, 0 );
                paddingRect.Move( remainingWidth, 0 );
                borderRect.Move( remainingWidth, 0 );
                normalFlowBorderRect.Move( remainingWidth, 0 );
                marginRect.Resize( remainingWidth, 0 );
                marginRect.Move( -remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                contentRect.Move( -remainingWidth, 0 );
                paddingRect.Move( -remainingWidth, 0 );
                borderRect.Move( -remainingWidth, 0 );
                normalFlowBorderRect.Move( -remainingWidth, 0 );
                marginRect.Move( -remainingWidth, 0 );
                marginRect.Resize( remainingWidth, 0 );

                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                // move rect right to fill the void
                contentRect.Move( 0, remainingHeight / 2 );
                paddingRect.Move( 0, remainingHeight / 2 );
                borderRect.Move( 0, remainingHeight / 2 );
                normalFlowBorderRect.Move( 0, remainingHeight / 2 );
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }
            }
        }
    else
        {
        for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
            {
            const TDesC8& attributeName = aAutoProperties[i];
            if ( attributeName == XnPropertyNames::style::common::KHeight )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow height to fill the void
                contentRect.SetHeight( remainingHeight );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect( contentRect,
                    normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );

                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }

            // several auto properties
            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is right margin also set to AUTO?
                    CXnProperty* rightMargin = aNode.MarginRightL();
                    if ( rightMargin )
                        {
                        if ( IsPropertyAutoL( *rightMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect left
                            contentRect.Move( -( remainingWidth / 2 ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            marginRect.Move( -remainingWidth, 0 );
                            borderRect.Move( -( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move(
                                -( remainingWidth / 2 ), 0 );
                            paddingRect.Move( -( remainingWidth / 2 ), 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only left margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect left
                            contentRect.Move( ( remainingWidth ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            marginRect.Move( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is left margin also set to AUTO?
                    CXnProperty* leftMargin = aNode.MarginLeftL();
                    if ( leftMargin )
                        {
                        if ( IsPropertyAutoL( *leftMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            contentRect.Move( -( remainingWidth / 2 ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            marginRect.Move( -remainingWidth, 0 );
                            borderRect.Move( -( remainingWidth / 2 ), 0 );
                            normalFlowBorderRect.Move(
                                -( remainingWidth / 2 ), 0 );
                            paddingRect.Move( -( remainingWidth / 2 ), 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only right margin set to AUTO
                            TInt usedWidth = size.iWidth;
                            TInt remainingWidth = aWidthToFit - usedWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            contentRect.Move( -remainingWidth, 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            marginRect.Move( -remainingWidth, 0 );
                            borderRect.Move( -remainingWidth, 0 );
                            normalFlowBorderRect.Move( -remainingWidth, 0 );
                            paddingRect.Move( -remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }
            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is bottom margin also set to AUTO?
                    CXnProperty* bottomMargin = aNode.MarginBottomL();
                    if ( bottomMargin )
                        {
                        if ( IsPropertyAutoL( *bottomMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            contentRect.Move( 0, ( remainingHeight / 2 ) );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, ( remainingHeight / 2 ) );
                            normalFlowBorderRect.Move(
                                0, ( remainingHeight / 2 ) );
                            paddingRect.Move( 0, ( remainingHeight / 2 ) );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only top margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            contentRect.Move( 0, remainingHeight );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight );
                            normalFlowBorderRect.Move( 0, remainingHeight );
                            paddingRect.Move( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is top margin also set to AUTO?
                    CXnProperty* topMargin = aNode.MarginTopL();
                    if ( topMargin )
                        {
                        if ( IsPropertyAutoL( *topMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, ( remainingHeight / 2 ) );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, ( remainingHeight / 2 ) );
                            normalFlowBorderRect.Move(
                                0, ( remainingHeight / 2 ) );
                            paddingRect.Move( 0, ( remainingHeight / 2 ) );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only bottom margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            marginRect.Resize( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }
            }
        }
    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    return contentRect.Size();
    }

// -----------------------------------------------------------------------------
// SubtractRect()
// Calculate the difference of two rectangles and return it as
// -----------------------------------------------------------------------------
//
static TRect SubtractRect( const TRect& aOriginal, const TRect& aSubtracter )
    {
    return TRect( aOriginal.iTl.iX - aSubtracter.iTl.iX,
        aOriginal.iTl.iY - aSubtracter.iTl.iY,
        aOriginal.iBr.iX - aSubtracter.iBr.iX,
        aOriginal.iBr.iY - aSubtracter.iBr.iY );
    }

// -----------------------------------------------------------------------------
// AddRect()
// Add two rectangles together and return the result
// -----------------------------------------------------------------------------
//
static TRect AddRect( const TRect& aOriginal, const TRect& aAdded )
    {
    return TRect( aOriginal.iTl.iX + aAdded.iTl.iX,
        aOriginal.iTl.iY + aAdded.iTl.iY,
        aOriginal.iBr.iX + aAdded.iBr.iX,
        aOriginal.iBr.iY + aAdded.iBr.iY );
    }

// -----------------------------------------------------------------------------
// AutoStaticTBL()
// Calculate properties set to AUTO when positioning is static, layout vertical
// and direction left-to-right
// -----------------------------------------------------------------------------
//
static TSize AutoStaticTBL( CXnNode& aNode, TInt aWidthToFit, TInt aHeightToFit,
    RArray< TPtrC8 >& aAutoProperties, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // Static positioning, vertical, left to right
    TBool horizontalMarginProcessed( EFalse );
    TBool verticalMarginProcessed( EFalse );
    TBool allVerticalValuesProcessed( EFalse );

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );

    TSize size( CalculateTotalDimensionsL( aNode, EFalse, ETrue, aParentRect,
        aGraphicsDevice, aHorizontalUnitInPixels, aVerticalUnitInPixels ) );

    for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
        {
        // search for height property
        const TDesC8& attributeName = aAutoProperties[i];
        if ( attributeName == XnPropertyNames::style::common::KHeight )
            {
            //  Calculate the difference so that it can be returned
            TRect paddingRectDiff( SubtractRect( paddingRect, contentRect ) );
            TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
            TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
            TRect normalFlowBorderRectDiff( SubtractRect( normalFlowBorderRect,
                contentRect ) );

            // grow height to fill the void
            contentRect.SetHeight( aHeightToFit - size.iHeight );
            paddingRect = AddRect( contentRect, paddingRectDiff );
            borderRect = AddRect( contentRect, borderRectDiff );
            normalFlowBorderRect = AddRect(
                contentRect, normalFlowBorderRectDiff );
            marginRect = AddRect( contentRect, marginRectDiff );
            size = TSize( size.iWidth, aHeightToFit );
            allVerticalValuesProcessed = ETrue;
            }
        }

    // if exactly one value is AUTO, value is computed to make equality true
    if ( aAutoProperties.Count() == 1 )
        {
        if ( !allVerticalValuesProcessed )
            {
            const TDesC8& attributeName = aAutoProperties[0];
            if ( attributeName == XnPropertyNames::style::common::KWidth )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow width to fill the void
                contentRect.SetWidth( remainingWidth );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect( contentRect,
                    normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );
                size = TSize( aWidthToFit, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                // move rect right to fill the void
                contentRect.Move( remainingWidth, 0 );
                paddingRect.Move( remainingWidth, 0 );
                borderRect.Move( remainingWidth, 0 );
                normalFlowBorderRect.Move( remainingWidth, 0 );
                marginRect.Resize( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                marginRect.Resize( remainingWidth, 0 );
                size = TSize( size.iWidth + remainingWidth, size.iHeight );
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                // move rect right to fill the void
                contentRect.Move( 0, remainingHeight / 2 );
                paddingRect.Move( 0, remainingHeight / 2 );
                borderRect.Move( 0, remainingHeight / 2 );
                normalFlowBorderRect.Move( 0, remainingHeight / 2 );
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                TInt remainingHeight = aHeightToFit - size.iHeight;
                if ( remainingHeight < 0 )
                    {
                    remainingHeight = 0;
                    }
                // set margin rect
                marginRect.Resize( 0, remainingHeight );
                size = TSize( size.iWidth, size.iHeight + remainingHeight );
                }
            }
        }
    else
        {
        for ( TInt i = 0; i < aAutoProperties.Count(); i++ )
            {
            const TDesC8& attributeName = aAutoProperties[i];
            if ( attributeName == XnPropertyNames::style::common::KWidth )
                {
                TInt remainingWidth = aWidthToFit - size.iWidth;
                if ( remainingWidth < 0 )
                    {
                    remainingWidth = 0;
                    }
                //  Calculate the difference so that it can be returned
                TRect paddingRectDiff( SubtractRect(
                    paddingRect, contentRect ) );
                TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
                TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
                TRect normalFlowBorderRectDiff( SubtractRect(
                    normalFlowBorderRect, contentRect ) );

                // grow width to fill the void
                contentRect.SetWidth( remainingWidth );
                paddingRect = AddRect( contentRect, paddingRectDiff );
                borderRect = AddRect( contentRect, borderRectDiff );
                normalFlowBorderRect = AddRect( contentRect,
                    normalFlowBorderRectDiff );
                marginRect = AddRect( contentRect, marginRectDiff );
                size = TSize( aWidthToFit, size.iHeight );
                }

            // several auto properties
            if ( attributeName == XnPropertyNames::style::common::KMarginLeft )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is right margin also set to AUTO?
                    CXnProperty* rightMargin = aNode.MarginRightL();
                    if ( rightMargin )
                        {
                        if ( IsPropertyAutoL( *rightMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move(( remainingWidth / 2 ), 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( remainingWidth / 2, 0 );
                            normalFlowBorderRect.Move( remainingWidth / 2, 0 );
                            paddingRect.Move( remainingWidth / 2, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only left margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( remainingWidth, 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( remainingWidth, 0 );
                            normalFlowBorderRect.Move( remainingWidth, 0 );
                            paddingRect.Move( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginRight )
                {
                if ( !horizontalMarginProcessed )
                    {
                    // is left margin also set to AUTO?
                    CXnProperty* leftMargin = aNode.MarginLeftL();
                    if ( leftMargin )
                        {
                        if ( IsPropertyAutoL (*leftMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( remainingWidth / 2, 0 );
                            marginRect.Resize( remainingWidth, 0 );
                            borderRect.Move( remainingWidth / 2, 0 );
                            normalFlowBorderRect.Move( remainingWidth / 2, 0 );
                            paddingRect.Move( remainingWidth / 2, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        else
                            {
                            // only right margin set to AUTO
                            TInt remainingWidth = aWidthToFit - size.iWidth;
                            if ( remainingWidth < 0 )
                                {
                                remainingWidth = 0;
                                }
                            // move rect right to fill the void
                            marginRect.Resize( remainingWidth, 0 );
                            horizontalMarginProcessed = ETrue;
                            size = TSize( size.iWidth + remainingWidth,
                                size.iHeight );
                            }
                        }
                    }
                }

            if ( attributeName == XnPropertyNames::style::common::KMarginTop )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is bottom margin also set to AUTO?
                    CXnProperty* bottomMargin = aNode.MarginBottomL();
                    if ( bottomMargin )
                        {
                        if ( IsPropertyAutoL( *bottomMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, remainingHeight / 2 );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight / 2 );
                            normalFlowBorderRect.Move( 0, remainingHeight / 2 );
                            paddingRect.Move( 0, remainingHeight / 2 );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only top margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            contentRect.Move( 0, remainingHeight );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight );
                            normalFlowBorderRect.Move( 0, remainingHeight );
                            paddingRect.Move( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }

            if ( attributeName ==
                 XnPropertyNames::style::common::KMarginBottom )
                {
                if ( !verticalMarginProcessed && !allVerticalValuesProcessed )
                    {
                    // is top margin also set to AUTO?
                    CXnProperty* topMargin = aNode.MarginTopL();
                    if ( topMargin )
                        {
                        if ( IsPropertyAutoL( *topMargin ) )
                            {
                            // both margins set to AUTO, values equal
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            TRect normalFlowBorderRect(
                                aNode.NormalFlowBorderRect() );
                            contentRect.Move( 0, remainingHeight / 2 );
                            marginRect.Resize( 0, remainingHeight );
                            borderRect.Move( 0, remainingHeight / 2 );
                            normalFlowBorderRect.Move( 0, remainingHeight / 2 );
                            paddingRect.Move( 0, remainingHeight / 2 );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        else
                            {
                            // only bottom margin set to AUTO
                            TInt remainingHeight = aHeightToFit - size.iHeight;
                            if ( remainingHeight < 0 )
                                {
                                remainingHeight = 0;
                                }
                            // move rect right to fill the void
                            marginRect.Resize( 0, remainingHeight );
                            verticalMarginProcessed = ETrue;
                            size = TSize( size.iWidth,
                                size.iHeight + remainingHeight );
                            }
                        }
                    }
                }
            }
        }
    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    return contentRect.Size();
    }

// -----------------------------------------------------------------------------
// CalculateAbsolutePositionsL()
// Move the rect by it's absolute positioning.
// -----------------------------------------------------------------------------
//
static void CalculateAbsolutePositionsL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    CXnProperty* topProperty = aNode.TopL();
    CXnProperty* bottomProperty = aNode.BottomL();
    CXnProperty* leftProperty = aNode.LeftL();
    CXnProperty* rightProperty = aNode.RightL();

    TBool verticalValuesProcessed = EFalse;
    TBool horizontalValuesProcessed = EFalse;

    TInt top = 0;
    TInt bottom = 0;
    TInt left = 0;
    TInt right = 0;

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );

    if ( HasNodeAutoValuesL( aNode ) )
        {
        CXnProperty* marginTopProperty = aNode.MarginTopL();
        CXnProperty* marginBottomProperty = aNode.MarginBottomL();
        CXnProperty* marginLeftProperty = aNode.MarginLeftL();
        CXnProperty* marginRightProperty = aNode.MarginRightL();
        CXnProperty* heightProperty = aNode.HeightL();
        CXnProperty* widthProperty = aNode.WidthL();
        CXnProperty* maxHeightProperty = aNode.MaxHeightL();
        CXnProperty* maxWidthProperty = aNode.MaxWidthL();
        CXnProperty* minHeightProperty = aNode.MinHeightL();
        CXnProperty* minWidthProperty = aNode.MinWidthL();

        // move auto value node to it's parent's top left corner
        TInt offsetx = 0;
        TInt offsety = 0;

        offsetx = aParentRect.iTl.iX - marginRect.iTl.iX;
        offsety = aParentRect.iTl.iY - marginRect.iTl.iY;

        contentRect.Move( offsetx, offsety );
        paddingRect.Move( offsetx, offsety );
        borderRect.Move( offsetx, offsety );
        marginRect.Move( offsetx, offsety );

        // get values
        if ( topProperty && !IsPropertyAutoL( *topProperty ) &&
             !IsPropertyNone( *topProperty ) )
            {
            top = VerticalPixelValueL( topProperty, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        if ( bottomProperty && !IsPropertyAutoL( *bottomProperty ) &&
             !IsPropertyNone( *bottomProperty ) )
            {
            bottom = VerticalPixelValueL( bottomProperty, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        if ( leftProperty && !IsPropertyAutoL( *leftProperty ) &&
             !IsPropertyNone( *leftProperty ) )
            {
            left = HorizontalPixelValueL( leftProperty, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        if ( rightProperty && !IsPropertyAutoL( *rightProperty ) &&
             !IsPropertyNone( *rightProperty ) )
            {
            right = HorizontalPixelValueL( rightProperty, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        // node has auto values
        TSize spaceUsed( CalculateTotalDimensionsL( aNode, EFalse, EFalse,
            aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels ) );
        TInt heightToUse =
            aParentRect.Height() - spaceUsed.iHeight - top - bottom;
        TInt widthToUse =
            aParentRect.Width() - spaceUsed.iWidth - left - right;

        if ( heightToUse < 0 )
            {
            heightToUse = 0;
            }
        if ( widthToUse < 0 )
            {
            widthToUse = 0;
            }
        // vertical auto values
        // (margin top, margin bottom, height, top, bottom)
        if ( IsPropertyAutoL( *heightProperty ) )
            {
            contentRect.Resize( 0, heightToUse );
            paddingRect.Resize( 0, heightToUse );
            borderRect.Resize( 0, heightToUse );
            marginRect.Resize( 0, heightToUse );

            if ( IsLargerThanMaxSizeL( aParentRect, aNode, aGraphicsDevice,
                 aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
                {
                if ( !IsPropertyNone( *maxHeightProperty ) )
                    {
                    TInt maxheight = HorizontalPixelValueL( maxHeightProperty,
                        aParentRect.Height(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    if ( maxheight < contentRect.Height() )
                        {
                        TInt excessheight = contentRect.Height() - maxheight;
                        contentRect.Resize( 0, -excessheight );
                        paddingRect.Resize( 0, -excessheight );
                        borderRect.Resize( 0, -excessheight );
                        marginRect.Resize( 0, -excessheight );
                        }
                    }
                }
            if ( IsSmallerThanMinSizeL( aParentRect, aNode, aGraphicsDevice,
                 aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
                {
                if ( !IsPropertyNone( *minHeightProperty ) )
                    {
                    TInt minheight = HorizontalPixelValueL( minHeightProperty,
                        aParentRect.Height(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    if ( minheight > contentRect.Height() )
                        {
                        TInt missingheight = minheight - contentRect.Height();
                        contentRect.Resize( 0, missingheight );
                        paddingRect.Resize( 0, missingheight );
                        borderRect.Resize( 0, missingheight );
                        marginRect.Resize( 0, missingheight );
                        }
                    }
                }
            verticalValuesProcessed = ETrue;
            }

        if ( !verticalValuesProcessed )
            {
            if ( IsPropertyAutoL( *topProperty ) &&
                 IsPropertyAutoL( *bottomProperty ) )
                {
                // move the box down
                contentRect.Move( 0, heightToUse / 2 );
                paddingRect.Move( 0, heightToUse / 2 );
                borderRect.Move( 0, heightToUse / 2 );
                marginRect.Move( 0, heightToUse / 2 );
                }
            if ( IsPropertyAutoL( *topProperty ) &&
                 !IsPropertyAutoL( *bottomProperty ) )
                {
                // move the box down
                contentRect.Move( 0, heightToUse );
                paddingRect.Move( 0, heightToUse );
                borderRect.Move( 0, heightToUse );
                marginRect.Move( 0, heightToUse );
                }
            if ( IsPropertyAutoL( *marginTopProperty ) &&
                 IsPropertyAutoL( *marginBottomProperty ) )
                {
                // both margins auto, equal values
                marginRect.Resize( 0, heightToUse );
                paddingRect.Move( 0, heightToUse / 2 );
                borderRect.Move( 0, heightToUse / 2 );
                contentRect.Move( 0, heightToUse / 2 );
                }
            if ( !IsPropertyAutoL( *marginTopProperty ) &&
                 IsPropertyAutoL( *marginBottomProperty ) )
                {
                // only top margin auto
                marginRect.Resize( 0, heightToUse );
                paddingRect.Move( 0, heightToUse );
                borderRect.Move( 0, heightToUse );
                contentRect.Move( 0, heightToUse );
                }
            if ( IsPropertyAutoL( *marginTopProperty ) &&
                 !IsPropertyAutoL( *marginBottomProperty ) )
                {
                // only bottom margin auto
                marginRect.Resize( 0, heightToUse );
                }
            }

        // horizontal auto values
        // (margin left, margin right, width)
        if ( IsPropertyAutoL( *widthProperty ) )
            {
            contentRect.Resize( widthToUse, 0 );
            paddingRect.Resize( widthToUse, 0 );;
            borderRect.Resize( widthToUse, 0 );
            marginRect.Resize( widthToUse, 0 );
            if ( IsLargerThanMaxSizeL( aParentRect, aNode, aGraphicsDevice,
                 aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
                {
                if ( !IsPropertyNone( *maxWidthProperty ) )
                    {
                    TInt maxwidth = HorizontalPixelValueL( maxWidthProperty,
                        aParentRect.Width(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    if ( maxwidth < contentRect.Width() )
                        {
                        TInt excesswidth = contentRect.Width() - maxwidth;
                        contentRect.Resize( -excesswidth, 0 );
                        paddingRect.Resize( -excesswidth, 0 );
                        borderRect.Resize( -excesswidth, 0 );
                        marginRect.Resize( -excesswidth, 0 );
                        }
                    }
                }
            if ( IsSmallerThanMinSizeL( aParentRect, aNode, aGraphicsDevice,
                 aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
                {
                if ( !IsPropertyNone( *minWidthProperty ) )
                    {
                    TInt minwidth = HorizontalPixelValueL( minWidthProperty,
                        aParentRect.Width(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    if ( minwidth > contentRect.Width() )
                        {
                        TInt missingwidth = minwidth - contentRect.Width();
                        contentRect.Resize( missingwidth, 0 );
                        paddingRect.Resize( missingwidth, 0 );
                        borderRect.Resize( missingwidth, 0 );
                        marginRect.Resize( missingwidth, 0 );
                        }
                    }
                }
            horizontalValuesProcessed = ETrue;
            }
        if ( !horizontalValuesProcessed )
            {
            if ( IsPropertyAutoL( *leftProperty ) &&
                 IsPropertyAutoL( *rightProperty ) )
                {
                // move the box left
                contentRect.Move( widthToUse / 2, 0 );
                paddingRect.Move( widthToUse / 2, 0 );
                borderRect.Move( widthToUse / 2, 0 );
                marginRect.Move( widthToUse / 2, 0 );
                }
            if ( IsPropertyAutoL( *leftProperty ) &&
                 !IsPropertyAutoL( *rightProperty ) )
                {
                // move the box left
                contentRect.Move( widthToUse, 0 );
                paddingRect.Move( widthToUse, 0 );
                borderRect.Move( widthToUse, 0 );
                marginRect.Move( widthToUse, 0 );
                }
            if ( IsPropertyAutoL( *marginLeftProperty ) &&
                 IsPropertyAutoL( *marginRightProperty ) )
                {
                // both margins auto, equal values
                marginRect.Resize( widthToUse, 0 );
                paddingRect.Move( widthToUse / 2, 0 );
                borderRect.Move( widthToUse / 2, 0 );
                contentRect.Move( widthToUse / 2, 0 );
                }
            if ( !IsPropertyAutoL( *marginLeftProperty ) &&
                 IsPropertyAutoL( *marginRightProperty ) )
                {
                // only right margin auto
                marginRect.Resize( widthToUse, 0 );
                paddingRect.Move( widthToUse, 0 );
                borderRect.Move( widthToUse, 0 );
                contentRect.Move( widthToUse, 0 );
                }
            if ( IsPropertyAutoL( *marginLeftProperty ) &&
                 !IsPropertyAutoL( *marginRightProperty ) )
                {
                // only left margin auto
                marginRect.Resize( widthToUse, 0 );
                }
            }
        }

    // Move the rect to desired position
    if ( topProperty )
        {
        if ( !IsPropertyAutoL( *topProperty ) &&
             !IsPropertyNone( *topProperty ) )
            {
            TInt y = VerticalPixelValueL( topProperty, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            contentRect.Move( 0, y );
            paddingRect.Move( 0, y );
            borderRect.Move( 0, y );
            marginRect.Move( 0, y );
            }
        }
    if ( leftProperty )
        {
        if ( !IsPropertyAutoL( *leftProperty ) &&
             !IsPropertyNone( *leftProperty ) )
            {
            TInt x = HorizontalPixelValueL( leftProperty, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            contentRect.Move( x, 0 );
            paddingRect.Move( x, 0 );
            borderRect.Move( x, 0 );
            marginRect.Move( x, 0 );
            }
        }
    if ( bottomProperty )
        {
         if ( !IsPropertyAutoL( *bottomProperty ) &&
              !IsPropertyNone( *bottomProperty ) )
            {
            TInt py = VerticalPixelValueL( bottomProperty,
                aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels );
            contentRect.Move( 0, -py );
            paddingRect.Move( 0, -py );
            borderRect.Move( 0, -py );
            marginRect.Move( 0, -py );
            }
        }
    if ( rightProperty )
        {
        if ( !IsPropertyAutoL( *rightProperty ) &&
             !IsPropertyNone( *rightProperty ) )
            {
            TInt px = HorizontalPixelValueL( rightProperty,
                aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels );
            contentRect.Move( -px, 0 );
            paddingRect.Move( -px, 0 );
            borderRect.Move( -px, 0 );
            marginRect.Move( -px, 0 );
            }
        }

    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );

    DropExceedingRectL( aParentRect, aNode );
    }

// -----------------------------------------------------------------------------
// CalculateBorderL()
// Move the rect by adding the border.
// -----------------------------------------------------------------------------
//
static void CalculateBorderL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt offsetx = 0;
    TInt offsety = 0;

    TInt borderleft = 0;
    TInt borderright = 0;
    TInt bordertop = 0;
    TInt borderbottom = 0;

    CXnProperty* commonBorderStyle = aNode.BorderStyleL();
    CXnProperty* borderImage = aNode.BorderImageL();
    CXnProperty* borderStyle = aNode.BorderLeftStyleL();
    CXnProperty* borderproperty = aNode.BorderLeftL();

    // if border width is defined
    CXnProperty* borderwidthproperty = aNode.BorderWidthL();
    if ( !borderproperty )
        {
        borderproperty = borderwidthproperty;
        }

    if ( borderwidthproperty && ( ( commonBorderStyle &&
         !IsPropertyNone( *commonBorderStyle ) ) ||
        ( borderImage && !IsPropertyNone( *borderImage ) ) ) )
        {
        TInt bordervertical = 0;
        TInt borderhorizontal = 0;

        DetermineBorderWidthsL( borderwidthproperty, bordervertical,
            borderhorizontal, aParentRect, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );

        borderleft = borderhorizontal;
        borderright = borderhorizontal;
        bordertop = bordervertical;
        borderbottom = bordervertical;
        }

    TInt ignore;
    // use border width only if there is valid border style or border image
    if ( borderproperty && ( ( borderStyle &&
         !IsPropertyNone( *borderStyle ) ) ||
        ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) ||
        ( borderImage && !IsPropertyNone( *borderImage ) ) ) )
        {
        DetermineBorderWidthsL( borderproperty, ignore, borderleft,
            aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        }
    borderStyle = aNode.BorderRightStyleL();
    borderproperty = aNode.BorderRightL();
    if ( !borderproperty )
        {
        borderproperty = borderwidthproperty;
        }
    if ( borderproperty && ( ( borderStyle &&
         !IsPropertyNone( *borderStyle ) ) ||
        ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) ||
        ( borderImage && !IsPropertyNone( *borderImage ) ) ) )
        {
        DetermineBorderWidthsL( borderproperty, ignore, borderright,
            aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        }
    borderStyle = aNode.BorderTopStyleL();
    borderproperty = aNode.BorderTopL();
    if ( !borderproperty )
        {
        borderproperty = borderwidthproperty;
        }
    if ( borderproperty && ( ( borderStyle &&
         !IsPropertyNone( *borderStyle ) ) ||
        ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) ||
        ( borderImage && !IsPropertyNone( *borderImage ) ) ) )
        {
        DetermineBorderWidthsL( borderproperty, bordertop, ignore,
            aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        }
    borderStyle = aNode.BorderBottomStyleL();
    borderproperty = aNode.BorderBottomL();
    if ( !borderproperty )
        {
        borderproperty = borderwidthproperty;
        }
    if ( borderproperty && ( ( borderStyle &&
         !IsPropertyNone( *borderStyle ) ) ||
        ( commonBorderStyle && !IsPropertyNone( *commonBorderStyle ) ) ||
        ( borderImage && !IsPropertyNone( *borderImage ) ) ) )
        {
        DetermineBorderWidthsL( borderproperty, borderbottom, ignore,
            aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        }

    offsetx = borderleft; // move the client rect of the element this much right
    offsety = bordertop; //  move the client rect of the element this much down

    TRect newrect( aNode.Rect() );
    newrect.Move( offsetx, offsety );
    aNode.SetRect( newrect );
    // and move the padding rect also, Ari 5.8.2005
    TRect paddingrect = aNode.PaddingRect();
    TPoint origin( paddingrect.iTl );
    paddingrect.Move( offsetx, offsety );
    aNode.SetPaddingRect( paddingrect );

    TRect borderrect( origin, TSize(
        borderleft + borderright + paddingrect.Width(),
        bordertop + borderbottom + paddingrect.Height() ) );
    TRect normalFlowBorderRect( origin, TSize(
        borderleft + borderright + paddingrect.Width(),
        bordertop + borderbottom + paddingrect.Height() ) );
    aNode.SetBorderRect( borderrect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    }

// -----------------------------------------------------------------------------
// CalculateAbsoluteMarginL()
// Place child areas to parent rect and calculate margins.
// -----------------------------------------------------------------------------
//
static void CalculateAbsoluteMarginL( CXnNode& aParent, CXnNode& aNode,
    CXnNode* aPreviousSibling, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt previousPosition = 0;
    CXnProperty* marginProperty = NULL;
    TInt marginLeft = 0;
    TInt marginRight = 0;
    TInt marginTop = 0;
    TInt marginBottom = 0;

    marginProperty = aNode.MarginLeftL();

    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginLeft = HorizontalPixelValueL( marginProperty,
                aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginRightL();

    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginRight = HorizontalPixelValueL( marginProperty,
                aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginTopL();

    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginTop = VerticalPixelValueL( marginProperty,
                aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginBottomL();

    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginBottom = VerticalPixelValueL( marginProperty,
                aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels );
            }
        }

    TInt offsetx = 0;
    TInt offsety = 0;

    if ( &aParent == &aNode )
        {
        aParentRect.Move( -aParentRect.iTl.iX, -aParentRect.iTl.iY );
        }

    // if all properties are defined and block progression is LR, margin right
    // is ignored and replaced by remaining
    // if block progression is RL, same thing to margin left

    // previous sibling was found, get it's margin and previous coordinate to
    // define next child's position
    previousPosition = ( aPreviousSibling != NULL ) ?
        GetPositionL( *aPreviousSibling, aParentBlockProgression,
        aParentDirection ) : 0;

    // Move the rect considering previous child
    // Margins are collapsed, larger is used
    if ( aParentDirection == XnPropertyNames::style::common::direction::KLTR )
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KLR ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KRL )
            {
            // offset to parent rect's top left corner
            if ( !aPreviousSibling )
                {
                offsetx = aParentRect.iTl.iX;
                }

            offsety = aParentRect.iTl.iY + marginTop;
            TInt prevMarginRight = 0;

            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                prevMarginRight =
                    previousMarginRect.iBr.iX - previousBorderRect.iBr.iX;
                }

            if ( prevMarginRight > marginLeft )
                {
                offsetx += previousPosition + prevMarginRight;
                }
            else
                {
                offsetx += previousPosition + marginLeft;
                }
            }
        else // TB or BT
            {
            // offset to parent rect's top left corner
            offsetx = aParentRect.iTl.iX + marginLeft;

            if ( !aPreviousSibling )
                {
                offsety = aParentRect.iTl.iY;
                }

            TInt prevMarginBottom = 0;

            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                prevMarginBottom =
                    previousMarginRect.iBr.iY - previousBorderRect.iBr.iY;
                }

            if ( prevMarginBottom > marginTop )
                {
                offsety += previousPosition + prevMarginBottom;
                }
            else
                {
                offsety += previousPosition + marginTop;
                }
            }
        }
    else // RTL
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KLR ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KRL )
            {
            // offset to parent rect's top right corner
            if ( !aPreviousSibling )
                {
                offsetx = aParentRect.Width() - aNode.BorderRect().Width() +
                    aParentRect.iTl.iX;
                }
            else
                {
                offsetx = previousPosition - aNode.BorderRect().Width();
                }

            offsety = aParentRect.iTl.iY + marginTop;

            TInt prevMarginLeft = 0;

            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                prevMarginLeft =
                    previousBorderRect.iTl.iX - previousMarginRect.iTl.iX;
                }

            if ( prevMarginLeft > marginRight )
                {
                offsetx -= prevMarginLeft;
                }
            else
                {
                offsetx -= marginRight;
                }
            }
        else // TB or BT
            {
            // offset to parent rect's bottom left corner
            offsetx += aParentRect.iTl.iX + marginRight;

            if ( !aPreviousSibling )
                {
                // start from bottom of parent rect
                offsety = aParentRect.Height() - aNode.BorderRect().Height();
                }
            else
                {
                offsety = previousPosition - aNode.BorderRect().Height();
                }

            TInt prevMarginTop = 0;

            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                prevMarginTop =
                    previousBorderRect.iTl.iY - previousMarginRect.iTl.iY;
                }

            // collapse margins
            if ( prevMarginTop > marginBottom )
                {
                offsety -= prevMarginTop;
                }
            else
                {
                offsety -= marginBottom;
                }
            }
        }

    TRect borderRect = TRect( aNode.BorderRect() );
    TRect normalFlowBorderRect = TRect( aNode.NormalFlowBorderRect() );
    // this is the top left corner of margin rect
    TPoint origin( borderRect.iTl - TPoint( marginLeft, marginTop ) );

    if ( &aParent != &aNode )
        {
        // set content rect
        TRect newRect( aNode.Rect() );
        newRect.Move( offsetx, offsety );
        aNode.SetRect( newRect );

        // set padding rect
        TRect paddingRect( aNode.PaddingRect() );
        paddingRect.Move( offsetx, offsety );
        aNode.SetPaddingRect( paddingRect );

        // set border rect
        borderRect.Move( offsetx, offsety );
        normalFlowBorderRect.Move( offsetx, offsety );
        // this is the top left corner of margin rect
        origin = TPoint( borderRect.iTl - TPoint( marginLeft, marginTop ) );
        aNode.SetBorderRect( borderRect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        }

    TRect marginRect( origin,
        TSize( marginLeft + marginRight + borderRect.Width(),
        marginTop + marginBottom + borderRect.Height() ) );

    aNode.SetMarginRect( marginRect );

    // Within this function, the rects are adjusted if the display-priority is 0
    // and margin rect exceeds parent's content rect.
    DropExceedingRectL( aParentRect, aNode );

    if( IsNodeTooltip( aNode ) || aNode.Type()->Type() == KWidgetExtensionNodeName || 
         aNode.Type()->Type() == KPopUpNodeName )
        {
        // because tooltip has it's own window, move margin rect to 0,0
        // and all other rects as much up left
        TRect marginRect = aNode.MarginRect();

        TRect borderRect = aNode.BorderRect();
        TRect paddingRect = aNode.PaddingRect();
        TRect contentRect = aNode.Rect();
        TInt x = marginRect.iTl.iX;
        TInt y = marginRect.iTl.iY;
        marginRect.Move( -x, -y );

        borderRect.Move( -x, -y );
        paddingRect.Move( -x, -y );
        contentRect.Move( -x, -y );

        aNode.SetMarginRect( marginRect );

        aNode.SetBorderRect( borderRect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        aNode.SetPaddingRect( paddingRect );
        aNode.SetRect( contentRect );
        }
    }

// -----------------------------------------------------------------------------
// CalculateAbsoluteMarginL()
// Place child areas to parent rect and calculate margins.
// -----------------------------------------------------------------------------
//
static void CalculateMarginL( CXnNode& aParent, CXnNode& aNode,
    CXnNode* aPreviousSibling, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels, TInt& aColumnWidth, TInt& aColumnMargin )
    {
    if( aParent.Control() && aParent.Control()->OwnsWindow() )
        {
        aParentRect = TRect( aParentRect.iTl - aParent.MarginRect().iTl, aParentRect.Size() );
        }

    CXnProperty* marginProperty = NULL;
    TInt marginLeft = 0;
    TInt marginRight = 0;
    TInt marginTop = 0;
    TInt marginBottom = 0;

    marginProperty = aNode.MarginLeftL();
    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginLeft = HorizontalPixelValueL(
                marginProperty, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginRightL();
    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginRight = HorizontalPixelValueL(
                marginProperty, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginTopL();
    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginTop = VerticalPixelValueL(
                marginProperty, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }

    marginProperty = aNode.MarginBottomL();
    if ( marginProperty )
        {
        if ( !IsPropertyAutoL( *marginProperty ) )
            {
            marginBottom = VerticalPixelValueL(
                marginProperty, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }

    TInt offsetx = 0;
    TInt offsety = 0;

    //What's this???
    if ( &aParent == &aNode )
        {
        aParentRect.Move( -aParentRect.iTl.iX, -aParentRect.iTl.iY );
        }

    // Move the rect considering previous child
    // Margins are collapsed, larger is used
    if ( aParentDirection == XnPropertyNames::style::common::direction::KLTR )
        {
        //   LTR , TB
        //  *********
        //  | 1 | 4 |
        //  | 2 | 5 |
        //  | 3 | 6 |
        //  *********
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB )
            {
            // First we need to know where to layout next sibling
            // Try first under the previous sibling...
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutUnderPrevious( EFalse );
                TInt height = aNode.MarginRect().Height();
                TInt prevMarginBottom =
                    previousMarginRect.iBr.iY - previousBorderRect.iBr.iY;
                if ( prevMarginBottom < marginTop )
                    {
                    height -= prevMarginBottom;
                    }
                else
                    {
                    height -= marginTop;
                    }
                if(height <= aParentRect.iBr.iY - previousMarginRect.iBr.iY || IsSrollableBox(aNode))
                    {
                    layoutUnderPrevious = ETrue;
                    }

                // ...then to the next column
                if ( !layoutUnderPrevious )
                    {
                    TInt width =
                        aNode.BorderRect().Width() + marginLeft + marginRight;
                    if ( aColumnMargin < marginLeft )
                        {
                        width -= aColumnMargin;
                        }
                    else
                        {
                        width -= marginLeft;
                        }
                    if ( width + aColumnWidth <= aParentRect.Width() )
                        {
                        offsety = aParentRect.iTl.iY + marginTop;
                        if ( aColumnMargin < marginLeft )
                            {
                            offsetx = aParentRect.iTl.iX + aColumnWidth -
                                aColumnMargin + marginLeft;
                            }
                        else
                            {
                            offsetx = aParentRect.iTl.iX + aColumnWidth;
                            }
                        }
                    else
                        {
                        layoutUnderPrevious = ETrue;
                        }
                    }
                if ( layoutUnderPrevious )
                    {
                    offsetx = 
                        aPreviousSibling->MarginRect().iTl.iX + marginLeft;
                    if ( prevMarginBottom > marginTop )
                        {
                        // margin of this node is collapsed
                        offsety = previousMarginRect.iBr.iY;
                        }
                    else
                        {
                        // margin of the previous node is collapsed
                        offsety = previousBorderRect.iBr.iY + marginTop;
                        }
                    }
                }
            else
                {
                // offset to parent rect's top left corner
                offsetx = aParentRect.iTl.iX + marginLeft;
                offsety = aParentRect.iTl.iY + marginTop;
                }
            }

        //   LTR , BT
        //  *********
        //  | 3 | 6 |
        //  | 2 | 5 |
        //  | 1 | 4 |
        //  *********
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KBT )
            {
            // First we need to know where to layout next sibling
            // Try first above the previous sibling...
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutAbovePrevious( EFalse );
                TInt height =
                    aNode.BorderRect().Height() + marginTop + marginBottom;
                TInt prevMarginTop =
                    previousBorderRect.iTl.iY - previousMarginRect.iTl.iY;
                if ( prevMarginTop < marginBottom )
                    {
                    height -= prevMarginTop;
                    }
                else
                    {
                    height -= marginBottom;
                    }
                if ( height <= previousMarginRect.iTl.iY - aParentRect.iTl.iY )
                    {
                    layoutAbovePrevious = ETrue;
                    }

                // ...then to the next column
                if ( !layoutAbovePrevious )
                    {
                    TInt width =
                        aNode.BorderRect().Width() + marginLeft + marginRight;
                    if ( aColumnMargin < marginLeft )
                        {
                        width -= aColumnMargin;
                        }
                    else
                        {
                        width -= marginLeft;
                        }
                    if ( width + aColumnWidth <= aParentRect.Width() )
                        {
                        offsety = aParentRect.iBr.iY - marginBottom -
                            aNode.BorderRect().Height();
                        if ( aColumnMargin < marginLeft )
                            {
                            offsetx = aParentRect.iTl.iX + aColumnWidth -
                                aColumnMargin + marginLeft;
                            }
                        else
                            {
                            offsetx = aParentRect.iTl.iX + aColumnWidth;
                            }
                        }
                    else
                        {
                        layoutAbovePrevious = ETrue;
                        }
                    }
                if ( layoutAbovePrevious )
                    {
                    offsetx =
                        aPreviousSibling->MarginRect().iTl.iX + marginLeft;
                    if ( prevMarginTop > marginBottom )
                        {
                        // margin of this node is collapsed
                        offsety = previousMarginRect.iTl.iY -
                            aNode.BorderRect().Height();
                        }
                    else
                        {
                        // margin of the previous node is collapsed
                        offsety = previousBorderRect.iTl.iY - marginBottom -
                            aNode.BorderRect().Height();
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iTl.iX + marginLeft;
                offsety = aParentRect.iBr.iY - marginBottom -
                    aNode.BorderRect().Height();
                }
            }

        //   LTR , LR
        //  *********
        //  | 5 | 6 |
        //  | 3 | 4 |
        //  | 1 | 2 |
        //  *********
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KLR )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutNextToPrevious( EFalse );
                TInt width =
                    aNode.BorderRect().Width() + marginRight + marginLeft;
                TInt prevMarginRight =
                    previousMarginRect.iBr.iX - previousBorderRect.iBr.iX;
                if ( prevMarginRight < marginLeft )
                    {
                    width -= prevMarginRight;
                    }
                else
                    {
                    width -= marginLeft;
                    }
                if ( width <= aParentRect.iBr.iX - previousMarginRect.iBr.iX )
                    {
                    layoutNextToPrevious = ETrue;
                    }

                if ( !layoutNextToPrevious )
                    {
                    TInt height = aNode.BorderRect().Height() + marginTop +
                        marginBottom;
                    if ( aColumnMargin < marginBottom )
                        {
                        height -= aColumnMargin;
                        }
                    else
                        {
                        height -= marginBottom;
                        }
                    if ( height + aColumnWidth <= aParentRect.Height() )
                        {
                        offsetx = aParentRect.iTl.iX + marginLeft;
                        if ( aColumnMargin < marginTop )
                            {
                            offsety = aParentRect.iBr.iY - aColumnWidth +
                                aColumnMargin - aNode.BorderRect().Height() -
                                marginBottom;
                            }
                        else
                            {
                            offsety = aParentRect.iBr.iY - aColumnWidth -
                                aNode.BorderRect().Height();
                            }
                        }
                    else
                        {
                        layoutNextToPrevious = ETrue;
                        }
                    }
                if ( layoutNextToPrevious )
                    {
                    offsety = aPreviousSibling->MarginRect().iBr.iY -
                        aNode.BorderRect().Height() - marginBottom;
                    if ( prevMarginRight > marginLeft )
                        {
                        // margin of this node is collapsed
                        offsetx = previousMarginRect.iBr.iX;
                        }
                    else
                        {
                        // margin of the previous node is collapsed
                        offsetx = previousBorderRect.iBr.iX + marginLeft;
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iTl.iX + marginLeft;
                offsety = aParentRect.iBr.iY - aNode.BorderRect().Height() -
                    marginBottom;
                }
            }

        //   LTR , RL
        //  *********
        //  | 1 | 2 |
        //  | 3 | 4 |
        //  | 5 | 6 |
        //  *********

        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KRL )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutNextToPrevious( EFalse );
                TInt width =
                    aNode.BorderRect().Width() + marginRight + marginLeft;
                TInt prevMarginRight =
                    previousMarginRect.iBr.iX - previousBorderRect.iBr.iX;
                if ( prevMarginRight < marginLeft )
                    {
                    width -= prevMarginRight;
                    }
                else
                    {
                    width -= marginLeft;
                    }
                if ( width <= aParentRect.iBr.iX - previousMarginRect.iBr.iX )
                    {
                    layoutNextToPrevious = ETrue;
                    }

                if ( !layoutNextToPrevious )
                    {
                    TInt height =
                        aNode.BorderRect().Height() + marginTop + marginBottom;
                    if ( aColumnMargin < marginTop )
                        {
                        height -= aColumnMargin;
                        }
                    else
                        {
                        height -= marginTop;
                        }
                    if ( height + aColumnWidth <= aParentRect.Height() )
                        {
                        offsetx = aParentRect.iTl.iX + marginLeft;
                        if ( aColumnMargin < marginTop )
                            {
                            offsety = aParentRect.iTl.iY + aColumnWidth -
                                aColumnMargin + marginTop;
                            }
                        else
                            {
                            offsety = aParentRect.iTl.iY + aColumnWidth;
                            }
                        }
                    else
                        {
                        layoutNextToPrevious = ETrue;
                        }
                    }
                if ( layoutNextToPrevious )
                    {
                    offsety = aPreviousSibling->MarginRect().iTl.iY + marginTop;
                    if ( prevMarginRight > marginLeft )
                        {
                        // margin of this node is collapsed
                        offsetx = previousMarginRect.iBr.iX;
                        }
                    else
                        {
                        // margin of the previous node is collapsed
                        offsetx = previousBorderRect.iBr.iX + marginLeft;
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iTl.iX + marginLeft;
                offsety = aParentRect.iTl.iY + marginTop;
                }
            }
        }

    else
        {
        //   RTL , TB
        //  *********
        //  | 4 | 1 |
        //  | 5 | 2 |
        //  | 6 | 3 |
        //  *********
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutUnderPrevious( EFalse );
                TInt height = aNode.MarginRect().Height();
                TInt prevMarginBottom =
                    previousMarginRect.iBr.iY - previousBorderRect.iBr.iY;
                if ( prevMarginBottom < marginTop )
                    {
                    height -= prevMarginBottom;
                    }
                else
                    {
                    height -= marginTop;
                    }
                if ( height <= aParentRect.iBr.iY - previousMarginRect.iBr.iY )
                    {
                    layoutUnderPrevious = ETrue;
                    }

                if ( !layoutUnderPrevious )
                    {
                    TInt width =
                        aNode.BorderRect().Width() + marginLeft + marginRight;
                    if ( aColumnMargin < marginRight )
                        {
                        width -= aColumnMargin;
                        }
                    else
                        {
                        width -= marginRight;
                        }
                    if ( width + aColumnWidth <= aParentRect.Width() )
                        {
                        offsety = aParentRect.iTl.iY + marginTop;
                        if ( aColumnMargin < marginRight )
                            {
                            offsetx = aParentRect.iBr.iX - aColumnWidth +
                                aColumnMargin - marginRight -
                                aNode.BorderRect().Width();
                            }
                        else
                            {
                            offsetx = aParentRect.iBr.iX - aColumnWidth -
                                aNode.BorderRect().Width();
                            }
                        }
                    else
                        {
                        layoutUnderPrevious = ETrue;
                        }
                    }
                if ( layoutUnderPrevious )
                    {
                    offsetx = aPreviousSibling->MarginRect().iBr.iX -
                        marginRight - aNode.BorderRect().Width();
                    if ( prevMarginBottom > marginTop )
                        {
                        offsety = previousMarginRect.iBr.iY;
                        }
                    else
                        {
                        offsety = previousBorderRect.iBr.iY + marginTop;
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iBr.iX - marginRight -
                    aNode.BorderRect().Width();
                offsety = aParentRect.iTl.iY + marginTop;
                }
            }

        //   RTL , BT
        //  *********
        //  | 6 | 3 |
        //  | 5 | 2 |
        //  | 4 | 1 |
        //  *********
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KBT )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutAbovePrevious( EFalse );
                TInt height =
                    aNode.BorderRect().Height() + marginTop + marginBottom;
                TInt prevMarginTop =
                    previousBorderRect.iTl.iY - previousMarginRect.iTl.iY;
                if ( prevMarginTop < marginBottom )
                    {
                    height -= prevMarginTop;
                    }
                else
                    {
                    height -= marginBottom;
                    }
                if ( height <= previousMarginRect.iTl.iY - aParentRect.iTl.iY )
                    {
                    layoutAbovePrevious = ETrue;
                    }

                if ( !layoutAbovePrevious )
                    {
                    TInt width =
                        aNode.BorderRect().Width() + marginLeft + marginRight;
                    if ( aColumnMargin < marginRight )
                        {
                        width -= aColumnMargin;
                        }
                    else
                        {
                        width -= marginRight;
                        }
                    if ( width + aColumnWidth <= aParentRect.Width() )
                        {
                        offsety = aParentRect.iBr.iY - marginBottom -
                            aNode.BorderRect().Height();
                        if ( aColumnMargin < marginRight )
                            {
                            offsetx = aParentRect.iBr.iX - aColumnWidth +
                                aColumnMargin - marginRight -
                                aNode.BorderRect().Width();
                            }
                        else
                            {
                            offsetx = aParentRect.iBr.iX - aColumnWidth -
                                aNode.BorderRect().Width();
                            }
                        }
                    else
                        {
                        layoutAbovePrevious = ETrue;
                        }
                    }
                if ( layoutAbovePrevious )
                    {
                    offsetx = aPreviousSibling->MarginRect().iBr.iX -
                        marginRight - aNode.BorderRect().Width();
                    if ( prevMarginTop > marginBottom )
                        {
                        // margin of this node is collapsed
                        offsety = previousMarginRect.iTl.iY -
                            aNode.BorderRect().Height();
                        }
                    else
                        {
                        // margin of the previous node is collapsed
                        offsety = previousBorderRect.iTl.iY - marginBottom -
                            aNode.BorderRect().Height();
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iBr.iX - marginRight -
                    aNode.BorderRect().Width();
                offsety = aParentRect.iBr.iY - marginBottom -
                    aNode.BorderRect().Height();
                }
            }

        //   RTL , LR
        //  *********
        //  | 6 | 5 |
        //  | 4 | 3 |
        //  | 2 | 1 |
        //  *********
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KLR )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutNextToPrevious( EFalse );
                TInt width =
                    aNode.BorderRect().Width() + marginRight + marginLeft;
                TInt prevMarginLeft =
                    previousBorderRect.iTl.iX - previousMarginRect.iTl.iX;
                if ( prevMarginLeft < marginRight )
                    {
                    width -= prevMarginLeft;
                    }
                else
                    {
                    width -= marginRight;
                    }
                if ( width <= previousMarginRect.iTl.iX - aParentRect.iTl.iX )
                    {
                    layoutNextToPrevious = ETrue;
                    }

                if ( !layoutNextToPrevious )
                    {
                    TInt height =
                        aNode.BorderRect().Height() + marginTop + marginBottom;
                    if ( aColumnMargin < marginBottom )
                        {
                        height -= aColumnMargin;
                        }
                    else
                        {
                        height -= marginBottom;
                        }
                    if ( height + aColumnWidth <= aParentRect.Height() )
                        {
                        offsetx = aParentRect.iBr.iX - marginRight -
                            aNode.BorderRect().Width();
                        if ( aColumnMargin < marginTop )
                            {
                            offsety = aParentRect.iBr.iY - aColumnWidth +
                                aColumnMargin - aNode.BorderRect().Height() -
                                marginBottom;
                            }
                        else
                            {
                            offsety = aParentRect.iBr.iY - aColumnWidth -
                                aNode.BorderRect().Height();
                            }
                        }
                    else
                        {
                        layoutNextToPrevious = ETrue;
                        }
                    }
                if ( layoutNextToPrevious )
                    {
                    offsety = aPreviousSibling->MarginRect().iBr.iY -
                        aNode.BorderRect().Height() - marginBottom;
                    if ( prevMarginLeft > marginRight )
                        {
                        offsetx = previousMarginRect.iTl.iX -
                            aNode.BorderRect().Width();
                        }
                    else
                        {
                        offsetx = previousBorderRect.iTl.iX - marginRight -
                            aNode.BorderRect().Width();
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iBr.iX - marginRight -
                    aNode.BorderRect().Width();
                offsety = aParentRect.iBr.iY - marginBottom -
                    aNode.BorderRect().Height();
                }
            }

        //   RTL , RL
        //  *********
        //  | 2 | 1 |
        //  | 4 | 3 |
        //  | 6 | 5 |
        //  *********
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KRL )
            {
            if ( aPreviousSibling )
                {
                TRect previousMarginRect = aPreviousSibling->MarginRect();
                TRect previousBorderRect = aPreviousSibling->BorderRect();
                TBool layoutNextToPrevious( EFalse );
                TInt width =
                    aNode.BorderRect().Width() + marginRight + marginLeft;
                TInt prevMarginLeft =
                    previousBorderRect.iTl.iX - previousMarginRect.iTl.iX;
                if ( prevMarginLeft < marginRight )
                    {
                    width -= prevMarginLeft;
                    }
                else
                    {
                    width -= marginRight;
                    }
                if ( width <= previousMarginRect.iTl.iX - aParentRect.iTl.iX )
                    {
                    layoutNextToPrevious = ETrue;
                    }
                if ( !layoutNextToPrevious )
                    {
                    TInt height =
                        aNode.BorderRect().Height() + marginTop + marginBottom;
                    if ( aColumnMargin < marginTop )
                        {
                        height -= aColumnMargin;
                        }
                    else
                        {
                        height -= marginTop;
                        }
                    if ( height + aColumnWidth <= aParentRect.Height() )
                        {
                        offsetx = aParentRect.iBr.iX - marginRight -
                            aNode.BorderRect().Width();
                        if ( aColumnMargin < marginTop )
                            {
                            offsety = aParentRect.iTl.iY + aColumnWidth -
                                aColumnMargin + marginTop;
                            }
                        else
                            {
                            offsety = aParentRect.iTl.iY + aColumnWidth;
                            }
                        }
                    else
                        {
                        layoutNextToPrevious = ETrue;
                        }
                    }
                if ( layoutNextToPrevious )
                    {
                    offsety = aPreviousSibling->MarginRect().iTl.iY + marginTop;
                    if ( prevMarginLeft > marginRight )
                        {
                        offsetx = previousMarginRect.iTl.iX -
                            aNode.BorderRect().Width();
                        }
                    else
                        {
                        offsetx = previousBorderRect.iTl.iX - marginRight -
                            aNode.BorderRect().Width();
                        }
                    }
                }
            else
                {
                offsetx = aParentRect.iBr.iX - marginRight -
                    aNode.BorderRect().Width();
                offsety = aParentRect.iTl.iY + marginTop;
                }
            }
        }

    TRect borderRect = TRect( aNode.BorderRect() );
    TRect normalFlowBorderRect = TRect( aNode.NormalFlowBorderRect() );
    // this is the top left corner of margin rect
    TPoint origin( borderRect.iTl - TPoint( marginLeft, marginTop ) );

    if ( &aParent != &aNode )
        {
        // set content rect
        TRect newRect( aNode.Rect() );
        newRect.Move( offsetx, offsety );
        aNode.SetRect( newRect );

        // set padding rect
        TRect paddingRect( aNode.PaddingRect() );
        paddingRect.Move( offsetx, offsety );
        aNode.SetPaddingRect( paddingRect );

        // set border rect
        borderRect.Move( offsetx, offsety );
        normalFlowBorderRect.Move( offsetx, offsety );
        // this is the top left corner of margin rect
        origin = TPoint( borderRect.iTl - TPoint( marginLeft, marginTop ) );
        aNode.SetBorderRect( borderRect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        }

    TRect marginRect( origin,
        TSize( marginLeft + marginRight + borderRect.Width(),
        marginTop + marginBottom + borderRect.Height() ) );

    aNode.SetMarginRect( marginRect );

    // Within this function, the rects are adjusted if the display-priority is
    // 0 and margin rect exceeds parent's content rect.
    DropExceedingRectL( aParentRect, aNode );

    // Update column width & column margin width according to block
    // progression & direction.
    if ( aParentBlockProgression ==
        XnPropertyNames::style::common::block_progression::KRL )
        {
        TInt newColumnWidth = marginRect.iBr.iY - aParentRect.iTl.iY;
        if ( newColumnWidth > aColumnWidth )
            {
            aColumnWidth = newColumnWidth;
            aColumnMargin = marginRect.iBr.iY - borderRect.iBr.iY;
            }
        }
    else if ( aParentBlockProgression ==
              XnPropertyNames::style::common::block_progression::KLR )
        {
        TInt newColumnWidth = aParentRect.iBr.iY - marginRect.iTl.iY;
        if ( newColumnWidth > aColumnWidth )
            {
            aColumnWidth = newColumnWidth;
            aColumnMargin = borderRect.iTl.iY - marginRect.iTl.iY;
            }
        }
    else if ( aParentDirection ==
              XnPropertyNames::style::common::direction::KLTR )
        {
        // same for tb and bt
        TInt newColumnWidth = marginRect.iBr.iX - aParentRect.iTl.iX;
        if ( newColumnWidth > aColumnWidth )
            {
            aColumnWidth = newColumnWidth;
            aColumnMargin = marginRect.iBr.iX - borderRect.iBr.iX;
            }
        }
    else if ( aParentDirection ==
              XnPropertyNames::style::common::direction::KRTL )
        {
        // same for tb and bt
        TInt newColumnWidth = aParentRect.iBr.iX - marginRect.iTl.iX;
        if ( newColumnWidth > aColumnWidth )
            {
            aColumnWidth = newColumnWidth;
            aColumnMargin = borderRect.iTl.iX - marginRect.iTl.iX;
            }
        }

    if ( IsNodeTooltip( aNode ) )
        {
        // because tooltip has it's own window, move margin rect to 0,0
        // and all other rects as much up left
        TRect marginRect = aNode.MarginRect();
        TRect borderRect = aNode.BorderRect();
        TRect paddingRect = aNode.PaddingRect();
        TRect contentRect = aNode.Rect();
        TInt x = marginRect.iTl.iX;
        TInt y = marginRect.iTl.iY;
        marginRect.Move( -x, -y );
        borderRect.Move( -x, -y );
        paddingRect.Move( -x, -y );
        contentRect.Move( -x, -y );
        aNode.SetMarginRect( marginRect );
        aNode.SetBorderRect( borderRect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        aNode.SetPaddingRect( paddingRect );
        aNode.SetRect( contentRect );
        }
    }

// -----------------------------------------------------------------------------
// CalculatePaddingL()
// Move the rect by adding the padding.
// -----------------------------------------------------------------------------
//
static void CalculatePaddingL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt offsetx = 0;
    TInt offsety = 0;

    TInt paddingleft = 0;
    TInt paddingright = 0;
    TInt paddingtop = 0;
    TInt paddingbottom = 0;
    TRect parentRect = aParentRect;

    CXnProperty* paddingproperty = aNode.PaddingLeftL();

    if ( paddingproperty )
        {
        TRAPD( error, paddingleft = HorizontalPixelValueL( paddingproperty,
            parentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels ) );

        if ( error != KErrNone )
            {
            paddingleft = 0;
            }
        }

    paddingproperty = aNode.PaddingRightL();

    if ( paddingproperty )
        {
        TRAPD( error, paddingright = HorizontalPixelValueL( paddingproperty,
            parentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels ) );

        if ( error != KErrNone )
            {
            paddingright = 0;
            }
        }

    paddingproperty = aNode.PaddingTopL();

    if ( paddingproperty )
        {
        TRAPD( error, paddingtop = VerticalPixelValueL( paddingproperty,
            parentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels ) );

        if ( error != KErrNone )
            {
            paddingtop = 0;
            }
        }

    paddingproperty = aNode.PaddingBottomL();

    if ( paddingproperty )
        {
        TRAPD( error, paddingbottom = VerticalPixelValueL( paddingproperty,
            parentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels ) );

        if ( error != KErrNone )
            {
            paddingbottom = 0;
            }
        }

    // move the client rect of the element this much right
    offsetx = paddingleft;
    //  move the client rect of the element this much down
    offsety = paddingtop;

    TRect newrect( aNode.Rect() );
    TPoint origin( newrect.iTl );
    newrect.Move( offsetx, offsety );
    aNode.SetRect( newrect );

    // set padding rect
    TRect paddingrect( origin, TSize( paddingleft + paddingright +
        newrect.Width(), paddingtop + paddingbottom + newrect.Height() ) );

    aNode.SetPaddingRect( paddingrect );
    }

// -----------------------------------------------------------------------------
// CalculateRectL()
// Creates rect for CXnComponent.
// -----------------------------------------------------------------------------
//
static void CalculateRectL( CXnNode& aNode, CXnNode& aParent,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt width = 0;
    TInt height = 0;
    TRect parentRect = aParent.Rect();
    CXnProperty* widthproperty = aNode.WidthL();

    if ( widthproperty )
        {
        // if width is auto, it is set to 0 at this point and calculated later
        if ( IsPropertyAutoL( *widthproperty ) )
            {
            width = 0;
            }
        else
            {
            width = HorizontalPixelValueL( widthproperty, parentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }

    CXnProperty* heightproperty = aNode.HeightL();

    if ( heightproperty )
        {
        // if height is auto, height is set to 0 at this point
        // and calculated later
        if ( IsPropertyAutoL( *heightproperty ) )
            {
            height = 0;
            }
        else
            {
            height = VerticalPixelValueL( heightproperty, parentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }

    // check that rect sizes are in max and min bounds
    CXnProperty* maxheightproperty = aNode.MaxHeightL();
    CXnProperty* minheightproperty = aNode.MinHeightL();
    CXnProperty* maxwidthproperty = aNode.MaxWidthL();
    CXnProperty* minwidthproperty = aNode.MinWidthL();

    if ( maxwidthproperty )
        {
        if ( !IsPropertyNone( *maxwidthproperty ) )
            {
            TInt maxwidth = HorizontalPixelValueL(
                maxwidthproperty, parentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );

            if ( width > maxwidth )
                {
                width = maxwidth;
                }
            }
        }

    if ( minwidthproperty )
        {
        if ( !IsPropertyNone( *minwidthproperty ) )
            {
            TInt minwidth = HorizontalPixelValueL(
                minwidthproperty, parentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );

            if ( width < minwidth )
                {
                width = minwidth;
                }
            }
        }

    if ( maxheightproperty )
        {
        if ( !IsPropertyNone( *maxheightproperty ) )
            {
            TInt maxheight = VerticalPixelValueL(
                maxheightproperty, parentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );

            if ( height > maxheight )
                {
                height = maxheight;
                }
            }
        }

    if ( minheightproperty )
        {
        if ( !IsPropertyNone( *minheightproperty ) )
            {
            TInt minheight = VerticalPixelValueL(
                minheightproperty, parentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );

            if ( height < minheight )
                {
                height = minheight;
                }
            }
        }

    if ( aNode.ViewNodeImpl() && height != 0 && width != 0 )
        {
        TRect rect( 0, 0, width, height );
        TRect paddingrect( 0, 0, width, height );
        TRect borderrect( 0, 0, width, height );
        TRect normalFlowBorderRect( 0, 0, width, height );
        TRect marginrect( 0, 0, width, height );

        aNode.SetRect( rect );
        aNode.SetPaddingRect( paddingrect );
        aNode.SetBorderRect( borderrect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        aNode.SetMarginRect( marginrect );
        }
    else if ( !aNode.ViewNodeImpl() )
        {
        TInt adaptive( aNode.IsAdaptive() );

        if ( adaptive && ( IsNodeTooltip( aNode ) || IsAbsoluteL( aNode ) ) )
            {
            CXnUiEngine* engine( aNode.UiEngine() );

            TSize maxSize( engine->ClientRect().Size() );

            if ( adaptive & XnAdaptive::EHeight )
                {
                height = maxSize.iHeight;
                }
            if ( adaptive & XnAdaptive::EWidth )
                {
                width = maxSize.iWidth;
                }
            }

        TRect rect( 0, 0, width, height );
        TRect paddingrect( 0, 0, width, height );
        TRect borderrect( 0, 0, width, height );
        TRect normalFlowBorderRect( 0, 0, width, height );
        TRect marginrect( 0, 0, width, height );

        aNode.SetRect( rect );
        aNode.SetPaddingRect( paddingrect );
        aNode.SetBorderRect( borderrect );
        aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
        aNode.SetMarginRect( marginrect );
        }
    }

// -----------------------------------------------------------------------------
// CalculateRelativePositionL()
// Move the rect by it's relative position.
// -----------------------------------------------------------------------------
//
static void CalculateRelativePositionsL( const TRect& aParentRect,
    CXnNode& aNode, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels )
    {
    CXnProperty* top = aNode.TopL();
    CXnProperty* bottom = aNode.BottomL();
    CXnProperty* left = aNode.LeftL();
    CXnProperty* right = aNode.RightL();

    // Move rect by offset
    TRect newRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );

    TRect parentRect = aParentRect;

    if ( top && !IsPropertyAutoL( *top ) )
        {
        newRect.Move( 0, VerticalPixelValueL( top, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        paddingRect.Move( 0, VerticalPixelValueL( top, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        borderRect.Move( 0, VerticalPixelValueL( top, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        marginRect.Move( 0, VerticalPixelValueL( top, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        }
    if ( bottom && !IsPropertyAutoL( *bottom ) )
        {
        newRect.Move( 0, -VerticalPixelValueL( bottom, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        paddingRect.Move( 0, -VerticalPixelValueL( bottom, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        borderRect.Move( 0, -VerticalPixelValueL( bottom, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        marginRect.Move( 0, -VerticalPixelValueL( bottom, parentRect.Height(),
            aGraphicsDevice, aVerticalUnitInPixels ) );
        }
    if ( left && !IsPropertyAutoL( *left ) )
        {
        newRect.Move( HorizontalPixelValueL( left, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        paddingRect.Move( HorizontalPixelValueL( left, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        borderRect.Move( HorizontalPixelValueL( left, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        marginRect.Move( HorizontalPixelValueL( left, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        }
    if ( right && !IsPropertyAutoL( *right ) )
        {
        newRect.Move( -HorizontalPixelValueL( right, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        paddingRect.Move( -HorizontalPixelValueL( right, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        borderRect.Move( -HorizontalPixelValueL( right, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        marginRect.Move( -HorizontalPixelValueL( right, parentRect.Width(),
            aGraphicsDevice, aHorizontalUnitInPixels ), 0 );
        }

    aNode.SetRect( newRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );

    DropExceedingRectL( parentRect, aNode, ETrue );
    }

// -----------------------------------------------------------------------------
// CalculateSpaceUsedByChildrenL()
// Return space used by node's children.
// -----------------------------------------------------------------------------
//
static TSize CalculateSpaceUsedByChildrenL( RPointerArray< CXnNode >& aChildren,
    CXnNode& aNode, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, CGraphicsDevice& aScreenDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels )
    {
    TInt usedSiblingHeight( 0 );
    TInt usedSiblingWidth( 0 );

    TRect rect( aNode.Rect() );

    TSize largestSize( 0, 0 );

    for ( TInt i = 0; i < aChildren.Count(); i++ )
        {
        CXnNode* node( aChildren[i] );

        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL( *node ) && !IsNodeTooltip( *node ) )
            {
            TSize siblingSize = CalculateTotalDimensionsL( *node, ETrue,
                EFalse, rect, aScreenDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );

            if ( node->IsAdaptive() )
                {
                RPointerArray< CXnNode >& children( node->Children() );

                CXnProperty* blockProgressionProperty(
                    node->BlockProgressionL() );
                CXnProperty* directionProperty( node->DirectionL() );

                const TDesC8* blockProgression(
                    &XnPropertyNames::style::common::block_progression::KTB() );
                const TDesC8* direction(
                    &XnPropertyNames::style::common::direction::KLTR() );

                if ( directionProperty )
                    {
                    direction = &directionProperty->StringValue();
                    }

                if ( blockProgressionProperty )
                    {
                    blockProgression = &blockProgressionProperty->StringValue();
                    }

                siblingSize += CalculateSpaceUsedByChildrenL( children, aNode,
                    *blockProgression, *direction,
                    aScreenDevice, aHorizontalUnitInPixels,
                    aVerticalUnitInPixels );
                }

            if ( siblingSize.iWidth > largestSize.iWidth )
                {
                largestSize.iWidth = siblingSize.iWidth;
                }
            if ( siblingSize.iHeight > largestSize.iHeight )
                {
                largestSize.iHeight = siblingSize.iHeight;
                }

            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KTB )
                {
                usedSiblingHeight += siblingSize.iHeight;
                usedSiblingWidth = largestSize.iWidth;
                }

            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KLR ||
                 aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KRL )
                {
                usedSiblingWidth += siblingSize.iWidth;
                usedSiblingHeight = largestSize.iHeight;
                }

            // collapse margins
            if ( aChildren.Count() < i )
                {
                CXnNode* next( aChildren[i + 1] );

                if ( ( aParentBlockProgression ==
                       XnPropertyNames::style::common::block_progression::KLR ||
                       aParentBlockProgression ==
                       XnPropertyNames::style::common::
                       block_progression::KRL ) && aParentDirection ==
                       XnPropertyNames::style::common::direction::KLTR )
                    {
                    TInt thisSiblingsRightMargin =
                        node->MarginRect().iBr.iX - node->BorderRect().iBr.iX;
                    TInt nextSiblingsLeftMargin =
                        next->BorderRect().iTl.iX - next->MarginRect().iTl.iX;

                    if ( nextSiblingsLeftMargin > thisSiblingsRightMargin )
                        {
                        usedSiblingWidth -= thisSiblingsRightMargin;
                        }
                    }

                if ( ( aParentBlockProgression ==
                       XnPropertyNames::style::common::block_progression::KRL ||
                       aParentBlockProgression ==
                       XnPropertyNames::style::common
                       ::block_progression::KLR ) && aParentDirection ==
                       XnPropertyNames::style::common::direction::KRTL )
                    {
                    TInt thisSiblingsLeftMargin =
                        node->MarginRect().iTl.iX - node->BorderRect().iTl.iX;
                    TInt nextSiblingsRightMargin =
                        next->BorderRect().iBr.iX - next->MarginRect().iBr.iX;

                    if ( nextSiblingsRightMargin > thisSiblingsLeftMargin )
                        {
                        usedSiblingWidth =- thisSiblingsLeftMargin;
                        }
                    }

                if ( ( aParentBlockProgression ==
                       XnPropertyNames::style::common::block_progression::KTB ||
                       aParentBlockProgression ==
                       XnPropertyNames::style::common::
                       block_progression::KBT ) && aParentDirection ==
                       XnPropertyNames::style::common::direction::KLTR )
                    {
                    TInt thisSiblingsBottomMargin =
                        node->MarginRect().iBr.iY - node->BorderRect().iBr.iY;
                    TInt nextSiblingsTopMargin =
                        next->BorderRect().iTl.iY - next->MarginRect().iTl.iY;

                    if ( nextSiblingsTopMargin > thisSiblingsBottomMargin )
                        {
                        usedSiblingHeight -= thisSiblingsBottomMargin;
                        }
                    }

                if ( ( aParentBlockProgression ==
                       XnPropertyNames::style::common::block_progression::KTB ||
                       aParentBlockProgression ==
                       XnPropertyNames::style::common::
                       block_progression::KBT ) && aParentDirection ==
                       XnPropertyNames::style::common::direction::KRTL )
                    {
                    TInt thisSiblingsTopMargin =
                        node->MarginRect().iTl.iY - node->BorderRect().iTl.iY;
                    TInt nextSiblingsBottomMargin =
                        next->BorderRect().iBr.iY - next->MarginRect().iBr.iY;

                    if ( nextSiblingsBottomMargin > thisSiblingsTopMargin )
                        {
                        usedSiblingHeight -= thisSiblingsTopMargin;
                        }
                    }
                }
            }
        }

    return TSize( usedSiblingWidth, usedSiblingHeight );
    }

// -----------------------------------------------------------------------------
// CalculateTotalDimensionsL()
// Scale dimensions for areas that have properties set to AUTO.
// -----------------------------------------------------------------------------
//
static TSize CalculateTotalDimensionsL( CXnNode& aNode, TBool aIgnoreAutoValues,
    TBool aIgnoreMinSizes, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // AUTO areas return their defined dimensions if not ignored
    // if ignored, 0 is returned as area dimensions
    CXnProperty* width( aNode.WidthL() );
    CXnProperty* height( aNode.HeightL() );
    TInt adaptive( aNode.IsAdaptive() );
    TBool autoNotFound( EFalse );

    if ( adaptive )
        {
        // Force to measure adaptive size
        aIgnoreAutoValues = EFalse;
        autoNotFound = ETrue;
        }
    if ( aIgnoreAutoValues )
        {
        TInt horizontalAuto( HasNodeHorizontalAutoValuesL( aNode ) );
        TInt verticalAuto( HasNodeVerticalAutoValuesL( aNode ) );
        if ( horizontalAuto && verticalAuto )
            {
            return TSize( 0, 0 );
            }
        else if ( horizontalAuto )
            {
            TInt h = VerticalPixelValueL( height, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            return TSize( 0, h );
            }
        else if ( verticalAuto )
            {
            TInt w = HorizontalPixelValueL( width, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            return TSize( w, 0 );
            }
        autoNotFound = ETrue;
        }

    TInt x( 0 );
    TInt y( 0 );

    if ( !aIgnoreMinSizes )
        {
        if ( !autoNotFound && HasNodeAutoValuesL( aNode ) )
            {
            // if auto values, check minimum sizes
            if ( height )
                {
                if ( IsPropertyAutoL( *height ) )
                    {
                    CXnProperty* minheight( aNode.MinHeightL() );
                    if ( minheight )
                        {
                        if ( !IsPropertyNone( *minheight ) )
                            {
                            y += VerticalPixelValueL( minheight,
                                aParentRect.Height(), aGraphicsDevice,
                                aVerticalUnitInPixels );
                            }
                        }
                    }
                }
            if ( width )
                {
                if ( IsPropertyAutoL( *width ) )
                    {
                    CXnProperty* minwidth( aNode.MinWidthL() );
                    if ( minwidth )
                        {
                        if ( !IsPropertyNone( *minwidth ) )
                            {
                            x += HorizontalPixelValueL( minwidth,
                                aParentRect.Width(), aGraphicsDevice,
                                aHorizontalUnitInPixels );
                            }
                        }
                    }
                }
            }
        }

    CXnProperty* marginLeft = aNode.MarginLeftL();
    CXnProperty* marginRight = aNode.MarginRightL();
    CXnProperty* borderLeft = aNode.BorderLeftL();
    CXnProperty* borderRight = aNode.BorderRightL();
    CXnProperty* paddingLeft = aNode.PaddingLeftL();
    CXnProperty* paddingRight = aNode.PaddingRightL();
    CXnProperty* marginTop = aNode.MarginTopL();
    CXnProperty* marginBottom = aNode.MarginBottomL();
    CXnProperty* borderTop = aNode.BorderTopL();
    CXnProperty* borderBottom = aNode.BorderBottomL();
    CXnProperty* paddingTop = aNode.PaddingTopL();
    CXnProperty* paddingBottom = aNode.PaddingBottomL();
    CXnProperty* borderWidth = aNode.BorderWidthL();
    CXnProperty* borderStyle = aNode.BorderStyleL();
    CXnProperty* borderImage = aNode.BorderImageL();
    CXnProperty* borderLeftStyle = aNode.BorderLeftStyleL();
    CXnProperty* borderRightStyle = aNode.BorderRightStyleL();
    CXnProperty* borderTopStyle = aNode.BorderTopStyleL();
    CXnProperty* borderBottomStyle = aNode.BorderBottomStyleL();

    TInt borderWidthValue( 0 );
    TInt borderHeightValue( 0 );
    TInt ignore;

    if ( marginLeft )
        {
        if ( autoNotFound || !IsPropertyAutoL( *marginLeft ) )
            {
            x += HorizontalPixelValueL( marginLeft, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }
    if ( marginRight )
        {
        if ( autoNotFound || !IsPropertyAutoL( *marginRight ) )
            {
            x += HorizontalPixelValueL( marginRight, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }
    if ( paddingLeft )
        {
        TRAP_IGNORE( x += HorizontalPixelValueL( paddingLeft,
            aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels ) );
        }
    if ( paddingRight )
        {
        TRAP_IGNORE( x += HorizontalPixelValueL( paddingRight,
            aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels ) );
        }
    if ( marginTop )
        {
        if ( autoNotFound || !IsPropertyAutoL( *marginTop ) )
            {
            y += VerticalPixelValueL( marginTop, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }
    if ( marginBottom )
        {
        if ( autoNotFound || !IsPropertyAutoL( *marginBottom ) )
            {
            y += VerticalPixelValueL( marginBottom, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }
    if ( paddingTop )
        {
        TRAP_IGNORE( y += VerticalPixelValueL( paddingTop,
            aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels ) );
        }
    if ( paddingBottom )
        {
        TRAP_IGNORE( y += VerticalPixelValueL( paddingBottom,
            aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels ) );
        }
    // is border image or border style defined?
    // if not, borders are ignored
    if ( borderWidth )
        {
        if ( borderImage || borderStyle )
            {
            DetermineBorderWidthsL( borderWidth, borderWidthValue,
                borderHeightValue, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            x += ( borderWidthValue * 2 );
            y += ( borderHeightValue * 2 );
            }
        }
    if ( borderLeft )
        {
        if ( borderImage || borderStyle || borderLeftStyle )
            {
            x -= borderWidthValue;
            TInt borderLeftValue( 0 );
            DetermineBorderWidthsL( borderLeft, ignore, borderLeftValue,
                aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            x += borderLeftValue;
            }
        }
    if ( borderRight )
        {
        if ( borderImage || borderStyle || borderRightStyle )
            {
            x -= borderWidthValue;
            TInt borderRightValue( 0 );
            DetermineBorderWidthsL( borderRight, ignore, borderRightValue,
                aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            x += borderRightValue;
            }
        }
    if ( borderTop )
        {
        if ( borderImage || borderStyle || borderTopStyle )
            {
            y -= borderHeightValue;
            TInt borderTopValue( 0 );
            DetermineBorderWidthsL( borderTop, borderTopValue, ignore,
                aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            y += borderTopValue;
            }
        }
    if ( borderBottom )
        {
        if ( borderImage || borderStyle || borderBottomStyle )
            {
            y -= borderHeightValue;
            TInt borderBottomValue( 0 );
            DetermineBorderWidthsL( borderBottom, borderBottomValue, ignore,
                aParentRect, aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            y += borderBottomValue;
            }
        }

    TInt maxHeight( 0 );
    TInt maxWidth( 0 );
    TInt minHeight( 0 );
    TInt minWidth( 0 );

    if ( HasNodeMinSizesL( aNode ) )
        {
        CXnProperty* heightproperty( aNode.HeightL() );
        CXnProperty* widthproperty( aNode.WidthL() );
        if ( heightproperty )
            {
            CXnProperty* minheightproperty( aNode.MinHeightL() );
            if ( minheightproperty )
                {
                if ( !IsPropertyNone( *minheightproperty ) )
                    {
                    minHeight = VerticalPixelValueL( minheightproperty,
                        aParentRect.Height(), aGraphicsDevice,
                        aVerticalUnitInPixels );
                    }
                }
            }
        if ( widthproperty )
            {
            CXnProperty* minwidthproperty( aNode.MinWidthL() );
            if ( minwidthproperty )
                {
                if (!IsPropertyNone( *minwidthproperty ) )
                    {
                    minWidth = HorizontalPixelValueL( minwidthproperty,
                        aParentRect.Width(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    }
                }
            }
        }
     if ( HasNodeMaxSizesL( aNode ) )
        {
        CXnProperty* heightProperty( aNode.HeightL() );
        CXnProperty* widthProperty( aNode.WidthL() );
        if ( heightProperty )
            {
            CXnProperty* maxHeightProperty( aNode.MaxHeightL() );
            if ( maxHeightProperty )
                {
                if ( !IsPropertyNone( *maxHeightProperty ) )
                    {
                    maxHeight = VerticalPixelValueL( maxHeightProperty,
                        aParentRect.Height(), aGraphicsDevice,
                        aVerticalUnitInPixels );
                    }
                }
            }
        if ( widthProperty )
            {
            CXnProperty* maxWidthProperty( aNode.MaxWidthL() );
            if ( maxWidthProperty )
                {
                if ( !IsPropertyNone( *maxWidthProperty ) )
                    {
                    maxWidth = HorizontalPixelValueL( maxWidthProperty,
                        aParentRect.Width(), aGraphicsDevice,
                        aHorizontalUnitInPixels );
                    }
                }
            }
        }
    if ( width && height )
        {
        if ( ( adaptive & XnAdaptive::EWidth ) &&
             ( adaptive & XnAdaptive::EHeight ) )
            {
            TSize available( aParentRect.Size() - TSize( x, y ) );
            if ( available.iWidth > 0 )
                {
                if ( available.iWidth > maxWidth && maxWidth > 0 )
                    {
                    available.iWidth = maxWidth;
                    }
                }
            if ( available.iHeight > 0 )
                {
                if ( available.iHeight > maxHeight && maxHeight > 0 )
                    {
                    available.iHeight = maxHeight;
                    }
                }
            TSize size( aNode.MeasureAdaptiveContentL( available ) );
            x += size.iWidth;
            y += size.iHeight;
            if ( minWidth > x )
                {
                x = minWidth;
                }
            if ( minHeight > y )
                {
                y = minHeight;
                }
            return TSize( x, y );
            }
        }
    if ( width )
        {
        if ( adaptive & XnAdaptive::EWidth )
            {
            TSize available( aParentRect.Size() - TSize( x, y ) );
            if ( available.iWidth > 0 )
                {
                if ( available.iWidth > maxWidth && maxWidth > 0 )
                    {
                    available.iWidth = maxWidth;
                    }
                x += aNode.MeasureAdaptiveContentL( available ).iWidth;
                if ( minWidth > x )
                    {
                    x = minWidth;
                    }
                }
            }
        else if ( autoNotFound || !IsPropertyAutoL( *width ) )
            {
            x += HorizontalPixelValueL( width, aParentRect.Width(),
                aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }
    if ( height )
        {
        if ( adaptive & XnAdaptive::EHeight )
            {
            TSize available( aParentRect.Size() - TSize( x, y ) );
            if ( available.iHeight > 0 )
                {
                if ( available.iHeight > maxHeight && maxHeight > 0 )
                    {
                    available.iHeight = maxHeight;
                    }
                y += aNode.MeasureAdaptiveContentL( available ).iHeight;
                if ( minHeight > y )
                    {
                    y = minHeight;
                    }
                }
            }
        else if ( autoNotFound || !IsPropertyAutoL( *height ) )
            {
            y += VerticalPixelValueL( height, aParentRect.Height(),
                aGraphicsDevice, aVerticalUnitInPixels );
            }
        }
    return TSize( x, y );
    }

// -----------------------------------------------------------------------------
// CutOnePixelFromPercentChildNodeL()
// If node has a child node with percentace defined sizes,
// one pixel may be cut off from it to fix possible rounding problem.
// -----------------------------------------------------------------------------
//
static TBool CutOnePixelFromPercentChildNodeL( CXnNode& aNode,
    const TDesC8& aParentBlockProgression )
    {
    RPointerArray< CXnNode >& children = aNode.Children();
    // find the last percent value node
    // possible percent values:
    // width
    // height
    // paddings
    // margins
    // borders
    for ( TInt i = children.Count() -1; i < -1 ; --i )
        {
        CXnNode* currentNode = children[i];

        TInt x = 0;
        TInt y = 0;

        CXnProperty* widthHeight;
        CXnProperty* paddingTopLeft;
        CXnProperty* paddingBottomRight;
        CXnProperty* borderTopLeft;
        CXnProperty* borderBottomRight;
        CXnProperty* marginTopLeft;
        CXnProperty* marginBottomRight;

        if ( ( aParentBlockProgression ==
               XnPropertyNames::style::common::block_progression::KLR ) ||
               aParentBlockProgression ==
               XnPropertyNames::style::common::block_progression::KRL )
            {
            x = -1;
            y = 0;
            widthHeight = currentNode->WidthL();
            paddingTopLeft = currentNode->PaddingLeftL();
            paddingBottomRight = currentNode->PaddingRightL();
            borderTopLeft = currentNode->BorderLeftL();
            borderBottomRight = currentNode->BorderRightL();
            marginTopLeft = currentNode->MarginLeftL();
            marginBottomRight = currentNode->MarginRightL();
            }
        else
            {
            y = -1;
            x = 0;
            widthHeight = currentNode->HeightL();
            paddingTopLeft = currentNode->PaddingTopL();
            paddingBottomRight = currentNode->PaddingBottomL();
            borderTopLeft = currentNode->BorderTopL();
            borderBottomRight = currentNode->BorderBottomL();
            marginTopLeft = currentNode->MarginTopL();
            marginBottomRight = currentNode->MarginBottomL();
            }
        if ( widthHeight )
            {
            if ( IsValuePercentageL( *widthHeight ) )
                {
                // shrink width one pixel
                TRect rect( currentNode->Rect() );
                rect.Resize( x, y );
                currentNode->SetRect( rect );
                TRect paddingRect( currentNode->PaddingRect() );
                paddingRect.Resize( x, y );
                currentNode->SetPaddingRect( paddingRect );
                TRect borderRect( currentNode->BorderRect() );
                borderRect.Resize( x, y );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( paddingTopLeft )
            {
            if ( IsValuePercentageL( *paddingTopLeft ) )
                {
                // shrink left padding one pixel
                TRect rect( currentNode->Rect() );
                rect.Move( x, y );
                currentNode->SetRect( rect );
                TRect paddingRect( currentNode->PaddingRect() );
                paddingRect.Resize( x, y );
                currentNode->SetPaddingRect( paddingRect );
                TRect borderRect( currentNode->BorderRect() );
                TRect normalFlowBorderRect(
                    currentNode->NormalFlowBorderRect() );
                borderRect.Resize( x, y );
                normalFlowBorderRect.Resize( x, y );
                currentNode->SetBorderRect( borderRect );
                aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( paddingBottomRight )
            {
            if ( IsValuePercentageL( *paddingBottomRight ) )
                {
                // shrink right padding one pixel
                TRect paddingRect( currentNode->PaddingRect() );
                paddingRect.Resize( x, y );
                currentNode->SetPaddingRect( paddingRect );
                TRect borderRect( currentNode->BorderRect() );
                TRect normalFlowBorderRect(
                    currentNode->NormalFlowBorderRect() );
                borderRect.Resize( x, y );
                normalFlowBorderRect.Resize( x, y );
                currentNode->SetBorderRect( borderRect );
                aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( borderTopLeft )
            {
            if ( IsValuePercentageL( *borderTopLeft ) )
                {
                TRect rect( currentNode->Rect() );
                rect.Move( x, y );
                currentNode->SetRect( rect );
                TRect paddingRect( currentNode->PaddingRect() );
                paddingRect.Move( x, y );
                currentNode->SetPaddingRect( paddingRect );
                TRect borderRect( currentNode->BorderRect() );
                TRect normalFlowBorderRect(
                    currentNode->NormalFlowBorderRect() );
                borderRect.Resize( x, y );
                normalFlowBorderRect.Resize( x, y );
                currentNode->SetBorderRect( borderRect );
                aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( borderBottomRight )
            {
            if ( IsValuePercentageL( *borderBottomRight ) )
                {
                TRect borderRect( currentNode->BorderRect() );
                TRect normalFlowBorderRect( currentNode->BorderRect() );
                borderRect.Resize( x, y );
                normalFlowBorderRect.Resize( x, y );
                currentNode->SetBorderRect( borderRect );
                aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( marginTopLeft )
            {
            if ( IsValuePercentageL( *marginTopLeft ) )
                {
                TRect rect( currentNode->Rect() );
                rect.Move( x, y );
                currentNode->SetRect( rect );
                TRect paddingRect( currentNode->PaddingRect() );
                paddingRect.Move( x, y );
                currentNode->SetPaddingRect( paddingRect );
                TRect borderRect( currentNode->BorderRect() );
                TRect normalFlowBorderRect(
                    currentNode->NormalFlowBorderRect() );
                borderRect.Move( x, y );
                normalFlowBorderRect.Move( x, y );
                currentNode->SetBorderRect( borderRect );
                aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        if ( marginBottomRight )
            {
            if ( IsValuePercentageL( *marginBottomRight ) )
                {
                TRect marginRect( currentNode->MarginRect() );

                marginRect.Resize( x, y );

                currentNode->SetMarginRect( marginRect );

                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// DetermineBorderWidthsL()
// Determine border widths.
// -----------------------------------------------------------------------------
//
static void DetermineBorderWidthsL( CXnProperty* aBorderWidthProperty,
    TInt& aBordervertical, TInt& aBorderhorizontal, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    CXnDomList& propertyValueList =
        aBorderWidthProperty->Property()->PropertyValueList();
    for ( TInt i = propertyValueList.Length() - 1; i >= 0; --i )
        {
        CXnDomPropertyValue* value =
            static_cast< CXnDomPropertyValue* >( propertyValueList.Item( i ) );
        if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
             value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
            {
            // border width was defined as string
            const TDesC8& borderwidth = aBorderWidthProperty->StringValue();
            if ( borderwidth ==
                 XnPropertyNames::style::common::border_width::KThin )
                {
                aBordervertical = KXnBorderThin;
                aBorderhorizontal = KXnBorderThin;
                }
            else if ( borderwidth ==
                      XnPropertyNames::style::common::border_width::KMedium )
                {
                aBordervertical = KXnBorderMedium;
                aBorderhorizontal = KXnBorderMedium;
                }
            else
                {
                aBordervertical = KXnBorderThick;
                aBorderhorizontal = KXnBorderThick;
                }
            }
        else
            {
            // border width was defined as int
            aBordervertical = VerticalPixelValueL( aBorderWidthProperty,
                aParentRect.Height(), aGraphicsDevice, aVerticalUnitInPixels );
            aBorderhorizontal = HorizontalPixelValueL( aBorderWidthProperty,
                aParentRect.Width(), aGraphicsDevice, aHorizontalUnitInPixels );
            }
        }
    }

// -----------------------------------------------------------------------------
// DisplayedChildrenCountL()
// Return number of node's displayed children.
// -----------------------------------------------------------------------------
//
static TInt DisplayedChildrenCountL( CXnNode& aNode,
    RPointerArray< CXnNode >* aDisplayedChildren )
    {
    RPointerArray< CXnNode >& children = aNode.Children();
    TInt displayedCount( 0 );

    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* node( children[i] );
        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL( *node ) && !IsNodeTooltip( *node ) )
            {
            if ( aDisplayedChildren )
                {
                aDisplayedChildren->AppendL( node );
                }
            displayedCount++;
            }
        }

    return displayedCount;
    }

// -----------------------------------------------------------------------------
// DisplayedParentL()
// Return node's displayed parent.
// -----------------------------------------------------------------------------
//
static CXnNode* DisplayedParentL( CXnNode& aNode )
    {
    if ( aNode.ViewNodeImpl() )
        {
        return &aNode;
        }
    CXnNode* parent = aNode.Parent();
    return parent;
    }

// -----------------------------------------------------------------------------
// DomPropertyValueFromProperty()
// Return DOM property value from property.
// -----------------------------------------------------------------------------
//
static CXnDomPropertyValue* DomPropertyValueFromProperty(
    CXnProperty* aProperty )
    {
    if ( !aProperty )
        {
        return NULL;
        }
    CXnDomProperty* domProperty = aProperty->Property();
    if ( !domProperty )
        {
        return NULL;
        }
    return static_cast<
        CXnDomPropertyValue* >( domProperty->PropertyValueList().Item( 0 ) );
    }

// -----------------------------------------------------------------------------
// DomPropertyValueFromProperty()
// Return DOM property value from property.
// -----------------------------------------------------------------------------
// 
static TBool IsChildOfScrollableControl( CXnNode& aNode )
    {
    CXnNode* parent = &aNode;
    while( parent )
        {
        if( IsSrollableBox( *parent ) )
            {
            return ETrue;
            }
        parent = parent->Parent();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// DropExceedingRectL()
// Drop node if node's margin rect exceeds it's parent's content rect.
// -----------------------------------------------------------------------------
//
static void DropExceedingRectL( const TRect& aParentRect, CXnNode& aNode,
    TBool aDropRelativeNode )
    {
    if( IsChildOfScrollableControl( aNode ) )
        {
        return;
        }
    
    if ( IsNodeTooltip( aNode ) )
        {
        // tooltip never dropped
        return;
        }

    CXnProperty* positionProp( aNode.PositionL() );

    if ( positionProp )
        {
        const TDesC8& value( positionProp->StringValue() );
        if ( value == XnPropertyNames::style::common::position::KFloating )
            {
            // Don't change floating positioned box dimensions
            return;
            }
        }

    TRect parentRect( aParentRect );
    TRect marginRect( aNode.MarginRect() );

    // does the margin rect fit in its parent?
    TInt parentLeft = parentRect.iTl.iX;
    TInt parentRight = parentRect.iBr.iX;
    TInt parentTop = parentRect.iTl.iY;
    TInt parentBottom = parentRect.iBr.iY;

    TInt rectLeft = marginRect.iTl.iX;
    TInt rectRight = marginRect.iBr.iX;
    TInt rectTop = marginRect.iTl.iY;
    TInt rectBottom = marginRect.iBr.iY;

    TInt priority = 0;
    CXnProperty* priorityProperty = aNode.DisplayPriorityL();

    if ( priorityProperty )
        {
        TRAPD( error, priority = priorityProperty->FloatValueL() );
        if ( error != KErrNone )
            {
            priority = 0;
            }
        }

    TInt leftOffset( 0 );
    TInt rightOffset( 0 );
    TInt topOffset( 0 );
    TInt bottomOffset( 0 );

    if ( priority != 0 )
        {
        TBool drop = CalculateExceedingArea( *aNode.Parent(), aNode,
            leftOffset, rightOffset, topOffset, bottomOffset );
        if ( drop )
            {
            aNode.UiEngine()->SetNodeDroppedL( aNode, XnNodeLayout::EDropped );
            }
        }
    // If display priority is 0 as much as possible must be shown, but
    // parent's content rect must not be exceeded.
    else
        {
        // This must not be done for tracster. It is a special component,
        // which takes care
        // of its final layout and therefore its children can exceed
        // the parent's rect.
        CXnNode* parent = aNode.Parent();

        // If the node is relatively positioned, and this is not called from
        // CalculateRelativePositionsL(), then must not be cropped!!

        // Default is static
        const TDesC8* value(
            &XnPropertyNames::style::common::position::KStatic() );

        if ( positionProp && positionProp->StringValue() != KNullDesC8 )
            {
            value = &positionProp->StringValue();
            }
        if ( !aDropRelativeNode &&
             XnPropertyNames::style::common::position::KRelative() == *value )
            // relative nodes must not be adjusted, if this is not called
            // from CalculateRelativePositionsL().
            {
            return;
            }
        if ( rectLeft < parentLeft )
            {
            leftOffset = parentLeft - rectLeft;
            }
        if ( rectRight > parentRight )
            {
            rightOffset = rectRight - parentRight;
            }
        if ( rectTop < parentTop )
            {
            topOffset = parentTop - rectTop;
            }
        if ( rectBottom > parentBottom )
            {
            bottomOffset = rectBottom - parentBottom;
            }
        AdjustRectsL( aNode, leftOffset, rightOffset, topOffset, bottomOffset );
        }
    }

// -----------------------------------------------------------------------------
// CalculateExceedingArea()
// Calculates how much a dropped node affects to the area of its parent
// that is content based node.
// Returns whether a child's margin rect exceeds its parents content
// rect or not.
// -----------------------------------------------------------------------------
//
static TBool CalculateExceedingArea( CXnNode& aParent, CXnNode& aChild,
    TInt& aLeftOffset, TInt& aRightOffset, TInt& aTopOffset,
    TInt& aBottomOffset )
    {
    TBool ret( EFalse );

    TRect parentRect( aParent.Rect() );
    TInt parentLeft = parentRect.iTl.iX;
    TInt parentRight = parentRect.iBr.iX;
    TInt parentTop = parentRect.iTl.iY;
    TInt parentBottom = parentRect.iBr.iY;

    TRect marginRect( aChild.MarginRect() );
    TInt rectLeft = marginRect.iTl.iX;
    TInt rectRight = marginRect.iBr.iX;
    TInt rectTop = marginRect.iTl.iY;
    TInt rectBottom = marginRect.iBr.iY;

    TInt adaptive( aParent.IsAdaptive() );

    if ( rectLeft < parentLeft )
        {
        if ( adaptive & XnAdaptive::EWidth )
            {
            aLeftOffset = rectRight - parentLeft;
            if ( aLeftOffset < 0 )
                {
                aLeftOffset = 0;
                }
            }
        ret = ETrue;
        }
    if ( rectRight > parentRight )
        {
        if ( adaptive & XnAdaptive::EWidth )
            {
            aRightOffset = parentRight - rectLeft;
            if ( aRightOffset < 0 )
                {
                aRightOffset = 0;
                }
            }
        ret = ETrue;
        }
    if ( rectTop < parentTop )
        {
        if ( adaptive & XnAdaptive::EHeight )
            {
            aTopOffset = rectBottom - parentTop;
            if ( aTopOffset < 0 )
                {
                aTopOffset = 0;
                }
            }
        ret = ETrue;
        }
    if ( rectBottom > parentBottom )
        {
        if ( adaptive & XnAdaptive::EHeight )
            {
            aBottomOffset = parentBottom - rectTop;
            if ( aBottomOffset < 0 )
                {
                aBottomOffset = 0;
                }
            }
        ret = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// AdjustRectsL()
// Adjust all the rects according to the given offsets.
// -----------------------------------------------------------------------------
//
static void AdjustRectsL( CXnNode& aNode, TInt aLeftOffset, TInt aRightOffset,
    TInt aTopOffset, TInt aBottomOffset )
    {
    if ( aLeftOffset || aRightOffset || aTopOffset || aBottomOffset )
        {
#ifdef _XN3_DEBUG_
        CXnProperty* id( aNode.IdL() );

        if ( id )
            {
            TBuf8< 256 > debug;
            debug.Append( _L8( "Node id: " ) );
            debug.Append( id->StringValue() );
            debug.Append( _L8( "doesn't fit to parent and will be adjusted" ) );

            RDebug::RawPrint( debug );
            }
#endif

        // Adjust margin rect
        TRect marginRect( aNode.MarginRect() );
        TRect newRect = TRect( marginRect.iTl.iX + aLeftOffset,
            marginRect.iTl.iY + aTopOffset, marginRect.iBr.iX - aRightOffset,
            marginRect.iBr.iY - aBottomOffset );
        aNode.SetMarginRect( newRect );

        // Adjust border rect
        TRect borderRect( aNode.BorderRect() );
        newRect = TRect( borderRect.iTl.iX + aLeftOffset,
            borderRect.iTl.iY + aTopOffset, borderRect.iBr.iX - aRightOffset,
            borderRect.iBr.iY - aBottomOffset );
        aNode.SetBorderRect( newRect );

        // Adjust normal flow border rect
        TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );
        newRect = TRect( normalFlowBorderRect.iTl.iX + aLeftOffset,
            normalFlowBorderRect.iTl.iY + aTopOffset,
            normalFlowBorderRect.iBr.iX - aRightOffset,
            normalFlowBorderRect.iBr.iY - aBottomOffset );
        aNode.SetNormalFlowBorderRect( newRect );

        // Adjust padding rect
        TRect paddingRect( aNode.PaddingRect() );
        newRect = TRect( paddingRect.iTl.iX + aLeftOffset,
            paddingRect.iTl.iY + aTopOffset, paddingRect.iBr.iX - aRightOffset,
            paddingRect.iBr.iY - aBottomOffset );
        aNode.SetPaddingRect( newRect );

        // Adjust content rect
        TRect contentRect( aNode.Rect() );
        newRect = TRect( contentRect.iTl.iX + aLeftOffset,
            contentRect.iTl.iY + aTopOffset, contentRect.iBr.iX - aRightOffset,
            contentRect.iBr.iY - aBottomOffset );
        aNode.SetRect( newRect );
        }
    }

// -----------------------------------------------------------------------------
// ClearRects()
// Sets sizes of the rects to 0.
// -----------------------------------------------------------------------------
//
static void ClearRects( CXnNode& aNode, TBool aRecurse )
    {
    TRect empty;

    aNode.SetRect( empty );
    aNode.SetPaddingRect( empty );
    aNode.SetBorderRect( empty );
    aNode.SetNormalFlowBorderRect( empty );
    aNode.SetMarginRect( empty );

    CCoeControl* control( aNode.Control() );

    if ( control )
        {
        control->SetRect( empty );
        }

    if ( aRecurse )
        {
        RPointerArray< CXnNode >& children( aNode.Children() );
        for ( TInt i = 0; i < children.Count(); i++ )
            {
            ClearRects( *children[i], aRecurse );
            }
        }
    }

// -----------------------------------------------------------------------------
// FitChildrenIntoParentL()
// Drop child nodes until they fit into the parent. Also, if node has
// a child node with percentace defined sizes, one pixel may be cut off from it
// to fix possible rounding problem.
// -----------------------------------------------------------------------------
//
static void FitChildrenIntoParentL( CXnNode& aNode,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
	if( IsSrollableBox( aNode ) )
	    {
	    return;
	    }
    RPointerArray< CXnNode >& children( aNode.Children() );

    if ( children.Count() == 0 )
        {
        return;
        }

    CXnProperty* blockProgressionProperty( aNode.BlockProgressionL() );
    CXnProperty* directionProperty( aNode.DirectionL() );

    const TDesC8* blockProgression = NULL;

    if ( !blockProgressionProperty )
        {
        // using default
        blockProgression =
            &XnPropertyNames::style::common::block_progression::KTB();
        }
    else
        {
        blockProgression = &blockProgressionProperty->StringValue();
        }

    const TDesC8* direction( NULL );

    if ( !directionProperty )
        {
        // use default
        direction = &XnPropertyNames::style::common::direction::KLTR();
        }
    else
        {
        direction = &directionProperty->StringValue();
        }

    TSize space = CalculateSpaceUsedByChildrenL( children, aNode,
        *blockProgression, *direction, aGraphicsDevice,
        aHorizontalUnitInPixels, aVerticalUnitInPixels );
    TSize parentSize( aNode.Rect().Size() );

    while( ( ( parentSize.iHeight < space.iHeight ) &&
             ( parentSize.iHeight > 0 ) ) ||
           ( ( parentSize.iWidth < space.iWidth ) && 
             ( parentSize.iWidth > 0 ) ) )
        {
        // if areas go over just one pixel,
        // cut the pixel from a percent sized node
        if ( ( parentSize.iHeight == space.iHeight - 1 ) ||
             ( parentSize.iWidth == space.iWidth - 1 ) )
            {
            CutOnePixelFromPercentChildNodeL( aNode, *blockProgression );
            // Childs are now in parent size
            return;
            }

        TInt childCount( DisplayedChildrenCountL( aNode ) );

        PriorizeChildrenL( aNode );

        if ( DisplayedChildrenCountL( aNode ) == childCount )
            {
            // cannot drop any more children, but they still don't fit
            return;
            }

        space = CalculateSpaceUsedByChildrenL( children, aNode,
            *blockProgression, *direction, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );
        }

    return;
    }

// -----------------------------------------------------------------------------
// GetPositionL()
// Return position coordinate of a CXnComponent. Returned position depends on
// given block progression.
// -----------------------------------------------------------------------------
//
static TInt GetPositionL( CXnNode& aNode, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection )
    {
    TInt returnValue = 0;
    if ( aParentDirection == XnPropertyNames::style::common::direction::KLTR )
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KBT )
            {
            returnValue = aNode.NormalFlowBorderRect().iBr.iY;
            }
        else    // LR or RL
            {
            returnValue = aNode.NormalFlowBorderRect().iBr.iX;
            }
        }
    else // RTL
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KBT )
            {
            returnValue = aNode.NormalFlowBorderRect().iTl.iY;
            }
        else    // LR or RL
            {
            returnValue = aNode.NormalFlowBorderRect().iTl.iX;
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// HasNodeAutoValues()
// Check if node has properties set to AUTO.
// -----------------------------------------------------------------------------
//
static TInt HasNodeAutoValuesL( CXnNode& aNode )
    {
    return HasNodeHorizontalAutoValuesL( aNode ) ||
        HasNodeVerticalAutoValuesL( aNode );
    }

// -----------------------------------------------------------------------------
// HasNodeHorizontalValues()
// Check if node has properties set to AUTO.
// -----------------------------------------------------------------------------
//
static TInt HasNodeHorizontalAutoValuesL( CXnNode& aNode )
    {
    TInt autoCount = 0;

    CXnProperty* width = aNode.WidthL();

    if ( width )
        {
        if ( IsPropertyAutoL( *width ) )
             {
             autoCount++;
             }
        }
    else
        {
        // width is auto as default
        autoCount++;
        }

    CXnProperty* marginleft = aNode.MarginLeftL();

    if ( marginleft )
        {
        if ( IsPropertyAutoL( *marginleft ) )
            {
            autoCount++;
            }
        }

    CXnProperty* marginright = aNode.MarginRightL();

    if ( marginright )
        {
        if ( IsPropertyAutoL( *marginright ) )
            {
            autoCount++;
            }
        }

    if ( !IsAbsoluteL( aNode ) && !IsRelativeL( aNode ) )
        {
        return autoCount;
        }

    CXnProperty* left = aNode.LeftL();

    if ( left )
        {
        if ( IsPropertyAutoL( *left ) )
            {
            autoCount++;
            }
        }

    CXnProperty* right = aNode.RightL();

    if ( right )
        {
        if ( IsPropertyAutoL( *right ) )
            {
            autoCount++;
            }
        }

    return autoCount;
    }

// -----------------------------------------------------------------------------
// HasNodeVerticalAutoValues()
// Check if node has properties set to AUTO.
// -----------------------------------------------------------------------------
//
static TInt HasNodeVerticalAutoValuesL( CXnNode& aNode )
    {
    TInt autoCount = 0;

    CXnProperty* height = aNode.HeightL();

    if ( height )
        {
        if ( IsPropertyAutoL( *height ) )
            {
            autoCount++;
            }
        }
    else
        {
        // height is auto as default
        autoCount++;
        }

    CXnProperty* margintop = aNode.MarginTopL();

    if ( margintop )
        {
        if ( IsPropertyAutoL( *margintop ) )
            {
            autoCount++;
            }
        }

    CXnProperty* marginbottom = aNode.MarginBottomL();

    if ( marginbottom )
        {
        if ( IsPropertyAutoL( *marginbottom ) )
            {
            autoCount++;
            }
        }

    if ( !IsAbsoluteL( aNode ) && !IsRelativeL( aNode ) )
        {
        return autoCount;
        }

    CXnProperty* top = aNode.TopL();

    if ( top )
        {
        if ( IsPropertyAutoL( *top ) )
            {
            autoCount++;
            }
        }

    CXnProperty* bottom = aNode.BottomL();

    if ( bottom )
        {
        if ( IsPropertyAutoL( *bottom ) )
            {
            autoCount++;
            }
        }

    return autoCount;
    }

// -----------------------------------------------------------------------------
// HasNodeMinSizesL()
// Check if node has minimum size values.
// -----------------------------------------------------------------------------
//
static TBool HasNodeMinSizesL( CXnNode& aNode )
    {
    CXnProperty* minWidth = aNode.MinWidthL();

    if ( minWidth )
        {
        if ( !IsPropertyNone( *minWidth ) )
            {
            return ETrue;
            }
        }

    CXnProperty* minHeight = aNode.MinHeightL();

    if ( minHeight )
        {
        if ( !IsPropertyNone( *minHeight ) )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// HasNodeMaxSizesL()
// Check if node has maximum size values.
// -----------------------------------------------------------------------------
//
static TBool HasNodeMaxSizesL( CXnNode& aNode )
    {
    CXnProperty* maxWidth = aNode.MaxWidthL();

    if ( maxWidth )
        {
        if ( !IsPropertyNone( *maxWidth ) )
            {
            return ETrue;
            }
        }

    CXnProperty* maxHeight = aNode.MaxHeightL();

    if ( maxHeight )
        {
        if ( !IsPropertyNone( *maxHeight ) )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// HorizontalCentimetersToPixelsL()
// Convert horizontal centimeter value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalCentimetersToPixelsL( TReal& aCentimeters,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aCentimeters * KCentimetersAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.HorizontalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// HorizontalInchesToPixelsL()
// Convert horizontal inch value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalInchesToPixelsL( TReal& aInches,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aInches * KInchesAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.HorizontalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// HorizontalMillimetersToPixelsL()
// Convert horizontal millimeter value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalMillimetersToPixelsL( TReal& aMillimeters,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aMillimeters * KMillimetersAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.HorizontalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// HorizontalPicasToPixelsL()
// Convert horizontal pica value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalPicasToPixelsL( TReal& aPicas,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aPicas * KPicasAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.HorizontalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// HorizontalPixelValueL()
// Convert horizontal property to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalPixelValueL( CXnProperty* aValue, TInt aReferenceValue,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels )
    {
    return HorizontalValueToPixelsL( DomPropertyValueFromProperty( aValue ),
        static_cast< TReal >( aReferenceValue ), aScreenDevice,
        aHorizontalUnitInPixels );
    }

// -----------------------------------------------------------------------------
// HorizontalPointsToPixelsL()
// Convert horizontal point value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalPointsToPixelsL( TReal& aPoints,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aPoints * KPointsAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.HorizontalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// HorizontalUnitsToPixelsL()
// Convert horizontal unit value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalUnitsToPixelsL( TReal aUnits, TReal aUnitInPixels )
    {
    TInt pixels = aUnits * aUnitInPixels;
    return pixels;
    }

// -----------------------------------------------------------------------------
// VerticalUnitsToPixelsL()
// Convert vertical unit value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalUnitsToPixelsL( TReal aUnits, TReal aUnitInPixels )
    {
    TInt pixels = aUnits * aUnitInPixels;
    return pixels;
    }

// -----------------------------------------------------------------------------
// HorizontalValueToPixelsL()
// Convert horizontal property value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt HorizontalValueToPixelsL( CXnDomPropertyValue* aValue,
    TReal aReferenceValue, CGraphicsDevice& aScreenDevice, TReal aUnitInPixels )
    {
    if ( !aValue )
        {
        User::Leave( KXnErrHorizontalValueNULL );
        }
    CXnDomPropertyValue::TPrimitiveValueType valueType =
        aValue->PrimitiveValueType();
    //  Handle error cases first, inherit is handled elsewhere, none returns 0
    switch ( valueType )
        {
        case CXnDomPropertyValue::EUnknown:
        case CXnDomPropertyValue::EEms:
        case CXnDomPropertyValue::EExs:
        case CXnDomPropertyValue::EDeg:
        case CXnDomPropertyValue::ERad:
        case CXnDomPropertyValue::EGrad:
        case CXnDomPropertyValue::EMs:
        case CXnDomPropertyValue::ES:
        case CXnDomPropertyValue::EHz:
        case CXnDomPropertyValue::EKHz:
        case CXnDomPropertyValue::EDimension:
        case CXnDomPropertyValue::EString:
        case CXnDomPropertyValue::EUri:
        case CXnDomPropertyValue::EIdent:
        case CXnDomPropertyValue::EAttr:
        case CXnDomPropertyValue::ECounter:
        case CXnDomPropertyValue::ERect:
        case CXnDomPropertyValue::ERgbColor:
        case CXnDomPropertyValue::ERgbaColor:
        case CXnDomPropertyValue::EFunction:
            break;
        default:
            {
            TReal value = aValue->FloatValueL();
            if ( valueType == CXnDomPropertyValue::ENumber ||
                 valueType == CXnDomPropertyValue::EPx )
                {
                return static_cast< TInt >( value );
                }
            else if ( valueType == CXnDomPropertyValue::EPercentage )
                {
                return PercentageToPixelsL( value, aReferenceValue );
                }
            else if ( valueType == CXnDomPropertyValue::EIn )
                {
                return HorizontalInchesToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::ECm )
                {
                return HorizontalCentimetersToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EMm )
                {
                return HorizontalMillimetersToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EPt )
                {
                return HorizontalPointsToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EPc )
                {
                return HorizontalPicasToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EUnitValue )
                {
                return HorizontalUnitsToPixelsL( value, aUnitInPixels );
                }
            }
            break;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// IsAbsoluteL()
// Check if node's positioning is absolute.
// -----------------------------------------------------------------------------
//
static TBool IsAbsoluteL( CXnNode& aNode )
    {
    CXnProperty* prop( aNode.PositionL() );

    if ( prop )
        {
        const TDesC8& value( prop->StringValue() );
        if ( value == XnPropertyNames::style::common::position::KAbsolute ||
             value == XnPropertyNames::style::common::position::KFloating )
            {
            return ETrue;
            }
         }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsLargerThanMaxSizeL()
// Check if node rect size exceeds it's max sizes.
// -----------------------------------------------------------------------------
//
static TBool IsLargerThanMaxSizeL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels, 
    TReal aVerticalUnitInPixels )
    {
    TRect parentRect = aParentRect;
    CXnProperty* heightProperty = aNode.HeightL();
    CXnProperty* widthProperty = aNode.WidthL();

    if ( heightProperty )
        {
        CXnProperty* maxHeightProperty = aNode.MaxHeightL();
        if ( maxHeightProperty )
            {
            if ( !IsPropertyNone( *maxHeightProperty ) )
                {
                TInt maxHeight = VerticalPixelValueL( maxHeightProperty,
                    parentRect.Height(), aScreenDevice, aVerticalUnitInPixels );
                TInt height = aNode.Rect().Height();
                if ( height > maxHeight )
                    {
                    // height larger than max
                    return ETrue;
                    }
                }
            }
        }

    if ( widthProperty )
        {
        CXnProperty* maxWidthProperty = aNode.MaxWidthL();
        if ( maxWidthProperty )
            {
            if ( !IsPropertyNone( *maxWidthProperty ) )
                {
                TInt maxWidth = HorizontalPixelValueL( maxWidthProperty,
                    parentRect.Width(), aScreenDevice, aHorizontalUnitInPixels );
                TInt width = aNode.Rect().Width();
                if ( width > maxWidth )
                    {
                    // width larger than max
                    return ETrue;
                    }
                }
            }
        }

    // width and height within limits
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsNodeDisplayedL()
// Check if node is able to be displayed.
// -----------------------------------------------------------------------------
//
static TBool IsNodeDisplayedL( CXnNode& aNode )
    {
    if ( !aNode.IsLayoutCapable() )
        {
        return EFalse;
        }

    // Am I displayed?
    CXnProperty* displayProp( aNode.DisplayL() );

    if ( displayProp )
        {
        const TDesC8& display( displayProp->StringValue() );

        if ( display != XnPropertyNames::style::common::display::KBlock )
            {
            return EFalse;
            }
        }

    CXnNode* parent( aNode.Parent() );

    // Is my parent displayed?
    for ( ; parent; parent = parent->Parent() )
        {
        CXnProperty* displayProp( parent->DisplayL() );

        if ( displayProp )
            {
            const TDesC8& display( displayProp->StringValue() );

            if ( display != XnPropertyNames::style::common::display::KBlock )
                {
                return EFalse;
                }
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// IsNodeTooltip()
// Check if node is a tooltip.
// -----------------------------------------------------------------------------
//
static TBool IsNodeTooltip( CXnNode& aNode )
    {
    _LIT8( tooltip, "tooltip" );

    if ( aNode.Type()->Type() == tooltip )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsPropertyAutoL()
// Check if property is set to AUTO.
// -----------------------------------------------------------------------------
//
static TBool IsPropertyAutoL( CXnProperty& aProperty )
    {
    if ( &aProperty )
        {
        TBool returnValue;
        TInt index = 0;
        CXnDomProperty* domProperty = aProperty.Property();
        CXnDomPropertyValue* domPropertyValue =
            static_cast< CXnDomPropertyValue* >(
            domProperty->PropertyValueList().Item( index ) );
        returnValue = domPropertyValue->IsAutoIdent();
        if ( !returnValue )
            {
            // if return value is string and not auto or none,
            // return ETrue because auto is used as default value
            if ( IsStringValueL( aProperty ) && !IsPropertyNone( aProperty ) )
                {
                return ETrue;
                }
            }
        return returnValue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsPropertyNone()
// Check if property is set to NONE.
// -----------------------------------------------------------------------------
//
static TBool IsPropertyNone( CXnProperty& aProperty )
    {
    if ( &aProperty )
        {
          TInt index = 0;
          CXnDomProperty* domProperty = aProperty.Property();
          CXnDomPropertyValue* domPropertyValue =
              static_cast< CXnDomPropertyValue* >(
              domProperty->PropertyValueList().Item( index ) );
          return domPropertyValue->IsNoneIdent();
          }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsRelativeL()
// Check if node's positioning is absolute.
// -----------------------------------------------------------------------------
//
static TBool IsRelativeL( CXnNode& aNode )
    {
    CXnProperty* positioningProperty = aNode.PositionL();
    if ( positioningProperty )
        {
        const TDesC8& positioning = positioningProperty->StringValue();
        if ( positioning == XnPropertyNames::style::common::position::KRelative )
            {
            return ETrue;
            }
         }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsSmallerThanMinSizeL()
// Check if node rect size is smaller than it's min sizes.
// -----------------------------------------------------------------------------
//
static TBool IsSmallerThanMinSizeL( const TRect& aParentRect, CXnNode& aNode,
    CGraphicsDevice& aScreenDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TRect parentRect = aParentRect;
    CXnProperty* heightproperty = aNode.HeightL();
    CXnProperty* widthproperty = aNode.WidthL();

    if ( heightproperty )
        {
        CXnProperty* minheightproperty = aNode.MinHeightL();
        if ( minheightproperty )
            {
            if ( !IsPropertyNone( *minheightproperty ) )
                {
                TInt minheight = VerticalPixelValueL( minheightproperty,
                    parentRect.Height(), aScreenDevice, aVerticalUnitInPixels );
                TInt height = aNode.Rect().Height();
                if ( height < minheight )
                    {
                    // height smaller than minimum
                    return ETrue;
                    }
                }
            }
        }
    if ( widthproperty )
        {
        CXnProperty* minwidthproperty = aNode.MinWidthL();
        if ( minwidthproperty )
            {
            if ( !IsPropertyNone( *minwidthproperty ) )
                {
                TInt minwidth = HorizontalPixelValueL( minwidthproperty,
                    parentRect.Width(), aScreenDevice, aHorizontalUnitInPixels );
                TInt width = aNode.Rect().Width();
                if ( width < minwidth )
                    {
                    // width smaller than minimum
                    return ETrue;
                    }
                }
            }
        }
    // width and height within limits
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsStringValueL()
// Check if property value is string.
// -----------------------------------------------------------------------------
//
static TBool IsStringValueL( CXnProperty& aProperty )
    {
    CXnDomList& propertyValueList = aProperty.Property()->PropertyValueList();
    CXnDomPropertyValue* value =
        static_cast< CXnDomPropertyValue* >( propertyValueList.Item( 0 ) );
    if ( value->PrimitiveValueType() == CXnDomPropertyValue::EString ||
         value->PrimitiveValueType() == CXnDomPropertyValue::EIdent )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsValuePercentageL()
// Check if property value is percentage.
// -----------------------------------------------------------------------------
//
static TBool IsValuePercentageL( CXnProperty& aProperty )
    {
    CXnDomProperty* domProperty = aProperty.Property();
    CXnDomPropertyValue* domPropertyValue = static_cast<
        CXnDomPropertyValue* >( domProperty->PropertyValueList().Item( 0 ) );
    if ( !domPropertyValue )
        {
        User::Leave( KXnErrDomPropertyValueNULL );
        }
    CXnDomPropertyValue::TPrimitiveValueType valueType =
        domPropertyValue->PrimitiveValueType();
    if ( valueType == CXnDomPropertyValue::EPercentage )
        {
        return ETrue;
        }
    return EFalse;
    }
 
// -----------------------------------------------------------------------------
// PercentageToPixelsL()
// Convert percentage value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt PercentageToPixelsL( TReal& aPercentage, TReal aReferenceValue )
    {
    TReal value = aReferenceValue * aPercentage * KIntPercentageConstant;
    return static_cast< TInt >( value + KIntConversionConstant );
    }

// -----------------------------------------------------------------------------
// GetPreviousNodeL()
// -----------------------------------------------------------------------------
//
static CXnNode* GetPreviousNodeL( CXnNode& aNode )
    {
    CXnNode* returnValue = NULL;
    CXnNode* parent = aNode.Parent();

    if ( !parent )
        {
        return returnValue;
        }

    RPointerArray< CXnNode >& nodes = parent->Children();

    for ( TInt currentIndex = 0; currentIndex < nodes.Count(); currentIndex++ )
        {
        CXnNode* checkNode = nodes[currentIndex];
        if ( &aNode == checkNode && currentIndex > 0 )
            {
            for ( TInt candidateIndex =
                currentIndex - 1; candidateIndex >= 0; candidateIndex-- )
                {
                checkNode = nodes[candidateIndex];
                if ( IsNodeDisplayedL( *checkNode ) &&
                     !checkNode->IsDropped() && !IsAbsoluteL( *checkNode ) &&
                     !IsNodeTooltip( *checkNode ) )
                    {
                    returnValue = nodes[candidateIndex];
                    break;
                    }
                }
            }
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// PlaceAreasL()
// Move the area rects to their places. All positioning is done here.
// -----------------------------------------------------------------------------
//
static void PlaceAreasL( CXnNode& aNode, RPointerArray< CXnNode >& aLaidOutList,
    TInt aLayoutPhase, CGraphicsDevice& aGraphicsDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels )
    {
    CXnNodeBreadthFirstIterator< CXnNode >* iterator = 
            CXnNodeBreadthFirstIterator< CXnNode >::NewL( aNode );
    CleanupStack::PushL( iterator );

    RPointerArray< CXnNode > absoluteNodes;
    CleanupClosePushL( absoluteNodes );

    RPointerArray< CXnNode > relativeNodes;
    CleanupClosePushL( relativeNodes );

    RPointerArray< CXnNode > autoNodes;
    CleanupClosePushL( autoNodes );

    RPointerArray< CXnNode > adaptiveNodes;
    CleanupClosePushL( adaptiveNodes );

    CXnProperty* parentBlockProgressionProperty( NULL );
    CXnProperty* parentDirectionProperty( NULL );
    const TDesC8* parentBlockProgression( NULL );
    const TDesC8* parentDirection( NULL );

    CXnUiEngine* engine( aNode.UiEngine() );
    TSize maxAvailableSize( engine->ClientRect().Size() );

    // Start to go through the tree
    CXnNode* previous( NULL );

    CXnNode* parent( NULL );
    TInt columnWidth;
    TInt columnMargin;
    TRect parentRect;

#ifdef _XN3_DEBUG_
    RDebug::Print( _L("*** XML UI place areas, layout phase=%d"), aLayoutPhase );
    
    TBuf8< 256 > debug;       
#endif

    // Place areas according their parents and positioning
    for( CXnNode* node = iterator->Value(); node; node = iterator->NextL() )
        {
#ifdef _XN3_DEBUG_
        debug = _L8( "* layouting node: " );
        debug.Append( node->Type()->Type() );
        CXnProperty* id( node->IdL() );
        if ( id )
            {
            debug.Append( _L8( " id=" ) );
            debug.Append( id->StringValue() );
            }
#endif

        if ( node->IsLaidOut() )
            {
#ifdef _XN3_DEBUG_
            debug.Append( _L8( ", already laidout" ) );
            RDebug::RawPrint( debug );
#endif

            continue;
            }

        if ( !IsNodeDisplayedL( *node ) || node->IsDropped() )
            {
#ifdef _XN3_DEBUG_
            debug.Append( _L8( ", not displayed or dropped" ) );
            RDebug::RawPrint( debug );
#endif
            continue;
            }

#ifdef _XN3_DEBUG_
        RDebug::RawPrint( debug );
#endif

        switch ( aLayoutPhase )
            {
            case XnLayoutPhase::ELayout:
                if ( aLaidOutList.Find( node ) == KErrNotFound )
                    {
                    aLaidOutList.AppendL( node );
                    }
                /* flow through */
            case XnLayoutPhase::EMeasure:
                if ( node->ViewNodeImpl() )
                    {
                    // check that view's children fit into it
                    FitChildrenIntoParentL( *node, aGraphicsDevice,
                        aHorizontalUnitInPixels, aVerticalUnitInPixels );
                    continue;
                    }
                engine->CheckAdaptiveContentL( *node, maxAvailableSize );
                if ( node->IsDropped() )
                    {
                    // Adaptive content check dropped this node
                    continue;
                    }
                break;
            default:
                // Should not happen
                continue;
            }
		
        if( node->Type()->Type() == KScrollableBoxNodeName )            
            {
            CXnScrollableControlAdapter* cntrl = node->ScrollableControl();
            if( cntrl )
                {
                cntrl->ResetState();
                }
            }
				
        CXnNode* tmpParent( DisplayedParentL( *node ) );

        // Is parent changed?
        if ( tmpParent != parent )
            {
            columnWidth = 0;
            columnMargin = 0;
            if ( parent )
                {
                 // Process auto areas
                ProcessAutoAreasL( *parent, &autoNodes, *parentBlockProgression,
                    *parentDirection, aGraphicsDevice, aHorizontalUnitInPixels,
                    aVerticalUnitInPixels );
                autoNodes.Reset();
                // Pocess relative areas
                for ( TInt i = 0; i < relativeNodes.Count(); i++ )
                    {
                    CalculateRelativePositionsL( parentRect, *relativeNodes[i],
                    aGraphicsDevice, aHorizontalUnitInPixels,
                    aVerticalUnitInPixels );
                    }
                relativeNodes.Reset();
                // Process absolute areas
                for ( TInt i = 0; i < absoluteNodes.Count(); i++ )
                    {
                    CalculatePaddingL( parentRect, *absoluteNodes[i],
                        aGraphicsDevice, aHorizontalUnitInPixels,
                        aVerticalUnitInPixels );
                    CalculateBorderL( *absoluteNodes[i], parentRect,
                        aGraphicsDevice, aHorizontalUnitInPixels,
                        aVerticalUnitInPixels );
                    CalculateAbsoluteMarginL( *parent, *absoluteNodes[i], NULL,
                        *parentBlockProgression, *parentDirection, parentRect,
                        aGraphicsDevice, aHorizontalUnitInPixels,
                        aVerticalUnitInPixels );
                    CalculateAbsolutePositionsL( *absoluteNodes[i], parentRect,
                        aGraphicsDevice, aHorizontalUnitInPixels,
                        aVerticalUnitInPixels );
                    }
                absoluteNodes.Reset();
                previous = NULL;
                }
            // Get new parent properties
            parent = tmpParent;
            parentBlockProgressionProperty = parent->BlockProgressionL();
            parentDirectionProperty = parent->DirectionL();
            if ( !parentBlockProgressionProperty )
                {
                // block progression default is tb
                parentBlockProgression =
                    &XnPropertyNames::style::common::block_progression::KTB();
                }
            else
                {
                parentBlockProgression =
                    &parentBlockProgressionProperty->StringValue();
                }
            if ( !parentDirectionProperty )
                {
                // direction default is ltr
                parentDirection =
                    &XnPropertyNames::style::common::direction::KLTR();
                }
            else
                {
                parentDirection = &parentDirectionProperty->StringValue();
                }
            }

        parentRect = parent->Rect();

        CalculateRectL( *node, *parent, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );

        // Which positioning is used?
        CXnProperty* positioningProp( node->PositionL() );
        // Static is the default
        const TDesC8* positioning(
            &XnPropertyNames::style::common::position::KStatic() );

        if ( positioningProp )
            {
            const TDesC8& value( positioningProp->StringValue() );
            if ( value != KNullDesC8 )
                {
                positioning = &value;
                }
            }
        // Adaptive nodes' _must_ be collected here
        if ( node->IsAdaptive() && adaptiveNodes.Find( node ) == KErrNotFound )
            {
            // Postpone adaptive area processing
            adaptiveNodes.AppendL( node );
            }
        // Tooltips are processed as absolute areas
        TBool absolute( IsAbsoluteL( *node ) || IsNodeTooltip( *node ) );
        if ( !node->ViewNodeImpl() && HasNodeAutoValuesL( *node ) && !absolute )
            {
            // if area has auto values, process it later
            autoNodes.AppendL( node );
            }
        if ( absolute )
            {
            // Absolute positioning area found, take it out of normal flow
            // Because of that, previous node is not set
            // Absolute area is put to a array to be processed later
            absoluteNodes.AppendL( node );
            continue;
            }
        // static or relative positioning
        CalculatePaddingL( parentRect, *node, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );
        CalculateBorderL( *node, parentRect, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );
        previous = GetPreviousNodeL( *node );
        CalculateMarginL( *parent, *node, previous, *parentBlockProgression,
            *parentDirection, parentRect, aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels, columnWidth,
            columnMargin );
        FitChildrenIntoParentL( *node, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        if ( XnPropertyNames::style::common::position::KRelative() ==
             *positioning )
            {
            // Relative positioning area found, add it's offset later
            relativeNodes.AppendL( node );
            }
        previous = node;
        }

    if ( parent )
        {
        // Are there auto nodes left?
        ProcessAutoAreasL( *parent, &autoNodes, *parentBlockProgression,
            *parentDirection, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        // Are there relative nodes left?
        for ( TInt i = 0; i < relativeNodes.Count(); i++ )
            {
            CalculateRelativePositionsL( parentRect, *relativeNodes[i],
                aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            }
        // Are there absolute nodes left?
        for ( TInt i = 0; i < absoluteNodes.Count(); i++ )
            {
            CalculatePaddingL( parentRect, *absoluteNodes[i], aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            CalculateBorderL( *absoluteNodes[i], parentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            CalculateAbsoluteMarginL( *parent, *absoluteNodes[i], NULL,
                *parentBlockProgression, *parentDirection, parentRect,
                aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            FitChildrenIntoParentL( *absoluteNodes[i], aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            CalculateAbsolutePositionsL( *absoluteNodes[i], parentRect,
                aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            }
        }

    for ( TInt i = 0; i < adaptiveNodes.Count(); i++ )
        {
        // Fix parent size to its childs content size
        ProcessAdaptiveAreasL( *adaptiveNodes[i], aGraphicsDevice,
            aHorizontalUnitInPixels, aVerticalUnitInPixels );
        }

    // close node arrays and destroy iterator
    CleanupStack::PopAndDestroy( 5, iterator );
    }

// -----------------------------------------------------------------------------
// PriorizeChildrenL()
// Find a child node with highest priority value and drop all
// child nodes with highest priority.
// -----------------------------------------------------------------------------
//
static void PriorizeChildrenL( CXnNode& aNode )
    {
    RPointerArray< CXnNode >& children( aNode.Children() );

    CXnNode* candidate( NULL );
    TInt highest( 0 );

    // Find the highest priority value among the children
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* node( children[i] );
        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL( *node ) && !IsNodeTooltip( *node ) )
            {
            CXnProperty* priorityProperty( node->DisplayPriorityL() );
            if ( priorityProperty )
                {
                TInt priority( priorityProperty->FloatValueL() );
                if ( priority > highest )
                    {
                    // highest found, set new top priority
                    highest = priority;
                    candidate = node;
                    }
                }
            }
        }
    if ( candidate )
        {
        aNode.UiEngine()->SetNodeDroppedL(
            *candidate, XnNodeLayout::EDropped  );
        }
    }

// -----------------------------------------------------------------------------
// CalculateAutoPropertiesOfNodesL()
//
// -----------------------------------------------------------------------------
//
static void CalculateAutoPropertiesOfNodesL(
    RPointerArray< CXnNode >* aAutoNodeArray, TInt& aWidthAutoCount,
    TInt& aHeightAutoCount )
    {
    aWidthAutoCount = 0;
    aHeightAutoCount = 0;

    // calculate the number of auto properties in each direction
    for ( TInt i = 0; i < aAutoNodeArray->Count(); ++i )
        {
        CXnNode* tmp = ( *aAutoNodeArray )[i];
        if ( !tmp->IsDropped() )
            {
            if ( HasNodeVerticalAutoValuesL( *tmp ) )
                {
                ++aHeightAutoCount;
                }
            if ( HasNodeHorizontalAutoValuesL( *tmp ) )
                {
                ++aWidthAutoCount;
                }
            }
        }
    if ( aWidthAutoCount == 0 )
        {
        aWidthAutoCount = 1;
        }
    if ( aHeightAutoCount == 0 )
        {
        aHeightAutoCount = 1;
        }
    }

// -----------------------------------------------------------------------------
// ProcessAutoAreasL()
// Process nodes with layout properties defined as auto.
// -----------------------------------------------------------------------------
//
static void ProcessAutoAreasL( CXnNode& aParent,
    RPointerArray< CXnNode >* aAutoNodeArray,
    const TDesC8& aParentBlockProgression, const TDesC8& aParentDirection,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt autoCount( aAutoNodeArray->Count() );

    if ( autoCount == 0 )
        {
        return;
        }

    //  Just to be sure, drop others first if needed
    FitChildrenIntoParentL( aParent, aGraphicsDevice, aHorizontalUnitInPixels,
        aVerticalUnitInPixels );

    TInt originalHeightAutoCount( 0 );
    TInt originalWidthAutoCount( 0 );

    CalculateAutoPropertiesOfNodesL( aAutoNodeArray, originalWidthAutoCount,
        originalHeightAutoCount );

    TInt heightAutoCount( originalHeightAutoCount );
    TInt widthAutoCount( originalWidthAutoCount );

    // calculate space to use (parent height and width - placed areas)
    CXnNode* parent( &aParent );
    TRect parentRect( parent->Rect() );
    
    if( parent->Control() && parent->Control()->OwnsWindow() )
        {
        parentRect = TRect( parentRect.iTl - parent->MarginRect().iTl, parentRect.Size() );
        }

    TInt parentWidth( parentRect.Width() );
    TInt parentHeight( parentRect.Height() );

    RPointerArray< CXnNode >& children( parent->Children() );

    RPointerArray< CXnNode > layoutedAutoNodes;
    CleanupClosePushL( layoutedAutoNodes );

    TBool layoutedAutoNodesAdded( EFalse );
    TBool layoutingHeight( EFalse );
    TBool layoutingWidth( EFalse );

    TInt displayedChildCount( DisplayedChildrenCountL( aParent ) );

    do
        {
        layoutedAutoNodesAdded = EFalse;

        //  count the size used by the non auto nodes and
        //  auto nodes which have been calculated
        TSize calculatedAutosize = CalculateSpaceUsedByAutoChildrenL(
            *aAutoNodeArray, layoutedAutoNodes, *parent,
            aParentBlockProgression, aParentDirection, aGraphicsDevice );

        TInt usedSiblingHeight( calculatedAutosize.iHeight );
        TInt usedSiblingWidth( calculatedAutosize.iWidth );

        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KBT )
            {
            // calculate space for each auto area
            parentHeight = parentRect.Height() - usedSiblingHeight;
            parentHeight = parentHeight / heightAutoCount;
            layoutingHeight = ETrue;
            }
        else // assume that block progression is LR or RL
            {
            // calculate space for each auto area
            parentWidth = parentRect.Width() - usedSiblingWidth;
            parentWidth = parentWidth / widthAutoCount;
            layoutingWidth = ETrue;
            }
        for ( TInt i = 0; i < autoCount; i++ )
            {
            CXnNode* node( ( *aAutoNodeArray )[i] );
            if ( layoutedAutoNodes.Find( node ) != KErrNotFound )
                {
                //  The size has already been calculated and fixed
                continue;
                }
            if ( node->IsDropped() )
                {
                //  The node is dropped, no need to recalculate
                continue;
                }
            CXnProperty* heightProperty = node->HeightL();
            CXnProperty* widthProperty = node->WidthL();
            CXnProperty* marginLeft = node->MarginLeftL();
            CXnProperty* marginRight = node->MarginRightL();
            CXnProperty* marginTop = node->MarginTopL();
            CXnProperty* marginBottom = node->MarginBottomL();
            TInt freeHeight = parentHeight;
            TInt freeWidth = parentWidth;
            TBool verticalValuesAuto = IsPropertyAutoL( *heightProperty ) ||
                IsPropertyAutoL( *marginTop ) ||
                IsPropertyAutoL( *marginBottom );
            if ( !verticalValuesAuto == !heightProperty )
                {
                verticalValuesAuto = ETrue;
                }
            TBool horizontalValuesAuto = IsPropertyAutoL( *widthProperty ) ||
                IsPropertyAutoL( *marginLeft ) ||
                IsPropertyAutoL( *marginRight );
            if ( !horizontalValuesAuto == !widthProperty )
                {
                horizontalValuesAuto = ETrue;
                }
            TInt adaptive( node->IsAdaptive() );
            // I have childs share the available space between them
            if ( adaptive & XnAdaptive::EHeight )
                {
                // Give all available height to the adaptive node's children
                freeHeight = parentRect.Height() - usedSiblingHeight;
                }
            if ( adaptive & XnAdaptive::EWidth )
                {
                // Give all available width to the adaptive node's children
                freeWidth = parentRect.Width() - usedSiblingWidth;
                }
            ScaleAutoAreaL( *node, freeWidth, freeHeight,
                aParentBlockProgression, aParentDirection,
                parentRect, aGraphicsDevice, aHorizontalUnitInPixels,
                aVerticalUnitInPixels );
            FitChildrenIntoParentL( *node, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            //  handle autonodes which had auto width or height,
            // but also min or max limit
            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KTB ||
                 aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KBT )
                {
                if ( IsPropertyAutoL( *heightProperty ) &&
                   ( freeHeight != node->MarginRect().Height() ) )
                    {
                    if ( layoutedAutoNodes.Find( node ) == KErrNotFound )
                        {
                        //  The height has been callculated already
                        // and it doesn't depend on the expected
                        //  auto size anymore, mark the node as layouted
                        // and reduce the parent free size
                        layoutedAutoNodes.AppendL( node );
                        heightAutoCount--;
                        layoutedAutoNodesAdded = ETrue;
                        }
                    }
                }
            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KLR ||
                 aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KRL )
                {
                if ( IsPropertyAutoL( *widthProperty ) &&
                   ( freeWidth != node->MarginRect().Width() ) )
                    {
                    if ( layoutedAutoNodes.Find ( node ) == KErrNotFound )
                        {
                        //  The width has been callculated already
                        // and it doesn't depend on the expected
                        // auto size anymore, mark the node as layouted
                        // and reduce the parent free size
                        layoutedAutoNodes.AppendL( node );
                        widthAutoCount--;
                        layoutedAutoNodesAdded = ETrue;
                        }
                    }
                }
            }

        // drop areas if children don't fit
        FitAutoChildrenIntoParentL( *aAutoNodeArray, layoutedAutoNodes,
            *parent, aParentBlockProgression, aParentDirection,
            aGraphicsDevice );

        //  if something was dropped, reset the layouted auto nodes array
        TInt newDisplayedChildCount = DisplayedChildrenCountL( aParent );

        if ( displayedChildCount != newDisplayedChildCount )
            {
            // if nodes with auto values are dropped
            // recalculate the auto counts
            CalculateAutoPropertiesOfNodesL( aAutoNodeArray,
                originalWidthAutoCount, originalHeightAutoCount );

            //  recalculate everything again because something was dropped
            layoutedAutoNodes.Reset();
            displayedChildCount = newDisplayedChildCount;
            heightAutoCount = originalHeightAutoCount;
            widthAutoCount = originalWidthAutoCount;
            layoutedAutoNodesAdded = ETrue;
            }

        } 
    while( layoutedAutoNodesAdded && ( ( heightAutoCount && layoutingHeight ) ||
         ( widthAutoCount && layoutingWidth ) ) );

    MoveAutoAreaL( parentRect, children, aParentBlockProgression,
        aParentDirection );

    CleanupStack::PopAndDestroy( &layoutedAutoNodes );
    }

// -----------------------------------------------------------------------------
// CalculateCollapseMarginsL()
// Return the amount the margins should be collapsed in two list. The list
// element contains the amount of margins collapsed in pixels to the previous
// layoutable, nondropped, static and nontooltip node.
// When calculating the movement of the node, this value should be decreased
// from the movement.
// When calculating the total size of the auto proerety nodes, this value
// should be decreased from the size taken.
// If the children list contains one of the above mentioned type of nodes,
// then the value for those are always 0.
// For example, if there are three children and the middle one has been
// dropped, the the returned list contains two elemnts, first is 0 and
// the second one is the amount of collapsed margins between the first and the
// last node.
// The caller is responsible of releasing the returned arrays.
// -----------------------------------------------------------------------------
//
static void CalculateCollapseMarginsL( RPointerArray< CXnNode >& aChildren,
    RArray< TInt >& aCollapsedMarginsWidth,
    RArray< TInt >& aCollapsedMarginsHeight,
    const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection )
    {
    aCollapsedMarginsWidth.Reset();
    aCollapsedMarginsHeight.Reset();

    //  Find first node which needs to be taken into account
    CXnNode* previousNode = NULL;
    TInt startIndex = aChildren.Count();
    for ( TInt i = 0; !previousNode && i < aChildren.Count(); i++ )
        {
        if ( IsNodeDisplayedL( *aChildren[i] ) &&
             !aChildren[i]->IsDropped() &&
             !IsAbsoluteL(*aChildren[i]) &&
             !IsNodeTooltip(*aChildren[i]) &&
             aChildren[i]->IsLayoutCapable())
            {
            previousNode = aChildren[i];
            startIndex = aChildren.Find( previousNode ) + 1;
            }
        }

    //  Add needed amount of collapsed margins as 0
    for ( TInt i = 1; i < startIndex; i++ )
        {
        User::LeaveIfError( aCollapsedMarginsWidth.Append( 0 ) );
        User::LeaveIfError( aCollapsedMarginsHeight.Append( 0 ) );
        }

    //  Calculate collapsed for the rest of the nodes, 0 if not needed
    for ( TInt i = startIndex; i < aChildren.Count() && previousNode; i++ )
        {
        CXnNode* node = aChildren[i];
        TInt collapsedWidth = 0;
        TInt collapsedHeight = 0;

        if ( IsNodeDisplayedL( *node ) && 
             !node->IsDropped() &&
             !IsAbsoluteL( *node ) &&
             !IsNodeTooltip( *node ) &&
             node->IsLayoutCapable() )
            {
            if ( ( aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KLR ||
                   aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KRL ) &&
                   aParentDirection ==
                   XnPropertyNames::style::common::direction::KLTR )
                {
                TInt previousRightMargin = Abs( previousNode->
                    MarginRect().iBr.iX - previousNode->BorderRect().iBr.iX );
                TInt nodeLeftMargin = Abs( node->BorderRect().iTl.iX -
                    node->MarginRect().iTl.iX );

                collapsedWidth = -( previousRightMargin > nodeLeftMargin ) ?
                    nodeLeftMargin : previousRightMargin;
                }

            if ( ( aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KRL || 
                   aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KLR ) &&
                   aParentDirection ==
                   XnPropertyNames::style::common::direction::KRTL )
                {
                TInt previousLeftMargin = Abs( previousNode->
                    MarginRect().iTl.iX - previousNode->BorderRect().iTl.iX );
                TInt nodeRightMargin = Abs( node->BorderRect().iBr.iX -
                    node->MarginRect().iBr.iX );

                collapsedWidth = ( previousLeftMargin > nodeRightMargin ) ?
                    nodeRightMargin : previousLeftMargin;
                }

            if ( ( aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KTB ||
                   aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KBT ) &&
                   aParentDirection ==
                   XnPropertyNames::style::common::direction::KLTR )
                {
                TInt previousBottomMargin = Abs( previousNode->
                    MarginRect().iBr.iY - previousNode->BorderRect().iBr.iY );
                TInt nodeTopMargin = Abs( node->BorderRect().iTl.iY -
                    node->MarginRect().iTl.iY );

                collapsedHeight = ( previousBottomMargin > nodeTopMargin ) ?
                    nodeTopMargin : previousBottomMargin;
                }

            if ( ( aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KTB ||
                   aParentBlockProgression ==
                   XnPropertyNames::style::common::block_progression::KBT ) &&
                   aParentDirection ==
                   XnPropertyNames::style::common::direction::KRTL )
                {
                TInt previousTopMargin = Abs( previousNode->\
                    MarginRect().iTl.iY - previousNode->BorderRect().iTl.iY );
                TInt nodeBottomMargin = Abs( node->BorderRect().iBr.iY -
                    node->MarginRect().iBr.iY );

                collapsedHeight = -( ( previousTopMargin > nodeBottomMargin ) ?
                    nodeBottomMargin : previousTopMargin );
                }
            }
        User::LeaveIfError( aCollapsedMarginsWidth.Append( collapsedWidth ) );
        User::LeaveIfError( aCollapsedMarginsHeight.Append( collapsedHeight ) );
        previousNode = node;
        }
    }

// -----------------------------------------------------------------------------
// CalculateSpaceUsedByAutoChildrenL()
// Return space used by node's children including auto nodes which have
// been calculated.
// -----------------------------------------------------------------------------
//
static TSize CalculateSpaceUsedByAutoChildrenL(
    RPointerArray< CXnNode >& aAutoChildren,
    RPointerArray< CXnNode >& aCalculatedAutoChildren,
    CXnNode& aParent, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, CGraphicsDevice& /*aScreenDevice*/ )
    {
    TInt usedSiblingHeight( 0 );
    TInt usedSiblingWidth( 0 );

    TSize largestSize( 0, 0 );

    //  Calculate collapsed margins
    RArray< TInt > collapsedMarginsWidth;
    RArray< TInt > collapsedMarginsHeight;

    CleanupClosePushL( collapsedMarginsWidth );
    CleanupClosePushL( collapsedMarginsHeight );

    RPointerArray< CXnNode >& children( aParent.Children() );

    CalculateCollapseMarginsL( children,
                               collapsedMarginsWidth,
                               collapsedMarginsHeight,
                               aParentBlockProgression,
                               aParentDirection );

    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNode* node( children[i] );
        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL( *node ) && !IsNodeTooltip( *node ) )
            {
            //  Calculate how much does the node takes space,
            //  0 is used for auto nodes which have not been calculated
            TSize siblingSize( node->MarginRect().Size() );
            if ( aAutoChildren.Find( node ) != KErrNotFound &&
                 aCalculatedAutoChildren.Find( node ) == KErrNotFound )
                {
                //  Node is auto node and its values have not been fixed yet
                //  The node may have auto in either direction
                // so that must be checked
                if ( HasNodeVerticalAutoValuesL( *node ) )
                    {
                    siblingSize.iHeight = 0;
                    }
                if ( HasNodeHorizontalAutoValuesL( *node ) )
                    {
                    siblingSize.iWidth = 0;
                    }
                }
            // Update the largets found node rect
            if ( siblingSize.iWidth > largestSize.iWidth )
                {
                largestSize.iWidth = siblingSize.iWidth;
                }
            if ( siblingSize.iHeight > largestSize.iHeight )
                {
                largestSize.iHeight = siblingSize.iHeight;
                }
            //  Add the used area
            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KTB ||
                 aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KBT )
                {
                usedSiblingHeight += siblingSize.iHeight;
                usedSiblingWidth = largestSize.iWidth;
                }
            if ( aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KLR ||
                 aParentBlockProgression ==
                 XnPropertyNames::style::common::block_progression::KRL )
                {
                usedSiblingWidth += siblingSize.iWidth;
                usedSiblingHeight = largestSize.iHeight;
                }
            //  Find the children index from all children
            TInt childrenIndex( children.Find( node ) );
            //  No collapsing for just one node
            if ( childrenIndex > 0 )
                {
                //  Take the collapsed margins into account
                // (the value is 0 if not collapsed)
                usedSiblingWidth -= collapsedMarginsWidth[childrenIndex - 1];
                usedSiblingHeight -= collapsedMarginsHeight[childrenIndex - 1];
                }
            }
        }
    //  Collapsed arrays
    CleanupStack::PopAndDestroy( 2 );

    return TSize( usedSiblingWidth, usedSiblingHeight );
    }

// -----------------------------------------------------------------------------
// MoveAutoAreaL()
// Move nodes after all auto properties have been handled.
// -----------------------------------------------------------------------------
//
static void MoveAutoAreaL( TRect& parentRect,
    RPointerArray< CXnNode >& aChildren, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection )
    {
    //  Calculate collapsed margins
    RArray< TInt > collapsedMarginsWidth;
    RArray< TInt > collapsedMarginsHeight;

    CleanupClosePushL( collapsedMarginsWidth );
    CleanupClosePushL( collapsedMarginsHeight );

    CalculateCollapseMarginsL( aChildren, collapsedMarginsWidth,
        collapsedMarginsHeight, aParentBlockProgression,
        aParentDirection );

    CXnNode* previousNode( NULL );
    TInt startIndex( aChildren.Count() );

    for ( TInt i = 0; !previousNode && i < aChildren.Count(); i++ )
        {
        CXnNode* node( aChildren[i] );
        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL (*node ) && !IsNodeTooltip( *node ) )
            {
            previousNode = node;
            startIndex = i + 1;
            }
        }
    if ( !previousNode )
        {
        //  Collapsed arrays
        CleanupStack::PopAndDestroy( 2 );
        return;
        }

    //  Which way are the boxes layouted?
    //
    //  1) ltr, tb:  2) ltr, bt:  3) ltr, rl:  4) ltr, lr:
    //     1 4          3 6          1 2 3        4 5 6
    //     2 5          2 5          4 5 6        1 2 3
    //     3 6          1 4
    //
    //  5) rtl, tb:  6) rtl, bt:  7) rtl, rl:  8) rtl, lr:
    //     4 1          6 3          3 2 1        6 5 4
    //     5 2          5 2          6 5 4        3 2 1
    //     6 3          4 1
    //

    TInt layoutType( 0 );
    if ( aParentDirection == XnPropertyNames::style::common::direction::KLTR )
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB )
            {
            layoutType = 1;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KBT )
            {
            layoutType = 2;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KRL )
            {
            layoutType = 3;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KLR )
            {
            layoutType = 4;
            }
        }
    else if ( aParentDirection ==
              XnPropertyNames::style::common::direction::KRTL )
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB )
            {
            layoutType = 5;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KBT )
            {
            layoutType = 6;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KRL )
            {
            layoutType = 7;
            }
        else if ( aParentBlockProgression ==
                  XnPropertyNames::style::common::block_progression::KLR )
            {
            layoutType = 8;
            }
        }
    TRect previousContentRect( previousNode->Rect() );
    TRect previousPaddingRect( previousNode->PaddingRect() );
    TRect previousBorderRect( previousNode->BorderRect() );
    TRect previousMarginRect( previousNode->MarginRect() );
    TRect previousNormalFlowBorderRect( previousNode->NormalFlowBorderRect() );
    TInt widthToMove = 0;
    TInt heightToMove = 0;

    //  Move the first node to the correct place
    switch ( layoutType )
        {
        case 1:
            {
            //  Top to parent top
            heightToMove = parentRect.iTl.iY - previousMarginRect.iTl.iY;
            }
            break;
        case 2:
            {
            //  Bottom to parent bottom
            heightToMove = parentRect.iBr.iY - previousMarginRect.iBr.iY;
            }
            break;
        case 3:
            {
            //  Left to parent left
            widthToMove = parentRect.iTl.iX - previousMarginRect.iTl.iX;
            }
            break;
        case 4:
            {
            heightToMove = parentRect.iBr.iY - previousMarginRect.iBr.iY;
            widthToMove = parentRect.iTl.iX - previousMarginRect.iTl.iX;
            }
            break;
        case 5: // flow through
        case 7:
            {
            heightToMove = parentRect.iTl.iY - previousMarginRect.iTl.iY;
            widthToMove = parentRect.iBr.iX - previousMarginRect.iBr.iX;
            }
            break;
        case 6: // flow through
        case 8:
            {
            heightToMove = parentRect.iBr.iY - previousMarginRect.iBr.iY;
            widthToMove = parentRect.iBr.iX - previousMarginRect.iBr.iX;
            }
            break;
        }

    //  Move rects
    previousContentRect.Move( widthToMove, heightToMove );
    previousPaddingRect.Move( widthToMove, heightToMove );
    previousBorderRect.Move( widthToMove, heightToMove );
    previousMarginRect.Move( widthToMove, heightToMove );
    previousNormalFlowBorderRect.Move( widthToMove, heightToMove );

    //  Set rects
    previousNode->SetRect( previousContentRect );
    previousNode->SetPaddingRect( previousPaddingRect );
    previousNode->SetBorderRect( previousBorderRect );
    previousNode->SetMarginRect( previousMarginRect );
    previousNode->SetNormalFlowBorderRect( previousNormalFlowBorderRect );

    for ( TInt i = startIndex; i < aChildren.Count() && previousNode; i++ )
        {
        CXnNode* node( aChildren[i] );

        if ( IsNodeDisplayedL( *node ) && !node->IsDropped() &&
             !IsAbsoluteL( *node ) && !IsNodeTooltip( *node ) )
            {
            //  Get rects
            TRect contentRect( node->Rect() );
            TRect paddingRect( node->PaddingRect() );
            TRect borderRect( node->BorderRect() );
            TRect marginRect( node->MarginRect() );
            TRect normalFlowBorderRect( node->NormalFlowBorderRect() );
            previousMarginRect = previousNode->MarginRect();

            widthToMove = 0;
            heightToMove = 0;

            //  Count movements
            switch ( layoutType )
                {
                case 1:
                    {
                    //  Move node top to the previous bottom
                    heightToMove = previousMarginRect.iBr.iY - marginRect.iTl.iY;
                    heightToMove -= collapsedMarginsHeight[i - 1];
                    }
                    break;
                case 2:
                    {
                    //  Move node bottom to the previous top
                    heightToMove = previousMarginRect.iTl.iY - marginRect.iBr.iY;
                    heightToMove += collapsedMarginsHeight[i - 1];
                    }
                    break;
                case 3:
                    {
                    //  Move node left to the previous right
                    widthToMove = previousMarginRect.iBr.iX - marginRect.iTl.iX;
                    widthToMove -= collapsedMarginsWidth[i - 1];
                    }
                    break;
                case 4:
                    {
                    heightToMove = previousMarginRect.iTl.iY - marginRect.iTl.iY;
                    widthToMove = previousMarginRect.iBr.iX - marginRect.iTl.iX;
                    widthToMove -= collapsedMarginsWidth[i - 1];
                    }
                    break;
                case 5:
                    {
                    //  Move node top to the previous bottom
                    heightToMove = previousMarginRect.iBr.iY - marginRect.iTl.iY;
                    heightToMove -= collapsedMarginsHeight[i - 1];
                    widthToMove = previousMarginRect.iTl.iX - marginRect.iTl.iX;
                    }
                    break;
                case 6:
                    {
                    //  Move node bottom to the previous top
                    heightToMove = previousMarginRect.iTl.iY - marginRect.iBr.iY;
                    heightToMove += collapsedMarginsHeight[i - 1];
                    widthToMove = previousMarginRect.iTl.iX - marginRect.iTl.iX;
                    }
                    break;
                case 7:
                    {
                    //  Move node left to the previous right
                    widthToMove = previousMarginRect.iTl.iX - marginRect.iBr.iX;
                    widthToMove += collapsedMarginsWidth[i - 1];
                    }
                    break;
                case 8:
                    {
                    heightToMove = previousMarginRect.iTl.iY - marginRect.iTl.iY;
                    widthToMove = previousMarginRect.iTl.iX - marginRect.iBr.iX;
                    widthToMove += collapsedMarginsWidth[i - 1];
                    }
                    break;
                }

            //  Move rects
            contentRect.Move( widthToMove, heightToMove );
            paddingRect.Move( widthToMove, heightToMove );
            borderRect.Move( widthToMove, heightToMove );
            marginRect.Move( widthToMove, heightToMove );
            normalFlowBorderRect.Move( widthToMove, heightToMove );

            //  Set rects
            node->SetRect( contentRect );
            node->SetPaddingRect( paddingRect );
            node->SetBorderRect( borderRect );
            node->SetMarginRect( marginRect );
            node->SetNormalFlowBorderRect( normalFlowBorderRect );

            previousNode = node;
            }
        }

    //  Collapsed arrays
    CleanupStack::PopAndDestroy( 2 );
    }

// -----------------------------------------------------------------------------
// ScaleAutoAreaL()
// Scale dimensions for areas that have properties set to AUTO.
// -----------------------------------------------------------------------------
//
static TSize ScaleAutoAreaL( CXnNode& aNode, TInt aWidthToFit,
    TInt aHeightToFit, const TDesC8& aParentBlockProgression,
    const TDesC8& aParentDirection, TRect& aParentRect,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    // if only one value is auto, it is computed from equality
    // if width is set to AUTO, all other values become 0
    // if both margin left and right are set to AUTO, their computed
    // values are equal
    TSize sizeOfAutoArea( 0, 0 );

    RArray< TPtrC8 > autoArray;
    CleanupClosePushL( autoArray );

    CXnProperty* top = aNode.TopL();
    if ( IsPropertyAutoL( *top ) )
        {
        TPtrC8 propertyName = top->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* bottom = aNode.BottomL();
    if ( IsPropertyAutoL( *bottom ) )
        {
        TPtrC8 propertyName = bottom->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* left = aNode.LeftL();
    if ( IsPropertyAutoL( *left ) )
        {
        TPtrC8 propertyName = left->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* right = aNode.RightL();
    if ( IsPropertyAutoL( *right ) )
        {
        TPtrC8 propertyName = right->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* width = aNode.WidthL();
    if ( IsPropertyAutoL( *width ) )
        {
        TPtrC8 propertyName = width->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    if ( !width )
        {
        TPtrC8 propertyName = XnPropertyNames::style::common::KWidth();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* height = aNode.HeightL();
    if ( IsPropertyAutoL( *height ) )
        {
        TPtrC8 propertyName = height->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    if ( !height )
        {
        TPtrC8 propertyName = XnPropertyNames::style::common::KHeight();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* marginTop = aNode.MarginTopL();
    if ( IsPropertyAutoL( *marginTop ) )
        {
        TPtrC8 propertyName = marginTop->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* marginBottom = aNode.MarginBottomL();
    if ( IsPropertyAutoL( *marginBottom ) )
        {
        TPtrC8 propertyName = marginBottom->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* marginLeft = aNode.MarginLeftL();
    if ( IsPropertyAutoL( *marginLeft ) )
        {
        TPtrC8 propertyName = marginLeft->Property()->Name();
        autoArray.AppendL( propertyName );
        }
    CXnProperty* marginRight = aNode.MarginRightL();
    if ( IsPropertyAutoL( *marginRight ) )
        {
        TPtrC8 propertyName = marginRight->Property()->Name();
        autoArray.AppendL( propertyName );
        }

    if ( aParentDirection == XnPropertyNames::style::common::direction::KLTR )
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KBT )
            {
            sizeOfAutoArea = TSize( AutoStaticTBL( aNode, aWidthToFit,
                aHeightToFit, autoArray, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels ) );
            }
        else // block progression LR
            {
            sizeOfAutoArea = TSize( AutoStaticLRL( aNode, aWidthToFit,
                aHeightToFit, autoArray, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels ) );
            }
        }
    else // direction RTL
        {
        if ( aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KTB ||
             aParentBlockProgression ==
             XnPropertyNames::style::common::block_progression::KBT )
            {
            sizeOfAutoArea = TSize( AutoStaticBTL( aNode, aWidthToFit,
                aHeightToFit, autoArray, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels ) );
            }
        else // block progression RL
            {
            sizeOfAutoArea = TSize( AutoStaticRLL( aNode, aWidthToFit,
                aHeightToFit, autoArray, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels ) );
            }
        }

    // if size not within min&max size limits, resize
    // (min-size)
    if ( HasNodeMinSizesL( aNode ) )
        {
        if ( IsSmallerThanMinSizeL( aParentRect, aNode, aGraphicsDevice,
             aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
            {
            GrowToMinSizeL( aNode, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            }
        }

     if ( HasNodeMaxSizesL( aNode ) )
        {
        if ( IsLargerThanMaxSizeL( aParentRect, aNode, aGraphicsDevice,
             aHorizontalUnitInPixels, aVerticalUnitInPixels ) )
            {
            ShrinkToMaxSizeL( aNode, aParentRect, aGraphicsDevice,
                aHorizontalUnitInPixels, aVerticalUnitInPixels );
            }
        }

    CleanupStack::PopAndDestroy( &autoArray );

    //  Return the context rect size because the min-max
    // test might have changed the size of autoarea
    return aNode.Rect().Size();
    }

// -----------------------------------------------------------------------------
// GrowToMinSizeL()
//
// -----------------------------------------------------------------------------
//
static void GrowToMinSizeL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aScreenDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels )
    {
    // grow to min size
    CXnProperty* minWidthProperty = aNode.MinWidthL();
    CXnProperty* minHeightProperty = aNode.MinHeightL();

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );
    TRect marginRect( aNode.MarginRect() );

    if ( minWidthProperty )
        {
        if ( !IsPropertyNone( *minWidthProperty ) )
            {
            TInt minWidth = HorizontalPixelValueL( minWidthProperty,
                aParentRect.Width(), aScreenDevice, aHorizontalUnitInPixels );
            if ( contentRect.Width() < minWidth )
                {
                TInt widthToGrow = minWidth - contentRect.Width();
                contentRect.Resize( widthToGrow, 0 );
                paddingRect.Resize( widthToGrow, 0 );
                borderRect.Resize( widthToGrow, 0 );
                normalFlowBorderRect.Resize( widthToGrow, 0 );
                marginRect.Resize( widthToGrow, 0 );
                }
            }
        }

    if ( minHeightProperty )
        {
        if ( !IsPropertyNone( *minHeightProperty ) )
            {
            TInt minHeight = VerticalPixelValueL( minHeightProperty,
                aParentRect.Height(), aScreenDevice, aVerticalUnitInPixels );
            if ( contentRect.Height() < minHeight )
                {
                TInt heightToGrow = minHeight - contentRect.Height();
                contentRect.Resize( 0, heightToGrow );
                paddingRect.Resize( 0, heightToGrow );
                borderRect.Resize( 0, heightToGrow );
                normalFlowBorderRect.Resize( 0, heightToGrow );
                marginRect.Resize( 0, heightToGrow );
                }
            }
        }

    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    aNode.SetMarginRect( marginRect );
    }

// -----------------------------------------------------------------------------
// ShrinkToMaxSizeL()
// -----------------------------------------------------------------------------
//
static void ShrinkToMaxSizeL( CXnNode& aNode, TRect& aParentRect,
    CGraphicsDevice& aScreenDevice,
    TReal aHorizontalUnitInPixels, TReal aVerticalUnitInPixels )
    {
    // grow to min size
    CXnProperty* maxWidthProperty = aNode.MaxWidthL();
    CXnProperty* maxHeightProperty = aNode.MaxHeightL();

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );
    TRect marginRect( aNode.MarginRect() );

    if ( maxWidthProperty )
        {
        if ( !IsPropertyNone( *maxWidthProperty ) )
            {
            TInt maxWidth = HorizontalPixelValueL( maxWidthProperty,
                aParentRect.Width(), aScreenDevice, aHorizontalUnitInPixels );
            if ( contentRect.Width() > maxWidth )
                {
                TInt widthToShrink = contentRect.Width() - maxWidth;
                contentRect.Resize( -widthToShrink, 0 );
                paddingRect.Resize( -widthToShrink, 0 );
                borderRect.Resize( -widthToShrink, 0 );
                normalFlowBorderRect.Resize( -widthToShrink, 0 );
                marginRect.Resize( -widthToShrink, 0 );
                }
            }
        }

    if ( maxHeightProperty )
        {
        if ( !IsPropertyNone( *maxHeightProperty ) )
            {
            TInt maxHeight = VerticalPixelValueL( maxHeightProperty,
                aParentRect.Height(), aScreenDevice, aVerticalUnitInPixels );
            if ( contentRect.Height() > maxHeight )
                {
                TInt heightToShrink = contentRect.Height() - maxHeight;
                contentRect.Resize( 0, -heightToShrink );
                paddingRect.Resize( 0, -heightToShrink );
                borderRect.Resize( 0, -heightToShrink );
                normalFlowBorderRect.Resize( 0, -heightToShrink );
                marginRect.Resize( 0, -heightToShrink );
                }
            }
        }

    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );
    aNode.SetMarginRect( marginRect );
    }

// -----------------------------------------------------------------------------
// FitAutoChildrenIntoParentL()
//
// -----------------------------------------------------------------------------
//
static void FitAutoChildrenIntoParentL( RPointerArray< CXnNode >& aAutoChildren,
    RPointerArray< CXnNode >& aCalculatedAutoChildren, CXnNode& aParent,
    const TDesC8& aParentBlockProgression, const TDesC8& aParentDirection,
    CGraphicsDevice& aGraphicsDevice )
    {
    if( IsSrollableBox( aParent ) )
        {
        return;
        }
    
    TSize space = CalculateSpaceUsedByAutoChildrenL( aAutoChildren,
        aCalculatedAutoChildren, aParent, aParentBlockProgression,
        aParentDirection, aGraphicsDevice );

    TSize parentSize( aParent.Rect().Size() );

    while( ( ( parentSize.iHeight < space.iHeight ) &&
             ( parentSize.iHeight > 0 ) ) ||
           ( ( parentSize.iWidth < space.iWidth ) &&
             ( parentSize.iWidth > 0 ) ) )
        {
        // if areas go over just one pixel, cut the pixel
        // from a percent sized node
        if ( ( parentSize.iHeight == space.iHeight - 1 ) ||
             ( parentSize.iWidth == space.iWidth - 1 ) )
            {
            CutOnePixelFromPercentChildNodeL( aParent, aParentBlockProgression );
            return;
            }

        TInt childCount( DisplayedChildrenCountL( aParent ) );

        PriorizeChildrenL( aParent );

        if ( DisplayedChildrenCountL( aParent ) == childCount )
            {
            // cannot drop any more children, but they still don't fit
            return;
            }

        space = CalculateSpaceUsedByAutoChildrenL( aAutoChildren,
            aCalculatedAutoChildren, aParent, aParentBlockProgression,
            aParentDirection, aGraphicsDevice );
        }
    }

// -----------------------------------------------------------------------------
// VerticalCentimetersToPixelsL()
// Convert vertical centimeter value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalCentimetersToPixelsL( TReal& aCentimeters,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aCentimeters * KCentimetersAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.VerticalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// VerticalInchesToPixelsL()
// Convert vertical inch value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalInchesToPixelsL( TReal& aInches,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aInches * KInchesAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.VerticalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// VerticalMillimetersToPixelsL()
// Convert vertical millimeter value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalMillimetersToPixelsL( TReal& aMillimeters,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aMillimeters * KMillimetersAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.VerticalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// VerticalPicasToPixelsL()
// Convert vertical pica value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalPicasToPixelsL( TReal& aPicas,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aPicas * KPicasAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.VerticalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// VerticalPixelValueL()
// Convert vertical property to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalPixelValueL( CXnProperty* aValue, TInt aReferenceValue,
    CGraphicsDevice& aScreenDevice, TReal aVerticalUnitInPixels )
    {
    return VerticalValueToPixelsL( DomPropertyValueFromProperty( aValue ),
        static_cast< TReal >( aReferenceValue ), aScreenDevice,
        aVerticalUnitInPixels );
    }

// -----------------------------------------------------------------------------
// VerticalPointsToPixelsL()
// Convert vertical point value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalPointsToPixelsL( TReal& aPoints,
    CGraphicsDevice& aScreenDevice )
    {
    TReal twips = aPoints * KPointsAsTwips;
    TInt intTwips = static_cast< TInt >( twips + KIntConversionConstant );
    return aScreenDevice.VerticalTwipsToPixels( intTwips );
    }

// -----------------------------------------------------------------------------
// VerticalValueToPixelsL()
// Convert vertical property value to pixel value.
// -----------------------------------------------------------------------------
//
static TInt VerticalValueToPixelsL( CXnDomPropertyValue* aValue,
    TReal aReferenceValue, CGraphicsDevice& aScreenDevice,
    TReal aVerticalUnitInPixels )
    {
    if ( !aValue )
        {
        User::Leave( KXnErrDomPropertyValueNULL );
        }
    CXnDomPropertyValue::TPrimitiveValueType valueType =
        aValue->PrimitiveValueType();
    //  Handle error cases first, inherit is handled elsewhere, none returns 0
    switch ( valueType )
        {
        case CXnDomPropertyValue::EUnknown:
        case CXnDomPropertyValue::EEms:
        case CXnDomPropertyValue::EExs:
        case CXnDomPropertyValue::EDeg:
        case CXnDomPropertyValue::ERad:
        case CXnDomPropertyValue::EGrad:
        case CXnDomPropertyValue::EMs:
        case CXnDomPropertyValue::ES:
        case CXnDomPropertyValue::EHz:
        case CXnDomPropertyValue::EKHz:
        case CXnDomPropertyValue::EDimension:
        case CXnDomPropertyValue::EString:
        case CXnDomPropertyValue::EUri:
        case CXnDomPropertyValue::EIdent:
        case CXnDomPropertyValue::EAttr:
        case CXnDomPropertyValue::ECounter:
        case CXnDomPropertyValue::ERect:
        case CXnDomPropertyValue::ERgbColor:
        case CXnDomPropertyValue::ERgbaColor:
        case CXnDomPropertyValue::EFunction:
            break;
        default:
            {
            TReal value = aValue->FloatValueL();
            if ( valueType == CXnDomPropertyValue::ENumber ||
                 valueType == CXnDomPropertyValue::EPx )
                {
                return static_cast< TInt >( value );
                }
            else if ( valueType == CXnDomPropertyValue::EPercentage )
                {
                return PercentageToPixelsL( value, aReferenceValue );
                }
            else if ( valueType == CXnDomPropertyValue::EIn )
                {
                return VerticalInchesToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::ECm )
                {
                return VerticalCentimetersToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EMm )
                {
                return VerticalMillimetersToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EPt )
                {
                return VerticalPointsToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EPc )
                {
                return VerticalPicasToPixelsL( value, aScreenDevice );
                }
            else if ( valueType == CXnDomPropertyValue::EUnitValue )
                {
                return VerticalUnitsToPixelsL( value, aVerticalUnitInPixels );
                }
            }
            break;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// ProcessAdaptiveAreasL()
// Process adaptive areas
// -----------------------------------------------------------------------------
//
static void ProcessAdaptiveAreasL( CXnNode& aParent,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    CXnProperty* blockProgressionProperty( aParent.BlockProgressionL() );
    CXnProperty* directionProperty( aParent.DirectionL() );
    const TDesC8* blockProgression(
        &XnPropertyNames::style::common::block_progression::KTB() );
    const TDesC8* direction(
        &XnPropertyNames::style::common::direction::KLTR() );

    if ( directionProperty )
        {
        direction = &directionProperty->StringValue();
        }

    if ( blockProgressionProperty )
        {
        blockProgression = &blockProgressionProperty->StringValue();
        }

    CalculateAdaptiveSizeL( aParent, *blockProgression, *direction,
        aGraphicsDevice, aHorizontalUnitInPixels, aVerticalUnitInPixels );
    }

// -----------------------------------------------------------------------------
// CalculateAdaptiveSizeL()
// Fixes the adaptive node size to its layouted children size
// -----------------------------------------------------------------------------
//
static void CalculateAdaptiveSizeL( CXnNode& aNode,
    const TDesC8& aParentBlockProgression, const TDesC8& /*aParentDirection*/,
    CGraphicsDevice& aGraphicsDevice, TReal aHorizontalUnitInPixels,
    TReal aVerticalUnitInPixels )
    {
    TInt adaptive( aNode.IsAdaptive() );
    CXnNode* parent( aNode.Parent() );

    if ( !adaptive || !parent )
        {
        return;
        }

    TInt adaptiveHeight( 0 );
    TInt adaptiveWidth( 0 );

    RPointerArray< CXnNode > children;
    CleanupClosePushL( children );

    TInt count( DisplayedChildrenCountL( aNode, &children ) );

    TSize size;

    if ( count == 0 )
        {
        TRect parentRect( parent->Rect() );
        
        if( parent->Control() && parent->Control()->OwnsWindow() )
            {
            parentRect = TRect( parentRect.iTl - parent->MarginRect().iTl, parentRect.Size() );
            }
                
        RPointerArray< CXnNode >& siblings( parent->Children() );
        
        for ( TInt i = 0; i < siblings.Count(); i++ )
            {
            CXnNode* sibling( siblings[i] );
            
            if ( sibling == &aNode )
                {
                break;
                }
            
            if ( IsNodeDisplayedL( *sibling ) && !sibling->IsDropped() &&
                 !IsAbsoluteL( *sibling ) && !IsNodeTooltip( *sibling ) )
                {
                TRect marginRect( sibling->MarginRect() );
            
                TInt availableWidth( parentRect.Width() );
                TInt availableHeight( parentRect.Height() );    
                
                if ( aParentBlockProgression ==
                     XnPropertyNames::style::common::block_progression::KTB ||
                     aParentBlockProgression ==
                     XnPropertyNames::style::common::block_progression::KBT )
                    {
                    availableHeight -= marginRect.Height();
                    
                    if ( availableHeight < 0 )
                        {
                        parentRect.SetHeight( 0 );                        
                        }
                    else
                        {
                        parentRect.SetHeight( availableHeight );
                        }
                    }
                else // LR / RL
                    {
                    availableWidth -= marginRect.Width();
                    
                    if ( availableWidth < 0 )
                        {
                        parentRect.SetWidth( 0 );
                        }
                    else
                        {
                        parentRect.SetWidth( availableWidth );
                        }
                    }                 
                }
            }
        
        // I don't have any displayed childrens, fix my own size
        size = CalculateTotalDimensionsL( aNode, ETrue, EFalse,
            parentRect, aGraphicsDevice, aHorizontalUnitInPixels,
            aVerticalUnitInPixels );
        adaptiveHeight = size.iHeight;
        adaptiveWidth = size.iWidth;
        }
    else
        {
        TBool autoHeight( EFalse );
        TBool autoWidth( EFalse );
        TBool sizeKnown( EFalse );
        for ( TInt i = 0; i < children.Count(); i++ )
            {
            CXnNode* node( children[i] );
            if ( !IsNodeDisplayedL( *node ) )
                {
                continue;
                }
            CXnProperty* heightProperty( node->HeightL() );
            if ( !( node->IsAdaptive() & XnAdaptive::EHeight ) &&
                  ( adaptive & XnAdaptive::EHeight ) &&
                    IsPropertyAutoL( *heightProperty ) )
                {
                adaptiveHeight = node->MarginRect().Size().iHeight;
                autoHeight = ETrue;
                if ( !( adaptive & XnAdaptive::EWidth ) )
                    {
                    // Height is now known and width is not adaptive
                    // -> size is now known
                    sizeKnown = ETrue;
                    }
                }
            CXnProperty* widthProperty( node->WidthL() );
            if ( !( node->IsAdaptive() & XnAdaptive::EWidth ) &&
                  ( adaptive & XnAdaptive::EWidth ) &&
                    IsPropertyAutoL( *widthProperty ) )
                {
                adaptiveWidth = node->MarginRect().Size().iWidth;
                autoWidth = ETrue;
                if ( !( adaptive & XnAdaptive::EHeight ) )
                    {
                    // Width is now known and height is not adaptive
                    // -> size is now known
                    sizeKnown = ETrue;
                    }
                }
            if ( autoWidth && autoHeight )
                {
                // Both auto -> size is now known
                sizeKnown = ETrue;
                }
            if ( !sizeKnown )
                {
                // Fix adaptive sized child's size first
                ProcessAdaptiveAreasL( *node, aGraphicsDevice,
                    aHorizontalUnitInPixels, aVerticalUnitInPixels );
                TSize marginSize( node->MarginRect().Size() );
                if ( aParentBlockProgression ==
                     XnPropertyNames::style::common::block_progression::KTB ||
                     aParentBlockProgression ==
                     XnPropertyNames::style::common::block_progression::KBT )
                    {
                    if ( !autoHeight && ( adaptive & XnAdaptive::EHeight ) )
                        {
                        adaptiveHeight += marginSize.iHeight;
                        }
                    if ( !autoWidth && ( adaptive & XnAdaptive::EWidth ) )
                        {
                        if ( marginSize.iWidth > adaptiveWidth )
                            {
                            adaptiveWidth = marginSize.iWidth;
                            }
                        }
                    }
                else if ( aParentBlockProgression ==
                          XnPropertyNames::style::common::
                          block_progression::KLR || aParentBlockProgression ==
                          XnPropertyNames::style::common::
                          block_progression::KRL )
                    {
                    if ( !autoHeight && ( adaptive & XnAdaptive::EHeight ) )
                        {
                        if ( marginSize.iHeight > adaptiveHeight )
                            {
                            adaptiveHeight = marginSize.iHeight;
                            }
                        }
                    if ( !autoWidth && ( adaptive & XnAdaptive::EWidth ) )
                        {
                        adaptiveWidth += marginSize.iWidth;
                        }
                    }
                }
            }
        }

    TRect contentRect( aNode.Rect() );
    TRect paddingRect( aNode.PaddingRect() );
    TRect borderRect( aNode.BorderRect() );
    TRect marginRect( aNode.MarginRect() );
    TRect normalFlowBorderRect( aNode.NormalFlowBorderRect() );

    TRect paddingRectDiff( SubtractRect( paddingRect, contentRect ) );
    TRect borderRectDiff( SubtractRect( borderRect, contentRect ) );
    TRect marginRectDiff( SubtractRect( marginRect, contentRect ) );
    TRect normalFlowBorderRectDiff( SubtractRect(
        normalFlowBorderRect, contentRect ) );

    TInt dy( 0 );
    TInt dx( 0 );

    if ( adaptive & XnAdaptive::EHeight )
        {
        if ( adaptiveHeight <= 0 )
            {
            adaptiveHeight = 0;
            }
        dy = contentRect.Size().iHeight - adaptiveHeight;
        }
    if ( adaptive & XnAdaptive::EWidth )
        {
        if ( adaptiveWidth <= 0 )
            {
            adaptiveWidth = 0;
            }
        dx = contentRect.Size().iWidth - adaptiveWidth;
        }

    contentRect.Resize( -dx, -dy );

    paddingRect = AddRect( contentRect, paddingRectDiff );
    borderRect = AddRect( contentRect, borderRectDiff );
    marginRect = AddRect( contentRect, marginRectDiff );
    normalFlowBorderRect = AddRect( contentRect, normalFlowBorderRectDiff );

    aNode.SetRect( contentRect );
    aNode.SetPaddingRect( paddingRect );
    aNode.SetBorderRect( borderRect );
    aNode.SetMarginRect( marginRect );
    aNode.SetNormalFlowBorderRect( normalFlowBorderRect );

    aNode.FixAdaptiveSizeL( contentRect.Size() );

    aNode.UiEngine()->CheckAdaptiveContentL( aNode, contentRect.Size() );

    CleanupStack::PopAndDestroy( &children );
    }

// -----------------------------------------------------------------------------
// BuildTriggerNodeLC
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerNodeLC( CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );

    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );

    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );

    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );

    CleanupStack::PushL( nameValue );

    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue, *aUiEngine.ODT()->DomDocument().StringPool() );

    CleanupStack::Pop( nameValue );

    CleanupStack::PushL( name );

    node->SetPropertyL( name );

    CleanupStack::Pop( name );

    //CleanupStack::Pop( node );

    return node;
    }

// -----------------------------------------------------------------------------
// UpdateInternalUnits
// Updates UiEngineImpl unit values
// -----------------------------------------------------------------------------
//
static void UpdateInternalUnits( TReal& aHorizontalUnitInPixels,
    TReal& aVerticalUnitInPixels, TRect aRect )
    {
    TAknWindowComponentLayout referenceLayout =
        AknLayoutScalable_Avkon::aid_value_unit2();
    TRect parent( aRect );

    TAknLayoutRect rect;

    // TAknWindowLineLayout should go directly
    //to LayoutRect/LayoutControl etc. methods
    rect.LayoutRect( parent, referenceLayout.LayoutLine() );

    TReal rectWidth = rect.Rect().Width();

    if ( rectWidth == 0 )
        {
        aHorizontalUnitInPixels = 0;
        }

    aHorizontalUnitInPixels = rectWidth / 10;

    TReal rectHeight = rect.Rect().Height();

    if ( rectHeight == 0 )
        {
        aVerticalUnitInPixels = 0;
        }

    aVerticalUnitInPixels = rectHeight / 10;
    }

// -----------------------------------------------------------------------------
// BuildScreenDeviceChangeTriggerNodeLC
// Build trigger node for screen device change event
// -----------------------------------------------------------------------------
//
static CXnNode* BuildScreenDeviceChangeTriggerNodeLC( CXnUiEngine& aUiEngine )
    {
    CXnDomStringPool* sp( aUiEngine.ODT()->DomDocument().StringPool() );

    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );

    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );

    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );

    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL( sp );
    CleanupStack::PushL( nameValue );

    nameValue->SetStringValueL( CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::KScreenDeviceChange );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName, nameValue, *sp );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );

    node->SetPropertyL( name );
    CleanupStack::Pop( name );

    CXnDomPropertyValue* reasonValue = CXnDomPropertyValue::NewL( sp );
    CleanupStack::PushL( reasonValue );

    const TDesC8* reasonString( NULL );

    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        reasonString = &XnPropertyNames::action::trigger::name::
            orientation::reason::KLandscape;
        }
    else
        {
        reasonString = &XnPropertyNames::action::trigger::name::
            orientation::reason::KPortrait;
        }

    reasonValue->SetStringValueL( CXnDomPropertyValue::EString, *reasonString );

    CXnProperty* reason = CXnProperty::NewL( XnPropertyNames::action::trigger::
        name::orientation::KReason, reasonValue, *sp );

    CleanupStack::Pop( reasonValue );

    CleanupStack::PushL( reason );

    node->SetPropertyL( reason );

    CleanupStack::Pop( reason );

    return node;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
static void ReportScreenDeviceChangedL( const CXnPluginData& aPluginData )
    {    
    CXnNode* node( NULL );
    
    if ( aPluginData.Node() )
        {
        node = aPluginData.Node()->LayoutNode();
        }
    
    if ( !node )
        {
        return;        
        }
    
    CXnNode* trigger( BuildScreenDeviceChangeTriggerNodeLC( *node->UiEngine() ) );

    node->ReportXuikonEventL( *trigger );
    
    CleanupStack::PopAndDestroy( trigger );
    
    RPointerArray< CXnPluginData >& plugins( aPluginData.PluginData() );
    
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        ReportScreenDeviceChangedL( *plugins[i] );
        }     
    }

// -----------------------------------------------------------------------------
// FillFocusCandidatesL
// -----------------------------------------------------------------------------
//
static void FillFocusCandidatesL( CXnNode* aParent,
    RPointerArray< CXnNode >& aArray )
    {
    RPointerArray< CXnNode >& children( aParent->Children() );

    aArray.AppendL( aParent );

    for ( TInt i = 0; i < children.Count(); i++ )
        {
        FillFocusCandidatesL( children[i], aArray );
        }
    }

// -----------------------------------------------------------------------------
// SetAdaptivesL
// -----------------------------------------------------------------------------
//
static TBool SetAdaptivesL( CXnNode& aNode )
    {
    TBool retval( EFalse );

    // Check if the node is adaptive
    if ( aNode.IsAdaptive( ETrue ) && IsNodeDisplayedL( aNode ) )
        {
        retval = ETrue;
        // Mark adaptive node to be measured
        aNode.SetAdaptiveL( XnAdaptive::EMeasure );
        }

    RPointerArray< CXnNode >& children( aNode.Children() );

    for ( TInt i = 0; i < children.Count(); i++ )
        {
        TBool ret( SetAdaptivesL( *children[i] ) );
        if ( ret )
            {
            retval = ret;
            }
        }

    return retval;
    }

#ifdef _XN3_DEBUG_
// -----------------------------------------------------------------------------
// TraceTreeL
// -----------------------------------------------------------------------------
//
static void TraceTreeL( CXnNode* aRootNode )
    {
    CXnDepthFirstTreeIterator< CXnNode >* iterator = 
            CXnDepthFirstTreeIterator< CXnNode >::NewL( *aRootNode );
    CleanupStack::PushL( iterator );

    RDebug::Print( _L("*** XML UI layout:") );

    TBuf8< 512 > debug;

    TInt level( 0 );

    for( CXnNode* node = iterator->Value(); node;
        node = iterator->NextL() )
        {
        debug = _L8( "* " );

        debug.Append( node->Type()->Type() );

        CXnProperty* id( node->IdL() );

        if ( id )
            {
            debug.Append( _L8( " id=" ) );
            debug.Append( id->StringValue() );
            }
                            
        debug.AppendFormat( _L8( " laidout=%d" ), node->IsLaidOut() );
        debug.AppendFormat( _L8( " displayed=%d" ),
            IsNodeDisplayedL( *node ) );

        CCoeControl* control( node->Control() );

        if ( control )
            {
            TBool visible( control->IsVisible() );
            debug.AppendFormat( _L8( " visible=%d" ), visible );

            TRect rect;
            TPoint tl;
            TPoint br;
            
            rect = node->MarginRect();
            tl = rect.iTl;
            br = rect.iBr;
            
            debug.Append( _L8( "; Margin " ) );
            debug.AppendFormat(
                _L8( "tl(%d,%d) br(%d,%d)" ), tl.iX, tl.iY, br.iX, br.iY );

            rect = node->BorderRect();
            tl = rect.iTl;
            br = rect.iBr;
            
            debug.Append( _L8( "; Border " ) );
            debug.AppendFormat(
                _L8( "tl(%d,%d) br(%d,%d)" ), tl.iX, tl.iY, br.iX, br.iY );

            rect = node->PaddingRect();
            tl = rect.iTl;
            br = rect.iBr;
            
            debug.Append( _L8( "; Padding " ) );
            debug.AppendFormat(
                _L8( "tl(%d,%d) br(%d,%d)" ), tl.iX, tl.iY, br.iX, br.iY );

            rect = node->Rect();
            tl = rect.iTl;
            br = rect.iBr;
            
            debug.Append( _L8( "; Content " ) );
            debug.AppendFormat(
                _L8( "tl(%d,%d) br(%d,%d)" ), tl.iX, tl.iY, br.iX, br.iY );
            
            if ( control->Rect() != node->BorderRect() )
                {
                debug.Append( _L8( "; control rect != rect set by layout algorithm " ) );

                rect = control->Rect();
                tl = rect.iTl;
                br = rect.iBr;
                                
                debug.AppendFormat(
                    _L8( "tl(%d,%d) br(%d,%d)" ), tl.iX, tl.iY, br.iX, br.iY );                
                }
            }
        else
            {
            debug.Append( _L8( ", no control" ) );
            }
        RDebug::RawPrint( debug );
        }

    CleanupStack::PopAndDestroy( iterator );
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::CXnUiEngineImpl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnUiEngineImpl::CXnUiEngineImpl( CXnUiEngine& aUiEngine, 
    CXnAppUiAdapter& aAdapter )     
    : iUiEngine( &aUiEngine ), iAppUiAdapter( aAdapter ), 
    iViewManager( aAdapter.ViewManager() )    
    {
    iAppUiAdapter.UiStateListener().AddObserver( *this );
    
    iViewManager.AddObserver( *this );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::ConstructL()
    {                  
    iEditMode = CXnEditMode::NewL( *iUiEngine );
   
    iCurrentGraphicsDevice = CCoeEnv::Static()->ScreenDevice();

    // Update the units here, even the refence client rect is only a quess.
    // That is because we don't know the UI furniture yet. Units are
    // needed in controls constructions
    UpdateInternalUnits( 
        iHorizontalUnitInPixels, 
        iVerticalUnitInPixels,
        iAppUiAdapter.ClientRect() );        
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnUiEngineImpl* CXnUiEngineImpl::NewL( CXnUiEngine& aUiEngine,
    CXnAppUiAdapter& aAdapter )
    {
    return new ( ELeave ) CXnUiEngineImpl( aUiEngine, aAdapter );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::~CXnUiEngineImpl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnUiEngineImpl::~CXnUiEngineImpl()
    {        
    iAppUiAdapter.UiStateListener().RemoveObserver( *this );
    
    iViewManager.RemoveObserver( *this );
           
    delete iEditMode;

    iFocusCandidateList.Reset();

    if ( iRedrawRegions.Count() )
        {
        iRedrawRegions.ResetAndDestroy();
        }

    iDirtyList.Reset();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AppUiAdapter()
// -----------------------------------------------------------------------------
//
CXnAppUiAdapter& CXnUiEngineImpl::AppUiAdapter() const
    {
    return iAppUiAdapter;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::LayoutUIL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::LayoutUIL( CXnNode* /*aNode*/ )
    {
    if ( IsLayoutDisabled() )
        {     
        iLayoutControl |= XnLayoutControl::ELayoutUI;
        return;
        }

    iLayoutControl &= ~XnLayoutControl::ELayoutUI;

    if ( iDirtyList.Count() == 0 )
        {
        // Nothing to do, layout is up-to-date
        return;
        }

    DisableRenderUiLC();
    CXnNode* candidate( StartLayoutFromL() );
    PrepareRunLayoutL();
    iDirtyList.Reset();

    // Run layout until it is fully calculated
    do
        {
        iLayoutPhase = RunLayoutL( candidate );
        }
    while ( iLayoutPhase != XnLayoutPhase::ENone );

    CXnNode* focused( FocusedNode() );

    if ( focused && focused->IsDropped() )
        {
        // The currently focused node is dropped, run losevisualization
        CXnNode* loseVisualisation =
            BuildTriggerNodeLC( *iUiEngine,
                XnPropertyNames::action::trigger::name::KLoseVisualisation );

        focused->ReportXuikonEventL( *loseVisualisation );
        CleanupStack::PopAndDestroy( loseVisualisation );

        if ( FocusedNode() == focused )
            {
            // Remove focus as losevisualization trigger didn't
            // change focus from dropped node to another one
            SetFocusedNodeL( NULL );
            }
        }

    if ( !FocusedNode() && iAppUiAdapter.FocusShown() )
        {
        // No focus, try if some of the focus candidates can be focused
        RootNode()->RunFocusChangeL( iFocusCandidateList );
        }
        
    iFocusCandidateList.Reset();

    iAppUiAdapter.EffectManager()->UiLayouted();
    
    // Layout is now recalculated
    CleanupStack::PopAndDestroy(); // anonymous
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::LayoutFromNodeL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::LayoutFromNodeL( CXnNode& aNode )
    {
    PrepareRunLayoutL( aNode );

    // Run layout until it is fully calculated
    do
        {
        iLayoutPhase = RunLayoutFromNodeL( aNode );
        }
    while ( iLayoutPhase != XnLayoutPhase::ENone );
    }

// -----------------------------------------------------------------------------
// PrepareRunLayoutL()
// Set dropped flags of all nodes in the tree to ENone.
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::PrepareRunLayoutL()
    {
    if ( iLayoutPhase == XnLayoutPhase::ENone )
        {
        iLayoutPhase = XnLayoutPhase::ELayout;

        for ( TInt i = 0; i < iDirtyList.Count(); i++ )
            {
            CXnNode* dirty( iDirtyList[i] );

            // Clear dropped flags recursively
            SetNodeDroppedL( *dirty, XnNodeLayout::ENone );

            if ( SetAdaptivesL( *dirty ) )
                {
                // Adaptive node causes measure phase
                iLayoutPhase = XnLayoutPhase::EMeasure;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// PrepareRunLayoutL()
// Set dropped flags of all nodes in the tree to ENone.
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::PrepareRunLayoutL( CXnNode& aNode )
    {
    if ( iLayoutPhase == XnLayoutPhase::ENone )
        {
        iLayoutPhase = XnLayoutPhase::ELayout;

        CXnNode* dirty( &aNode );
        dirty->ClearRenderedAndLaidOut();
        
        // Clear dropped flags recursively
        SetNodeDroppedL( *dirty, XnNodeLayout::ENone );

        if ( SetAdaptivesL( *dirty ) )
            {
            // Adaptive node causes measure phase
            iLayoutPhase = XnLayoutPhase::EMeasure;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RunLayoutL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::RunLayoutL( CXnNode* aNode )
    {
    TRect clientRect( ClientRect() );
    // Move it to 0, 0
    clientRect.Move( -clientRect.iTl.iX, -clientRect.iTl.iY );

    RPointerArray< CXnNode > laidOutList;
    CleanupClosePushL( laidOutList );

    if ( IsNodeDisplayedL( *aNode ) )
        {
        if ( aNode->ViewNodeImpl() && !aNode ->IsLaidOut() )
            {
            aNode->SetMarginRect( clientRect );
            aNode->SetBorderRect( clientRect );
            aNode->SetNormalFlowBorderRect( clientRect );
            aNode->SetPaddingRect( clientRect );
            aNode->SetRect( clientRect );
            }

        // Put areas to place
        PlaceAreasL( *aNode, laidOutList, iLayoutPhase,
            *iCurrentGraphicsDevice, iHorizontalUnitInPixels,
            iVerticalUnitInPixels );
        }

    TInt nextPhase;

    switch ( iLayoutPhase )
        {
        case XnLayoutPhase::EMeasure:
            nextPhase = XnLayoutPhase::ELayout;
            break;
        case XnLayoutPhase::ELayout:
            // Layout is now calculated
            iCurrentView->SetLaidOutL();
            iLayoutControl &= ~XnLayoutControl::EViewDirty;
            // Check nodes which dimensions are changed
            for ( TInt i = 0; i < laidOutList.Count(); i++ )
                {
                CXnNode* node( laidOutList[i] );
                if( IsSrollableBox( *node ) && node->ScrollableControl() )
                    {
                    node->ScrollableControl()->LayoutChangedL();
                    }
                if ( /*IsNodeTooltip( *node ) ||*/ !node->IsLaidOut() )
                    {
                    continue;
                    }
                CXnControlAdapter* adapter( node->Control() );
                if ( adapter )
                    {
                    TRect rect;
                    
                    if ( node->ViewNodeImpl() )
                        {
                        rect = ClientRect();
                        
                        // Move it to 0, 0
                        //rect.Move( -rect.iTl.iX, -rect.iTl.iY );                        
                        }
                    else
                        {
                        rect = node->BorderRect();
                        }
                    
                    if ( adapter->Rect() != rect )
                        {
                        AddToRedrawListL( node, rect );
                        
                        adapter->SetRect( rect );                        

                        CXnProperty* prop = node->GetPropertyL(
                            XnPropertyNames::common::KSizeAware );
                        if ( prop && prop->StringValue() ==
                             XnPropertyNames::KTrue )
                            {
                            CXnNode* sizeChanged =
                                BuildTriggerNodeLC( *iUiEngine,
                                XnPropertyNames::action::trigger::name::
                                KSizeChanged );
                            node->ReportXuikonEventL( *sizeChanged );
                            CleanupStack::PopAndDestroy( sizeChanged );
                            }
                        }
                    }
                }
            /* flow through */
        default:
            nextPhase = XnLayoutPhase::ENone;
            break;
        }
    CleanupStack::PopAndDestroy( &laidOutList );
    return nextPhase;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RunLayoutL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::RunLayoutFromNodeL( CXnNode& aNode )
    {
    TRect clientRect( ClientRect() );
    // Move it to 0, 0
    clientRect.Move( -clientRect.iTl.iX, -clientRect.iTl.iY );

    RPointerArray< CXnNode > laidOutList;
    CleanupClosePushL( laidOutList );

    if ( IsNodeDisplayedL( aNode ) )
        {
        if ( aNode.ViewNodeImpl() && !aNode.IsLaidOut() )
            {
            aNode.SetMarginRect( clientRect );
            aNode.SetBorderRect( clientRect );
            aNode.SetNormalFlowBorderRect( clientRect );
            aNode.SetPaddingRect( clientRect );
            aNode.SetRect( clientRect );
            }

        // Put areas to place
        PlaceAreasL( aNode, laidOutList, iLayoutPhase,
            *iCurrentGraphicsDevice, iHorizontalUnitInPixels,
            iVerticalUnitInPixels );
        }

    TInt nextPhase;

    switch ( iLayoutPhase )
        {
        case XnLayoutPhase::EMeasure:
            nextPhase = XnLayoutPhase::ELayout;
            break;
        case XnLayoutPhase::ELayout:
            // Layout is now calculated
            aNode.SetLaidOutL();

            // Check nodes which dimensions are changed
            for ( TInt i = 0; i < laidOutList.Count(); i++ )
                {
                CXnNode* node( laidOutList[i] );
                if( IsSrollableBox( *node ) && node->ScrollableControl() )
                    {
                    node->ScrollableControl()->LayoutChangedL();
                    }
                if ( !node->IsLaidOut() )
                    {
                    continue;
                    }
                CXnControlAdapter* adapter( node->Control() );
                if ( adapter )
                    {
                    TRect rect;
                    
                    if ( node->ViewNodeImpl() )
                        {
                        rect = ClientRect();
                        }
                    else
                        {
                        rect = node->BorderRect();
                        }
                    if ( adapter->Rect() != rect )
                        {
                        adapter->SetRect( rect );
                        }
                    }
                }
            /* flow through */
        default:
            nextPhase = XnLayoutPhase::ENone;
            break;
        }
    CleanupStack::PopAndDestroy( &laidOutList );
    return nextPhase;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RenderUIL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::RenderUIL( CXnNode* /*aNode*/ )
    {
    if ( IsLayoutDisabled() )
        {
        // Layout is not up-to-date
        iLayoutControl |= XnLayoutControl::ERenderUI;
        return;
        }
    
#ifdef _XN3_DEBUG_
    TraceTreeL(iCurrentView );
#endif                          

    __PRINTS("*** CXnUiEngineImpl::RenderUIL ***");
    
    for( TInt i=0; i<iRedrawRegions.Count(); i++)
        {
        CCoeControl* control = iRedrawRegions[i]->iControl;
        RRegion& redrawRegion = iRedrawRegions[i]->iRegion;
        
        if( redrawRegion.CheckError() )
            {                
            // Mark tree rendered
            iCurrentView->SetRenderedL();
            // Error occured during dirty set, redraw whole window
            control->DrawNow();               
            
            __PRINTS("* CXnUiEngineImpl::RenderUIL - redraw region error -> full redraw ***");
            }                                        
        else 
            {
            if( !redrawRegion.IsEmpty() )                                        
                {
                // Mark tree rendered
                iCurrentView->SetRenderedL();
                                
                if ( iAppUiAdapter.EffectManager()->ControlEffectActive( control ) )
                    {
                    // control effect is ongoing, no need to draw control yet
                    redrawRegion.Clear();
                    continue;
                    }
                
                TInt count( redrawRegion.Count() );
                
                if( count > 2 )
                    {
                    // Make bounding rect over the dirty areas
                    TRect boundingRect( redrawRegion.BoundingRect() );
                    
                    __PRINT( __DBG_FORMAT( "* CXnUiEngineImpl::RenderUIL - redrawing bounding rect iTl.iX: %d, iTl.iY: %d, iBr.iX: %d, iBr.iY: %d" ),          
                        boundingRect.iTl.iX, boundingRect.iTl.iY, boundingRect.iBr.iX, boundingRect.iBr.iY );                                         
                    
                    control->DrawNow( boundingRect );                    
                    }                               
                else
                    {
                    for( TInt i = 0; i < count; i++ )
                        {
                        // Draw every dirty area separately
                        TRect redrawRect( redrawRegion[i] );

                        __PRINT( __DBG_FORMAT( "* CXnUiEngineImpl::RenderUIL - redrawing rect iTl.iX: %d, iTl.iY: %d, iBr.iX: %d, iBr.iY: %d" ),          
                                redrawRect.iTl.iX, redrawRect.iTl.iY, redrawRect.iBr.iX, redrawRect.iBr.iY );                                         
                        
                        control->DrawNow( redrawRect );
                        }
                    }  
                }
            else
                {
                __PRINTS("* CXnUiEngineImpl::RenderUIL - nothing to redraw ***");
                }
            }  
        
        redrawRegion.Clear();  
        }

    RefreshMenuL();

    iLayoutControl &= ~XnLayoutControl::ERenderUI;
    
    iAppUiAdapter.EffectManager()->UiRendered();
    
    __PRINTS("*** CXnUiEngineImpl::RenderUIL - done ***");
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RenderFromNodeL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::RenderFromNodeL( CXnNode& aNode )
    {
#ifdef _XN3_DEBUG_
    TraceTreeL( &aNode );
#endif 
    CCoeControl* control = WindowOwningControl( aNode );
    control->DrawNow();                                              
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RootNode()
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::RootNode()
    {
    return iViewManager.RootNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ODT()
// -----------------------------------------------------------------------------
//
CXnODT* CXnUiEngineImpl::ODT()
    {
    return iViewManager.ODT();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::FindNodeByIdL()
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::FindNodeByIdL( const TDesC& aAreaId,
    const TDesC& aNamespace )
    {
    HBufC8* id = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aAreaId );
    CleanupStack::PushL( id );

    HBufC8* ns = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aNamespace );
    CleanupStack::PushL( ns );

    CXnNode* returnValue( FindNodeByIdL( *id, *ns ) );

    CleanupStack::PopAndDestroy( ns );
    CleanupStack::PopAndDestroy( id );

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::FindNodeByIdL()
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::FindNodeByIdL( const TDesC8& aAreaId,
    const TDesC8& aNamespace )
    {
    // Find the namespace where to start node id look-up 
    CXnViewData& active( iViewManager.ActiveViewData() );
    
    CXnPluginData* pluginData( active.Plugin( aNamespace ) ); 
        
    if ( aNamespace != KNullDesC8 && !pluginData )
        {
        RPointerArray< CXnPluginData >& views( 
            iViewManager.ActiveAppData().PluginData() );
        
        for ( TInt i = 0; i < views.Count(); i++ )
            {
            CXnViewData* view = static_cast< CXnViewData* >( views[i] );
            
            if ( view != &active )
                {
                pluginData = view->Plugin( aNamespace );
                
                if ( pluginData )
                    {
                    break;
                    }
                }
            }                
        }
    
    if ( !pluginData )
        {
        return NULL;
        }

    CXnNode* root( pluginData->Node()->LayoutNode() );

    return ::FindNodeByIdL( root, aAreaId, aNamespace );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::FindNodeByClassL()
// -----------------------------------------------------------------------------
//
CXnPointerArray* CXnUiEngineImpl::FindNodeByClassL(
    const TDesC& aClassId, const TDesC& aNamespace )
    {
    HBufC8* classId = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aClassId );
    CleanupStack::PushL( classId );

    HBufC8* ns = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aNamespace );
    CleanupStack::PushL( ns );
    
    CXnPointerArray* returnValue( FindNodeByClassL( *classId, *ns ) );

    CleanupStack::PopAndDestroy( ns );
    CleanupStack::PopAndDestroy( classId );

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::FindNodeByClassL()
// -----------------------------------------------------------------------------
//
CXnPointerArray* CXnUiEngineImpl::FindNodeByClassL(
    const TDesC8& aClassId, const TDesC8& aNamespace )
    {
    // Find the namespace where to start node class look-up
    CXnViewData& active( iViewManager.ActiveViewData() );
    
    CXnPluginData* pluginData( active.Plugin( aNamespace ) ); 
        
    if ( aNamespace != KNullDesC8 && !pluginData )
        {
        RPointerArray< CXnPluginData >& views( 
            iViewManager.ActiveAppData().PluginData() );
        
        for ( TInt i = 0; i < views.Count(); i++ )
            {
            CXnViewData* view = static_cast< CXnViewData* >( views[i] );
            
            if ( view != &active )
                {
                pluginData = view->Plugin( aNamespace );
                
                if ( pluginData )
                    {
                    break;
                    }
                }
            }                
        }
    
    CXnPointerArray* array = CXnPointerArray::NewL();
    CleanupStack::PushL( array );
    
    if ( pluginData )
        {
        CXnNode* root( pluginData->Node()->LayoutNode() );
        ::FindNodeByClassL( root, aClassId, *array, aNamespace );
        }

    CleanupStack::Pop( array );

    return array;
    }
    
// -----------------------------------------------------------------------------
// CXnUiEngineImpl::FindContentSourceNodesL()
// -----------------------------------------------------------------------------
//
CXnPointerArray* CXnUiEngineImpl::FindContentSourceNodesL(
    const TDesC8& aNamespace )
    {
    CXnViewData& active( iViewManager.ActiveViewData() );

    CXnPluginData* pluginData( active.Plugin( aNamespace ) );
    
    if ( aNamespace != KNullDesC8 && !pluginData )
        {
        RPointerArray< CXnPluginData >& views( 
            iViewManager.ActiveAppData().PluginData() );
        
        for ( TInt i = 0; i < views.Count(); i++ )
            {
            CXnViewData* view = static_cast< CXnViewData* >( views[i] );
            
            if ( view != &active )
                {
                pluginData = view->Plugin( aNamespace );
                
                if ( pluginData )
                    {
                    break;
                    }
                }
            }                    
        }
          
    CXnPointerArray* array = CXnPointerArray::NewL();
    CleanupStack::PushL( array );
    
    if ( pluginData )
        {       
        RPointerArray< CXnNode > list;
        CleanupClosePushL( list );
       
        pluginData->ContentSourceNodesL( list );
       
        const TInt count( list.Count() );
       
        array->Container().ReserveL( count );
       
        for ( TInt i = 0; i < count; i++ )
            {
            array->Container().Append( list[i] );
            }
       
        CleanupStack::PopAndDestroy( &list );
        }
   
    CleanupStack::Pop( array );

    return array;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::Resources()
// -----------------------------------------------------------------------------
//
CArrayPtrSeg< CXnResource >& CXnUiEngineImpl::Resources()
    {
    return iViewManager.Resources();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::SetFocusedNodeL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::SetFocusedNodeL( CXnNode* aFocusedNode, TInt aSource )
    {
    if ( iKeyEventDispatcher )
        {
        iKeyEventDispatcher->SetNodeL( aFocusedNode, aSource );
        }    
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FocusedNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::FocusedNode()
    {
    if ( iKeyEventDispatcher )
        {
        return iKeyEventDispatcher->FocusedNode();
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::IsEditMode
// Returns ETrue if the current mode is edit, otherwise EFalse.
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::IsEditMode()
    {
    return ( iEditMode->EditState() != CXnEditMode::ENone );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyViewActivatedL()
// -----------------------------------------------------------------------------
// 
void CXnUiEngineImpl::NotifyViewActivatedL( const CXnViewData& /*aViewData*/ )
    {
    iCurrentView = iViewManager.ViewNode();

    iControlAdapterList = &iViewManager.Controls();
    iCurrentViewControlAdapter = iCurrentView->Control();
    
    iDirtyList.Reset();

    iRedrawRegions.ResetAndDestroy();
       
    // Remove previous menubar and stylus popup node
    iMenuNode = NULL;
    iStylusPopupNode = NULL;

    if ( iKeyEventDispatcher )
        {
        iKeyEventDispatcher->ResetMenuNodeL();    
        }
    
    RPointerArray< CXnNode >& children( iCurrentView->Children() );

    for ( TInt count = children.Count() - 1; count >= 0 ; --count )
        {
        CXnNode* node( children[count] );

        // Check that the given type of a control is parent
        // (or ancestor) of this control
        const TDesC8& type( node->Type()->Type() );
        
        if ( type == KXnMenuBar )
            {
            iMenuNode = node;
            XnMenuInterface::MXnMenuInterface* menuIf( NULL );
            XnComponentInterface::MakeInterfaceL( menuIf, node->AppIfL() );
            if ( menuIf )
                {
                menuIf->SetObserver( *this );
                }
            }
        else if ( type == _L8("styluspopup") )
            {
            iStylusPopupNode = node;
            
            static_cast< CXnPopupControlAdapter* > 
				( node->Control() )->SetObserver( *this );
            }
        }

    SetClientRectL( iAppUiAdapter.ClientRect(), EFalse );
    
    RootNode()->SetDirtyL();
           
    ForceRenderUIL();
           
    iLayoutControl &= ~XnLayoutControl::ERefreshMenu;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyViewLoadedL()
// -----------------------------------------------------------------------------
// 
void CXnUiEngineImpl::NotifyViewLoadedL( const CXnViewData& aViewData )
    {
    ReportScreenDeviceChangedL( aViewData );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyWidgetAdditionL()
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::NotifyWidgetAdditionL(
    const CXnPluginData& aPluginData )
    {
    ReportScreenDeviceChangedL( aPluginData );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyViewAdditionL()
// -----------------------------------------------------------------------------
// 
void CXnUiEngineImpl::NotifyViewAdditionL( const CXnViewData& aViewData )
    {
    ReportScreenDeviceChangedL( aViewData );
    }
    
// -----------------------------------------------------------------------------
// CXnUiEngineImpl::DynInitMenuItemL()
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::DynInitMenuItemL( CXnNodeAppIf& aMenuItem,
    CXnNodeAppIf* aPlugin )
    {       
    // dynamic menuitem types reserved for HS internal use,
    // internal menuitems' origin must be in active view namespace
    _LIT( KAddWidget, "hs_add_widget" );
    _LIT( KRemoveWidget, "hs_remove_widget" );
    _LIT( KAddView, "hs_add_view" );
    _LIT( KRemoveView, "hs_remove_view" );
    _LIT( KOnline, "hs_online" );
    _LIT( KOffline, "hs_offline" );
                  
    CXnProperty* prop( 
        aMenuItem.GetPropertyL( XnPropertyNames::menu::KItemType ) );
    
    TBool retval( EFalse );            
    
    if( !prop )
        {
        // No type defined, declaration error
        return retval;        
        }
                
    CXnViewData& viewData( iViewManager.ActiveViewData() );
    
    TInt viewAmount( iViewManager.ViewAmount() );
    
    // Check if the menuitem is in view namespace
    TBool internal( aMenuItem.Namespace() == ActiveView()->Namespace() );

    HBufC* itemType( prop->StringValueL() );
    CleanupStack::PushL( itemType );
    
    if ( internal )
        {
        if ( *itemType == KAddWidget )
            {
            if ( aPlugin && &aPlugin->Node() )
                {
                CXnPluginData* data( viewData.Plugin( &aPlugin->Node() ) );
                
                if ( data && !data->Occupied() )
                    {
                    retval = ETrue;
                    }
                }        
            }   
        else if ( *itemType == KRemoveWidget )
            {                                          
            CXnNode* node( ( aPlugin ) ? &aPlugin->Node() : FocusedNode() );

            if ( node )
                {
                CXnPluginData* data( viewData.Plugin( node ) );
                
                if ( data && data->Removable() && data->Occupied() )
                    {
                    retval = ETrue;
                    }                              
                }            
            }
        else if ( *itemType == KAddView )
            {
            // checks Max pages value - from RootData
            if ( viewAmount < iViewManager.MaxPages() )
                {
                retval = ETrue;
                }
            }
        else if ( *itemType == KRemoveView )
            {
            if ( viewData.Removable() && viewAmount > 1 )
                {
                retval = ETrue;
                }
            }
        else if ( *itemType == KOnline || *itemType == KOffline )
            {
            retval = iAppUiAdapter.DynInitMenuItemL( *itemType );
            }       
        }
    else
        {
        const TDesC8& ns( aMenuItem.Namespace() );
        
        CXnPluginData* data( viewData.Plugin( ns ) );
        
        if ( data )
            {
            RPointerArray< CXnNode > nodes;
            CleanupClosePushL( nodes );

            data->ContentSourceNodesL( nodes );
            
            RPointerArray< CXnNodeAppIf > list;
            CleanupClosePushL( list );

            for ( TInt i = 0; i < nodes.Count(); i++ )
                {
                list.AppendL( &nodes[i]->AppIfL() );
                }
            
            retval = iAppUiAdapter.DynInitMenuItemL( *itemType, &list );
            
            CleanupStack::PopAndDestroy( 2, &nodes ); // list            
            }               
        }
    
    CleanupStack::PopAndDestroy( itemType );
                  
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::VerticalPixelValueL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::VerticalPixelValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return ::VerticalPixelValueL( aValue, aReferenceValue,
        *iCurrentGraphicsDevice, iVerticalUnitInPixels );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HorizontalPixelValueL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::HorizontalPixelValueL(
    CXnProperty* aValue, TInt aReferenceValue )
    {
    return ::HorizontalPixelValueL( aValue, aReferenceValue,
        *iCurrentGraphicsDevice, iHorizontalUnitInPixels );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::VerticalTwipValueL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::VerticalTwipValueL( CXnProperty* aValue,
    TInt aReferenceValue )
    {
    return iCurrentGraphicsDevice->VerticalPixelsToTwips(
        VerticalPixelValueL( aValue, aReferenceValue ) );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HorizontalTwipValueL()
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::HorizontalTwipValueL( CXnProperty* aValue,
    TInt aReferenceValue )
    {
    return iCurrentGraphicsDevice->HorizontalPixelsToTwips(
        HorizontalPixelValueL( aValue, aReferenceValue ) );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ViewManager()
// -----------------------------------------------------------------------------
//
CXnViewManager* CXnUiEngineImpl::ViewManager()
    {
    return &iViewManager;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ActiveView()
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::ActiveView()
    {
    return iViewManager.ViewNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RefreshMenuL
// Refresh current menu
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::RefreshMenuL()
    {
    if ( IsLayoutDisabled() )
        {
        return;
        }
    
    if ( iLayoutControl & XnLayoutControl::ERefreshMenu )         
        {
        if ( iKeyEventDispatcher )
            {
            iLayoutControl &= ~XnLayoutControl::ERefreshMenu;
            iKeyEventDispatcher->RefreshMenuL();        
            }               
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AddDirtyNodeL
// Add a dirty node. This method must be called via CXnNode SetDirtyL
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::AddDirtyNodeL( CXnNode* aNode, TInt aLevel )
    {
    CXnViewData& data( iViewManager.ActiveViewData() );
    
    if ( !aNode || ( aNode != RootNode() && !data.Plugin( aNode->Namespace() ) ) )
        {
        // No node, or node doesn't belong to active view namespace
        return;
        }

    if ( aNode->Type()->Type() == KMenuBar )
        {
        iLayoutControl |= XnLayoutControl::ERefreshMenu;
        return;
        }

    if ( iLayoutControl & XnLayoutControl::EViewDirty )
        {
        // nothing to do
        return;
        }

    if ( aLevel == XnDirtyLevel::ERender )
        {
        if( aNode == RootNode() )        
            {
            TXnDirtyRegion* dirtyRegion = FindDirtyRegionL( *iCurrentView );
            if( dirtyRegion )
                {
                dirtyRegion->iRegion.Clear();                
                }
            aNode = iCurrentView;        
            }

        // Add to redraw list
        AddToRedrawListL( aNode );
        }
    else
        {
        // Add to dirty list for relayout and redraw
        AddToDirtyListL( aNode );
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ScreenDeviceSize
// Get the size of the current screen device
// -----------------------------------------------------------------------------
//
TSize CXnUiEngineImpl::ScreenDeviceSize() const
    {
    if ( iCurrentGraphicsDevice )
        {
        return iCurrentGraphicsDevice->SizeInPixels();
        }

    return TSize( 0, 0 );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::IsDialogDisplaying
// Whether the dialog is displaying or not.
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::IsDialogDisplaying()
    {    
    return iAppUiAdapter.IsDisplayingControlBetweenPriorities(
        ECoeStackPriorityDialog - 1, KXnStackPriorityKeyEventDispatcher - 1 );
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HandleResourceChangeL
// Handles a change to the control's resources of type aType
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::HandleResourceChangeL( TInt aType )
    {
    if ( iMenuNode )
        {
        CXnControlAdapter* adapter( iMenuNode->Control() );
        
        if ( adapter )
            {
            adapter->HandleResourceChange( aType );
            }        
        }
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        HandleDynamicLayoutVariantSwitchL();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        HandleSkinChangeL();
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HandleSkinChangeL
// Handles a skin change to the controls
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::HandleSkinChangeL()
    {
    // Force relayout
    DisableRenderUiLC();
    
    RootNode()->SetDirtyL();

    for ( TInt i = 0; i < iControlAdapterList->Count(); i++ )
        {
        CXnControlAdapter* adapter( ( *iControlAdapterList )[i] );
        adapter->SkinChanged();
        }

    ForceRenderUIL();
    
    CleanupStack::PopAndDestroy();
    
    // Handle inactive views
    RPointerArray< CXnPluginData >& views(
            iAppUiAdapter.ViewManager().ActiveAppData().PluginData() );

    for ( TInt i = 0; i < views.Count(); i++ )
        {
        CXnViewData* view = static_cast< CXnViewData* >( views[i] );
        if ( view && !view->Active() )
            {
            RPointerArray< CXnControlAdapter > controls;
            CleanupClosePushL( controls );
            view->ControlsL( controls );
            for ( TInt j = 0; j < controls.Count(); j++ )
                {
                controls[j]->SkinChanged();
                }
            CleanupStack::PopAndDestroy( &controls );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HandleDynamicLayoutVariantSwitchL
// Handles a KEikDynamicLayoutVariantSwitch resource change
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::HandleDynamicLayoutVariantSwitchL()
    {
    // Must return here if there is no current view or
    // controladapterlist. This may occur when the phone
    // is booted for the first time and the location/date
    // query is visible.
    if ( !ActiveView() )
        {
        return;
        }

    // remove focus
    iAppUiAdapter.HideFocus();

    // Update client rect
    SetClientRectL( iAppUiAdapter.ClientRect(), EFalse );

    // Update background rect
    // Bg rect is always screen size.
    TRect bgRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, bgRect );
    iAppUiAdapter.ViewAdapter().BgManager().SetRect( bgRect );

    iEditMode->HandleScreenDeviceChangedL();
    
    // Force relayout
    DisableRenderUiLC();
    
    RootNode()->SetDirtyL();

    ReportScreenDeviceChangeL();
    
    iAppUiAdapter.EffectManager()->OrientationChanged();
            
    if ( !iControlAdapterList )
        {
        iControlAdapterList = &iViewManager.Controls();
        }
    for ( TInt i = 0; i < iControlAdapterList->Count(); i++ )
        {
        CXnControlAdapter* adapter( ( *iControlAdapterList )[i] );
        
        adapter->HandleScreenDeviceChangedL();
        }
    
    ForceRenderUIL();
    
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::IsMenuDisplaying
// Checks whether the menu is displaying or not.
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::IsMenuDisplaying()
    {
    if ( iKeyEventDispatcher )
        {
        return iKeyEventDispatcher->IsMenuFocused();    
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::MenubarNode
// Returns menubar node
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::MenuBarNode() const
    {
    return iMenuNode;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::StylusPopupNode
// Returns stylus popup node
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::StylusPopupNode() const
    {
    return iStylusPopupNode;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::AddPassiveFocusedNodeL( CXnNode* aNode )
    {
    if ( iKeyEventDispatcher )
        {
        iKeyEventDispatcher->AddPassiveFocusedNodeL( aNode );    
        }    
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::RemovePassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::RemovePassiveFocusedNodeL( CXnNode* aNode )
    {
    if ( iKeyEventDispatcher )
        {
        iKeyEventDispatcher->RemovePassiveFocusedNodeL( aNode );    
        }    
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::ClearPassiveFocusedNodesL()
    {
    if ( iKeyEventDispatcher )
        {
        iKeyEventDispatcher->ClearPassiveFocusedNodesL();
        }    
    }

// -----------------------------------------------------------------------------
// SetNodeDroppedL()
// Sets node's dropped state
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::SetNodeDroppedL( CXnNode& aNode, TInt aDropped ) const
    {
    if ( aDropped != XnNodeLayout::ENone )
        {
        CXnNode* parent( aNode.Parent() );
        TInt parentAdaptive( parent->IsAdaptive( ETrue ) );

        if ( iLayoutPhase == XnLayoutPhase::EMeasure )
            {
            if ( !parentAdaptive )
                {
                // Only adaptive childs can be dropped during measure phase
                return;
                }
            }

#ifdef _XN3_DEBUG_
        CXnProperty* id( aNode.IdL() );

        if ( id )
            {
            TBuf8< 256 > debug;
            debug.Append( _L8( "Node id: " ) );
            debug.Append( id->StringValue() );
            debug.Append( _L8( " will be set to dropped" ) );
            RDebug::RawPrint( debug );
            }
#endif

        // Node will be dropped
        aNode.SetDropped( aDropped );

        // Get ride of its rects
        ClearRects( aNode );

        if ( parentAdaptive )
            {
            // Is there adaptive content left after dropping?
            CheckAdaptiveContentL( *parent, parent->Rect().Size() );
            }
        }
    else
        {
        // Node is no more dropped
        aNode.SetDropped( aDropped );
        }
    }

// -----------------------------------------------------------------------------
// CheckAdaptiveContentL()
// Checks if node is adaptive, but doesn't have any content
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::CheckAdaptiveContentL(
    CXnNode& aNode, TSize aAvailableSize ) const
    {
    TInt adaptive( aNode.IsAdaptive( ETrue ) );
    TBool isEmpty( EFalse );

    if ( adaptive )
        {
        if ( aAvailableSize == TSize::EUninitialized )
            {
            // Use max available size
            aAvailableSize = ScreenDeviceSize();
            }

        const TDesC8& name( aNode.Type()->Type() );

        if ( name == KBoxNodeName || name == KButtonNodeName )
            {
            RPointerArray< CXnNode > displayedChildren;
            CleanupClosePushL( displayedChildren );

            TInt displayedCount( DisplayedChildrenCountL(
                aNode, &displayedChildren ) );

            if ( displayedCount == 0 )
                {
                isEmpty = ETrue;
                }
            else
                {
                for ( TInt i = 0; i < displayedChildren.Count(); i++ )
                    {
                    CheckAdaptiveContentL( *displayedChildren[i],
                        aAvailableSize );
                    }

                if ( DisplayedChildrenCountL( aNode ) == 0 )
                    {
                    isEmpty = ETrue;
                    }
                }

            CleanupStack::PopAndDestroy( &displayedChildren );
            }

        if ( isEmpty )
            {
            // No content available, drop adaptive node
            SetNodeDroppedL( aNode, XnNodeLayout::EDropped );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AddFocusCandidateL
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::AddFocusCandidateL( CXnNode* aNode )
    {
    if ( !aNode )
        {
        return;
        }
    TInt index( iFocusCandidateList.Find( aNode ) );
    if ( index == KErrNotFound )
        {
        iFocusCandidateList.AppendL( aNode );
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::IsLayoutDisabled
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::IsLayoutDisabled()
    {
    if ( !iCurrentView )
        {
        return ETrue;
        }
    
    TBool retval( EFalse );
    
    if ( !( iLayoutControl & XnLayoutControl::EIgnoreState ) )
        {
        if ( iDisableCount > 0 )
            {
            retval = ETrue;
            }
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// AddToRedrawListL
// Mark Control's appearance to be redrawn
// -----------------------------------------------------------------------------    
//
void CXnUiEngineImpl::AddToRedrawListL( CXnNode* aNode, TRect aRect )
    {   
    CXnControlAdapter* aAdapter( aNode->Control() );
           
    if( !aAdapter ) 
        {
        return;
        }

    if( aRect != TRect::EUninitialized )
        {
        // This is the new rect which will be set by layout algo
        AddRedrawRectL( aRect, *aNode );
        }
    
    TRect rect( aAdapter->Rect() );
            
    if( rect == TRect::EUninitialized )
        {
        // Don't add uninitialized rect
        return;
        }

    /*
    if( aNode->IsStateSet( XnPropertyNames::style::common::KFocus ) )
        {
        const TDesC8& name( aNode->DomNode()->Name() );

        if( name == KPlugin )
            {
            rect.Grow( KFocusGrowValue, KFocusGrowValue );
            }
        else
            {
            TRect marginRect( aNode->MarginRect() );
            
            CXnNode* parent( aNode->Parent() );
            
            for( ; parent; parent = parent->Parent() )
                {
                if( parent->DomNode()->Name() == KPlugin )
                    {
                    if( parent->Rect() == marginRect )
                        {
                        rect.Grow( KFocusGrowValue, KFocusGrowValue );
                        }
                    
                    break;
                    } 
                }                
            }
        }
        */
    GrowIfNeeded(aNode, rect);    
    AddRedrawRectL( rect, *aNode );                          
    }

// -----------------------------------------------------------------------------
// AddToDirtyListL
// Mark node to dirty list for relayout
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::AddToDirtyListL( CXnNode* aNode )
    {
    
    if ( aNode )
        {
        CXnNode* nodeToRedrawList( aNode );
        CXnNode* nodeToDirtyList( aNode );
    
        if ( aNode == RootNode() )
            {
            // Force relayout and redraw from current view
            iDirtyList.Reset();
            if ( !iCurrentView )
                {
                iCurrentView = iViewManager.ViewNode();
                }
            iDirtyList.AppendL( iCurrentView );
            TXnDirtyRegion* dirtyRegion = FindDirtyRegionL( *iCurrentView );
            if( dirtyRegion )
                {
                dirtyRegion->iRegion.Clear();        
                }
            iLayoutControl |= XnLayoutControl::EViewDirty;
            nodeToDirtyList = nodeToRedrawList = iCurrentView;
            nodeToDirtyList->ClearRenderedAndLaidOut();
    
            // Add to draw list for redraw
            AddToRedrawListL( nodeToRedrawList );
            }
        else
            {
            if ( !IsAbsoluteL( *aNode ) && !IsNodeTooltip( *aNode ) )
                {
                // Check adaptives in normal flow
                CXnNode* oldest( NULL );
                CXnNode* adaptive( aNode );
    
                if ( !aNode->IsAdaptive( ETrue ) )
                    {
                    adaptive = aNode->Parent();
                    }
    
                for ( ; adaptive && adaptive->IsAdaptive( ETrue );
                    adaptive = adaptive->Parent() )
                    {
                    oldest = adaptive;
                    }
    
                // Now we have found the oldest adaptive node if present
                if ( oldest )
                    {
                    nodeToRedrawList = nodeToDirtyList = adaptive;
                    }
                }
    
            RPointerArray< CXnNode > dirtyList;
            CleanupClosePushL( dirtyList );
            TInt count( iDirtyList.Count() );
            TBool found;
    
            // first, check that aNode's children are not in the dirty array
            for ( TInt i = 0; i < count; ++i )
                {
                found = EFalse;
                CXnNode* candidate( iDirtyList[i] );
    
                for ( CXnNode* node = candidate->Parent(); node && !found;
                    node = node->Parent() )
                    {
                    if ( nodeToDirtyList == node )
                        {
                        found = ETrue;
                        }
                    }
    
                if ( !found )
                    {
                    // Put candidate back to list as child is not found
                    dirtyList.AppendL( candidate );
                    }
                }
    
            found = EFalse;
    
            // second, check that aNode's parent is not in dirty array
            for ( TInt i = 0; i < count && !found; ++i )
                {
                CXnNode* candidate( iDirtyList[i] );
    
                for ( CXnNode* node = nodeToDirtyList; node && !found;
                    node = node->Parent() )
                    {
                    if ( node == candidate )
                        {
                        found = ETrue;
                        }
                    }
                }
    
            if ( !found && iDirtyList.Find( nodeToDirtyList ) == KErrNotFound )
                {
                // Add node to dirty list as parent is neither found
                dirtyList.AppendL( nodeToDirtyList );
                nodeToDirtyList->ClearRenderedAndLaidOut();
                
                // Add to draw list for redraw
                AddToRedrawListL( nodeToRedrawList );
                }
    
            // finally update the dirty list
            iDirtyList.Reset();
            iDirtyList = dirtyList;
    
            CleanupStack::Pop( &dirtyList );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::SetClientRectL
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::SetClientRectL( TRect aRect, TBool aDrawNow )
    {
    if ( iClientRect != aRect )
        {
        iClientRect = aRect;
        
        UpdateInternalUnits( iHorizontalUnitInPixels, iVerticalUnitInPixels,
            iClientRect );
        
        iEditMode->SetClientRect( aRect );
		
        CXnViewSwitcher* viewSwitcher( iAppUiAdapter.ViewSwitcher() );
        if( viewSwitcher )
            {
            viewSwitcher->SizeChanged( aRect );
            }
    
        if ( aDrawNow )
            {
            RootNode()->SetDirtyL();
            iUiEngine->RenderUIL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ClientRect
// -----------------------------------------------------------------------------
//
TRect CXnUiEngineImpl::ClientRect() const
    {
    return iClientRect;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::Editor
// -----------------------------------------------------------------------------
//
CXnEditor* CXnUiEngineImpl::Editor()const
    {
    return &iViewManager.Editor();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::EditMode
// -----------------------------------------------------------------------------
//
CXnEditMode* CXnUiEngineImpl::EditMode()
    {
    return iEditMode;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::StartLayoutFromL
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngineImpl::StartLayoutFromL()
    {
    if ( iLayoutControl & XnLayoutControl::EViewDirty )
        {
        return iCurrentView;
        }

    RPointerArray< CXnNode >parentArray;
    CleanupClosePushL( parentArray );
    TInt dirtyCount = iDirtyList.Count();
    CXnNode* startNode( NULL );

    for ( TInt dirtyIndex = 0; dirtyIndex < dirtyCount; dirtyIndex++ )
        {
        startNode = iDirtyList[dirtyIndex];

        for ( ;startNode && startNode != iCurrentView; )
            {
            parentArray.Append( startNode->Parent() );
            startNode = startNode->Parent();
            }
        }

    TInt parentCount = parentArray.Count();

    for ( TInt parentIndex = 0; parentIndex < parentCount; parentIndex++ )
        {
        TInt parentsFound = 0;

        for ( TInt checkIndex = 0; checkIndex < parentCount; checkIndex++ )
            {
            if ( parentArray[parentIndex] == parentArray[checkIndex] )
                {
                parentsFound++;

                if ( parentsFound == dirtyCount )
                    {
                    startNode = parentArray[parentIndex];
                    CleanupStack::PopAndDestroy( &parentArray );
                    return startNode;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( &parentArray );

    return iCurrentView;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::Plugins
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >* CXnUiEngineImpl::Plugins()
    {
    return &iViewManager.PluginNodes();
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::ForceRenderUIL
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::ForceRenderUIL( TBool aLayoutOnly )
    {
    iLayoutControl |= XnLayoutControl::EIgnoreState;
    if ( aLayoutOnly )
        {
        LayoutUIL();
        }
    else
        {
        LayoutUIL();
        RenderUIL();
        }
    iLayoutControl &= ~XnLayoutControl::EIgnoreState;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::GetThemeResource
// -----------------------------------------------------------------------------
//
TInt CXnUiEngineImpl::GetThemeResource( const TDesC& aPath, RFile& aFile )
    {
    TInt err = KErrNone;

    // parse file name and extension
    TParsePtrC fileParser( aPath );
    TPtrC filepath = fileParser.DriveAndPath();

    RFs session ( CCoeEnv::Static()->FsSession() );

    // open resource file
    session.SetSessionPath( filepath );
    session.ShareProtected();
    err = aFile.Open( session, aPath, EFileShareReadersOnly );
    if ( err != KErrNone )
        {
        aFile.Close();
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::AnalyseAddedWidgetL
// -----------------------------------------------------------------------------
//
TBool CXnUiEngineImpl::AnalyseAddedWidgetL( CXnNode& aNode )
    {
    TBool retval( ETrue );

    // Force layout to analyse added widget's dimensions
    ForceRenderUIL( ETrue );

    if ( aNode.IsAdaptive( ETrue ) )
        {
        // If widget place holder (plugin in view.xml) is adaptive, then
        // we need to verify that added widget will fit to available space
        retval = EFalse;

        CXnNode* parent( aNode.Parent() );
        RPointerArray< CXnNode >& children( parent->Children() );
        CXnProperty* prop( parent->BlockProgressionL() );

        const TDesC8* bp(
            &XnPropertyNames::style::common::block_progression::KTB() );

        if ( prop )
            {
            bp = &prop->StringValue();
            }

        prop = parent->DirectionL();

        const TDesC8* dir(
            &XnPropertyNames::style::common::direction::KLTR() );

        if ( prop )
            {
            // use default
            dir = &prop->StringValue();
            }

        TSize space( CalculateSpaceUsedByChildrenL( children, *parent,
            *bp, *dir, *iCurrentGraphicsDevice, iHorizontalUnitInPixels,
            iVerticalUnitInPixels ) );

        TSize parentSize( parent->Rect().Size() );

        if ( *bp == XnPropertyNames::style::common::block_progression::KTB ||
             *bp == XnPropertyNames::style::common::block_progression::KBT )
            {
            if ( parentSize.iHeight >= space.iHeight )
                {
                retval = ETrue;
                }
            }
        else // LR / RL
            {
            if ( parentSize.iWidth >= space.iWidth )
                {
                retval = ETrue;
                }
            }
        }

    return retval;
    }



// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyWidgetRemovalL( )
// Notify about widget removal
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::NotifyWidgetRemovalL(
    const CXnPluginData& /*aPluginData*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyConfigureWidgetL( )
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::NotifyConfigureWidgetL( 
    const CHsContentInfo& /*aContentInfo*/, CXnPluginData& /*aPluginData*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::NotifyViewDeactivatedL
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::NotifyViewDeactivatedL( 
    const CXnViewData& /*aViewData*/ )
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//               
void CXnUiEngineImpl::DisableRenderUiLC()
    {         
    CleanupStack::PushL( TCleanupItem( EnableRenderUi, this ) );
    
    iDisableCount++;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//               
EXPORT_C /* static */ void CXnUiEngineImpl::EnableRenderUi( TAny* aAny )
    {
    CXnUiEngineImpl* self = static_cast< CXnUiEngineImpl* >( aAny );
    
    if ( self->iDisableCount > 0 )
        {
        self->iDisableCount--;
        
        if ( self->iDisableCount == 0 )
            {
            TRAP_IGNORE(                    
            // Is menu refresh pending?
            if ( ( self->iLayoutControl & XnLayoutControl::ERefreshMenu ) &&
                   !self->IsMenuDisplaying() )
                {
                // RefreshMenuL will reset the state flag
                self->RefreshMenuL();
                }

            // Is layout pending?
            if ( self->iLayoutControl & XnLayoutControl::ELayoutUI )
                {
                self->LayoutUIL();
                }

            // Is render pending?
            if ( self->iLayoutControl & XnLayoutControl::ERenderUI )
                {
                self->RenderUIL();
                }
                );
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnUiEngineImpl::ReportScreenDeviceChangeL()
    {    
    // Notify current orientation to all views    
    RPointerArray< CXnPluginData >& plugins( 
        iViewManager.ActiveAppData().PluginData() );

    for( TInt i = 0; i < plugins.Count(); i++ )
        {        
        ReportScreenDeviceChangedL( *plugins[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::HandlePartialTouchInputL()
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::HandlePartialTouchInputL( CXnNode* aNode, TBool aEnable )
    {    
    CXnViewData& view( iViewManager.ActiveViewData() );

    CXnNode* editor( aEnable ? aNode : iSplitScreenState.iPartialScreenEditorNode ); 
		      
    CXnPluginData* plugin( view.Plugin( editor ) );
            
    if ( !plugin || !editor )
        {
        return;
        }
		
    CXnNode* editorplugin( plugin->Owner()->LayoutNode() );    
    
    DisableRenderUiLC();

    if ( aEnable )    
        {        
        iSplitScreenState.iPartialScreenOpen = ETrue;           
        iSplitScreenState.iPartialScreenEditorNode = editor;           

        // make sure that we always get up event
        CXnViewControlAdapter* control = 
            static_cast< CXnViewControlAdapter* >( view.ViewNode()->Control() );  
                                           
        control->ResetGrabbing();  
         
        // Block progression must be bottom-to-top when partial screen is open
        // Previous value needs to be stored first
        CXnProperty* prop( 
                editorplugin->Parent()->GetPropertyL( 
                        XnPropertyNames::style::common::KBlockProgression ) );                
        if ( prop )
            {
            iSplitScreenState.iPartialScreenBlock = &prop->StringValue();
            }
        else
            {
            iSplitScreenState.iPartialScreenBlock = 
                    &XnPropertyNames::style::common::block_progression::KTB();
            }

        SetPartialScreenBlockProgressionL( 
                editorplugin->Parent(), 
                XnPropertyNames::style::common::block_progression::KBT );

        // Hide all plugins except the one that contains given editor node
        RPointerArray< CXnNode >& plugins( *Plugins() );                   
        for( TInt i=0; i<plugins.Count(); i++ )
             {         
             CXnNode* pluginNode = plugins[i];
             if ( pluginNode != editorplugin )
                {
                SetNodeVisibleL(pluginNode, EFalse);
                }      
             }
        
        // Hide statuspane
        iAppUiAdapter.StatusPane()->MakeVisible( EFalse );
        } 
     
    else
        { 
        // Show plugin nodes again
        RPointerArray< CXnNode >& plugins( *Plugins() );

        for( TInt i=0; i<plugins.Count(); i++ )
           {           
           CXnNode* pluginNode = plugins[i];
           
           if ( pluginNode != editorplugin )
                {
                SetNodeVisibleL(pluginNode, ETrue);
                }
           }

        SetPartialScreenBlockProgressionL( 
                editorplugin->Parent(),
                *iSplitScreenState.iPartialScreenBlock );
        
        iSplitScreenState.iPartialScreenBlock = NULL;
        iSplitScreenState.iPartialScreenEditorNode = NULL;
        iSplitScreenState.iPartialScreenOpen = EFalse;
                 
        // Show statuspane again
        iAppUiAdapter.StatusPane()->MakeVisible(ETrue);
        }
    
    RootNode()->SetDirtyL();
    ForceRenderUIL();
    CleanupStack::PopAndDestroy();
    
    if ( aEnable )
        {    
        editor->PluginIfL().ReportTriggerEventL( 
            KSplitScreenEnabledTrigger, KNullDesC8, KNullDesC8 );             
        }
    else
        {
        editor->PluginIfL().ReportTriggerEventL( 
            KSplitScreenDisabledTrigger, KNullDesC8, KNullDesC8 );                                  
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CCoeControl* CXnUiEngineImpl::WindowOwningControl( CXnNode& aNode )
    {
    CXnNode* parent = &aNode;
    while( parent )
        {
        CXnControlAdapter* adapter = parent->Control();
        if( adapter )
            {
            if( parent->ViewNodeImpl() && 
                adapter == iCurrentViewControlAdapter )
                {
                return adapter;
                }
            else if( adapter->OwnsWindow() )
                {
                if( !IsNodeTooltip( *parent ) )
                    {
                    return adapter;
                    }
                else
                    {
                    return NULL;
                    }                
                }
            }
        parent = parent->Parent();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TXnDirtyRegion* CXnUiEngineImpl::FindDirtyRegionL( CXnNode& aNode )
    {
    CCoeControl* control = WindowOwningControl( aNode );
    if( !control )
        {
        return NULL;
        }
    for( TInt i=0; i<iRedrawRegions.Count(); i++ )
        {
        if( iRedrawRegions[i]->iControl == control )
            {
            return iRedrawRegions[i];
            }
        }
    TXnDirtyRegion* region = new (ELeave) TXnDirtyRegion;
    CleanupStack::PushL( region );
    region->iControl = control;
    iRedrawRegions.AppendL( region );
    CleanupStack::Pop();
    return region;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::AddRedrawRectL( TRect aRect, CXnNode& aNode )
    {
    TXnDirtyRegion* dirtyReg = FindDirtyRegionL( aNode );
    if( dirtyReg )
        {
        dirtyReg->iRegion.AddRect( aRect );        
        dirtyReg->iRegion.Tidy();
        }
    }

// -----------------------------------------------------------------------------
// SetEventDispatcher
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::SetEventDispatcher( CXnKeyEventDispatcher* aDispatcher )
    {
    iKeyEventDispatcher = aDispatcher;
    }

// -----------------------------------------------------------------------------
// NotifyStatusPaneSizeChanged
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::NotifyStatusPaneSizeChanged()
    {
    TRAP_IGNORE( iUiEngine->SetClientRectL( iAppUiAdapter.ClientRect(), EFalse ) );
    }
    
// -----------------------------------------------------------------------------
// NotifyResourceChanged
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::NotifyResourceChanged( TInt aType )
    {
    TRAP_IGNORE( HandleResourceChangeL( aType ) );
    }

// -----------------------------------------------------------------------------
// EnablePartialTouchInput 
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::EnablePartialTouchInput( CXnNode* aNode, TBool aEnable )
    {
    if( aEnable && !iSplitScreenState.iPartialScreenOpen ||
        !aEnable && iSplitScreenState.iPartialScreenOpen )
        {
        TRAP_IGNORE( HandlePartialTouchInputL( aNode, aEnable ) );
        }
    }

// -----------------------------------------------------------------------------
// SetNodeVisibleL
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::SetNodeVisibleL( CXnNode* aNode , TBool aVisible )
    {    
    CXnDomStringPool* sp( iUiEngine->ODT()->DomDocument().StringPool()); 
    
    if(!aVisible)
        {
        CXnProperty* display = CXnProperty::NewL(
        XnPropertyNames::style::common::KDisplay,
        XnPropertyNames::style::common::display::KNone,
        CXnDomPropertyValue::EString, *sp );
        
        CleanupStack::PushL( display );         
        aNode->SetPropertyL(display);             
        CleanupStack::Pop( display );
        }
    else
        {
         CXnProperty* visible = CXnProperty::NewL(
         XnPropertyNames::style::common::KDisplay,
         XnPropertyNames::style::common::display::KBlock,
         CXnDomPropertyValue::EString, *sp );
         
         CleanupStack::PushL( visible );                
         aNode->SetPropertyL(visible);                
         CleanupStack::Pop( visible );
        }
    }

// -----------------------------------------------------------------------------
// SetPartialScreenBlockProgressionL
// -----------------------------------------------------------------------------
void CXnUiEngineImpl::SetPartialScreenBlockProgressionL( 
        CXnNode* aParent, const TDesC8& aBlockProgression )
    {
    CXnDomStringPool* sp( iUiEngine->ODT()->DomDocument().StringPool());
    if( aParent && sp )
        {        
        CXnProperty* block_progression = CXnProperty::NewL(
                XnPropertyNames::style::common::KBlockProgression,
                aBlockProgression,
                CXnDomPropertyValue::EString, *sp );
        if ( block_progression )
            {
            CleanupStack::PushL( block_progression );            
            aParent->SetPropertyL(block_progression);            
            CleanupStack::Pop( block_progression );
            }
        }
    }

// -----------------------------------------------------------------------------
// isPartialInputActive
// -----------------------------------------------------------------------------
TBool CXnUiEngineImpl::IsPartialInputActive()
    {
    return iSplitScreenState.iPartialScreenOpen;
    }

// -----------------------------------------------------------------------------
// CXnUiEngineImpl::IsTextEditorActive()
// -----------------------------------------------------------------------------
//               
TBool CXnUiEngineImpl::IsTextEditorActive()
    {
    if( iKeyEventDispatcher )         
        {
        return iKeyEventDispatcher->IsTextEditorActive();         
        }
    
    return EFalse;
    }

// End of file
