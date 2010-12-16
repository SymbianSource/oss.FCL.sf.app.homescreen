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
* Description:  Implementation part of bridge pattern in CXnNode
*
*/

// System includes
#include <AknUtils.h>
#include <utf.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// User includes
#include "xnnodeimpl.h"
#include "xnnode.h"
#include "xnpropertylist.h"
#include "xnproperty.h"
#include "xnuiengine.h"
#include "xnappuiadapter.h"
#include "xncontroladapter.h"
#include "xnpopupcontroladapter.h"
#include "xnmenuadapter.h"
#include "xndomlist.h"
#include "xndomdocument.h"
#include "xnpointerarray.h"
#include "xnodt.h"
#include "xntype.h"
#include "xndomnode.h"
#include "xndomattribute.h"
#include "xndompropertyvalue.h"
#include "xnpopup.h"
#include "c_xnutils.h"
#include "xneditmode.h"
#include "xneditor.h"
#include "xntext.h"
#include "xnscrollablecontroladapter.h"
#include "xnviewdata.h"
#include "xnrootdata.h"
#include "xnpanic.h"
#include "xnlistquerydialogadapter.h"
#include "xneffectmanager.h"
#include "xnviewadapter.h"
#include "xnbackgroundmanager.h"
#include "xntexteditor.h"
#include "xnitemactivator.h"

// Local constants
_LIT8( KId, "id" );
_LIT8( KName, "name" );
_LIT8( KToolTip, "tooltip" );
_LIT8( KMenuBar, "menubar" );
_LIT8( KTextEditor, "texteditor" );
_LIT8( KPlugin, "plugin" );
_LIT8( KTemplate, "template" );
_LIT8( KIndex, "index" );

_LIT8( KActionsHandler, "actionshandler" );

const TInt KUnitMaxLen = 2;
const TInt KOneView = 1;

_LIT8( KPseudoClassFocus, "focus" );
_LIT8( KPseudoClassPressedDown, "presseddown" );
_LIT8( KPseudoClassPassiveFocus, "passivefocus" );
_LIT8( KPseudoClassHold, "hold" );
_LIT8( KPseudoClassHover, "hover" );
_LIT8( KPseudoClassLink, "link" );
_LIT8( KPseudoClassVisited, "visited" );
_LIT8( KPseudoClassActive, "active" );
_LIT8( KPseudoClassEdit, "edit" );

_LIT8( KCompound, "compound" );
_LIT8( KTrue, "true" );
_LIT8( KDisplay, "display" );
_LIT8( KBlock, "block" );
_LIT8( KNone, "none" );
_LIT8( KGainEnd, "gainend" );
_LIT8( KLoseEnd, "loseend" );

_LIT8( KWidgetExtensionNodeName, "widgetextension" );
_LIT8( KPopUpNodeName, "popup" );

struct CLayoutPropertyCache : public CBase
    {
    CLayoutPropertyCache() :
        iWidths( 1 ),
        iHeights( 1 ),
        iMarginLefts( 1 ),
        iMarginRights( 1 ),
        iBorderLefts( 1 ),
        iBorderRights( 1 ),
        iPaddingLefts( 1 ),
        iPaddingRights( 1 ),
        iMarginTops( 1 ),
        iMarginBottoms( 1 ),
        iBorderTops( 1 ),
        iBorderBottoms( 1 ),
        iPaddingTops( 1 ),
        iPaddingBottoms( 1 ),
        iBorderWidths( 1 ),
        iBlockProgressions( 1 ),
        iDirections( 1 ),
        iPositions( 1 ),
        iMaxHeights( 1 ),
        iMinHeights( 1 ),
        iMaxWidths( 1 ),
        iMinWidths( 1 ),
        iDisplays( 2 ),
        iVisibilities( 1 ),
        iLefts( 1 ),
        iRights( 1 ),
        iTops( 1 ),
        iBottoms( 1 ),
        iBorderLeftStyles( 1 ),
        iBorderRightStyles( 1 ),
        iBorderTopStyles( 1 ),
        iBorderBottomStyles( 1 ),
        iBorderStyles( 1 ),
        iDisplayPriorities( 1 ),
        iNavIndexes( 1 ),
        iBackgroundColors( 1 ),
        iBackgroundImages( 1 ),
        iFocusBackgrounds( 2 )
            {
            }

    ~CLayoutPropertyCache()
        {
        iWidths.Reset();
        iHeights.Reset();
        iMarginLefts.Reset();
        iMarginRights.Reset();
        iBorderLefts.Reset();
        iBorderRights.Reset();
        iPaddingLefts.Reset();
        iPaddingRights.Reset();
        iMarginTops.Reset();
        iMarginBottoms.Reset();
        iBorderTops.Reset();
        iBorderBottoms.Reset();
        iPaddingTops.Reset();
        iPaddingBottoms.Reset();
        iBorderWidths.Reset();
        iBlockProgressions.Reset();
        iDirections.Reset();
        iPositions.Reset();
        iMaxHeights.Reset();
        iMinHeights.Reset();
        iMaxWidths.Reset();
        iMinWidths.Reset();
        iDisplays.Reset();
        iVisibilities.Reset();
        iLefts.Reset();
        iRights.Reset();
        iTops.Reset();
        iBottoms.Reset();
        iBorderLeftStyles.Reset();
        iBorderRightStyles.Reset();
        iBorderTopStyles.Reset();
        iBorderBottomStyles.Reset();
        iBorderStyles.Reset();
        iDisplayPriorities.Reset();
        iNavIndexes.Reset();
        iBackgroundColors.Reset();
        iZIndexes.Reset();
        iBackgroundImages.Reset();
        iFocusBackgrounds.Reset();
        }

    CXnProperty* iWidth;
    RPointerArray< CXnProperty > iWidths;
    CXnProperty* iHeight;
    RPointerArray< CXnProperty > iHeights;
    CXnProperty* iMarginLeft;
    RPointerArray< CXnProperty > iMarginLefts;
    CXnProperty* iMarginRight;
    RPointerArray< CXnProperty > iMarginRights;
    CXnProperty* iBorderLeft;
    RPointerArray< CXnProperty > iBorderLefts;
    CXnProperty* iBorderRight;
    RPointerArray< CXnProperty > iBorderRights;
    CXnProperty* iPaddingLeft;
    RPointerArray< CXnProperty > iPaddingLefts;
    CXnProperty* iPaddingRight;
    RPointerArray< CXnProperty > iPaddingRights;
    CXnProperty* iMarginTop;
    RPointerArray< CXnProperty > iMarginTops;
    CXnProperty* iMarginBottom;
    RPointerArray< CXnProperty > iMarginBottoms;
    CXnProperty* iBorderTop;
    RPointerArray< CXnProperty > iBorderTops;
    CXnProperty* iBorderBottom;
    RPointerArray< CXnProperty > iBorderBottoms;
    CXnProperty* iPaddingTop;
    RPointerArray< CXnProperty > iPaddingTops;
    CXnProperty* iPaddingBottom;
    RPointerArray< CXnProperty > iPaddingBottoms;
    CXnProperty* iBorderWidth;
    RPointerArray< CXnProperty > iBorderWidths;
    CXnProperty* iBlockProgression;
    RPointerArray< CXnProperty > iBlockProgressions;
    CXnProperty* iDirection;
    RPointerArray< CXnProperty > iDirections;
    CXnProperty* iPosition;
    RPointerArray< CXnProperty > iPositions;
    CXnProperty* iMaxHeight;
    RPointerArray< CXnProperty > iMaxHeights;
    CXnProperty* iMinHeight;
    RPointerArray< CXnProperty > iMinHeights;
    CXnProperty* iMaxWidth;
    RPointerArray< CXnProperty > iMaxWidths;
    CXnProperty* iMinWidth;
    RPointerArray< CXnProperty > iMinWidths;
    CXnProperty* iDisplay;
    RPointerArray< CXnProperty > iDisplays;
    CXnProperty* iVisibility;
    RPointerArray< CXnProperty > iVisibilities;
    CXnProperty* iLeft;
    RPointerArray< CXnProperty > iLefts;
    CXnProperty* iRight;
    RPointerArray< CXnProperty > iRights;
    CXnProperty* iTop;
    RPointerArray< CXnProperty > iTops;
    CXnProperty* iBottom;
    RPointerArray< CXnProperty > iBottoms;
    CXnProperty* iBorderLeftStyle;
    RPointerArray< CXnProperty > iBorderLeftStyles;
    CXnProperty* iBorderRightStyle;
    RPointerArray< CXnProperty > iBorderRightStyles;
    CXnProperty* iBorderTopStyle;
    RPointerArray< CXnProperty > iBorderTopStyles;
    CXnProperty* iBorderBottomStyle;
    RPointerArray< CXnProperty > iBorderBottomStyles;
    CXnProperty* iBorderStyle;
    RPointerArray< CXnProperty > iBorderStyles;
    CXnProperty* iBorderImage;
    CXnProperty* iDisplayPriority;
    RPointerArray< CXnProperty > iDisplayPriorities;
    CXnProperty* iNavIndex;
    RPointerArray< CXnProperty > iNavIndexes;
    CXnProperty* iZIndex;
    RPointerArray< CXnProperty > iZIndexes;
    CXnProperty* iBackgroundColor;
    RPointerArray< CXnProperty > iBackgroundColors;
    CXnProperty* iBackgroundImage;
    RPointerArray< CXnProperty > iBackgroundImages;
    CXnProperty* iFocusBackground;
    RPointerArray< CXnProperty > iFocusBackgrounds;
    };

// Local function prototypes
static TBool IsTriggerRunnableL( CXnDomNode& aTriggerNode );
static TBool DoMatchTriggerForKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    CXnDomNode& aTriggerNode );
static void GetFocusCandidatesL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnDomNode& aEventNode,
    RPointerArray< CXnNode >& aFocusCandidates );
static void RunAppUiNotificationL( CXnUiEngine& aEngine, CXnNode& aNode,
    CXnDomNode& aEventNode, CXnDomNode& aTriggerNode, CXnNode& aEventData );
static void RunFullScreenEffectL(
    CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunActivateNextViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunActivatePreviousViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunAddViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunRemoveViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunActivateL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunActivateEditorL( CXnNodeImpl* aThis, CXnNode& aLayoutNode,
    CXnUiEngine& aEngine, CXnDomNode& aEventNode, TBool aActivate );
static void RunDeactivateL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunSystemSetPCDataL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, const TDesC8& aId,
    const TDesC8& aData );
static void GetNumberValues( const TDesC8& aData, TReal& aNumber,
    CXnDomPropertyValue::TPrimitiveValueType& aValueType );
static void RunSystemSetNumberL( CXnUiEngine& aEngine, CXnNode* aNode,
    const TDesC8& aName, const RPointerArray< HBufC8 >& aValues );
static void RunSystemSetRGBL( CXnUiEngine& aEngine, CXnNode* aNode,
    const TDesC8& aName, const RPointerArray< HBufC8 >& aValues );
static void RunSystemSetStringL(
    CXnUiEngine& aEngine, CXnNode* aNode, const TDesC8& aName,
    const RPointerArray< HBufC8 >& aValues,
    const CXnDomPropertyValue::TPrimitiveValueType aValueType );
static void GetSystemSetDataL( CXnDomNode& aDomNode, HBufC8*& aId,
    const TDesC8** aPseudoClass = NULL, const TDesC8** aName = NULL,
    RPointerArray< HBufC8 >* aValues = NULL, const TDesC8** aType = NULL,
    TBool aSettings = EFalse, const TDesC8& aProvidedId = KNullDesC8,
    const TDesC8** aClassId = NULL );
static void RunSystemSetL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnDomNode& aEventNode,
    TBool aSettings = EFalse, const TDesC8& aId = KNullDesC8 );
static void RunTryDisplayingMenuL(
    CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunTryDisplayingStylusPopupL(
    CXnNodeImpl* aThis, CXnNode& aNode, CXnUiEngine& aEngine,
    CXnDomNode& aEventNode );
static void RunTryDisplayingListQueryDialogL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnDomNode& aEventNode );
static void RunSetInitialFocusL( CXnUiEngine& aEngine );
static TBool RunEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    CXnDomNode& aEventNode, CXnDomNode& aTriggerNode, CXnNode& aEventData );
static void RunEventsL( CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    CXnDomNode& aActionNode, CXnDomNode& aTriggerNode, CXnNode& aEventData );
static TBool DoMatchTriggerForHighLevelKeyEventL( CXnNode& aNode,
    const TKeyEvent& aKeyEvent, TEventCode aType, const TDesC8& highLevelKey );
static TBool MatchTriggerForKeyEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    const TKeyEvent& aKeyEvent, TEventCode aType,
    CXnDomNode& aActionNode, CXnDomNode& aTriggerNode );
static TBool MatchTriggerForEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    CXnNode& aEventData, CXnDomNode& aActionNode, CXnDomNode& aTriggerNode,
    TInt aSource );
static TBool MatchActionForKeyEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    const TKeyEvent& aKeyEvent, TEventCode aType, CXnDomNode& aActionNode );
static TBool IsTriggerActiveL( CXnDomNode& aNode );
static TBool MatchActionForEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    CXnNode& aEventData, CXnDomNode& aActionNode, TInt aSource );
static CXnNode* FindLoopedFocusableNodeL( CXnNode& aNode, TBool aForward );
static CXnNode* FindNextFocusableNodeByNavIndexL( CXnNode& aNode, TBool aForward );
static TBool FindNextFocusableNodeL( CXnNode& aNode, TBool aForward );
static CXnNode* FindNextNodeFromRightL(
    RPointerArray< CXnNode >& aArray, CXnNode& aNode, TBool stayInNamespace = EFalse, CXnUiEngine* aEngine = NULL );
static CXnNode* FindNextNodeFromLeftL(
    RPointerArray< CXnNode >& aArray, CXnNode& aNode, TBool stayInNamespace = EFalse, CXnUiEngine* aEngine = NULL );
static CXnNode* FindNextNodeFromBelowL(
    RPointerArray< CXnNode >& aArray, CXnNode& aNode, TBool stayInNamespace = EFalse );
static CXnNode* FindNextNodeFromAboveL(
    RPointerArray< CXnNode >& aArray, CXnNode& aNode, TBool stayInNamespace = EFalse );
static CXnNode* FindPluginNode( CXnNode& aNode );
static TBool DoInternalFocusChangeL(
    CXnUiEngine& aEngine, CXnNode& aNode, const TKeyEvent& aKeyEvent,
    TEventCode aType );
static CXnNode* WidgetExtensionParent( CXnNode* aNode );
static TBool DoTriggerKeyEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    const TKeyEvent& aKeyEvent, TEventCode aType );
static void DoTriggerEventL(
    CXnNodeImpl* aThis, CXnUiEngine& aEngine, CXnNode& aNode,
    CXnNode& aActionsParent, CXnNode& aEventData, TInt aSource );
static TBool DoReceiveFocusL( CXnNode& aNode, CXnUiEngine& aEngine );
static void InformPropertyChangeL( CXnNode& aNode, CXnProperty* aProperty = NULL );
static CXnNode* BuildEventTypeNodeL( TEventCode aType, CXnUiEngine& aUiEngine );
static CXnNode* BuildModifiersNodeL(
    const TKeyEvent& aKeyEvent, CXnUiEngine& aUiEngine );
static CXnNode* BuildKeyCodeNodeL(
    const TKeyEvent& aKeyEvent, CXnUiEngine& aUiEngine );
static CXnNode* BuildScanCodeNodeL(
    const TKeyEvent& aKeyEvent, CXnUiEngine& aUiEngine );
static CXnNode* BuildRepeatsNodeL(
    const TKeyEvent& aKeyEvent, CXnUiEngine& aUiEngine );
static void BuildPropertyNodesL( CXnNode& aNode, const TKeyEvent& aKeyEvent,
    TEventCode aType, CXnUiEngine& aUiEngine );
static CXnNode* BuildKeyTriggerNodeL( const TKeyEvent& aKeyEvent, TEventCode aType,
    CXnUiEngine& aUiEngine );
static CXnDomPropertyValue* GetNavIndexL( CXnNode& aNode );
static CXnNode* BackwardL( CXnNode& aNode );
static CXnNode* ForwardL( CXnNode& aNode );
static CXnNode* BackwardLoopL( CXnNode& aNode );
static CXnNode* ForwardLoopL( CXnNode& aNode );
static TInt CompareNavIndexesL(
    CXnDomPropertyValue* aCurrentPropertyValue,
    CXnDomPropertyValue* aNextPropertyValue );
static void ReplaceOrAppend(
    CXnProperty* aProperty, RPointerArray< CXnProperty >& aArray,
    TBool aAppendOnly );
static TBool DisplayValueMatchL( CXnNode& aNode, const TDesC8& aDisplayValue );
static void SetStringPropertyToNodeL(
    CXnDomStringPool& aStringPool, CXnNode& aNode,
    const TDesC8& aPropertyName, const TDesC8& aPropertyValue );
static void SetFloatPropertyToNodeL(
    CXnDomStringPool& aStringPool, CXnNode& aNode,
    const TDesC8& aPropertyName, TReal aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType );
static CXnNode* FindYoungestSiblingWithDisplayL(
    RPointerArray< CXnNode >& aSiblingArray,
    const TDesC8& aDisplayValue,
    TInt& aIndex );
static CXnNode* FindOldestSiblingWithDisplayL(
    RPointerArray< CXnNode >& aSiblingArray,
    const TDesC8& aDisplayValue,
    TInt& aIndex );
static TBool IsCompoundNodeL( CXnNode* aNode );
static void NotifyScrollNodeL( CXnNode* aNode, TBool aBegin, TBool aGainEnd );
static void DoCompoundNodeBeginKeyL(
    RPointerArray< CXnNode >& aChildren, CXnNode* aFocusedNode );
static void DoCompoundNodeEndKeyL(
    RPointerArray< CXnNode >& aChildren, CXnNode* aFocusedNode );
static void DoCompoundNodeScrollingL(
    CXnNode& aFocusedNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType );
static CXnNode* BuildTriggerNodeL(
    CXnUiEngine& aUiEngine, const TDesC8& aTriggerName );
static CXnNode* IsNodeNavigableL( CXnNode* aParent );
static TBool IsNodeNavigableL( CXnNode& aNode, TBool aIgnoreDropped = EFalse );
static TInt DirtyLevelFromPropertyL( CXnNode& aNode, const TDesC8& aPropName );
static TBool IsNodeDisplayedL(
    CXnNode& aNode, TBool aCheckVisiblity = EFalse, TBool aCheckParent = ETrue );
static void EnableStatePropertyL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    CXnProperty* ( CXnNode::* aGetCachedPropertyL )(),
    CXnProperty*& aProperty,
    RPointerArray< CXnProperty >& aArray, TInt& aLevel,
    TBool aInformChange = EFalse );
static void DisableStatePropertyL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    CXnProperty* ( CXnNode::* aGetCachedPropertyL )(),
    CXnProperty*& aProperty,
    RPointerArray< CXnProperty >& aArray,
    RArray< CXnDomProperty::TPseudoClass >& aStates, TInt& aLevel,
    TBool aInformChange = EFalse );
static TBool MatchTitleScrollTriggerL(
    CXnNode& aEventData, CXnDomNode& aTriggerNode );
static CXnDomNode* GetTemplateEventL( 
    const TDesC8& aTemplate, 
    CXnNode& aNode, 
    CXnDomNode& aEventNode );    

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// GetTemplateEventL
// -----------------------------------------------------------------------------
//
static CXnDomNode* GetTemplateEventL( const TDesC8& aTemplate, CXnNode& aNode, 
    CXnDomNode& aEventNode )
    {
    CXnDomNode* retval( NULL );
    
    if ( aTemplate == KIndex() )
        {
        _LIT8( KTemplateIndex, "(#)" );
        
        const TDesC8& eventString( aEventNode.AttributeValue( KName ) );
        
        TInt index( eventString.Find( KTemplateIndex ) );
        
        CXnNode* parent( aNode.Parent() );
        
        if ( index != KErrNotFound && parent )
            {                                
            RPointerArray< CXnNode >& siblings( parent->Children() );
            
            TInt itemindex( siblings.Find( &aNode ) );
            
            if ( itemindex != KErrNotFound )
                {
                TBuf8< 4 > buf;
                buf.AppendNum( itemindex );
    
                CXnDomNode* clone( 
                    aEventNode.CloneL( *aEventNode.StringPool() ) );
                CleanupStack::PushL( clone );
                
                HBufC8* templateString = 
                    HBufC8::NewLC( eventString.Length() + 4 );

                TPtr8 ptr( templateString->Des() );
                
                ptr.Copy( eventString );
                ptr.Delete( index + 1, 1 );                    
                ptr.Insert( index + 1, buf );
                                
                CXnDomAttribute* attribute = static_cast< CXnDomAttribute* > 
                    ( clone->AttributeList().FindByName( 
                        XnPropertyNames::action::event::KName ) );
                                                
                if ( attribute )
                    {                                        
                    attribute->SetValueL( ptr );
                    }
                                
                CleanupStack::PopAndDestroy( templateString );
                
                CleanupStack::Pop( clone );
                retval = clone;
                }
            }    
        }
    
    return retval;
    }
    
// -----------------------------------------------------------------------------
// ResolveEffectId
// -----------------------------------------------------------------------------
//
static TInt ResolveEffectId( CXnDomNode& aEventNode )
    {
    TInt effect( 0 );
    const TDesC8& value( aEventNode.AttributeValue( 
        XnPropertyNames::common::KEffectId ) );
    if( value != KNullDesC8 )
        {
        TLex8 lex( value );
        lex.Val( effect );    
        }
    return effect;
    }

// -----------------------------------------------------------------------------
// Checks whether trigger is runnable
// -----------------------------------------------------------------------------
//
TBool IsTriggerRunnableL( CXnDomNode& aTriggerNode )
    {
    TBool ret( ETrue );
    CXnNode* layoutNode = aTriggerNode.LayoutNode();
    if ( layoutNode )
        {
        CXnProperty* display =
            layoutNode->GetPropertyL( XnPropertyNames::style::common::KDisplay );

        if ( display && display->StringValue() ==
             XnPropertyNames::style::common::display::KNone )
            {
            ret = EFalse;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// DirtyLevelFromPropertyL
// -----------------------------------------------------------------------------
//
static TInt DirtyLevelFromPropertyL( CXnNode& aNode, const TDesC8& aPropName )
    {
    if ( aPropName == XnPropertyNames::style::common::KDisplay ||
        aPropName == XnPropertyNames::style::common::KBlockProgression ||
        aPropName == XnPropertyNames::style::common::KDirection ||
        aPropName == XnPropertyNames::style::common::KPosition ||
        aPropName == XnPropertyNames::style::common::KTop ||
        aPropName == XnPropertyNames::style::common::KLeft ||
        aPropName == XnPropertyNames::style::common::KBottom ||
        aPropName == XnPropertyNames::style::common::KRight ||
        aPropName == XnPropertyNames::style::common::KWidth ||
        aPropName == XnPropertyNames::style::common::KHeight ||
        aPropName == XnPropertyNames::style::common::KMinWidth ||
        aPropName == XnPropertyNames::style::common::KMaxWidth ||
        aPropName == XnPropertyNames::style::common::KMinHeight ||
        aPropName == XnPropertyNames::style::common::KMaxHeight ||
        aPropName == XnPropertyNames::style::common::KS60DisplayPriority ||
        aPropName == XnPropertyNames::style::common::KMarginTop ||
        aPropName == XnPropertyNames::style::common::KMarginRight ||
        aPropName == XnPropertyNames::style::common::KMarginBottom ||
        aPropName == XnPropertyNames::style::common::KMarginLeft ||
        aPropName == XnPropertyNames::style::common::KBorderWidth ||
        aPropName == XnPropertyNames::style::common::KBorderTopWidth ||
        aPropName == XnPropertyNames::style::common::KBorderRightWidth ||
        aPropName == XnPropertyNames::style::common::KBorderBottomWidth ||
        aPropName == XnPropertyNames::style::common::KBorderLeftWidth ||
        aPropName == XnPropertyNames::style::common::KBorderLeftWidth ||
        aPropName == XnPropertyNames::style::common::KPaddingRight ||
        aPropName == XnPropertyNames::style::common::KPaddingBottom ||
        aPropName == XnPropertyNames::style::common::KPaddingLeft )
        {
        TInt retval( XnDirtyLevel::ELayoutAndRenderSiblings );

        CXnProperty* prop( aNode.PositionL() );

        if ( prop )
            {
            const TDesC8& value( prop->StringValue() );

            if ( value == XnPropertyNames::style::common::position::KAbsolute ||
                 value == XnPropertyNames::style::common::position::KFloating )
                {
                retval = XnDirtyLevel::ELayoutAndRender;
                }
            }

        if ( aNode.Type()->Type() == KToolTip )
            {
            retval = XnDirtyLevel::ELayoutAndRender;
            }

        return retval;
        }
    else
        {
        return XnDirtyLevel::ERender;
        }
    }

// -----------------------------------------------------------------------------
// IsNodeDisplayedL
// Check whether a node is displayed or not
// -----------------------------------------------------------------------------
//
static TBool IsNodeDisplayedL(
    CXnNode& aNode,
    TBool aCheckVisiblity,
    TBool aCheckParent )
    {
    if ( !aNode.IsLayoutCapable() )
        {
        return EFalse;
        }

    if ( aCheckVisiblity )
        {
        CXnProperty* visibilityProp( aNode.VisibilityL() );

        // Am I visible?
        if ( visibilityProp )
            {
            const TDesC8& visibility( visibilityProp->StringValue() );

            if ( visibility != XnPropertyNames::style::common::visibility::KVisible )
                {
                return EFalse;
                }
            }
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

    if ( aCheckParent )
        {
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
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// IsNodeNavigableL
// Check whether a node or its child is navigable, ignores dropped state
// -----------------------------------------------------------------------------
//
static CXnNode* IsNodeNavigableL( CXnNode* aParent )
    {
    if ( IsNodeNavigableL( *aParent, ETrue ) )
        {
        return aParent;
        }

    RPointerArray< CXnNode >& children( aParent->Children() );

    TInt count( children.Count() );

    CXnNode* retval = NULL;
    for ( TInt i = 0; i < count; i++ )
        {
        retval = IsNodeNavigableL( children[i] );
        if ( retval )
            {
            return retval;
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// IsNodeNavigableL()
// Check if node can be navigated
// -----------------------------------------------------------------------------
//
static TBool IsNodeNavigableL( CXnNode& aNode, TBool aIgnoreDropped )
    {
    if ( !aNode.IsLayoutCapable() || ( !aIgnoreDropped && aNode.IsDropped() ) )
        {
        return EFalse;
        }

    CXnProperty* displayProp( aNode.DisplayL() );

    if ( displayProp )
        {
        const TDesC8& display( displayProp->StringValue() );

        if ( display != XnPropertyNames::style::common::display::KBlock )
            {
            return EFalse;
            }
        }

    CXnProperty* visibilityProp( aNode.VisibilityL() );

    if ( visibilityProp )
        {
        const TDesC8& visibility( visibilityProp->StringValue() );

        if ( visibility != XnPropertyNames::style::common::visibility::KVisible )
            {
            return EFalse;
            }
        }

    CXnProperty* focusableProp(
        aNode.GetPropertyL( XnPropertyNames::common::KFocusable ) );

    if ( focusableProp )
        {
        const TDesC8& focus( focusableProp->StringValue() );

        if ( focus != XnPropertyNames::KTrue )
            {
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }
    CXnProperty* disabledProp(
        aNode.GetPropertyL( XnPropertyNames::common::KDisabled ) );

    if ( disabledProp )
        {
        const TDesC8& disabled( disabledProp->StringValue() );

        if ( disabled == XnPropertyNames::KTrue )
            {
            return EFalse;
            }
        }

    // The node can be navigated. Then check parents display and visibilty properties
    CXnNode* parent( aNode.Parent() );

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

        CXnProperty* visibilityProp( parent->VisibilityL() );

        if ( visibilityProp )
            {
            const TDesC8& visibility( visibilityProp->StringValue() );

            if ( visibility != XnPropertyNames::style::common::visibility::KVisible )
                {
                return EFalse;
                }
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// EnableStatePropertyL()
// -----------------------------------------------------------------------------
//
static void EnableStatePropertyL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    CXnProperty* ( CXnNode::* aGetCachedPropertyL )(),
    CXnProperty*& aProperty,
    RPointerArray< CXnProperty >& aArray, TInt& aLevel,
    TBool aInformChange )
    {
    TBool doInform( EFalse );

    for ( TInt i = aArray.Count() - 1; i >= 0; --i )
        {
        if ( aArray[i]->Property()->PseudoClass() == aPseudoClass )
            {
            CXnProperty* temp( aArray[i] );

            if ( aLevel < XnDirtyLevel::ELayoutAndRenderSiblings ||
                 aInformChange )
                {
                TInt level( XnDirtyLevel::ENone );

                // Get current cached prop
                CXnProperty* prop( ( aNode.*aGetCachedPropertyL )() );

                if ( !temp && prop )
                    {
                    level = DirtyLevelFromPropertyL(
                        aNode, prop->Property()->Name() );
                    }
                else if ( ( temp && !prop ) ||
                          ( temp && prop && !prop->EqualsL( *temp ) ) )
                    {
                    level = DirtyLevelFromPropertyL(
                        aNode, temp->Property()->Name() );
                    }

                if ( aInformChange && ( level != XnDirtyLevel::ENone ) )
                    {
                    doInform = ETrue;
                    }

                if ( level > aLevel && aNode.IsLayoutCapable())
                    {
                    aLevel = level;
                    }
                }

            aProperty = temp;

            if ( doInform )
                {
                // Property is changed
                InformPropertyChangeL( aNode, temp );
                }

            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// DisableStatePropertyL()
// -----------------------------------------------------------------------------
//
static void DisableStatePropertyL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    CXnProperty* ( CXnNode::* aGetCachedPropertyL )(),
    CXnProperty*& aProperty,
    RPointerArray< CXnProperty >& aArray,
    RArray< CXnDomProperty::TPseudoClass >& aStates, TInt& aLevel,
    TBool aInformChange )
    {
    CXnProperty* currentStateProperty( NULL );

    TBool doInform( EFalse );

    if ( aProperty && aProperty->Property()->PseudoClass() == aPseudoClass )
        {
        // Is there an other pseudo state where to return?
        for ( TInt i = aArray.Count() - 1; !currentStateProperty && i >= 0; --i )
            {
            for ( TInt j = aStates.Count() - 1; j >= 0; --j )
                {
                if ( aArray[i]->Property()->PseudoClass() == aStates[j] &&
                     aArray[i]->Property()->PseudoClass() != aPseudoClass )
                    {
                    currentStateProperty = aArray[i];
                    break;
                    }
                }
            }

        // No, use the property from state "none"
        for ( TInt i = aArray.Count() - 1; !currentStateProperty && i >= 0; --i )
            {
            for ( TInt j = aStates.Count() - 1; j >= 0; --j )
                {
                if ( aArray[i]->Property()->PseudoClass() == CXnDomProperty::ENone &&
                     aArray[i]->Property()->PseudoClass() != aPseudoClass )
                    {
                    currentStateProperty = aArray[i];
                    break;
                    }
                }
            }

        CXnProperty* temp( currentStateProperty );

        if ( aLevel < XnDirtyLevel::ELayoutAndRenderSiblings || aInformChange )
            {
            TInt level( XnDirtyLevel::ENone );

            // Get current cached prop
            CXnProperty* prop( ( aNode.*aGetCachedPropertyL )() );

            if ( !temp && prop )
                {
                level = DirtyLevelFromPropertyL( aNode, prop->Property()->Name() );
                }
            else if ( ( temp && !prop ) || ( temp && prop && !prop->EqualsL( *temp ) ) )
                {
                level = DirtyLevelFromPropertyL( aNode, temp->Property()->Name() );
                }

            if ( aInformChange && ( level != XnDirtyLevel::ENone ) )
                {
                doInform = ETrue;
                }

            if ( level > aLevel && aNode.IsLayoutCapable())
                {
                aLevel = level;
                }
            }

        aProperty = temp;

        if ( doInform )
            {
            // Property is changed
            InformPropertyChangeL( aNode, temp );
            }
        }
    }

// -----------------------------------------------------------------------------
// EnableStatePropertiesL()
// -----------------------------------------------------------------------------
//
static void EnableStatePropertiesL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    CLayoutPropertyCache*& aLayoutPropertyCache )
    {
    if ( !aLayoutPropertyCache )
        {
        aLayoutPropertyCache = new ( ELeave ) CLayoutPropertyCache;
        }

    TInt level( XnDirtyLevel::ENone );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::WidthL,
        aLayoutPropertyCache->iWidth,
        aLayoutPropertyCache->iWidths, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::HeightL,
        aLayoutPropertyCache->iHeight,
        aLayoutPropertyCache->iHeights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginLeftL,
        aLayoutPropertyCache->iMarginLeft,
        aLayoutPropertyCache->iMarginLefts, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginRightL,
        aLayoutPropertyCache->iMarginRight,
        aLayoutPropertyCache->iMarginRights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderLeftL,
        aLayoutPropertyCache->iBorderLeft,
        aLayoutPropertyCache->iBorderLefts, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderRightL,
        aLayoutPropertyCache->iBorderRight,
        aLayoutPropertyCache->iBorderRights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingLeftL,
        aLayoutPropertyCache->iPaddingLeft,
        aLayoutPropertyCache->iPaddingLefts, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingRightL,
        aLayoutPropertyCache->iPaddingRight,
        aLayoutPropertyCache->iPaddingRights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginTopL,
        aLayoutPropertyCache->iMarginTop,
        aLayoutPropertyCache->iMarginTops, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginBottomL,
        aLayoutPropertyCache->iMarginBottom,
        aLayoutPropertyCache->iMarginBottoms, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderTopL,
        aLayoutPropertyCache->iBorderTop,
        aLayoutPropertyCache->iBorderTops, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderBottomL,
        aLayoutPropertyCache->iBorderBottom,
        aLayoutPropertyCache->iBorderBottoms, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingTopL,
        aLayoutPropertyCache->iPaddingTop,
        aLayoutPropertyCache->iPaddingTops, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingBottomL,
        aLayoutPropertyCache->iPaddingBottom,
        aLayoutPropertyCache->iPaddingBottoms, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderWidthL,
        aLayoutPropertyCache->iBorderWidth,
        aLayoutPropertyCache->iBorderWidths, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BlockProgressionL,
        aLayoutPropertyCache->iBlockProgression,
        aLayoutPropertyCache->iBlockProgressions, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DirectionL,
        aLayoutPropertyCache->iDirection,
        aLayoutPropertyCache->iDirections, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PositionL,
        aLayoutPropertyCache->iPosition,
        aLayoutPropertyCache->iPositions, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MaxHeightL,
        aLayoutPropertyCache->iMaxHeight,
        aLayoutPropertyCache->iMaxHeights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MinHeightL,
        aLayoutPropertyCache->iMinHeight,
        aLayoutPropertyCache->iMinHeights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MaxWidthL,
        aLayoutPropertyCache->iMaxWidth,
        aLayoutPropertyCache->iMaxWidths, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MinWidthL,
        aLayoutPropertyCache->iMinWidth,
        aLayoutPropertyCache->iMinWidths, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DisplayL,
        aLayoutPropertyCache->iDisplay,
        aLayoutPropertyCache->iDisplays, level, ETrue );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::VisibilityL,
        aLayoutPropertyCache->iVisibility,
        aLayoutPropertyCache->iVisibilities, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::LeftL,
        aLayoutPropertyCache->iLeft,
        aLayoutPropertyCache->iLefts, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::RightL,
        aLayoutPropertyCache->iRight,
        aLayoutPropertyCache->iRights, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::TopL,
        aLayoutPropertyCache->iTop,
        aLayoutPropertyCache->iTops, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BottomL,
        aLayoutPropertyCache->iBottom,
        aLayoutPropertyCache->iBottoms, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderLeftStyleL,
        aLayoutPropertyCache->iBorderLeftStyle,
        aLayoutPropertyCache->iBorderLeftStyles, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderRightStyleL,
        aLayoutPropertyCache->iBorderRightStyle,
        aLayoutPropertyCache->iBorderRightStyles, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderTopStyleL,
        aLayoutPropertyCache->iBorderTopStyle,
        aLayoutPropertyCache->iBorderTopStyles, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderBottomStyleL,
        aLayoutPropertyCache->iBorderBottomStyle,
        aLayoutPropertyCache->iBorderBottomStyles, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderStyleL,
        aLayoutPropertyCache->iBorderStyle,
        aLayoutPropertyCache->iBorderStyles, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DisplayPriorityL,
        aLayoutPropertyCache->iDisplayPriority,
        aLayoutPropertyCache->iDisplayPriorities, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::NavIndexL,
        aLayoutPropertyCache->iNavIndex,
        aLayoutPropertyCache->iNavIndexes, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::ZIndexL,
        aLayoutPropertyCache->iZIndex,
        aLayoutPropertyCache->iZIndexes, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BackgroundColorL,
        aLayoutPropertyCache->iBackgroundColor,
        aLayoutPropertyCache->iBackgroundColors, level );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BackgroundImageL,
        aLayoutPropertyCache->iBackgroundImage,
        aLayoutPropertyCache->iBackgroundImages, level, ETrue );

    EnableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::FocusBackgroundL,
        aLayoutPropertyCache->iFocusBackground,
        aLayoutPropertyCache->iFocusBackgrounds, level );

    aNode.SetDirtyL( level );
    }

// -----------------------------------------------------------------------------
// DisableStatePropertiesL()
// -----------------------------------------------------------------------------
//
static void DisableStatePropertiesL(
    CXnNode& aNode,
    CXnDomProperty::TPseudoClass aPseudoClass,
    RArray< CXnDomProperty::TPseudoClass >& aStates,
    CLayoutPropertyCache*& aLayoutPropertyCache )
    {
    if ( !aLayoutPropertyCache )
        {
        aLayoutPropertyCache = new ( ELeave ) CLayoutPropertyCache;
        }

    TInt level( XnDirtyLevel::ENone );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::WidthL,
        aLayoutPropertyCache->iWidth,
        aLayoutPropertyCache->iWidths, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::HeightL,
        aLayoutPropertyCache->iHeight,
        aLayoutPropertyCache->iHeights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginLeftL,
        aLayoutPropertyCache->iMarginLeft,
        aLayoutPropertyCache->iMarginLefts, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginRightL,
        aLayoutPropertyCache->iMarginRight,
        aLayoutPropertyCache->iMarginRights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderLeftL,
        aLayoutPropertyCache->iBorderLeft,
        aLayoutPropertyCache->iBorderLefts, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderRightL,
        aLayoutPropertyCache->iBorderRight,
        aLayoutPropertyCache->iBorderRights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingLeftL,
        aLayoutPropertyCache->iPaddingLeft,
        aLayoutPropertyCache->iPaddingLefts, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingRightL,
        aLayoutPropertyCache->iPaddingRight,
        aLayoutPropertyCache->iPaddingRights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginTopL,
        aLayoutPropertyCache->iMarginTop,
        aLayoutPropertyCache->iMarginTops, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MarginBottomL,
        aLayoutPropertyCache->iMarginBottom,
        aLayoutPropertyCache->iMarginBottoms, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderTopL,
        aLayoutPropertyCache->iBorderTop,
        aLayoutPropertyCache->iBorderTops, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderBottomL,
        aLayoutPropertyCache->iBorderBottom,
        aLayoutPropertyCache->iBorderBottoms, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingTopL,
        aLayoutPropertyCache->iPaddingTop,
        aLayoutPropertyCache->iPaddingTops, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PaddingBottomL,
        aLayoutPropertyCache->iPaddingBottom,
        aLayoutPropertyCache->iPaddingBottoms, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderWidthL,
        aLayoutPropertyCache->iBorderWidth,
        aLayoutPropertyCache->iBorderWidths, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BlockProgressionL,
        aLayoutPropertyCache->iBlockProgression,
        aLayoutPropertyCache->iBlockProgressions, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DirectionL,
        aLayoutPropertyCache->iDirection,
        aLayoutPropertyCache->iDirections, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::PositionL,
        aLayoutPropertyCache->iPosition,
        aLayoutPropertyCache->iPositions, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MaxHeightL,
        aLayoutPropertyCache->iMaxHeight,
        aLayoutPropertyCache->iMaxHeights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MinHeightL,
        aLayoutPropertyCache->iMinHeight,
        aLayoutPropertyCache->iMinHeights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MaxWidthL,
        aLayoutPropertyCache->iMaxWidth,
        aLayoutPropertyCache->iMaxWidths, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::MinWidthL,
        aLayoutPropertyCache->iMinWidth,
        aLayoutPropertyCache->iMinWidths, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DisplayL,
        aLayoutPropertyCache->iDisplay,
        aLayoutPropertyCache->iDisplays, aStates, level, ETrue );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::VisibilityL,
        aLayoutPropertyCache->iVisibility,
        aLayoutPropertyCache->iVisibilities, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::LeftL,
        aLayoutPropertyCache->iLeft,
        aLayoutPropertyCache->iLefts, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::RightL,
        aLayoutPropertyCache->iRight,
        aLayoutPropertyCache->iRights, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::TopL,
        aLayoutPropertyCache->iTop,
        aLayoutPropertyCache->iTops, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BottomL,
        aLayoutPropertyCache->iBottom,
        aLayoutPropertyCache->iBottoms, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderLeftStyleL,
        aLayoutPropertyCache->iBorderLeftStyle,
        aLayoutPropertyCache->iBorderLeftStyles, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderRightStyleL,
        aLayoutPropertyCache->iBorderRightStyle,
        aLayoutPropertyCache->iBorderRightStyles, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderTopStyleL,
        aLayoutPropertyCache->iBorderTopStyle,
        aLayoutPropertyCache->iBorderTopStyles, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderBottomStyleL,
        aLayoutPropertyCache->iBorderBottomStyle,
        aLayoutPropertyCache->iBorderBottomStyles, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BorderStyleL,
        aLayoutPropertyCache->iBorderStyle,
        aLayoutPropertyCache->iBorderStyles, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::DisplayPriorityL,
        aLayoutPropertyCache->iDisplayPriority,
        aLayoutPropertyCache->iDisplayPriorities, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::NavIndexL,
        aLayoutPropertyCache->iNavIndex,
        aLayoutPropertyCache->iNavIndexes, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::ZIndexL,
        aLayoutPropertyCache->iZIndex,
        aLayoutPropertyCache->iZIndexes, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BackgroundColorL,
        aLayoutPropertyCache->iBackgroundColor,
        aLayoutPropertyCache->iBackgroundColors, aStates, level );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::BackgroundImageL,
        aLayoutPropertyCache->iBackgroundImage,
        aLayoutPropertyCache->iBackgroundImages, aStates, level, ETrue );

    DisableStatePropertyL(
        aNode, aPseudoClass, &CXnNode::FocusBackgroundL,
        aLayoutPropertyCache->iFocusBackground,
        aLayoutPropertyCache->iFocusBackgrounds, aStates, level );

    aNode.SetDirtyL( level );
    }

// -----------------------------------------------------------------------------
// AllocateLayoutCache()
// -----------------------------------------------------------------------------
//
static TBool AllocateLayoutCache( CLayoutPropertyCache*& aLayoutPropertyCache )
    {
    if ( !aLayoutPropertyCache )
        {
        aLayoutPropertyCache = new CLayoutPropertyCache;
        }

    return aLayoutPropertyCache ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// DoSetCachedProperty()
// -----------------------------------------------------------------------------
//
static void DoSetCachedProperty(
    CXnProperty* aProperty,\
    CXnProperty*& aCachedPropertyToSet,
    RPointerArray< CXnProperty >& aArray,
    TBool aInitializing,
    TBool aArrayOnly )
    {
    ReplaceOrAppend( aProperty, aArray, aInitializing );

    if ( !aArrayOnly )
        {
        aCachedPropertyToSet = aProperty;
        }
    }

// -----------------------------------------------------------------------------
// SetCachedProperty()
// -----------------------------------------------------------------------------
//
static void SetCachedProperty(
    CXnProperty* aProperty,
    CLayoutPropertyCache*& aLayoutPropertyCache,
    CXnProperty*& aName,
    CXnProperty*& aValue,
    CXnProperty*& aLabel,
    CXnProperty*& aInitialFocus,
    CXnProperty*& aClass,
    CXnProperty*& aId,
    CXnProperty*& aPath,
    CXnProperty*& aMaskPath,
    TBool aInitializing,
    TBool aArrayOnly)
    {
    const TDesC8& name = aProperty->Property()->Name();
    if ( !AllocateLayoutCache( aLayoutPropertyCache ) )
        {
        return;
        }
    if ( name == XnPropertyNames::common::KId )
        {
        aId = aProperty;
        }
    else if ( name == XnPropertyNames::common::KClass )
        {
        aClass = aProperty;
        }
    else if ( name == XnPropertyNames::style::common::KWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iWidth,
            aLayoutPropertyCache->iWidths,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KHeight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iHeight,
            aLayoutPropertyCache->iHeights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBlockProgression )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBlockProgression,
            aLayoutPropertyCache->iBlockProgressions,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KDirection )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iDirection,
            aLayoutPropertyCache->iDirections,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KPosition )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iPosition,
            aLayoutPropertyCache->iPositions,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KDisplay )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iDisplay,
            aLayoutPropertyCache->iDisplays,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KVisibility )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iVisibility,
            aLayoutPropertyCache->iVisibilities,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::action::KName )
        {
        aName = aProperty;
        }
    else if ( name == XnPropertyNames::action::KValue )
        {
        aValue = aProperty;
        }
    else if ( name == XnPropertyNames::menu::KLabel )
        {
        aLabel = aProperty;
        }
    else if ( name == XnPropertyNames::image::KPath )
        {
        aPath = aProperty;
        }
    else if ( name == XnPropertyNames::image::KMaskPath )
        {
        aMaskPath = aProperty;
        }
    else if ( name == XnPropertyNames::style::common::KMarginLeft )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMarginLeft,
            aLayoutPropertyCache->iMarginLefts,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMarginRight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMarginRight,
            aLayoutPropertyCache->iMarginRights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBorderLeftWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderLeft,
            aLayoutPropertyCache->iBorderLefts,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBorderRightWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderRight,
            aLayoutPropertyCache->iBorderRights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KPaddingLeft )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iPaddingLeft,
            aLayoutPropertyCache->iPaddingLefts,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KPaddingRight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iPaddingRight,
            aLayoutPropertyCache->iPaddingRights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMarginTop )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMarginTop,
            aLayoutPropertyCache->iMarginTops,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMarginBottom )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMarginBottom,
            aLayoutPropertyCache->iMarginBottoms,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBorderTopWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderTop,
            aLayoutPropertyCache->iBorderTops,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBorderBottomWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderBottom,
            aLayoutPropertyCache->iBorderBottoms,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KPaddingTop )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iPaddingTop,
            aLayoutPropertyCache->iPaddingTops,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KPaddingBottom )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iPaddingBottom,
            aLayoutPropertyCache->iPaddingBottoms,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBorderWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderWidth,
            aLayoutPropertyCache->iBorderWidths,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMaxHeight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMaxHeight,
            aLayoutPropertyCache->iMaxHeights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMinHeight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMinHeight,
            aLayoutPropertyCache->iMinHeights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMaxWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMaxWidth,
            aLayoutPropertyCache->iMaxWidths,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KMinWidth )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iMinWidth,
            aLayoutPropertyCache->iMinWidths,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KLeft )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iLeft,
            aLayoutPropertyCache->iLefts,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KRight )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iRight,
            aLayoutPropertyCache->iRights,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KTop )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iTop,
            aLayoutPropertyCache->iTops,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KBottom )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBottom,
            aLayoutPropertyCache->iBottoms,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderLeftStyle )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderLeftStyle,
            aLayoutPropertyCache->iBorderLeftStyles,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderRightStyle )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderRightStyle,
            aLayoutPropertyCache->iBorderRightStyles,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderTopStyle )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderTopStyle,
            aLayoutPropertyCache->iBorderTopStyles,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderBottomStyle )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderBottomStyle,
            aLayoutPropertyCache->iBorderBottomStyles,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderStyle )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBorderStyle,
            aLayoutPropertyCache->iBorderStyles,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBorderImage )
        {
        aLayoutPropertyCache->iBorderImage = aProperty;
        }
    else if ( name == XnPropertyNames::style::common::KZIndex )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iZIndex,
            aLayoutPropertyCache->iZIndexes,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBackGroundColor )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBackgroundColor,
            aLayoutPropertyCache->iBackgroundColors,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KBackGroundImage )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iBackgroundImage,
            aLayoutPropertyCache->iBackgroundImages,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::appearance::common::KFocusBackground )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iFocusBackground,
            aLayoutPropertyCache->iFocusBackgrounds,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::style::common::KS60DisplayPriority )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iDisplayPriority,
            aLayoutPropertyCache->iDisplayPriorities,
            aInitializing, aArrayOnly );
        }
    else if ( name == XnPropertyNames::common::KInitialFocus )
        {
        aInitialFocus = aProperty;
        }
    else if ( name == XnPropertyNames::style::common::KNavIndex )
        {
        DoSetCachedProperty(
            aProperty,
            aLayoutPropertyCache->iNavIndex,
            aLayoutPropertyCache->iNavIndexes,
            aInitializing, aArrayOnly );
        }
    }

// -----------------------------------------------------------------------------
// ReplaceOrAppend()
// -----------------------------------------------------------------------------
//
static void ReplaceOrAppend(
    CXnProperty* aProperty,
    RPointerArray< CXnProperty >& aArray,
    TBool aAppendOnly )
    {
    if ( !aAppendOnly )
        {
        for ( TInt i = aArray.Count() - 1; i >= 0; --i )
            {
            if ( aArray[i] == aProperty )
                {
                aArray.Remove( i );
                }
            else if ( aArray[i]->Property()->PseudoClass() ==
                      aProperty->Property()->PseudoClass() &&
                      aArray[i]->Property()->Name() ==
                      aProperty->Property()->Name() )
                {
                aArray.Remove( i );
                }
            }
        }
    aArray.Append( aProperty );
    }

// -----------------------------------------------------------------------------
// IsPropertyPseudoClass()
// -----------------------------------------------------------------------------
//
static TBool IsPropertyPseudoClass(
    CXnDomProperty::TPseudoClass aPseudoClass,
    RArray< CXnDomProperty::TPseudoClass >& aArray )
    {
    for ( TInt i = aArray.Count() - 1; i >= 0; --i )
        {
        if ( aArray[i] == aPseudoClass )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// InsertPropertyPseudoClassL()
// -----------------------------------------------------------------------------
//
static void InsertPropertyPseudoClassL(
    CXnDomProperty::TPseudoClass aPseudoClass,
    RArray< CXnDomProperty::TPseudoClass >& aArray )
    {
    if ( !IsPropertyPseudoClass( aPseudoClass, aArray ) )
        {
        User::LeaveIfError( aArray.Append( aPseudoClass ) );
        }
    }

// -----------------------------------------------------------------------------
// BuildEventTypeNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildEventTypeNodeL( TEventCode aType, CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::keyevent::KEventType );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::KName,
        nameValue, *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetFloatValueL( CXnDomPropertyValue::ENumber, aType );
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue, valueValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildModifiersNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildModifiersNodeL(
    const TKeyEvent& aKeyEvent,
    CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::keyevent::KModifiers );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetFloatValueL(
        CXnDomPropertyValue::ENumber, aKeyEvent.iModifiers );
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        valueValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildKeyCodeNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildKeyCodeNodeL(
    const TKeyEvent& aKeyEvent,
    CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::keyevent::KKeyCode );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetFloatValueL( CXnDomPropertyValue::ENumber, aKeyEvent.iCode );
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        valueValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildScanCodeNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildScanCodeNodeL(
    const TKeyEvent& aKeyEvent,
    CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::keyevent::KScanCode );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetFloatValueL(
        CXnDomPropertyValue::ENumber, aKeyEvent.iScanCode );
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        valueValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildRepeatsNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildRepeatsNodeL(
    const TKeyEvent& aKeyEvent,
    CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KProperty );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::keyevent::KRepeats );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CXnDomPropertyValue* valueValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( valueValue );
    valueValue->SetFloatValueL(
        CXnDomPropertyValue::ENumber, aKeyEvent.iRepeats );
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        valueValue,
        *aUiEngine.ODT()->DomDocument().StringPool()) ;
    CleanupStack::Pop( valueValue );
    CleanupStack::PushL( value );
    node->SetPropertyL( value );
    CleanupStack::Pop( value );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// BuildPropertyNodesL
// -----------------------------------------------------------------------------
//
static void BuildPropertyNodesL(
    CXnNode& aNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    CXnUiEngine& aUiEngine )
    {
    CXnNode* eventType = BuildEventTypeNodeL( aType, aUiEngine );
    CleanupStack::PushL( eventType );
    aNode.AddChildL( eventType );
    CleanupStack::Pop( eventType );
    CXnNode* modifiers = BuildModifiersNodeL( aKeyEvent, aUiEngine );
    CleanupStack::PushL( modifiers );
    aNode.AddChildL( modifiers );
    CleanupStack::Pop( modifiers );
    CXnNode* keyCode = BuildKeyCodeNodeL( aKeyEvent, aUiEngine );
    CleanupStack::PushL( keyCode );
    aNode.AddChildL( keyCode );
    CleanupStack::Pop( keyCode );
    CXnNode* scanCode = BuildScanCodeNodeL( aKeyEvent, aUiEngine );
    CleanupStack::PushL( scanCode );
    aNode.AddChildL( scanCode );
    CleanupStack::Pop( scanCode );
    CXnNode* repeats = BuildRepeatsNodeL( aKeyEvent, aUiEngine );
    CleanupStack::PushL( repeats );
    aNode.AddChildL( repeats );
    CleanupStack::Pop( repeats );
    }

// -----------------------------------------------------------------------------
// BuildKeyTriggerNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildKeyTriggerNodeL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    CXnUiEngine& aUiEngine )
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
    nameValue->SetStringValueL(
        CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::KKeyEvent );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    BuildPropertyNodesL( *node, aKeyEvent, aType, aUiEngine );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// PseudoClassFromName
// -----------------------------------------------------------------------------
//
static CXnDomProperty::TPseudoClass PseudoClassFromName( const TDesC8& aName )
    {
    if ( aName == KPseudoClassFocus )
        {
        return CXnDomProperty::EFocus;
        }
    else if ( aName == KPseudoClassPassiveFocus )
        {
        return CXnDomProperty::EPassiveFocus;
        }
    else if ( aName == KPseudoClassHold )
        {
        return CXnDomProperty::EHold;
        }
    else if ( aName == KPseudoClassHover )
        {
        return CXnDomProperty::EHover;
        }
    else if ( aName == KPseudoClassLink )
        {
        return CXnDomProperty::ELink;
        }
    else if ( aName == KPseudoClassVisited )
        {
        return CXnDomProperty::EVisited;
        }
    else if ( aName == KPseudoClassActive )
        {
        return CXnDomProperty::EActive;
        }
    else if ( aName == KPseudoClassEdit )
        {
        return CXnDomProperty::EEdit;
        }
    else if ( aName == KPseudoClassPressedDown )
        {
        return CXnDomProperty::EPressedDown;
        }
    return CXnDomProperty::ENone;
    }

// -----------------------------------------------------------------------------
// PseudoClassName
// -----------------------------------------------------------------------------
//
static const TDesC8& PseudoClassName( CXnDomProperty::TPseudoClass aPseudoClass )
    {
    switch ( aPseudoClass )
        {
        case CXnDomProperty::EFocus:
            return KPseudoClassFocus;
        case CXnDomProperty::EPassiveFocus:
            return KPseudoClassPassiveFocus;
        case CXnDomProperty::EHold:
            return KPseudoClassHold;
        case CXnDomProperty::EHover:
            return KPseudoClassHover;
        case CXnDomProperty::ELink:
            return KPseudoClassLink;
        case CXnDomProperty::EVisited:
            return KPseudoClassVisited;
        case CXnDomProperty::EActive:
            return KPseudoClassActive;
        case CXnDomProperty::EEdit:
            return KPseudoClassEdit;
        default:
            return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// DoMatchTriggerForKeyEventL
// -----------------------------------------------------------------------------
//
static TBool DoMatchTriggerForKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType,
    CXnDomNode& aTriggerNode )
    {
    TInt triggerEventType = EEventKeyDown;
    TInt triggerKeyCode = 0;
    TInt triggerScanCode = 0;
    TInt triggerModifiers = 0;
    CXnDomList& triggerChildren = aTriggerNode.ChildNodes();
    TInt triggerCount = triggerChildren.Length();
    TBool triggerScanCodeSet = EFalse;
    TBool triggerKeyCodeSet = EFalse;
    for ( TInt i = 0; i < triggerCount; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >(
            triggerChildren.Item( i ) );
        CXnDomList& attrs = node->AttributeList();
        CXnDomAttribute* nameAttribute = static_cast< CXnDomAttribute* >(
            attrs.FindByName( XnPropertyNames::action::KName ) );
        CXnDomAttribute* valueAttribute = static_cast< CXnDomAttribute* >(
            attrs.FindByName( XnPropertyNames::action::KValue ) );
        if ( !nameAttribute || !valueAttribute )
            {
            continue;
            }
        if ( nameAttribute->Value() ==
             XnPropertyNames::action::trigger::name::keyevent::KEventType )
            {
            TLex8 lex( valueAttribute->Value() );
            lex.Val( triggerEventType );
            }
        else if ( nameAttribute->Value() ==
                  XnPropertyNames::action::trigger::name::keyevent::KModifiers )
            {
            TLex8 lex( valueAttribute->Value() );
            lex.Val( triggerModifiers );
            }
        else if ( nameAttribute->Value() ==
                  XnPropertyNames::action::trigger::name::keyevent::KKeyCode )
            {
            TLex8 lex( valueAttribute->Value() );
            lex.Val( triggerKeyCode );
            triggerKeyCodeSet = ETrue;
            }
        else if ( nameAttribute->Value() ==
                  XnPropertyNames::action::trigger::name::keyevent::KScanCode )
            {
            TLex8 lex( valueAttribute->Value() );
            lex.Val( triggerScanCode );
            triggerScanCodeSet = ETrue;
            }
        }
    return
        aType == triggerEventType &&
        ( triggerKeyCodeSet  ? ( aKeyEvent.iCode == triggerKeyCode ) : ETrue ) &&
        ( triggerScanCodeSet ? ( aKeyEvent.iScanCode == triggerScanCode ) : ETrue ) &&
        ( ( aKeyEvent.iModifiers & 0xFFFFFFFE ) == ( triggerModifiers & 0xFFFFFFFE ) );
    }

// -----------------------------------------------------------------------------
// MatchTriggerForKeyEventL
// -----------------------------------------------------------------------------
//
static TBool MatchTriggerForKeyEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    CXnDomNode& aActionNode,
    CXnDomNode& aTriggerNode )
    {
    CXnDomList& attrs = aTriggerNode.AttributeList();
    CXnDomAttribute* attribute = static_cast< CXnDomAttribute* >(
        attrs.FindByName(XnPropertyNames::action::trigger::KName ) );
    if ( !attribute )
        {
        return EFalse;
        }

    const TDesC8& nameString = attribute->Value();
    if ( nameString == KNullDesC8 )
        {
        return EFalse;
        }
    if ( nameString == XnPropertyNames::action::trigger::name::KKeyEvent )
        {
        TBool match = DoMatchTriggerForKeyEventL( aKeyEvent, aType, aTriggerNode );
        if ( match )
            {
            CXnNode* eventNode = BuildKeyTriggerNodeL( aKeyEvent, aType, aEngine );
            CleanupStack::PushL( eventNode );
            RunEventsL( aThis, aEngine, aNode, aActionNode, aTriggerNode, *eventNode );
            CleanupStack::PopAndDestroy( eventNode );
            }
        return match;
        }
    else if ( aType == EEventKey &&
              ( nameString == XnPropertyNames::action::trigger::name::KUp ||
                nameString == XnPropertyNames::action::trigger::name::KDown ||
                nameString == XnPropertyNames::action::trigger::name::KLeft ||
                nameString == XnPropertyNames::action::trigger::name::KRight ) )
        {
        TBool match = DoMatchTriggerForHighLevelKeyEventL(
            aNode, aKeyEvent, aType, nameString );
        if ( match )
            {
            CXnNode* eventNode = BuildKeyTriggerNodeL( aKeyEvent, aType, aEngine );
            CleanupStack::PushL( eventNode );
            RunEventsL( aThis, aEngine, aNode, aActionNode, aTriggerNode, *eventNode );
            CleanupStack::PopAndDestroy( eventNode );
            }
        return match;
        }
    else if ( aKeyEvent.iScanCode == EStdKeyDevice3 &&
              aKeyEvent.iRepeats > 0 &&
              nameString == XnPropertyNames::action::trigger::name::KHold )
        {
        CXnNode* eventNode = BuildKeyTriggerNodeL( aKeyEvent, aType, aEngine );
        CleanupStack::PushL( eventNode );
        RunEventsL( aThis, aEngine, aNode, aActionNode, aTriggerNode, *eventNode );
        CleanupStack::PopAndDestroy( eventNode );
        return ETrue;
        }
   else if ( ( ( aKeyEvent.iScanCode == EStdKeyDevice0 ) ||
               ( aKeyEvent.iScanCode == EStdKeyDevice1 ) ) &&
             aKeyEvent.iRepeats > 0 &&
             nameString == XnPropertyNames::action::trigger::name::KHold )
        {
        CXnNode* eventNode = BuildKeyTriggerNodeL( aKeyEvent, aType, aEngine );
        CleanupStack::PushL( eventNode );
        RunEventsL( aThis, aEngine, aNode, aActionNode, aTriggerNode, *eventNode );
        CleanupStack::PopAndDestroy( eventNode );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchActivateTriggerPropertiesL
// Checks if triggered node properties match
// ones defined in theme
// -----------------------------------------------------------------------------
//
static TBool MatchActivateTriggerPropertiesL(
    CXnNode& aEventData,
    CXnDomNode& aTriggerNode )
    {
    RPointerArray< CXnNode >& eventChildren = aEventData.Children();
    TInt eventCount = eventChildren.Count();

    if ( eventCount )
        {
        CXnNode* node = eventChildren[0];
        CXnProperty* nameProperty = node->NameL();
        CXnProperty* valueProperty = node->ValueL();

        if ( !nameProperty || !valueProperty )
            {
            return EFalse;
            }

        const TDesC8& name = nameProperty->StringValue();

        if ( name == XnPropertyNames::action::trigger::name::keyevent::KEventType )
            {
            const TDesC8& valueName = valueProperty->StringValue();

            CXnDomList& children = aTriggerNode.ChildNodes();
            TInt count = children.Length();

            if ( ( valueName == _L8( "3" ) ) ) //keydown
                {
                // check that no eventype has been defined
                if ( !count )
                    {
                    return EFalse;
                    }
                }
            else if ( ( valueName ==  _L8( "2" ) ) ) //keyup
                {
                ///check that some eventtype has been defined
                if ( !count )
                    {
                    return EFalse;
                    }

                CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( 0 ) );
                CXnDomList& attrs = node->AttributeList();
                CXnDomAttribute* nameAttribute = static_cast< CXnDomAttribute* >(
                    attrs.FindByName( XnPropertyNames::action::KName ) );
                CXnDomAttribute* valueAttribute = static_cast< CXnDomAttribute* >(
                    attrs.FindByName( XnPropertyNames::action::KValue ) );

                if ( !nameAttribute || !valueAttribute )
                    {
                    return EFalse;
                    }
                
                const TDesC8& name = nameAttribute->Value();

                // check that eventype name matches
                if ( name !=
                     XnPropertyNames::action::trigger::name::keyevent::KEventType )
                    {
                    return EFalse;
                    }
                else
                    {
                    //check that eventype value matches
                    const TDesC8& valueName = valueAttribute->Value();
                    if ( valueName != _L8( "2" ) )
                        {
                        return EFalse;
                        }
                    }
                }
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// MatchScreenDeviceChangePropertiesL
// Checks if triggered node properties match
// ones defined in theme
// -----------------------------------------------------------------------------
//
static TBool MatchScreenDeviceChangePropertiesL(
    CXnNode& aEventData,
    CXnDomNode& aTriggerNode )
    {
    CXnDomList& attributeList( aTriggerNode.AttributeList() );

    const TDesC8& reason(
        XnPropertyNames::action::trigger::name::orientation::KReason );

    CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >
        ( attributeList.FindByName( reason ) ) );

    if ( !attribute )
        {
        // Reason not defined, run events for plain screendevicechange trigger
        return ETrue;
        }

    const TDesC8& value( attribute->Value() );

    if ( value ==
         XnPropertyNames::action::trigger::name::orientation::reason::KLandscape ||
         value ==
         XnPropertyNames::action::trigger::name::orientation::reason::KPortrait )
        {
        CXnProperty* reasonProp( aEventData.GetPropertyL( reason ) );

        if ( reasonProp )
            {
            const TDesC8& reasonValue( reasonProp->StringValue() );

            if ( reasonValue == value )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchSwipeTriggerL
// Check if trigger matches the one defined in theme
// -----------------------------------------------------------------------------
//
static TBool MatchSwipeTriggerL(
    CXnNode& aEventData,
    CXnDomNode& aTriggerNode )
    {
    CXnDomList& attributeList( aTriggerNode.AttributeList() );

     const TDesC8& reason( XnPropertyNames::action::trigger::name::swipe::KDirection);

     CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >
         ( attributeList.FindByName( reason ) ) );

     if ( !attribute )
         {
         // Reason not defined
         return EFalse;
         }

     const TDesC8& value( attribute->Value() );

     if ( value == XnPropertyNames::action::trigger::name::swipe::direction::KLeft ||
          value == XnPropertyNames::action::trigger::name::swipe::direction::KRight )
         {
         CXnProperty* reasonProp( aEventData.GetPropertyL( reason ) );

         if ( reasonProp )
             {
             const TDesC8& reasonValue( reasonProp->StringValue() );

             if ( reasonValue == value )
                 {
                 return ETrue;
                 }
             }
         }

     return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchValueAttributeTriggerL
// Check if trigger matches the one defined in theme
// -----------------------------------------------------------------------------
//
static TBool MatchValueAttributeTriggerL( CXnNode& aEventData,
    CXnDomNode& aTriggerNode )
    {
    CXnDomList& attributeList( aTriggerNode.AttributeList() );

     const TDesC8& reason( XnPropertyNames::action::KValue );

     CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >
         ( attributeList.FindByName( reason ) ) );

     if ( !attribute )
         {
         // Reason not defined
         return EFalse;
         }

     const TDesC8& value( attribute->Value() );

     CXnProperty* reasonProp( aEventData.GetPropertyL( reason ) );

     if ( reasonProp )
         {
         const TDesC8& reasonValue( reasonProp->StringValue() );

         if ( reasonValue == value )
             {
             return ETrue;
             }
         }

     return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchTitleScrollTriggerL
// -----------------------------------------------------------------------------
//
static TBool MatchTitleScrollTriggerL(CXnNode& aEventData,
        CXnDomNode& aTriggerNode )
    {
    CXnDomList& attributeList( aTriggerNode.AttributeList() );

     const TDesC8& reason( XnPropertyNames::action::trigger::name::KTitleIndex );

     CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >
         ( attributeList.FindByName( reason ) ) );

     if ( !attribute )
         {
         // Reason not defined
         return ETrue;
         }
     else
         {
         const TDesC8& value( attribute->Value() );

         CXnProperty* reasonProp( aEventData.GetPropertyL( reason ) );

         if ( reasonProp )
             {
             const TDesC8& reasonValue( reasonProp->StringValue() );

             if ( reasonValue == value )
                 {
                 return ETrue;
                 }
             }
         }

     return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchTriggerForEventL
// -----------------------------------------------------------------------------
//
static TBool MatchTriggerForEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnNode& aEventData,
    CXnDomNode& aActionNode,
    CXnDomNode& aTriggerNode,
    TInt /*aSource*/ )
    {
    const TDesC8& nameString( aTriggerNode.AttributeValue( KName ) );

    if ( nameString == KNullDesC8 )
        {
        return EFalse;
        }

    CXnProperty* eventName( aEventData.NameL() );
    const TDesC8& eventNameString( eventName->StringValue() );

    if ( nameString == eventNameString )
        {
        // Check if triggernode properties match the ones defined in theme
        // This is for identifying keyup in activate-trigger
        if ( nameString == XnPropertyNames::action::trigger::name::KActivate )
            {
            if ( !MatchActivateTriggerPropertiesL( aEventData, aTriggerNode ) )
                {
                return EFalse;
                }
            }
        else if ( nameString == XnPropertyNames::action::trigger::name::KTitleToScroll ||
                  nameString == XnPropertyNames::action::trigger::name::KTitleScrolled )
            {
            if ( !MatchTitleScrollTriggerL( aEventData, aTriggerNode ) )
                {
                return EFalse;
                }
            }
        else if ( nameString ==
                  XnPropertyNames::action::trigger::name::KScreenDeviceChange )
            {
            if ( !MatchScreenDeviceChangePropertiesL( aEventData, aTriggerNode ) )
                {
                return EFalse;
                }
            }
        else if ( nameString == XnPropertyNames::action::trigger::name::KSwipe )
            {
            if ( !MatchSwipeTriggerL( aEventData, aTriggerNode ) )
                {
                return EFalse;
                }
            }
        else if ( nameString == XnPropertyNames::action::trigger::name::KWidgetAmount ||
                  nameString == XnPropertyNames::action::trigger::name::KCallState ||
                  nameString == XnPropertyNames::action::trigger::name::KEditMode )
            {
            if ( !MatchValueAttributeTriggerL( aEventData, aTriggerNode ) )
                {
                return EFalse;
                }
            }

        RunEventsL( aThis, aEngine, aNode, aActionNode, aTriggerNode, aEventData );

        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetFocusCandidatesL
// -----------------------------------------------------------------------------
//
static void GetFocusCandidatesL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode,
    RPointerArray< CXnNode >& aFocusCandidates )
    {
    CXnDomList& children( aEventNode.ChildNodes() );

    TInt count( children.Length() );

    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode( static_cast< CXnDomNode* >( children.Item( i ) ) );

        const TDesC8& type( dnode->Name() );

        // Is the event's node child a property?
        if ( type == XnPropertyNames::action::KProperty )
            {
            CXnDomList& attributeList( dnode->AttributeList() );

            CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >
                ( attributeList.FindByName( XnPropertyNames::action::KValue ) ) );

            if ( attribute )
                {
                CXnNode* node( aEngine.FindNodeByIdL(
                    attribute->Value(), aThis->Namespace() ) );

                if ( node )
                    {
                    aFocusCandidates.AppendL( node );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunPassiveFocusChangeL
// -----------------------------------------------------------------------------
//
static void RunPassiveFocusChangeL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode,
    const TDesC8& aString )
    {
    CXnDomList& children = aEventNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();
        if ( type == XnPropertyNames::action::KProperty )
            {
            CXnDomList& attrs = dnode->AttributeList();
            CXnDomAttribute* attribute = static_cast< CXnDomAttribute* >(
                attrs.FindByName(XnPropertyNames::action::KValue ) );
            if ( attribute )
                {
                CXnNode* node = aEngine.FindNodeByIdL(
                    attribute->Value(), aThis->Namespace() );
                if ( node )
                    {
                    if ( aString == XnPropertyNames::action::event::KSetPassiveFocus )
                        {
                        aEngine.AddPassiveFocusedNodeL( node );
                        }
                    else if ( aString == XnPropertyNames::action::event::KResetPassiveFocus )
                        {
                        aEngine.RemovePassiveFocusedNodeL( node );
                        }
                    else if ( aString == XnPropertyNames::action::event::KSetActiveFocus )
                        {
                        CXnNode* previousNode = aEngine.FocusedNode();
                        if ( previousNode && previousNode != node )
                            {
                            if ( IsNodeNavigableL( *node ) )
                                {
                                node->SetStateL( XnPropertyNames::style::common::KFocus );
                                aEngine.AddPassiveFocusedNodeL( previousNode );
                                break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunAppUiNotificationL
// -----------------------------------------------------------------------------
//
static void RunAppUiNotificationL(
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnDomNode& aEventNode,
    CXnDomNode& aTriggerNode,
    CXnNode& aEventData )
    {       
    CXnAppUiAdapter& adapter( aEngine.AppUiAdapter() );
    
    // Check if event is template
    const TDesC8& templateEvent( aEventNode.AttributeValue( KTemplate ) );
    
    if ( templateEvent != KNullDesC8() )
        {
        CXnDomNode* event( 
            GetTemplateEventL( templateEvent, aNode, aEventNode ) );
        CleanupStack::PushL( event );

        if ( event )
            {
            adapter.HandleXuikonEventL(
                aNode.AppIfL(), aEventData.AppIfL(), aTriggerNode, *event );            
            }
        
        CleanupStack::PopAndDestroy( event );        
        }
    else
        {
        adapter.HandleXuikonEventL(
            aNode.AppIfL(), aEventData.AppIfL(), aTriggerNode, aEventNode );    
        }
    }

// -----------------------------------------------------------------------------
// RunFullScreenEffectL
// -----------------------------------------------------------------------------
//
static void RunFullScreenEffectL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    TInt effectId = ResolveEffectId( aEventNode ); 
    if( effectId )
        {
        aEngine.AppUiAdapter().EffectManager()->BeginFullscreenEffectL(
            effectId, aEngine.ViewManager()->ActiveViewData() );        
        }
    }

// -----------------------------------------------------------------------------
// RunActivateNextViewL
// -----------------------------------------------------------------------------
//
static void RunActivateNextViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    TInt effectid = ResolveEffectId( aEventNode );
    aEngine.ViewManager()->ActivateNextViewL( /*effectid*/ );
    }

// -----------------------------------------------------------------------------
// RunActivatePreviousViewL
// -----------------------------------------------------------------------------
//
static void RunActivatePreviousViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    TInt effectid = ResolveEffectId( aEventNode );
    aEngine.ViewManager()->ActivatePreviousViewL( /*effectid*/ );
    }

// -----------------------------------------------------------------------------
// RunAddViewL
// -----------------------------------------------------------------------------
//
static void RunAddViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    TInt effectid = ResolveEffectId( aEventNode );
    aEngine.ViewManager()->AddViewL( effectid );
    }

// -----------------------------------------------------------------------------
// RunRemoveViewL
// -----------------------------------------------------------------------------
//
static void RunRemoveViewL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    TInt effectid = ResolveEffectId( aEventNode );
    aEngine.ViewManager()->RemoveViewL( effectid );
    }

// -----------------------------------------------------------------------------
// RunActivateL
// -----------------------------------------------------------------------------
//
static void RunActivateL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {
    CXnDomList& children = aEventNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();
        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& id = dnode->AttributeValue( KId );
            CXnNode* node = aEngine.FindNodeByIdL( id, aThis->Namespace() );
            if ( node )
                {
                node->SetStateL( XnPropertyNames::style::common::KActive );
                return;
                }
            }
        }

    CXnNode* focused( aEngine.FocusedNode() );

    if ( focused )
        {
        focused->SetStateL( XnPropertyNames::style::common::KActive );
        }
    }

// -----------------------------------------------------------------------------
// RunActivateEditorL
// -----------------------------------------------------------------------------
//
static void RunActivateEditorL(
    CXnNodeImpl* aThis,
    CXnNode& aLayoutNode,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode,
    TBool aActivate )
    {
    CXnNode* editorNode( NULL );
    CXnDomList& children = aEventNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();
        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& id = dnode->AttributeValue( KId );
            CXnNode* node = aEngine.FindNodeByIdL( id, aThis->Namespace() );
            if( node && node->Type()->Type() == KTextEditor )
                {
                editorNode = node;
                break;
                }
            }
        }

    if( !editorNode && aLayoutNode.Type()->Type() == KTextEditor )
        {
        editorNode = &aLayoutNode;
        }
    
    if( editorNode )
        {
        XnTextEditorInterface::MXnTextEditorInterface* editorControl = NULL;
        
        XnComponentInterface::MakeInterfaceL( editorControl, editorNode->AppIfL() );
        if( editorControl )
            {
            if( aActivate )
                {
                editorControl->HandleEditorEvent(CXnTextEditor::KActivateTextEditor);
                }
            else
                {
                editorControl->HandleEditorEvent(CXnTextEditor::KDeactivateTextEditor);
                }
            }    
        }
    }

// -----------------------------------------------------------------------------
// RunEditL
// -----------------------------------------------------------------------------
//
static void RunEditL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {        
    aEngine.DisableRenderUiLC();
   
    // Set plugins to edit state
    RPointerArray< CXnPluginData >& views( 
        aEngine.ViewManager()->ActiveAppData().PluginData() );
    
    CXnViewData* view( NULL );
    
    for ( TInt i = 0; i < views.Count(); i++ )
        {
        view = static_cast< CXnViewData* >( views[i] );
        
        CXnPluginData* plugin( view->Plugin( aThis->Node() ) );
        
        if ( plugin && plugin == view )
            {
            break;
            }            
        }
    
    if ( !view )
        {
        return;
        }
    
    RPointerArray< CXnPluginData >& plugins( view->PluginData() );             

    CXnDomStringPool* sp( aEventNode.StringPool() );
    
    CXnDomList& children( aEventNode.ChildNodes() );

    TInt count( children.Length() );

    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();

        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& name = dnode->AttributeValue(
                XnPropertyNames::action::event::systemset::KName );
            const TDesC8& value = dnode->AttributeValue(
                XnPropertyNames::action::event::systemset::KValue );

            if ( name != KNullDesC8 && value != KNullDesC8 )
                {
                // Find node by id
                if ( name == XnPropertyNames::common::KId )
                    {
                    CXnNode* node( aEngine.FindNodeByIdL( value, aThis->Namespace() ) );

                    if ( node )
                        {
                        node->SetStateL( XnPropertyNames::style::common::KEdit );                        
                        }
                    }
                // Find nodes by class
                else if ( name == XnPropertyNames::common::KClass )
                    {
                    CXnPointerArray* array = aEngine.FindNodeByClassL( value, aThis->Namespace() );
                    CleanupStack::PushL( array );

                    if ( array )
                        {
                        const TInt count( array->Container().Count() );

                        for ( TInt i = 0; i < count; ++i )
                            {
                            CXnNode* node( static_cast< CXnNode* >(
                                array->Container()[i] ) );
                                                                                                             
                            CXnProperty* editable( node->GetPropertyL(
                                XnPropertyNames::style::common::edit::KEditable ) );

                            if ( !editable ||
                                 editable->StringValue() != XnPropertyNames::KFalse )
                                {
                                node->SetStateL( XnPropertyNames::style::common::KEdit );                               
                                }
                            }
                        }

                    CleanupStack::PopAndDestroy( array );
                    }
                }
            }
        }
    
    TBool useEmpty( view->UseEmptyWidget() );
    
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnNode* node( plugins[i]->Owner()->LayoutNode() );
        
        node->SetStateL( XnPropertyNames::style::common::KEdit );
                               
        if ( !plugins[i]->Occupied() && useEmpty )
            {                               
            // Make empty space visible
            SetStringPropertyToNodeL( *sp, *node,
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KVisible );                    
            }                                              
        }
    
    aEngine.EditMode()->SetEditModeL( CXnEditMode::EDragAndDrop );        
    aEngine.AppUiAdapter().ViewAdapter().CloseAllPopupsL();

    CleanupStack::PopAndDestroy();
    
    aEngine.AppUiAdapter().ViewAdapter().UpdateRskByUiStateL( *view );
    }

// -----------------------------------------------------------------------------
// RunResetEditL
// -----------------------------------------------------------------------------
//
static void RunResetEditL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {
    // Set plugins to edit state
    RPointerArray< CXnPluginData >& views( 
        aEngine.ViewManager()->ActiveAppData().PluginData() );
    
    CXnViewData* view( NULL );
    
    for ( TInt i = 0; i < views.Count(); i++ )
        {
        view = static_cast< CXnViewData* >( views[i] );
        
        CXnPluginData* plugin( view->Plugin( aThis->Node() ) );
        
        if ( plugin && plugin == view )
            {
            break;
            }            
        }
    
    if ( !view )
        {
        return;
        }
    
    CXnDomList& children( aEventNode.ChildNodes() );

    TInt count( children.Length() );

    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();

        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& name = dnode->AttributeValue(
                XnPropertyNames::action::event::systemset::KName );
            const TDesC8& value = dnode->AttributeValue(
                XnPropertyNames::action::event::systemset::KValue );

            if ( name != KNullDesC8 && value != KNullDesC8 )
                {
                if ( name == XnPropertyNames::common::KId )
                    {
                    CXnNode* node( aEngine.FindNodeByIdL( value, aThis->Namespace() ) );

                    if ( node )
                        {
                        node->UnsetStateL(
                            XnPropertyNames::style::common::KEdit );                        
                        }
                    }
                else if ( name == XnPropertyNames::common::KClass )
                    {
                    CXnPointerArray* array( aEngine.FindNodeByClassL( value, aThis->Namespace() ) );
                    CleanupStack::PushL( array );

                    const TInt count = array->Container().Count();

                    for ( TInt i = 0; i < count; ++i )
                        {
                        CXnNode* node = static_cast< CXnNode* >(
                            array->Container()[i] );

                        if ( node &&
                             node->IsStateSet( XnPropertyNames::style::common::KEdit ) )
                            {
                            node->UnsetStateL( XnPropertyNames::style::common::KEdit );                            
                            }
                        }

                    CleanupStack::PopAndDestroy( array );
                    }
                }
            }
        }
    
    RPointerArray< CXnPluginData>& plugins( view->PluginData() );             
    
    CXnDomStringPool* sp( aEventNode.StringPool() );

    TBool useEmpty( view->UseEmptyWidget() );
    
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnNode* node( plugins[i]->Owner()->LayoutNode() );
        
        node->UnsetStateL( XnPropertyNames::style::common::KEdit );
        
        if ( !plugins[i]->Occupied() && useEmpty )
            {                               
            // Make empty space blank
            SetStringPropertyToNodeL( *sp, *node,
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KBlank );                    
            }
        }                
    
    aEngine.EditMode()->SetEditModeL( CXnEditMode::ENone );   

    aEngine.AppUiAdapter().ViewAdapter().UpdateRskByUiStateL( *view );
    }

// -----------------------------------------------------------------------------
// RunDeactivateL
// -----------------------------------------------------------------------------
//
static void RunDeactivateL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {
    CXnDomList& children = aEventNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* dnode = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = dnode->Name();
        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& id = dnode->AttributeValue( KId );
            CXnNode* node = aEngine.FindNodeByIdL( id, aThis->Namespace() );
            if ( node )
                {
                node->UnsetStateL( XnPropertyNames::style::common::KActive );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunSystemSetPCDataL
// -----------------------------------------------------------------------------
//
static void RunSystemSetPCDataL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    const TDesC8& aId,
    const TDesC8& aData )
    {
    CXnNode* node = aEngine.FindNodeByIdL( aId, aThis->Namespace() );

    if ( node )
        {
        node->SetPCDataL( aData );
        }
    }

// -----------------------------------------------------------------------------
// GetNumberValues
// -----------------------------------------------------------------------------
//
static void GetNumberValues(
    const TDesC8& aData,
    TReal& aNumber,
    CXnDomPropertyValue::TPrimitiveValueType& aValueType )
    {
    TLex8 lex( aData );
    lex.Val( aNumber );
    TPtrC8 rest = lex.Remainder();
    if ( rest.FindF( _L8( "%" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EPercentage;
        }
    else if (rest.FindF( _L8( "em" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EEms;
        }
    else if ( rest.FindF( _L8( "ex" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EExs;
        }
    else if ( rest.FindF( _L8( "px" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EPx;
        }
    else if ( rest.FindF( _L8( "u" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EUnitValue;
        }
    else if ( rest.FindF( _L8( "cm" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::ECm;
        }
    else if ( rest.FindF( _L8( "mm" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EMm;
        }
    else if ( rest.FindF( _L8( "in" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EIn;
        }
    else if ( rest.FindF( _L8( "pt" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EPt;
        }
    else if ( rest.FindF( _L8( "pc" ) ) != KErrNotFound )
        {
        aValueType = CXnDomPropertyValue::EPc;
        }
    else
        {
        aValueType = CXnDomPropertyValue::ENumber;
        }
    }

// -----------------------------------------------------------------------------
// RunSystemSetNumberL
// -----------------------------------------------------------------------------
//
static void RunSystemSetNumberL(
    CXnUiEngine& aEngine,
    CXnNode* aNode,
    const TDesC8& aName,
    const RPointerArray< HBufC8 >& aValues )
    {
    if ( aNode )
        {
        CXnDomStringPool* sp = aEngine.ODT()->DomDocument().StringPool();
        CXnDomProperty* prop = CXnDomProperty::NewL( aName, sp );
        CleanupStack::PushL( prop );

        for ( TInt i = 0; i < aValues.Count(); i++ )
            {
            const TDesC8& value = ( *aValues[i] );

            CXnDomPropertyValue* propVal = CXnDomPropertyValue::NewL( sp );
            CleanupStack::PushL( propVal );

            CXnDomPropertyValue::TPrimitiveValueType type;
            TReal realValue;

            GetNumberValues( value, realValue, type );
            propVal->SetFloatValueL( type, realValue );

            prop->PropertyValueList().AddItemL( propVal );
            CleanupStack::Pop( propVal );
            }

        CXnProperty* propertyValue = CXnProperty::NewL( prop );

        CleanupStack::Pop( prop );
        CleanupStack::PushL( propertyValue );

        aNode->SetPropertyL( propertyValue );

        CleanupStack::Pop( propertyValue );
        }
    }

// -----------------------------------------------------------------------------
// RunSystemSetRGBL
// -----------------------------------------------------------------------------
//
static void RunSystemSetRGBL(
    CXnUiEngine& aEngine,
    CXnNode* aNode,
    const TDesC8& aName,
    const RPointerArray< HBufC8 >& aValues )
    {
    if ( aNode )
        {
        CXnDomStringPool* sp = aEngine.ODT()->DomDocument().StringPool();
        CXnDomProperty* prop = CXnDomProperty::NewL( aName, sp );
        CleanupStack::PushL( prop );

        for ( TInt i = 0; i < aValues.Count(); i++ )
            {
            const TDesC8& value = ( *aValues[i] );

            CXnDomPropertyValue* propVal = CXnDomPropertyValue::NewL( sp );
            CleanupStack::PushL( propVal );

            TRgb rgb;

            if ( CXnUtils::GetRgbValue( rgb, value ) )
                {
                propVal->SetRgbColorL( rgb );
                prop->PropertyValueList().AddItemL( propVal );
                CleanupStack::Pop( propVal );
                }
            else
                {
                CleanupStack::PopAndDestroy( propVal );
                }
            }

        CXnProperty* propertyValue = CXnProperty::NewL( prop );

        CleanupStack::Pop( prop );
        CleanupStack::PushL( propertyValue );

        aNode->SetPropertyL( propertyValue );

        CleanupStack::Pop( propertyValue );
        }
    }

// -----------------------------------------------------------------------------
// RunSystemSetStringL
// -----------------------------------------------------------------------------
//
static void RunSystemSetStringL(
    CXnUiEngine& aEngine,
    CXnNode* aNode,
    const TDesC8& aName,
    const RPointerArray< HBufC8 >& aValues,
    const CXnDomPropertyValue::TPrimitiveValueType aValueType )
    {
    if ( aNode )
        {
        CXnDomStringPool* sp = aEngine.ODT()->DomDocument().StringPool();
        CXnDomProperty* prop = CXnDomProperty::NewL( aName, sp );
        CleanupStack::PushL( prop );

        for ( TInt i = 0; i < aValues.Count(); i++ )
            {
            const TDesC8& value = ( *aValues[i] );

            CXnDomPropertyValue* propVal = CXnDomPropertyValue::NewL( sp );
            CleanupStack::PushL( propVal );

            propVal->SetStringValueL( aValueType, value );
            prop->PropertyValueList().AddItemL( propVal );
            CleanupStack::Pop( propVal );
            }

        CXnProperty* propertyValue = CXnProperty::NewL( prop );

        CleanupStack::Pop( prop );
        CleanupStack::PushL( propertyValue );

        aNode->SetPropertyL( propertyValue );

        CleanupStack::Pop( propertyValue );
        }
    }

// -----------------------------------------------------------------------------
// GetSystemSetDataL
// -----------------------------------------------------------------------------
//
static void GetSystemSetDataL(
    CXnDomNode& aDomNode,
    HBufC8*& aId,
    const TDesC8** aPseudoClass,
    const TDesC8** aName,
    RPointerArray< HBufC8 >* aValues,
    const TDesC8** aType,
    TBool aSettings,
    const TDesC8& aProvidedId,
    const TDesC8** aClassId )
    {
    CXnDomList& children = aDomNode.ChildNodes();

    if ( aSettings )
        {
        aId = aProvidedId.AllocL();
        }

    TInt count = children.Length();

    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& nodeType = node->Name();

        if ( nodeType == XnPropertyNames::action::KProperty )
            {
            const TDesC8& name = node->AttributeValue(
                XnPropertyNames::action::event::systemset::KName );
            const TDesC8& value = node->AttributeValue(
                XnPropertyNames::action::event::systemset::KValue );

            if ( !aSettings &&
                 ( name == XnPropertyNames::action::event::systemset::KId ) )
                {
                HBufC8* utfName = HBufC8::NewL( value.Length() );
                TPtr8 ptr = utfName->Des();
                ptr = value;
                aId = utfName;
                }
            else if ( name == XnPropertyNames::action::event::systemset::KPseudoClass )
                {
                *aPseudoClass = &value;
                }
            else if ( name == XnPropertyNames::action::event::systemset::KName )
                {
                *aName = &value;
                }
            else if ( name == XnPropertyNames::action::event::systemset::KValue )
                {
                if ( aValues )
                    {
                    HBufC8* valueH = HBufC8::NewLC( value.Length() + KUnitMaxLen );
                    TPtr8 ptr( valueH->Des() );
                    ptr.Append( value );
                    aValues->AppendL( valueH );     //take ownership
                    CleanupStack::Pop( valueH );
                    }
                }
            else if ( name == XnPropertyNames::action::event::systemset::KType )
                {
                *aType = &value;
                }
            else if ( name == XnPropertyNames::action::event::systemset::KUnit )
                {
                if ( value.Length() > KUnitMaxLen )
                    {
#ifdef _XN_DEBUG_
                    RDebug::Print( _L( "xnnodeimpl GetSystemSetDataL - unexpected length of type string" ) );
#endif
                    User::Leave( KErrNotSupported );
                    }
                if ( aValues )
                    {
                    TInt index = aValues->Count() - 1;
                    HBufC8* valueH = ( *aValues )[index];
                    TPtr8 ptr( valueH->Des() );
                    ptr.Append( value );
                    }
                }
            else if ( name == XnPropertyNames::action::event::systemset::KClass )
                {
                *aClassId = &value;
                }
            else
                {
                // Everyting is within the same property tag
                // e.g. <property name="display" value="block" type="string" id="my_id"/>

                // Save name...
                *aName = &name;

                // ...and value
                if ( aValues )
                    {
                    HBufC8* valueH = HBufC8::NewLC( value.Length() + KUnitMaxLen );
                    TPtr8 ptr( valueH->Des() );
                    ptr.Append( value );
                    aValues->AppendL( valueH );     //take ownership
                    CleanupStack::Pop( valueH );
                    }
                
                // ...and type
                const TDesC8& type = node->AttributeValue(
                    XnPropertyNames::action::event::systemset::KType );
                *aType = &type;

                // ...and id
                const TDesC8& id = node->AttributeValue(
                    XnPropertyNames::action::event::systemset::KId );
                HBufC8* utfName = HBufC8::NewL( id.Length() );
                TPtr8 ptr = utfName->Des();
                ptr = id;
                aId = utfName;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunTryDisplayingMenuL
// -----------------------------------------------------------------------------
//
static void RunTryDisplayingMenuL( CXnUiEngine& aEngine, CXnDomNode& aEventNode )
    {
    CXnNode* menubar( aEngine.MenuBarNode() );

    if ( menubar )
        {
        CXnDomList& children( aEventNode.ChildNodes() );

        TInt count( children.Length() );

        for ( TInt i = 0; i < count; ++i )
            {
            CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );

            const TDesC8& type( node->Name() );

            if ( type == XnPropertyNames::action::KProperty )
                {
                const TDesC8& id( node->AttributeValue( XnPropertyNames::action::KValue ) );

                if ( id != KNullDesC8 )
                    {
                    HBufC* menuId( CnvUtfConverter::ConvertToUnicodeFromUtf8L( id ) );
                    CleanupStack::PushL( menuId );

                    XnMenuInterface::MXnMenuInterface* menuIf( NULL );
                    XnComponentInterface::MakeInterfaceL( menuIf, menubar->AppIfL() );

                    if ( menuIf )
                        {
                        aEngine.Editor()->SetTargetPlugin( aEngine.FocusedNode() );
                        menuIf->TryDisplayingMenuBarL( *menuId, EFalse );
                        }

                    CleanupStack::PopAndDestroy( menuId );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunTryDisplayingListQueryDialogL
// -----------------------------------------------------------------------------
//
static void RunTryDisplayingListQueryDialogL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {
    CXnDomList& children( aEventNode.ChildNodes() );

    TInt count( children.Length() );

    if ( count > 0 )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( 0 ) );

        const TDesC8& type( node->Name() );

        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& id(
                node->AttributeValue( XnPropertyNames::action::KValue ) );

            if ( id != KNullDesC8 )
                {
                CXnNode* dialog( aEngine.FindNodeByIdL( id, aThis->Namespace() ) );

                if ( dialog && dialog->Type()->Type() == XnPropertyNames::listquerydialog::KListQueryDialog )
                    {
                    CXnListQueryDialogAdapter* adapter = 
                        static_cast< CXnListQueryDialogAdapter* > ( dialog->Control() );                
                    
                    adapter->TryDisplayingDialogL();
                    }
                
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunTryDisplayingStylusPopupL
// -----------------------------------------------------------------------------
//
static void RunTryDisplayingStylusPopupL(
    CXnNodeImpl* aThis,
    CXnNode& aNode,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode )
    {    
    CXnDomList& children( aEventNode.ChildNodes() );

    TInt count( children.Length() );

    if ( count > 0 )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( 0 ) );

        const TDesC8& type( node->Name() );

        if ( type == XnPropertyNames::action::KProperty )
            {
            const TDesC8& id( node->AttributeValue(
                XnPropertyNames::action::KValue ) );

            if ( id != KNullDesC8 )
                {
                CXnNode* popup( aEngine.FindNodeByIdL( id, aThis->Namespace() ) );

                if ( popup && popup->Type()->Type() == _L8("styluspopup") )
                    {
                    CXnPopupControlAdapter* adapter = 
                        static_cast< CXnPopupControlAdapter* > ( popup->Control() );                
                    
                    adapter->TryDisplayingStylusPopupL( aNode );                                       
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// RunSetInitialFocusL
// -----------------------------------------------------------------------------
//
static void RunSetInitialFocusL( CXnUiEngine& aEngine )
    {    
    TBool editState( aEngine.EditMode()->EditState() );
    
    CXnViewManager* manager( aEngine.ViewManager() );
        
    RPointerArray< CXnNode >& pluginNodes( manager->PluginNodes() );

    if ( editState )
        {
        TBool pluginFocused( EFalse );
        
        for ( TInt i = 0; i < pluginNodes.Count(); i++ )
            {
            CXnNode* node( pluginNodes[i] );
            
            if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
                {
                pluginFocused = ETrue;
                break;
                }
            }
        
        if ( !pluginFocused )
            {
            for ( TInt i = 0; i < pluginNodes.Count(); i++ )
                {
                CXnNode* node( pluginNodes[i] );
                
                node->SetStateWithoutNotificationL(
                    XnPropertyNames::style::common::KFocus );
    
                if ( node->IsStateSet( 
                    XnPropertyNames::style::common::KFocus ) )
                    {
                    break;
                    }
                }
            }
        }
    else
        {
        CXnViewData& viewData( manager->ActiveViewData() );
        
        RPointerArray< CXnPluginData > plugins;
        CleanupClosePushL( plugins );

        for( TInt i = 0; i < pluginNodes.Count(); i++ )
            {
            CXnPluginData* data( viewData.Plugin( pluginNodes[i] ) );
            
            if( data && data->Occupied() )
                {
                plugins.AppendL( data );
                }
            }
        
        plugins.Append( &viewData );
            
        RPointerArray< CXnNode > list;
        CleanupClosePushL( list );

        for( TInt i = 0; i < plugins.Count(); i++ )
            {
            plugins[i]->InitialFocusNodesL( list );
            }
               
        for ( TInt i = 0; i < list.Count(); i++ )
            {
            CXnNode* node( list[i] );

            node->SetStateWithoutNotificationL(
                XnPropertyNames::style::common::KFocus );

            if ( node->IsStateSet( XnPropertyNames::style::common::KFocus ) )
                {
                break;
                }
            }

        CleanupStack::PopAndDestroy( 2, &plugins ); // list         
        }
    }

// -----------------------------------------------------------------------------
// RunSystemSetL
// -----------------------------------------------------------------------------
//
static void RunSystemSetL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnDomNode& aEventNode,
    TBool aSettings,
    const TDesC8& aId )
    {
    HBufC8* id = NULL;
    const TDesC8* pseudoClass = NULL;
    const TDesC8* name = NULL;
    const TDesC8* type = NULL;
    const TDesC8* classId = NULL;

    RPointerArray< HBufC8 > values;
    CleanupResetAndDestroyPushL( values );

    GetSystemSetDataL(
        aEventNode, id, &pseudoClass, &name,
        &values, &type, aSettings, aId, &classId );

    CleanupStack::PushL( id );

    if ( !name && id )
        {
        for ( TInt i = 0; i < values.Count(); i++ )
            {
            const TDesC8* value = values[i];

            RunSystemSetPCDataL( aThis, aEngine, *id, *value );
            }
        }
    else if ( name && id && type )
        {
        CXnNode* node = aEngine.FindNodeByIdL( *id, aThis->Namespace() );

        if ( *type == XnPropertyNames::action::event::systemset::type::KString )
            {
            RunSystemSetStringL(
                aEngine, node, *name, values, CXnDomPropertyValue::EString );
            }
        else if ( *type ==
                  XnPropertyNames::action::event::systemset::type::KNumeric )
            {
            RunSystemSetNumberL(
                aEngine, node, *name, values );
            }
        else if ( *type == XnPropertyNames::action::event::systemset::type::KRGB )
            {
            RunSystemSetRGBL(
                aEngine, node, *name, values );
            }
        else if ( *type == XnPropertyNames::action::event::systemset::type::KUrl )
            {
            RunSystemSetStringL(
                aEngine, node, *name, values, CXnDomPropertyValue::EUri );
            }
        else if ( *type ==
                  XnPropertyNames::action::event::systemset::type::KAttribute )
            {
            RunSystemSetStringL(
                aEngine, node, *name, values, CXnDomPropertyValue::EAttr );
            }
        }
    else if ( name && classId && type )
        {
        CXnPointerArray* array = aEngine.FindNodeByClassL( *classId, aThis->Namespace() );
        CleanupStack::PushL( array );

        const TInt count = array->Container().Count();

        for ( TInt i = 0; i < count; ++i )
            {
            CXnNode* node = static_cast< CXnNode* >(array->Container()[i]);
            if ( *type == XnPropertyNames::action::event::systemset::type::KString )
                {
                RunSystemSetStringL(
                    aEngine, node, *name, values, CXnDomPropertyValue::EString );
                }
            else if ( *type ==
                      XnPropertyNames::action::event::systemset::type::KNumeric )
                {
                RunSystemSetNumberL(
                    aEngine, node, *name, values );
                }
            else if ( *type == XnPropertyNames::action::event::systemset::type::KRGB )
                {
                RunSystemSetRGBL(
                    aEngine, node, *name, values );
                }
            else if ( *type == XnPropertyNames::action::event::systemset::type::KUrl )
                {
                RunSystemSetStringL(
                    aEngine, node, *name, values, CXnDomPropertyValue::EUri );
                }
            else if ( *type ==
                      XnPropertyNames::action::event::systemset::type::KAttribute )
                {
                RunSystemSetStringL(
                    aEngine, node, *name, values, CXnDomPropertyValue::EAttr );
                }
            }
        CleanupStack::PopAndDestroy( array );
        }

    CleanupStack::PopAndDestroy( 2, &values );
    }

// -----------------------------------------------------------------------------
// RunEventL
// -----------------------------------------------------------------------------
//
static TBool RunEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnDomNode& aEventNode,
    CXnDomNode& aTriggerNode,
    CXnNode& aEventData )
    {
    const TDesC8& nameString( aEventNode.AttributeValue( KName ) );

    if ( nameString == KNullDesC8 )
        {
        return EFalse;
        }

    const TDesC8& system( XnPropertyNames::action::event::KSystemPrefix() );
    
    if ( nameString.Find( system ) != 0 )
        {
        // event doesn't contain "system/" prefix         
        RunAppUiNotificationL( aEngine, aNode, 
            aEventNode, aTriggerNode, aEventData );
        }    
    else if ( nameString == XnPropertyNames::action::event::KSetFocus )
        {
        RPointerArray< CXnNode > focusCandidates;
        CleanupClosePushL( focusCandidates );

        GetFocusCandidatesL( aThis, aEngine, aEventNode, focusCandidates );

        TBool retval( aNode.RunFocusChangeL( focusCandidates ) );

        CleanupStack::PopAndDestroy( &focusCandidates );

        return retval;
        }
    else if ( nameString == XnPropertyNames::action::event::KSetInitialFocus )
        {
        RunSetInitialFocusL( aEngine );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KSet )
        {
        RunSystemSetL( aThis, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KSetPassiveFocus ||
              nameString == XnPropertyNames::action::event::KResetPassiveFocus ||
              nameString == XnPropertyNames::action::event::KSetActiveFocus )
        {
        RunPassiveFocusChangeL( aThis, aEngine, aEventNode, nameString );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KClearPassiveFocuses )
        {
        aEngine.ClearPassiveFocusedNodesL();
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KSetEditMode )
        {
        RunEditL( aThis, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KResetEditMode )
        {
        RunResetEditL( aThis, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KRunAddWidgetQuery )
        {
        aEngine.Editor()->AddWidgetL();
        }
    else if ( nameString == XnPropertyNames::action::event::KRemoveFocusedWidget )
        {
        aEngine.Editor()->RemoveWidgetL( aEngine.FocusedNode() );               
        }
    else if ( nameString == XnPropertyNames::action::event::KActivateNextView )
        {
        RunActivateNextViewL( aEngine, aEventNode );
        }
    else if ( nameString == XnPropertyNames::action::event::KActivatePreviousView )
        {
        RunActivatePreviousViewL( aEngine, aEventNode );
        }
    else if ( nameString == XnPropertyNames::action::event::KAddView )
        {
        RunAddViewL( aEngine, aEventNode );
        }
    else if ( nameString == XnPropertyNames::action::event::KRemoveView )
        {
        RunRemoveViewL( aEngine, aEventNode );
        }
    else if ( nameString == XnPropertyNames::action::event::KActivate )
        {
        RunActivateL( aThis, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KActivateSelectedItem )
        {
        CXnNode* focusedNode( aEngine.FocusedNode() );
        
        if ( focusedNode )
            {
            aEngine.AppUiAdapter().ItemActivator().ActivateL( focusedNode );            
            }
        
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KDeactivate )
        {
        RunDeactivateL( aThis, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KTryDisplayingMenu )
        {
        RunTryDisplayingMenuL( aEngine, aEventNode );
        }
    else if ( nameString ==
              XnPropertyNames::action::event::KTryDisplayingListQueryDialog )
        {
        RunTryDisplayingListQueryDialogL( aThis, aEngine, aEventNode );
        }
    else if ( nameString == XnPropertyNames::action::event::KTryDisplayingStylusPopup )
        {
        RunTryDisplayingStylusPopupL( aThis, aNode, aEngine, aEventNode );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KToggleWidgetsVisibilty )
        {
        aEngine.Editor()->ToggleWidgetsVisibiltyL();

        // check if focused widget need to be changed
        CXnNode* focused( aEngine.FocusedNode() );

        if ( !focused || !IsNodeNavigableL( focused ) )
            {
            RunSetInitialFocusL( aEngine );
            }

        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KReportEnterEditMode ) 
            
        {
        RunFullScreenEffectL( aEngine, aEventNode );
        
        CXnNode* trigger( BuildTriggerNodeL( aEngine,                    
            XnPropertyNames::action::trigger::name::KEditMode ) );
        CleanupStack::PushL( trigger );

        CXnDomStringPool* sp( aEngine.ODT()->DomDocument().StringPool() );
        
        CXnProperty* value( CXnProperty::NewL(
            XnPropertyNames::action::KValue,
            XnPropertyNames::action::trigger::name::editmode::KEnter, 
            CXnDomPropertyValue::EString, *sp ) );
        CleanupStack::PushL( value );
        
        trigger->SetPropertyL( value );
        CleanupStack::Pop( value );

        aEngine.ActiveView()->ReportXuikonEventL( *trigger );
        
        CleanupStack::PopAndDestroy( trigger );

        return ETrue;
        }
    else if( nameString == XnPropertyNames::action::event::KReportExitEditMode )
        {
        RunFullScreenEffectL( aEngine, aEventNode );

        CXnNode* trigger( BuildTriggerNodeL( aEngine,                    
            XnPropertyNames::action::trigger::name::KEditMode ) );
        CleanupStack::PushL( trigger );
        
        CXnDomStringPool* sp( aEngine.ODT()->DomDocument().StringPool() );
        
        CXnProperty* value( CXnProperty::NewL(
            XnPropertyNames::action::KValue,
            XnPropertyNames::action::trigger::name::editmode::KExit, 
            CXnDomPropertyValue::EString, *sp ) );
        CleanupStack::PushL( value );
        
        trigger->SetPropertyL( value );
        CleanupStack::Pop( value );
        
        aEngine.ActiveView()->ReportXuikonEventL( *trigger );
        
        CleanupStack::PopAndDestroy( trigger );
                
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KSetWallpaper )
        {
        aEngine.AppUiAdapter().ViewAdapter().BgManager().SetWallpaperL();
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KActivateTextEditor)
        {
        RunActivateEditorL( aThis, aNode, aEngine, aEventNode, ETrue );
        return ETrue;
        }
    else if ( nameString == XnPropertyNames::action::event::KDeactivateTextEditor)
        {
        RunActivateEditorL( aThis, aNode, aEngine, aEventNode, EFalse );
        return ETrue;
        }    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// RunEventsL
// -----------------------------------------------------------------------------
//
static void RunEventsL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnDomNode& aActionNode,
    CXnDomNode& aTriggerNode,
    CXnNode& aEventData )
    {
    CXnDomList& children = aActionNode.ChildNodes();
    TInt count = children.Length();
    TBool render = EFalse;
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >(children.Item( i ) );
        const TDesC8& type = node->Name();
        if ( type == XnPropertyNames::action::KEvent )
            {
            TBool tmp = RunEventL(
                aThis, aEngine, aNode, *node, aTriggerNode, aEventData );
            if ( !render )
                {
                render = tmp;
                }
            }
        }
    if ( render )
        {
        aEngine.RenderUIL( NULL );
        }
    }

// -----------------------------------------------------------------------------
// DoMatchTriggerForHighLevelKeyEventL
// -----------------------------------------------------------------------------
//
static TBool DoMatchTriggerForHighLevelKeyEventL(
    CXnNode& aNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    const TDesC8& aHighLevelKey )
    {
    const TDesC8* key;

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            key = &XnPropertyNames::action::trigger::name::KLeft;
            break;
        case EKeyRightArrow:
            key = &XnPropertyNames::action::trigger::name::KRight;
            break;
        case EKeyUpArrow:
            key = &XnPropertyNames::action::trigger::name::KUp;
            break;
        case EKeyDownArrow:
            key = &XnPropertyNames::action::trigger::name::KDown;
            break;
        default:
            key = NULL;
            break;
        }

    if ( !key || aType != EEventKey )
        {
        return EFalse;
        }

    CXnNode* parent( aNode.Parent() );

    if ( !parent || aNode.ViewNodeImpl() )
        {
        if ( *key == aHighLevelKey )
            {
            return ETrue;
            }
        }
    else
        {
        const TDesC8* direction(
            &XnPropertyNames::style::common::direction::KLTR );
        const TDesC8* orientation(
            &XnPropertyNames::style::common::block_progression::KTB );

        CXnProperty* directionProperty( parent->DirectionL() );

        if ( directionProperty )
            {
            direction = &directionProperty->StringValue();
            }

        CXnProperty* orientationProperty( parent->BlockProgressionL() );

        if ( orientationProperty )
            {
            orientation = &orientationProperty->StringValue();
            }

        if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
             ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
               *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
            {
            if ( *key == aHighLevelKey )
                {
                return ETrue;
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                  ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
                    *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
            {
            if ( *key == XnPropertyNames::action::trigger::name::KRight &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KLeft )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KLeft &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KRight )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KDown &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KDown )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KUp &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KUp )
                {
                return ETrue;
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
                  *orientation == XnPropertyNames::style::common::block_progression::KTB )
            {
            if ( *key == XnPropertyNames::action::trigger::name::KUp &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KLeft )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KDown &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KRight )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KLeft &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KDown )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KRight &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KUp )
                {
                return ETrue;
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                  *orientation == XnPropertyNames::style::common::block_progression::KTB )
            {
            if ( *key == XnPropertyNames::action::trigger::name::KDown &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KLeft )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KUp &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KRight )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KLeft &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KDown )
                {
                return ETrue;
                }

            if ( *key == XnPropertyNames::action::trigger::name::KRight &&
                 aHighLevelKey == XnPropertyNames::action::trigger::name::KUp )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// MatchActionForKeyEventL
// -----------------------------------------------------------------------------
//
static TBool MatchActionForKeyEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    const TKeyEvent& aKeyEvent, TEventCode aType,
    CXnDomNode& aActionNode )
    {
    CXnDomList& children = aActionNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = node->Name();
        if ( type == XnPropertyNames::action::KTrigger )
            {
            if ( node && ( IsTriggerRunnableL( *node ) || IsTriggerActiveL( *node ) ) )
                {
                TBool match = MatchTriggerForKeyEventL(
                    aThis, aEngine, aNode, aKeyEvent, aType, aActionNode, *node );
                if ( match )
                    {
                    return match;
                    }
                }
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsTriggerActiveL
// -----------------------------------------------------------------------------
//
static TBool IsTriggerActiveL( CXnDomNode& aNode )
    {
    CXnDomList& propList = aNode.PropertyList();
    TInt count = propList.Length();
    for ( TInt j = 0; j < count; j++ )
        {
        CXnDomProperty* prop = static_cast< CXnDomProperty* >( propList.Item( j ) );
        const TDesC8& propName = prop->Name();
        if ( propName == XnPropertyNames::style::common::KDisplay )
            {
            CXnDomList& propValList = prop->PropertyValueList();
            TInt propValCount(propValList.Length());
            for ( TInt k = 0; k < propValCount; k++ )
                {
                CXnDomPropertyValue* propVal =
                    static_cast< CXnDomPropertyValue* >( propValList.Item( k ) );
                CXnDomPropertyValue::TPrimitiveValueType type =
                    propVal->PrimitiveValueType();
                switch ( type )
                    {
                    case CXnDomPropertyValue::EString:
                    case CXnDomPropertyValue::EIdent:
                    case CXnDomPropertyValue::EUri:
                    case CXnDomPropertyValue::EAttr:
                    case CXnDomPropertyValue::EUnknown:
                        {
                        const TDesC8& value = propVal->StringValueL();
                        if ( value == XnPropertyNames::style::common::display::KNone )
                            {
                            return EFalse;
                            }
                        else
                            {
                            return ETrue;
                            }
                        }
                    }
                }
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// MatchActionForEventL
// -----------------------------------------------------------------------------
//
static TBool MatchActionForEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnNode& aEventData,
    CXnDomNode& aActionNode,
    TInt aSource )
    {
    CXnDomList& children = aActionNode.ChildNodes();
    TInt count = children.Length();
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = node->Name();
        if ( type == XnPropertyNames::action::KTrigger &&
             node && IsTriggerRunnableL( *node ) )
            {
            TBool match = MatchTriggerForEventL(
                aThis, aEngine, aNode, aEventData, aActionNode, *node, aSource );
            if ( match )
                {
                return match;
                }
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// FindLoopedFocusableNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* FindLoopedFocusableNodeL( CXnNode& aNode, TBool aForward )
    {
    if ( aNode.ViewNodeImpl() )
        {
        return NULL;
        }
    CXnNode* nextNode = NULL;
    CXnProperty* navLoopProperty = aNode.GetPropertyL(
        XnPropertyNames::style::common::KS60NavLooping );
    if ( navLoopProperty )
        {
        const TDesC8& loopValue = navLoopProperty->StringValue();
        if ( loopValue != KNullDesC8 &&
             loopValue == XnPropertyNames::style::common::s60_focus_looping::KStop )
            {
            return NULL;
            }
        }
    if ( aForward )
        {
        nextNode = ForwardLoopL( aNode );
        }
    else
        {
        nextNode = BackwardLoopL( aNode );
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// GetNavIndexL
// -----------------------------------------------------------------------------
//
static CXnDomPropertyValue* GetNavIndexL( CXnNode& aNode )
    {
    CXnProperty* navIndexProperty = aNode.NavIndexL();
    CXnDomPropertyValue* value = NULL;
    if ( navIndexProperty )
        {
        value = static_cast< CXnDomPropertyValue* >(
            navIndexProperty->Property()->PropertyValueList().Item( 0 ) );
        }
    return value;
    }

// -----------------------------------------------------------------------------
// ForwardL
// -----------------------------------------------------------------------------
//
static CXnNode* ForwardL( CXnNode& aNode )
    {
    RPointerArray< CXnNode >& siblings = aNode.Parent()->Children();
    CXnNode* nextNode = NULL;
    CXnDomPropertyValue* navIndexValue = GetNavIndexL( aNode );
    CXnDomPropertyValue* prevIndexValue = NULL;
    CXnDomPropertyValue* tmpIndexValue = NULL;
    TInt count = siblings.Count();
    TInt nodePosition = count;
    TBool first = ETrue;
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* tmpNode = siblings[i];
        if ( tmpNode == &aNode )
            {
            nodePosition = i;
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        tmpIndexValue = GetNavIndexL( *tmpNode );
        if ( tmpIndexValue )
            {
            if ( tmpIndexValue->IsNoneIdent() )
                {
                // nav index set as none, skipping
                continue;
                }
            }
        if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) == 0 ) &&
             ( first ||
               ( CompareNavIndexesL( tmpIndexValue, prevIndexValue ) < 0 ) ) &&
             ( i > nodePosition ) )
            {
            first = EFalse;
            prevIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        else if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) > 0 ) &&
                  ( first ||
                    ( CompareNavIndexesL( tmpIndexValue, prevIndexValue ) < 0 ) ) )
            {
            first = EFalse;
            prevIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// ForwardLoopL
// -----------------------------------------------------------------------------
//
static CXnNode*  ForwardLoopL( CXnNode& aNode )
    {
    RPointerArray< CXnNode >& siblings = aNode.Parent()->Children();
    CXnNode* nextNode = NULL;
    CXnDomPropertyValue* navIndexValue = GetNavIndexL( aNode );
    CXnDomPropertyValue* tmpIndexValue = NULL;
    CXnDomPropertyValue* prevIndexValue = NULL;
    TInt count = siblings.Count();
    TInt nodePosition = count;
    TBool first = ETrue;
    for ( TInt i = 0; i < count; ++i )
        {
        CXnNode* tmpNode = siblings[i];
        if ( tmpNode == &aNode )
            {
            nodePosition = i;
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        tmpIndexValue = GetNavIndexL( *tmpNode );
        if ( tmpIndexValue )
            {
            if ( tmpIndexValue->IsNoneIdent() )
                {
                // nav index set as none, skipping
                continue;
                }
            }
       if ( ( CompareNavIndexesL( navIndexValue, tmpIndexValue ) == 0 ) &&
              ( first ||
                ( CompareNavIndexesL( tmpIndexValue, prevIndexValue ) < 0 ) ) &&
              ( i < nodePosition ) )
            {
            first = EFalse;
            prevIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        else if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) < 0 ) &&
                  ( first ||
                    ( CompareNavIndexesL( tmpIndexValue, prevIndexValue ) < 0 ) ) )
            {
            first = EFalse;
            prevIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// BackwardL
// -----------------------------------------------------------------------------
//
static CXnNode* BackwardL( CXnNode& aNode )
    {
    RPointerArray< CXnNode >& siblings = aNode.Parent()->Children();
    CXnNode* nextNode = NULL;
    CXnDomPropertyValue* navIndexValue = GetNavIndexL( aNode );
    CXnDomPropertyValue* tmpIndexValue = NULL;
    TInt count = siblings.Count();
    CXnDomPropertyValue* nextIndexValue = GetNavIndexL( *siblings[count - 1] );
    TInt nodePosition = 0;
    TBool first = ETrue;
    for ( TInt i = count - 1; i > -1; --i )
        {
        CXnNode* tmpNode = siblings[i];
        if ( tmpNode == &aNode )
            {
            nodePosition = i;
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        tmpIndexValue = GetNavIndexL( *tmpNode );
        if ( tmpIndexValue )
            {
            if ( tmpIndexValue->IsNoneIdent() )
                {
                // nav index set as none, skipping
                continue;
                }
            }
       if ( ( CompareNavIndexesL( navIndexValue, tmpIndexValue ) == 0 ) &&
             ( first ||
               ( CompareNavIndexesL( tmpIndexValue, nextIndexValue ) > 0 ) ) &&
               ( i < nodePosition ) )
            {
            first = EFalse;
            nextIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
       else if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) < 0 ) &&
                 ( first ||
                   ( CompareNavIndexesL( tmpIndexValue, nextIndexValue ) > 0 ) ) )
            {
            first = EFalse;
            nextIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// BackwardLoopL
// -----------------------------------------------------------------------------
//
static CXnNode* BackwardLoopL( CXnNode& aNode )
    {
    RPointerArray<CXnNode>& siblings = aNode.Parent()->Children();
    CXnNode* nextNode = NULL;
    CXnDomPropertyValue* navIndexValue = GetNavIndexL( aNode );
    CXnDomPropertyValue* tmpIndexValue = NULL;
    TInt count = siblings.Count();
    CXnDomPropertyValue* nextIndexValue = GetNavIndexL( *siblings[count - 1] );
    TInt nodePosition = 0;
    TBool first = ETrue;
    for ( TInt i = count - 1; i > -1; --i )
        {
        CXnNode* tmpNode = siblings[i];
        if ( tmpNode == &aNode )
            {
            nodePosition = i;
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        tmpIndexValue = GetNavIndexL( *tmpNode );
        if ( tmpIndexValue )
            {
            if ( tmpIndexValue->IsNoneIdent() )
                {
                // nav index set as none, skipping
                continue;
                }
            }
       if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) == 0 ) &&
             ( first ||
               ( CompareNavIndexesL( tmpIndexValue, nextIndexValue ) > 0 ) ) &&
             ( i > nodePosition ) )
            {
            first = EFalse;
            nextIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
       else if ( ( CompareNavIndexesL( tmpIndexValue, navIndexValue ) > 0 ) &&
                 ( first ||
                   ( CompareNavIndexesL( tmpIndexValue, nextIndexValue ) > 0 ) ) )
            {
            first = EFalse;
            nextIndexValue = tmpIndexValue;
            nextNode = tmpNode;
            }
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// CompareNavIndexesL
// -----------------------------------------------------------------------------
//
static TInt CompareNavIndexesL(
    CXnDomPropertyValue* aCurrentPropertyValue,
    CXnDomPropertyValue* aNextPropertyValue )
    {
    if ( !aCurrentPropertyValue && !aNextPropertyValue )
        {
        return 0;
        }
    else if ( aCurrentPropertyValue && !aNextPropertyValue )
        {
        if ( aCurrentPropertyValue->IsAutoIdent() )
            {
            return 0;
            }
        return 1;
        }
    else if ( !aCurrentPropertyValue && aNextPropertyValue )
        {
        if ( aNextPropertyValue->IsAutoIdent() )
            {
            return 0;
            }
        return -1;
        }
    else if ( aCurrentPropertyValue->IsAutoIdent() &&
              aNextPropertyValue->IsAutoIdent() )
        {
        return 0;
        }
    else if ( !aCurrentPropertyValue->IsAutoIdent() &&
              aNextPropertyValue->IsAutoIdent() )
        {
        return 1;
        }
    else if ( aCurrentPropertyValue->IsAutoIdent() &&
              !aNextPropertyValue->IsAutoIdent() )
        {
        return -1;
        }
    else
        {
        return
            aCurrentPropertyValue->FloatValueL() < aNextPropertyValue->FloatValueL() ?
            -1 :
            aCurrentPropertyValue->FloatValueL() == aNextPropertyValue->FloatValueL() ?
            0 :
            1;
        }
    }

// -----------------------------------------------------------------------------
// FindNextFocusableNodeByNavIndexL
// -----------------------------------------------------------------------------
//
static CXnNode* FindNextFocusableNodeByNavIndexL( CXnNode& aNode, TBool aForward )
    {
    if ( aNode.ViewNodeImpl() )
        {
        return NULL;
        }
    CXnNode* nextNode = NULL;
    if ( aForward )
        {
        nextNode = ForwardL( aNode );
        }
    else
        {
        nextNode = BackwardL( aNode );
        }
    return nextNode;
    }

// -----------------------------------------------------------------------------
// FindNextFocusableNodeL
// -----------------------------------------------------------------------------
//
TBool FindNextFocusableNodeL( CXnNode& aNode, TBool aForward )
    {
    CXnNode* node( FindNextFocusableNodeByNavIndexL( aNode, aForward ) );

    if ( !node )
        {
        node = FindLoopedFocusableNodeL( aNode, aForward );
        }

    if ( node )
        {
        node->SetStateL( XnPropertyNames::style::common::KFocus );
        if( node && node->ScrollableControl() )
            {
            node->ScrollableControl()->ShowItem( *node );
            }
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// FindNextNodeFromRightL
// -----------------------------------------------------------------------------
//
static CXnNode* FindNextNodeFromRightL(
    RPointerArray< CXnNode >& aArray,
    CXnNode& aNode,
    TBool stayInNamespace,
    CXnUiEngine* aEngine )
    {
    
    CXnNode* nextNode = NULL;
    TRect rect = aNode.PaddingRect();

    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        CXnNode* tmpNode = aArray[i];
        if ( tmpNode == &aNode )
            {
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
            {
            // do not shift focus to another view
            continue;
            }
        
        const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
        const TDesC8& nodeNamespace = aNode.Impl()->Namespace();
               
        if ( stayInNamespace && tmpNamespace != nodeNamespace )
            {
            // if staying in the same namespace is required,
            // do not shift focus to a different namespace
            continue;
            }
        
        CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
        
        if( stayInNamespace && !popupParent )
            {
            continue;
            }
        
        // TODO: only plugin widgets are available in edit mode

        TRect tmpRect = tmpNode->PaddingRect();

        // temp node qualification:
        // 1. left edge of temp node is to the right from focused's right edge
        // 2. top edge of temp node is NOT below the bottom edge of focused node
        // 3. bottom edge of temp node is NOT above the top edge of focused node
        if ( rect.iBr.iX - 1 < tmpRect.iTl.iX &&
             rect.iBr.iY - 1 >= tmpRect.iTl.iY &&
             rect.iTl.iY <= tmpRect.iBr.iY - 1 )
            {
            if ( !nextNode )
                {
                nextNode = tmpNode;
                }
            else
                {
                // left edge of temp node is closer to focused than next node's
                // left edge
                if ( tmpRect.iTl.iX < nextNode->PaddingRect().iTl.iX )
                    {
                    nextNode = tmpNode;
                    continue;
                    }
                // left edge of temp and next are at the same level
                else if ( tmpRect.iTl.iX == nextNode->PaddingRect().iTl.iX )
                    {
                    // top edges are at the same level, choose this one
                    if ( tmpRect.iTl.iY == rect.iTl.iY )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // top edges are not at the same level
                    else
                        {
                        if ( !AknLayoutUtils::LayoutMirrored() )
                            {
                            // candidate's top edge is below focused's top edge
                            // and previous candidate's top edge is above
                            // focused's top edge
                            if ( nextNode->PaddingRect().iTl.iY < rect.iTl.iY &&
                                 tmpRect.iTl.iY > rect.iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            // previous candidate's top edge is below focused's
                            // top edge. Also, candidate's top edge is below
                            // focused's top edge. Select the highest one.
                            else if ( nextNode->PaddingRect().iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            }
                        else // arabic/hebrew layout
                            {
                            // candidate's top edge is above focused's top edge
                            // and previous candidate's top edge is below
                            // focused's top edge
                            if ( nextNode->PaddingRect().iTl.iY > rect.iTl.iY &&
                                 tmpRect.iTl.iY < rect.iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            // candidate's and previous' top edge is below focused's
                            // top edge. Select the highest one.
                            else if ( nextNode->PaddingRect().iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            }
                        }
                    }
                else
                    {
                    continue;
                    }
                }
            }
        }

    // find node below or above to the right
    if ( !nextNode )
        {
        CXnNode* candidateAbove = NULL;
        CXnNode* candidateBelow = NULL;

        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            if ( tmpNode == &aNode )
                {
                continue;
                }
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();

            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
            
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
            
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            // TODO: only plugin widgets are available in edit mode

            TRect tmpRect = tmpNode->PaddingRect();

            // temp node qualification:
            // left edge of temp node is to the right from focused's right edge
            if ( rect.iBr.iX - 1 < tmpRect.iTl.iX )
                {
                // tmp is above the focused node
                if ( tmpRect.iBr.iY - 1 < rect.iTl.iY )
                    {
                    if ( candidateAbove == NULL )
                        {
                        candidateAbove = tmpNode;
                        }
                    else
                        {
                        // use pythagorean theorem to determine the closest node
                        TInt xDistTmp = tmpRect.iTl.iX - rect.iBr.iX;
                        TInt yDistTmp = tmpRect.iBr.iY - rect.iTl.iY;
                        TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                        TInt xCand = candidateAbove->PaddingRect().iTl.iX - rect.iBr.iX;
                        TInt yCand = candidateAbove->PaddingRect().iBr.iY - rect.iTl.iY;
                        TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                        if ( tmpDist < cDist )
                            {
                            candidateAbove = tmpNode;
                            }
                        }
                    }
                else
                    {
                    if ( candidateBelow == NULL )
                        {
                        candidateBelow = tmpNode;
                        }
                    else
                        {
                        // use pythagorean theorem to determine the closest node
                        TInt xDistTmp = tmpRect.iTl.iX - rect.iBr.iX;
                        TInt yDistTmp = tmpRect.iTl.iY - rect.iBr.iY;
                        TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                        TInt xCand = candidateBelow->PaddingRect().iTl.iX - rect.iBr.iX;
                        TInt yCand = candidateBelow->PaddingRect().iTl.iY - rect.iBr.iY;
                        TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                        if ( tmpDist < cDist )
                            {
                            candidateBelow = tmpNode;
                            }
                        }
                    }
                }
            }

        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            if ( candidateBelow )
                {
                nextNode = candidateBelow;
                }
            else
                {
                if ( candidateAbove )
                    {
                    nextNode = candidateAbove;
                    }
                }
            }
        else
            {
            if ( candidateAbove )
                {
                nextNode = candidateAbove;
                }
            else
                {
                if ( candidateBelow )
                    {
                    nextNode = candidateBelow;
                    }
                }
            }
        }

    // loop to the right
    if ( !nextNode )
        {
        if ( !stayInNamespace && aEngine &&
             aEngine->ViewManager()->ViewAmount() != KOneView )
            {
            aEngine->ViewManager()->ActivateNextViewL();
            return nextNode;
            }        
        
        CXnNode* candidateAbove = NULL;
        CXnNode* candidateBelow = NULL;

        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();
                   
            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
            
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
                
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            
            // TODO: only plugin widgets are available in edit mode
            TRect tmpRect = tmpNode->PaddingRect();

            // tmp is above the focused node
            if ( tmpRect.iBr.iY <= rect.iTl.iY )
                {
                if ( candidateAbove == NULL )
                    {
                    candidateAbove = tmpNode;
                    }
                else
                    {
                    // use pythagorean theorem to determine the closest node
                    TInt xDistTmp = tmpRect.iTl.iX;
                    TInt yDistTmp = tmpRect.iTl.iY - rect.iTl.iY;
                    TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                    TInt xCand = candidateAbove->PaddingRect().iTl.iX;
                    TInt yCand = candidateAbove->PaddingRect().iTl.iY - rect.iTl.iY;
                    TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                    if ( tmpDist < cDist )
                        {
                        candidateAbove = tmpNode;
                        }
                    }
                }
            else
                {
                if ( candidateBelow == NULL )
                    {
                    candidateBelow = tmpNode;
                    }
                else
                    {
                    // use pythagorean theorem to determine the closest node
                    TInt xDistTmp = tmpRect.iTl.iX;
                    TInt yDistTmp = rect.iTl.iY - tmpRect.iTl.iY;
                    TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                    TInt xCand = candidateBelow->PaddingRect().iTl.iX;
                    TInt yCand = rect.iTl.iY - candidateBelow->PaddingRect().iTl.iY;
                    TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                    if ( tmpDist < cDist )
                        {
                        candidateBelow = tmpNode;
                        }
                    }
                }
            }

        if ( candidateAbove && candidateBelow )
            {
            CXnNode* pluginAbove = FindPluginNode( *candidateAbove );
            CXnNode* pluginBelow = FindPluginNode( *candidateBelow );

            TReal aboveDist = 0;
            TReal belowDist = 0;

            if ( pluginAbove )
                {
                TInt xAbove = pluginAbove->PaddingRect().iTl.iX;
                TInt yAbove = 0;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    yAbove = rect.iTl.iY - pluginAbove->PaddingRect().iTl.iY;
                    }
                else
                    {
                    yAbove = rect.iTl.iY - pluginAbove->PaddingRect().iBr.iY;
                    }
                aboveDist = Abs( ( xAbove * xAbove ) + ( yAbove * yAbove ) );
                }
            else
                {
                TInt xAbove = candidateAbove->PaddingRect().iTl.iX;
                TInt yAbove = 0;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    yAbove = rect.iTl.iY - candidateAbove->PaddingRect().iTl.iY;
                    }
                else
                    {
                    yAbove = rect.iTl.iY - candidateAbove->PaddingRect().iBr.iY;
                    }

                aboveDist = Abs( ( xAbove * xAbove ) + ( yAbove * yAbove ) );
                }

            if ( pluginBelow )
                {
                TInt xBelow = pluginBelow->PaddingRect().iTl.iX;
                TInt yBelow = pluginBelow->PaddingRect().iTl.iY - rect.iTl.iY;
                belowDist = Abs( ( xBelow * xBelow ) + ( yBelow * yBelow ) );
                }
            else
                {
                TInt xBelow = candidateBelow->PaddingRect().iTl.iX;
                TInt yBelow = candidateBelow->PaddingRect().iTl.iY - rect.iTl.iY;
                belowDist = Abs( ( xBelow * xBelow ) + ( yBelow * yBelow ) );
                }

            if ( aboveDist < belowDist )
                {
                nextNode = candidateAbove;
                }
            else if ( belowDist < aboveDist )
                {
                nextNode = candidateBelow;
                }
            else
                {
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    nextNode = candidateBelow;
                    }
                else
                    {
                    nextNode = candidateAbove;
                    }
                }
            }
        else if ( candidateAbove )
            {
            nextNode = candidateAbove;
            }
        else if ( candidateBelow )
            {
            nextNode = candidateBelow;
            }
        else
            {
            // do nothing
            }
        }
    
    if ( stayInNamespace && !nextNode )
        {
        return &aNode;
        }
    
    return nextNode;
    }

// -----------------------------------------------------------------------------
// FindNextNodeFromLeftL
// -----------------------------------------------------------------------------
//
static CXnNode* FindNextNodeFromLeftL(
    RPointerArray< CXnNode >& aArray,
    CXnNode& aNode,
    TBool stayInNamespace,
    CXnUiEngine* aEngine )
    {
    CXnNode* nextNode = NULL;
    TRect rect = aNode.PaddingRect();

    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        CXnNode* tmpNode = aArray[i];
        if ( tmpNode == &aNode )
            {
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
            {
            // do not shift focus to another view
            continue;
            }
        
        const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
        const TDesC8& nodeNamespace = aNode.Impl()->Namespace();
               
        if ( stayInNamespace && tmpNamespace != nodeNamespace )
            {
            // if staying in the same namespace is required,
            // do not shift focus to a different namespace
            continue;
            }
        
        CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
        
        if( stayInNamespace && !popupParent )
            {
            continue;
            }
        
        // TODO: only plugin widgets are available in edit mode

        TRect tmpRect = tmpNode->PaddingRect();

        // temp node qualification:
        // 1. right edge of temp node is to the left from focused's left edge
        // 2. top edge of temp node is NOT below the bottom edge of focused node
        // 3. bottom edge of temp node is NOT above the top edge of focused node
        if ( rect.iTl.iX > tmpRect.iBr.iX - 1 &&
             rect.iBr.iY - 1 >= tmpRect.iTl.iY &&
             rect.iTl.iY <= tmpRect.iBr.iY - 1 )
            {
            if ( !nextNode )
                {
                nextNode = tmpNode;
                }
            else
                {
                // right edge of temp node is closer to focused than next node's
                // right edge
                if ( tmpRect.iBr.iX - 1 > nextNode->PaddingRect().iBr.iX - 1 )
                    {
                    nextNode = tmpNode;
                    continue;
                    }
                // left edge of temp and next are at the same level
                else if ( tmpRect.iBr.iX - 1 == nextNode->PaddingRect().iBr.iX - 1 )
                    {
                    // top edges are at the same level, choose this one
                    if ( tmpRect.iTl.iY == rect.iTl.iY )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // top edges are not at the same level
                    else
                        {
                        if ( !AknLayoutUtils::LayoutMirrored() )
                            {
                            // candidate's top edge is below focused's top edge
                            // and previous candidate's top edge is above focused's
                            // top edge
                            if ( nextNode->PaddingRect().iTl.iY > rect.iTl.iY &&
                                 tmpRect.iTl.iY < rect.iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            // candidate's and previous candidate's top edge is
                            // above focused's top edge. Select highest one.
                            else if ( nextNode->PaddingRect().iTl.iY < rect.iTl.iY &&
                                      tmpRect.iTl.iY < rect.iTl.iY &&
                                      tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            }
                        else // arabic/hebrew layout
                            {
                            // candidate's top edge is below focused's top edge
                            // and previous candidate's top edge is above
                            // focused's top edge
                            if ( nextNode->PaddingRect().iTl.iY < rect.iTl.iY &&
                                 tmpRect.iTl.iY > rect.iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            // previous candidate's top edge is below focused's
                            // top edge. Also, candidate's top edge is below
                            // focused's top edge. Select the highest one.
                            else if ( nextNode->PaddingRect().iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY > rect.iTl.iY &&
                                      tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                                {
                                nextNode = tmpNode;
                                continue;
                                }
                            }
                        }
                    }
                else
                    {
                    continue;
                    }
                }
            }
        }

    if ( !nextNode )
        {
        CXnNode* candidateAbove = NULL;
        CXnNode* candidateBelow = NULL;

        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            if ( tmpNode == &aNode )
                {
                continue;
                }
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();       
                   
            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
        
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
            
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            
            // TODO: only plugin widgets are available in edit mode
            TRect tmpRect = tmpNode->PaddingRect();

            // temp node qualification:
            // right edge of temp node is to the left from focused's left edge
            if ( tmpRect.iBr.iX < rect.iTl.iX - 1 )
                {
                // tmp is above the focused node
                if ( tmpRect.iBr.iY < rect.iTl.iY )
                    {
                    if ( candidateAbove == NULL )
                        {
                        candidateAbove = tmpNode;
                        }
                    else
                        {
                        // use pythagorean theorem to determine the closest node
                        TInt xDistTmp = rect.iTl.iX - tmpRect.iBr.iX;
                        TInt yDistTmp = rect.iTl.iY - tmpRect.iBr.iY;
                        TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                        TInt xCand = rect.iTl.iX - candidateAbove->PaddingRect().iTl.iX;
                        TInt yCand = rect.iTl.iY - candidateAbove->PaddingRect().iBr.iY;
                        TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                        if ( tmpDist < cDist )
                            {
                            candidateAbove = tmpNode;
                            }
                        }
                    }
                else
                    {
                    if ( candidateBelow == NULL )
                        {
                        candidateBelow = tmpNode;
                        }
                    else
                        {
                        // use pythagorean theorem to determine the closest node
                        TInt xDistTmp = rect.iTl.iX - tmpRect.iBr.iX;
                        TInt yDistTmp = rect.iBr.iY - tmpRect.iTl.iY;
                        TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                        TInt xCand = rect.iTl.iX - candidateBelow->PaddingRect().iBr.iX;
                        TInt yCand = rect.iBr.iY - candidateBelow->PaddingRect().iTl.iY;
                        TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                        if ( tmpDist < cDist )
                            {
                            candidateBelow = tmpNode;
                            }
                        }
                    }
                }
            }

        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            if ( candidateAbove )
                {
                nextNode = candidateAbove;
                }
            else
                {
                if ( candidateBelow )
                    {
                    nextNode = candidateBelow;
                    }
                }
            }
        else
            {
            if ( candidateBelow )
                {
                nextNode = candidateBelow;
                }
            else
                {
                if ( candidateAbove )
                    {
                    nextNode = candidateAbove;
                    }
                }
            }
        }

    // get view width to get looping x point
    TInt viewWidth = aNode.FindViewNode()->PaddingRect().Width();

    // loop to the left
    if ( !nextNode )
        {
        if ( !stayInNamespace && aEngine &&
             aEngine->ViewManager()->ViewAmount() != KOneView )
            {
            aEngine->ViewManager()->ActivatePreviousViewL();
            return nextNode;
            }
        
        CXnNode* candidateAbove = NULL;
        CXnNode* candidateBelow = NULL;

        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            /*
            if ( tmpNode == &aNode )
                {
                continue;
                }
            */
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();
                   
            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
            
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
            
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            
            // TODO: only plugin widgets are available in edit mode
            TRect tmpRect = tmpNode->PaddingRect();

            // tmp is above the focused node
            if ( tmpRect.iBr.iY <= rect.iTl.iY )
                {
                if ( candidateAbove == NULL )
                    {
                    candidateAbove = tmpNode;
                    }
                else
                    {
                    // use pythagorean theorem to determine the closest node
                    TInt xDistTmp = viewWidth - tmpRect.iBr.iX;
                    TInt yDistTmp = rect.iTl.iY - tmpRect.iBr.iY;
                    TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                    TInt xCand = viewWidth - candidateAbove->PaddingRect().iBr.iX;
                    TInt yCand = rect.iTl.iY - candidateAbove->PaddingRect().iTl.iY;
                    TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                    if ( tmpDist < cDist )
                        {
                        candidateAbove = tmpNode;
                        }
                    }
                }
            else
                {
                if ( candidateBelow == NULL )
                    {
                    candidateBelow = tmpNode;
                    }
                else
                    {
                    // use pythagorean theorem to determine the closest node
                    TInt xDistTmp = viewWidth - tmpRect.iBr.iX;
                    TInt yDistTmp = tmpRect.iTl.iY - rect.iTl.iY;
                    TReal tmpDist = Abs( ( xDistTmp * xDistTmp ) + ( yDistTmp * yDistTmp ) );

                    TInt xCand = viewWidth - candidateBelow->PaddingRect().iBr.iX;
                    TInt yCand = candidateBelow->PaddingRect().iTl.iY - rect.iTl.iY;
                    TReal cDist = Abs( ( xCand * xCand ) + ( yCand * yCand ) );

                    if ( tmpDist < cDist )
                        {
                        candidateBelow = tmpNode;
                        }
                    }
                }
            }

        if ( candidateAbove && candidateBelow )
            {
            CXnNode* pluginAbove = FindPluginNode( *candidateAbove );
            CXnNode* pluginBelow = FindPluginNode( *candidateBelow );

            TReal aboveDist = 0;
            TReal belowDist = 0;

            if ( pluginAbove )
                {
                // compare to bottom right when looping to left
                TInt xAbove = viewWidth - pluginAbove->PaddingRect().iBr.iX;
                TInt yAbove = 0;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    yAbove = rect.iTl.iY - pluginAbove->PaddingRect().iBr.iY;
                    }
                else
                    {
                    yAbove = rect.iTl.iY - pluginAbove->PaddingRect().iTl.iY;
                    }

                aboveDist = Abs( (xAbove * xAbove) + (yAbove * yAbove) );
                }
            else
                {
                TInt xAbove = viewWidth - candidateAbove->PaddingRect().iBr.iX;
                TInt yAbove = 0;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    yAbove = rect.iTl.iY - candidateAbove->PaddingRect().iBr.iY;
                    }
                else
                    {
                    yAbove = rect.iTl.iY - candidateAbove->PaddingRect().iTl.iY;
                    }
                aboveDist = Abs( ( xAbove * xAbove ) + ( yAbove * yAbove ) );
                }

            if ( pluginBelow )
                {
                TInt xBelow = viewWidth - pluginBelow->PaddingRect().iBr.iX;
                TInt yBelow = pluginBelow->PaddingRect().iTl.iY - rect.iTl.iY;
                belowDist = Abs( ( xBelow * xBelow ) + ( yBelow * yBelow ) );
                }
            else
                {
                TInt xBelow = viewWidth - candidateBelow->PaddingRect().iBr.iX;
                TInt yBelow = candidateBelow->PaddingRect().iTl.iY - rect.iTl.iY;
                belowDist = Abs( ( xBelow * xBelow ) + ( yBelow * yBelow ) );
                }

            if ( aboveDist < belowDist )
                {
                nextNode = candidateAbove;
                }
            else if ( belowDist < aboveDist )
                {
                nextNode = candidateBelow;
                }
            else
                {
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    nextNode = candidateBelow;
                    }
                else
                    {
                    nextNode = candidateAbove;
                    }
                }
            }
        else if ( candidateAbove )
            {
            nextNode = candidateAbove;
            }
        else if ( candidateBelow )
            {
            nextNode = candidateBelow;
            }
        else
            {
            // do nothing
            }
        }

    if ( stayInNamespace && !nextNode )
        {
        return &aNode;
        }
    
    return nextNode;
    }

// -----------------------------------------------------------------------------
// FindNextNodeFromBelowL
// -----------------------------------------------------------------------------
//
static CXnNode* FindNextNodeFromBelowL(
    RPointerArray< CXnNode >& aArray,
    CXnNode& aNode, TBool stayInNamespace )
    {
    CXnNode* nextNode = NULL;
    TRect rect = aNode.PaddingRect();

    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        CXnNode* tmpNode = aArray[i];
        if ( tmpNode == &aNode )
            {
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
            {
            // do not shift focus to another view
            continue;
            }        
        
        const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
        const TDesC8& nodeNamespace = aNode.Impl()->Namespace();
               
        if ( stayInNamespace && tmpNamespace != nodeNamespace )
            {
            // if staying in the same namespace is required,
            // do not shift focus to a different namespace
            continue;
            }
        
        CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
        
        if( stayInNamespace && !popupParent )
            {
            continue;
            }
        
        // TODO: only plugin widgets are available in edit mode

        TRect tmpRect = tmpNode->PaddingRect();

        // temp node qualification:
        // 1. top edge of the temp node is below bottom edge of focused node
        // 2. temp node's right or left edge is between focused's left or right edge
        if ( tmpRect.iTl.iY > rect.iBr.iY - 1 &&
             tmpRect.iBr.iX - 1 >= rect.iTl.iX &&
             tmpRect.iTl.iX <= rect.iBr.iX - 1 )
            {
            if ( !nextNode )
                {
                nextNode = tmpNode;
                }
            else
                {
                // temp node is closer than next node
                if ( tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                    {
                    nextNode = tmpNode;
                    continue;
                    }
                // top edge of temp node and next are at the same level
                else if ( tmpRect.iTl.iY == nextNode->PaddingRect().iTl.iY )
                    {
                    // western layout: take the leftmost one
                    if ( !AknLayoutUtils::LayoutMirrored() &&
                         tmpRect.iBr.iX - 1 < nextNode->PaddingRect().iBr.iX - 1 )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // arabic/hebrew layout: take the rightmost one
                    if ( AknLayoutUtils::LayoutMirrored() &&
                         tmpRect.iBr.iX - 1 > nextNode->PaddingRect().iBr.iX - 1 )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    }
                else
                    {
                    continue;
                    }
                }
            }
        }

    // no nodes found on the right, try looping
    if ( !nextNode )
        {
        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            TRect tmpRect = tmpNode->PaddingRect();
            if ( tmpNode == &aNode )
                {
                continue;
                }
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpRect.Intersects( rect ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();

            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
            
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
            
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            
            // TODO: only plugin widgets are available in edit mode

            // temp node's right or left edge must be between focused's left or
            // right edge
            if ( tmpRect.iBr.iX - 1 >= rect.iTl.iX &&
                 tmpRect.iTl.iX <= rect.iBr.iX - 1 )
                {
                if ( !nextNode )
                    {
                    nextNode = tmpNode;
                    }
                else
                    {
                    // temp node is closer than next node
                    if ( tmpRect.iTl.iY < nextNode->PaddingRect().iTl.iY )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // top edge of temp node and next are at the same level
                    else if ( tmpRect.iTl.iY == nextNode->PaddingRect().iTl.iY )
                        {
                        // western layout: take the leftmost one
                        if ( !AknLayoutUtils::LayoutMirrored() &&
                             tmpRect.iBr.iX - 1 < nextNode->PaddingRect().iBr.iX - 1 )
                            {
                            nextNode = tmpNode;
                            continue;
                            }
                        // arabic/hebrew layout: take the rightmost one
                        if ( AknLayoutUtils::LayoutMirrored() &&
                             tmpRect.iBr.iX - 1 > nextNode->PaddingRect().iBr.iX - 1 )
                            {
                            nextNode = tmpNode;
                            continue;
                            }
                        }
                    else
                        {
                        continue;
                        }
                    }
                }
            }
        }
    
    if ( stayInNamespace && !nextNode )
        {
        return &aNode;
        }
    
    return nextNode;
    }

// -----------------------------------------------------------------------------
// FindNextNodeFromAboveL
// -----------------------------------------------------------------------------
//
static CXnNode* FindNextNodeFromAboveL(
    RPointerArray< CXnNode >& aArray,
    CXnNode& aNode, TBool stayInNamespace )
    {
    CXnNode* nextNode = NULL;
    TRect rect = aNode.PaddingRect();

    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        CXnNode* tmpNode = aArray[i];
        if ( tmpNode == &aNode )
            {
            continue;
            }
        if ( !IsNodeNavigableL( *tmpNode ) )
            {
            continue;
            }
        if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
            {
            // do not shift focus to another view
            continue;
            }
        
        const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
        const TDesC8& nodeNamespace = aNode.Impl()->Namespace();       
       
        if ( stayInNamespace && tmpNamespace != nodeNamespace )
            {
            // if staying in the same namespace is required,
            // do not shift focus to a different namespace
            continue;
            }
        
        CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
        
        if( stayInNamespace && !popupParent )
            {
            continue;
            }
        
        // TODO: only plugin widgets are available in edit mode
        TRect tmpRect = tmpNode->PaddingRect();

        // temp node qualification:
        // 1. bottom edge of the temp node is above the top edge of focused node
        // 2. temp node's right or left edge is between focused's left or right edge
        if ( tmpRect.iBr.iY - 1 < rect.iTl.iY &&
             tmpRect.iBr.iX - 1 >= rect.iTl.iX &&
             tmpRect.iTl.iX <= rect.iBr.iX - 1 )
            {
            if ( !nextNode )
                {
                nextNode = tmpNode;
                }
            else
                {
                // temp node is closer than next node
                if ( tmpRect.iBr.iY - 1 > nextNode->PaddingRect().iBr.iY - 1 )
                    {
                    nextNode = tmpNode;
                    continue;
                    }
                // bottom edge of temp and next node are at the same level
                else if ( tmpRect.iBr.iY - 1 == nextNode->PaddingRect().iBr.iY - 1 )
                    {
                    // western layout: take the leftmost one
                    if ( !AknLayoutUtils::LayoutMirrored() &&
                         tmpRect.iBr.iX - 1 < nextNode->PaddingRect().iBr.iX - 1 )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // arabic/hebrew layout: take the rightmost one
                    if ( AknLayoutUtils::LayoutMirrored() &&
                         tmpRect.iBr.iX - 1 > nextNode->PaddingRect().iBr.iX - 1 )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    }
                else
                    {
                    continue;
                    }
                }
            }
        }

    // no nodes found on the right, try looping
    if ( !nextNode )
        {
        for ( TInt i = 0; i < aArray.Count(); ++i )
            {
            CXnNode* tmpNode = aArray[i];
            TRect tmpRect = tmpNode->PaddingRect();
            if ( tmpNode == &aNode )
                {
                continue;
                }
            if ( !IsNodeNavigableL( *tmpNode ) )
                {
                continue;
                }
            if ( tmpRect.Intersects( rect ) )
                {
                continue;
                }
            if ( tmpNode->FindViewNode() != tmpNode->UiEngine()->ActiveView() )
                {
                // do not shift focus to another view
                continue;
                }
            
            const TDesC8& tmpNamespace = tmpNode->Impl()->Namespace();
            const TDesC8& nodeNamespace = aNode.Impl()->Namespace();       
                   
            if ( stayInNamespace && tmpNamespace != nodeNamespace )
                {
                // if staying in the same namespace is required,
                // do not shift focus to a different namespace
                continue;
                }
            
            CXnNode* popupParent( WidgetExtensionParent( tmpNode ) );
            
            if( stayInNamespace && !popupParent )
                {
                continue;
                }
            
            // TODO: only plugin widgets are available in edit mode

            // temp node's right or left edge must be between focused's left or
            // right edge
            if ( tmpRect.iBr.iX - 1 >= rect.iTl.iX &&
                 tmpRect.iTl.iX <= rect.iBr.iX - 1 )
                {
                if ( !nextNode )
                    {
                    nextNode = tmpNode;
                    }
                else
                    {
                    // temp node is closer than next node
                    if ( tmpRect.iBr.iY - 1 > nextNode->PaddingRect().iBr.iY - 1 )
                        {
                        nextNode = tmpNode;
                        continue;
                        }
                    // bottom edge of temp and next node are at the same level
                    else if ( tmpRect.iBr.iY - 1 == nextNode->PaddingRect().iBr.iY - 1 )
                        {
                        // western layout: take the leftmost one
                        if ( !AknLayoutUtils::LayoutMirrored() &&
                             tmpRect.iBr.iX - 1 < nextNode->PaddingRect().iBr.iX - 1 )
                            {
                            nextNode = tmpNode;
                            continue;
                            }
                        // arabic/hebrew layout: take the rightmost one
                        if ( AknLayoutUtils::LayoutMirrored() &&
                             tmpRect.iBr.iX - 1 > nextNode->PaddingRect().iBr.iX - 1 )
                            {
                            nextNode = tmpNode;
                            continue;
                            }
                        }
                    else
                        {
                        continue;
                        }
                    }
                }
            }
        }
    
    if ( stayInNamespace && !nextNode )
        {
        return &aNode;
        }
    
    return nextNode;
    }

// -----------------------------------------------------------------------------
// FindPluginNode
// -----------------------------------------------------------------------------
//
static CXnNode* FindPluginNode( CXnNode& aNode )
    {
    CXnNode* pluginNode = NULL;
    CXnNode* tmp = &aNode;

    while( tmp->Parent() )
        {
        tmp = tmp->Parent();
        const TDesC8& nodeType = tmp->DomNode()->Name();

        if ( nodeType == KPlugin )
            {
            pluginNode = tmp;
            break;
            }
        }

    return pluginNode;
    }

// -----------------------------------------------------------------------------
// DoInternalFocusChangeL
// -----------------------------------------------------------------------------
//
static TBool DoInternalFocusChangeL( CXnUiEngine& aEngine,
    CXnNode& aNode, const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aNode.ViewNodeImpl() )
        {
        return EFalse;
        }
    if ( aType != 1 )
        {
        return EFalse;
        }
    if ( aKeyEvent.iScanCode != EStdKeyLeftArrow &&
         aKeyEvent.iScanCode != EStdKeyRightArrow &&
         aKeyEvent.iScanCode != EStdKeyDownArrow &&
         aKeyEvent.iScanCode != EStdKeyUpArrow )
        {
        return EFalse;
        }
    CXnNode* parent = aNode.Parent();
    if ( !parent )
        {
        return EFalse;
        }

    // Handle 'appearance' navigation first, if the current node has 
    // "nav-index: appearance"...
    CXnProperty* navind = aNode.NavIndexL();
    if ( navind && navind->StringValue() == XnPropertyNames::style::common::KAppearance )
        {
        
        CXnAppUiAdapter& appui = static_cast< CXnAppUiAdapter& >( *iAvkonAppUi );
        CXnPluginData* plugin( 
                    appui.ViewManager().ActiveViewData().Plugin( &aNode ) );
        
        // find if node is in a widget that contatins some opened popup window       
        TBool containsPopUp = ( plugin ? plugin->IsDisplayingPopup() : EFalse );

        CXnNode* nextNode( NULL );
        RPointerArray< CXnNode >& array = aEngine.ViewManager()->AppearanceNodes();
        if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
            {
            nextNode = FindNextNodeFromBelowL( array, aNode, containsPopUp );
            }
        else if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            nextNode = FindNextNodeFromAboveL( array, aNode, containsPopUp );
            }
        else if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            nextNode = FindNextNodeFromRightL( array, aNode, containsPopUp, &aEngine );
            }
        else if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            nextNode = FindNextNodeFromLeftL( array, aNode, containsPopUp, &aEngine );
            }
        if ( nextNode )
            {
            // focus plugin node if in edit mode
            if( aEngine.IsEditMode() )
                {
                if( nextNode->Type()->Type() != KPlugin )
                    {
                    CXnNode* pluginNode = FindPluginNode( *nextNode );
                    if( pluginNode )
                        {
                        nextNode = pluginNode;
                        }
                    }
                }

            if( nextNode && nextNode->ScrollableControl() )
                {
                nextNode->ScrollableControl()->ShowItem( *nextNode );
                }
            nextNode->SetStateL( XnPropertyNames::style::common::KFocus );
            return ETrue;
            }
        }
    else
        {
        const TDesC8* direction( &XnPropertyNames::style::common::direction::KLTR );
        const TDesC8* orientation( &XnPropertyNames::style::common::block_progression::KTB );

        CXnProperty* directionProperty( parent->DirectionL() );

        if ( directionProperty )
            {
            direction = &directionProperty->StringValue();
            }

        CXnProperty* orientationProperty( parent->BlockProgressionL() );

        if ( orientationProperty )
            {
            orientation = &orientationProperty->StringValue();
            }

        TBool match = EFalse;

        if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
             ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
               *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
            {
            if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
                {
                match = FindNextFocusableNodeL( aNode, EFalse );
                }
            if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
                {
                match = FindNextFocusableNodeL( aNode, ETrue );
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                  ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
                    *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
            {
            if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
                {
                match = FindNextFocusableNodeL( aNode, EFalse );
                }
            if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
                {
                match = FindNextFocusableNodeL( aNode, ETrue );
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
                  *orientation == XnPropertyNames::style::common::block_progression::KTB )
            {
            if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
                {
                match = FindNextFocusableNodeL( aNode, EFalse );
                }
            if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
                {
                match = FindNextFocusableNodeL( aNode, ETrue );
                }
            }
        else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                  *orientation == XnPropertyNames::style::common::block_progression::KTB )
            {
            if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
                {
                match = FindNextFocusableNodeL( aNode, EFalse );
                }
            if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
                {
                match = FindNextFocusableNodeL( aNode, ETrue );
                }
            }
        return match;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// WidgetExtensionParentL
// -----------------------------------------------------------------------------
//
static CXnNode* WidgetExtensionParent( CXnNode* aNode )
    {
    CXnNode* parent( aNode->Parent() );
    
    for( ; parent; )
        {
        CXnType* type( parent->Type() );
        
        if( type )
            {
            const TDesC8& typeDes( type->Type() );
            
            if( typeDes.Compare( KWidgetExtensionNodeName) == 0
                    || typeDes.Compare( KPopUpNodeName) == 0 )
                {
                return parent;
                }
            }
        
        parent = parent->Parent();
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// DoTriggerKeyEventL
// -----------------------------------------------------------------------------
//
static TBool DoTriggerKeyEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CXnDomNode* domNode = aNode.DomNode();
    CXnDomList& children = domNode->ChildNodes();
    TInt count = children.Length();
    TBool returnValue = EFalse;
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = node->Name();
        if ( type == XnPropertyNames::action::KActions )
            {
            CXnDomList& actionChildren = node->ChildNodes();
            TInt actionCount = actionChildren.Length();
            for ( TInt j = 0; j < actionCount; ++j )
                {
                CXnDomNode* actionNode =
                    static_cast< CXnDomNode* >( actionChildren.Item( j ) );
                const TDesC8& actionType = actionNode->Name();
                if ( actionType == XnPropertyNames::action::KAction )
                    {
                    TBool match = MatchActionForKeyEventL(
                        aThis, aEngine, aNode, aKeyEvent, aType, *actionNode );
                    if ( match )
                        {
                        returnValue = ETrue;
                        }
                    }
                }
            }
        }

    if ( returnValue )
        {
        // Return is removed from here to enable auto navigation altough the
        // action is alreadytriggered. This enables simpler theme definitions
        // especially in AI2 case. If regression is observed then return should
        // be restored here. Task ou1s60ui#3386.
        //return returnValue;
        }

    DoCompoundNodeScrollingL( aNode, aKeyEvent, aType );

    returnValue = DoInternalFocusChangeL( aEngine, aNode, aKeyEvent, aType );

    if ( returnValue )
        {
        aEngine.RenderUIL();
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// DoTriggerEventL
// -----------------------------------------------------------------------------
//
static void DoTriggerEventL(
    CXnNodeImpl* aThis,
    CXnUiEngine& aEngine,
    CXnNode& aNode,
    CXnNode& aActionsParent,
    CXnNode& aEventData,    
    TInt aSource )
    {    
    CXnDomList& children( aActionsParent.DomNode()->ChildNodes() );
    
    TInt count( children.Length() );
    
    for ( TInt i = 0; i < count; ++i )
        {
        CXnDomNode* node = static_cast< CXnDomNode* >( children.Item( i ) );
        const TDesC8& type = node->Name();
        if ( type == XnPropertyNames::action::KActions )
            {
            CXnDomList& actionChildren = node->ChildNodes();
            TInt actionCount = actionChildren.Length();
            for ( TInt j = 0; j < actionCount; ++j )
                {
                CXnDomNode* actionNode =
                    static_cast< CXnDomNode* >( actionChildren.Item( j ) );
                const TDesC8& actionType = actionNode->Name();
                if ( actionType == XnPropertyNames::action::KAction )
                    {
                    TBool match = MatchActionForEventL(
                        aThis, aEngine, aNode, aEventData, *actionNode, aSource );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DoReceiveFocusL
// -----------------------------------------------------------------------------
//
static TBool DoReceiveFocusL( CXnNode& aNode, CXnUiEngine& /*aEngine*/ )
    {
    CXnProperty* focusable = aNode.GetPropertyL(
        XnPropertyNames::common::KFocusable );
    CXnProperty* disabled = aNode.GetPropertyL(
        XnPropertyNames::common::KDisabled );
    if ( focusable )
        {
        const TDesC8& focusValue = focusable->StringValue();
        if ( focusValue != KNullDesC8 )
            {
            if ( focusValue != XnPropertyNames::KTrue )
                {
                return EFalse;
                }
            }
        }
    else
        {
        return EFalse;
        }
    if ( disabled )
        {
        const TDesC8& disabledValue = disabled->StringValue();
        if ( disabledValue != KNullDesC8 )
            {
            if ( disabledValue == XnPropertyNames::KTrue )
                {
                return EFalse;
                }
            }
        }
    if ( !IsNodeNavigableL( aNode ) )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// Informs the component about the property change.
// -----------------------------------------------------------------------------
//
static void InformPropertyChangeL( CXnNode& aNode, CXnProperty* aProperty )
    {
    //Inform the component about the property change.
    CXnNode* parent( aNode.Parent() );

    if ( parent )
        {
        if ( parent->Type()->Type() == KMenuBar )
            {
            CXnMenuAdapter* adapter =
                static_cast< CXnMenuAdapter* >( parent->Control() );

            if ( adapter )
                {
                adapter->HandleMenuPropertyChangeL( &aNode.PluginIfL(), aProperty );
                }
            }
        else if ( aNode.IsLayoutCapable() )
            {
            CXnControlAdapter* adapter( aNode.Control() );

            if ( adapter )
                {
                adapter->HandlePropertyChangeL( aProperty );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Informs parent and its children about the property change.
// -----------------------------------------------------------------------------
//
static void InformChildrenPropertyChangeL(
    CXnNode& aParent,
    CXnProperty* aProperty,
    TBool aInformParent = ETrue )
    {
    RPointerArray< CXnNode >& children( aParent.Children() );
    if ( aInformParent )
        {
        InformPropertyChangeL( aParent, aProperty );
        }
    TInt count( children.Count() );
    for ( TInt i = 0 ; i < count; i++ )
        {
        InformChildrenPropertyChangeL( *children[i], aProperty, ETrue );
        }
    }

// -----------------------------------------------------------------------------
// DisplayValueMatchL
// -----------------------------------------------------------------------------
//
static TBool DisplayValueMatchL(
    CXnNode& aNode,
    const TDesC8& aDisplayValue )
    {
    CXnProperty* display = aNode.GetPropertyL( KDisplay );
    if ( display )
        {
        const TDesC8& value = display->StringValue();
        if ( value.Compare( aDisplayValue ) == KErrNone )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// SetFloatPropertyToNodeL
// Set float property to a node
// -----------------------------------------------------------------------------
//
static void SetFloatPropertyToNodeL(
    CXnDomStringPool& aStringPool, CXnNode& aNode,
    const TDesC8& aPropertyName, TReal aPropertyValue,
    CXnDomPropertyValue::TPrimitiveValueType aValueType )
    {
    // create property
    CXnProperty* prop = CXnProperty::NewL(
        aPropertyName, aPropertyValue, aValueType, aStringPool );

    CleanupStack::PushL( prop );
    aNode.SetPropertyL( prop );
    CleanupStack::Pop( prop );
    }

// -----------------------------------------------------------------------------
// SetStringPropertyToNodeL
// Set string property to a node
// -----------------------------------------------------------------------------
//
static void SetStringPropertyToNodeL(
    CXnDomStringPool& aStringPool,
    CXnNode& aNode,
    const TDesC8& aPropertyName,
    const TDesC8& aPropertyValue )
    {
    // create property
    CXnProperty* prop = CXnProperty::NewL(
        aPropertyName, aPropertyValue, CXnDomPropertyValue::EString, aStringPool );

    CleanupStack::PushL( prop );
    aNode.SetPropertyL( prop );
    CleanupStack::Pop( prop );
    }

// -----------------------------------------------------------------------------
// FindYoungestSiblingWithDisplayL
// Seek youngest sibling node with given display value
// -----------------------------------------------------------------------------
//
static CXnNode* FindYoungestSiblingWithDisplayL(
    RPointerArray< CXnNode >& aSiblingArray,
    const TDesC8& aDisplayValue,
    TInt& aIndex )
    {
    aIndex = KErrNotFound;
    CXnNode* node = NULL;
    TInt siblingCount( aSiblingArray.Count() );
    //Find youngest sibling with display block
    for ( TInt i = siblingCount; i > 0; )
        {
        node = aSiblingArray[--i];
        if ( DisplayValueMatchL( *node, aDisplayValue ) )
            {
            aIndex = i;
            return node;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// FindOldestSiblingWithDisplayL
// Seek oldest sibling node with given display value
// -----------------------------------------------------------------------------
//
static CXnNode* FindOldestSiblingWithDisplayL(
    RPointerArray< CXnNode >& aSiblingArray,
    const TDesC8& aDisplayValue,
    TInt& aIndex )
    {
    aIndex = KErrNotFound;
    CXnNode* node = NULL;
    TInt siblingCount( aSiblingArray.Count() );
    //Find oldest sibling with display block
    for ( TInt i = 0; i < siblingCount; i++ )
        {
        node = aSiblingArray[i];
        if ( DisplayValueMatchL( *node, aDisplayValue ) )
            {
            aIndex = i;
            return node;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// ScrollBoxToBeginningL
// Show the oldest siblings that fit into the compound box.
// -----------------------------------------------------------------------------
//
static void ScrollBoxToBeginningL( RPointerArray< CXnNode >& aSiblingArray )
    {
    CXnNode* node = NULL;
    TInt toBeDisplayed(0);
    TInt siblingCount( aSiblingArray.Count() );
    for ( TInt i = 0; i < siblingCount; i++ )
        {
        node = aSiblingArray[i];
        if ( DisplayValueMatchL( *node, KBlock ) )
            {
            CXnDomStringPool* sp = node->DomNode()->StringPool();
            //Switch the display to 'none' to visible siblings.
            SetStringPropertyToNodeL( *sp, *node, KDisplay, KNone );
            node = aSiblingArray[toBeDisplayed];
            toBeDisplayed++;
            //Switch the display to 'block' to hidden siblings.
            SetStringPropertyToNodeL( *sp, *node, KDisplay, KBlock );
            }
        }
    }

// -----------------------------------------------------------------------------
// ScrollBoxToEndL
// Show the youngest siblings that fit into the compound box.
// -----------------------------------------------------------------------------
//
static void ScrollBoxToEndL( RPointerArray< CXnNode >& aSiblingArray )
    {
    CXnNode* node = NULL;
    TInt siblingCount( aSiblingArray.Count() );
    TInt toBeDisplayed( siblingCount - 1 );
    for ( TInt i = siblingCount; i > 0; )
        {
        node = aSiblingArray[--i];
        if ( DisplayValueMatchL( *node, KBlock ) )
            {
            CXnDomStringPool* sp = node->DomNode()->StringPool();
            //Switch the display to 'none' to visible siblings.
            SetStringPropertyToNodeL( *sp, *node, KDisplay, KNone );
            node = aSiblingArray[toBeDisplayed];
            toBeDisplayed--;
            //Switch the display to 'block' to hidden siblings.
            SetStringPropertyToNodeL( *sp, *node, KDisplay, KBlock );
            }
        }
    }

// -----------------------------------------------------------------------------
// IsCompoundNodeL
// Checks whether the node is coumound node.
// -----------------------------------------------------------------------------
//
static TBool IsCompoundNodeL( CXnNode* aNode )
    {
    if ( !aNode )
        {
        return EFalse;
        }
    CXnProperty* compoundProperty = aNode->GetPropertyL( KCompound );
    if ( compoundProperty )
        {
        const TDesC8& value = compoundProperty->StringValue();
        if ( !value.Compare( KTrue ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// IsLoopingSetL
// Checks whether the node is set to loop.
// -----------------------------------------------------------------------------
//
static TBool IsLoopingNodeL( CXnNode* aNode )
    {
    if ( !aNode )
        {
        return EFalse;
        }
    CXnProperty* navLoopProperty = aNode->GetPropertyL(
        XnPropertyNames::style::common::KS60NavLooping );
    if ( navLoopProperty )
        {
        const TDesC8& loopValue = navLoopProperty->StringValue();
        if ( loopValue == XnPropertyNames::style::common::s60_focus_looping::KStop )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// NotifyScrollNodeL
// Notify scroll node i.e compound box first and last siblings that there is
// nothing more to scroll
// -----------------------------------------------------------------------------
//
static void NotifyScrollNodeL( CXnNode* aNode, TBool aBegin, TBool aGainEnd )
    {
    RPointerArray< CXnNode >& siblings = aNode->Parent()->Children();
    // Last sibling is assumend to be an end scrolling node
    CXnNode* scrollNode = siblings[0];
    if ( !aBegin )
        {
        scrollNode = siblings[siblings.Count() - 1];
        }
    CXnNode* triggerNode =
        BuildTriggerNodeL( *scrollNode->UiEngine(), aGainEnd?KGainEnd:KLoseEnd );
    triggerNode->SetUiEngine( *scrollNode->UiEngine() );
    CleanupStack::PushL( triggerNode );
    scrollNode->ReportXuikonEventL( *triggerNode );
    CleanupStack::PopAndDestroy( triggerNode );
    }

// -----------------------------------------------------------------------------
// DoCompoundNodeBeginKeyL
// If key towards begin of list was pressed scroll to that direction
// -----------------------------------------------------------------------------
//
static void DoCompoundNodeBeginKeyL(
    RPointerArray< CXnNode >& aChildren,
    CXnNode* aFocusedNode )
    {
    TInt oldestIndex( 0 );
    // Notify scrollNode if aFocusedNode is node end of the list
    CXnNode* lastItem = aChildren[aChildren.Count() - 1];
    if ( aFocusedNode == lastItem && !IsLoopingNodeL( lastItem ) )
        {
        NotifyScrollNodeL( aChildren[0]->Parent(), EFalse, EFalse );
        }
    CXnNode* node = FindOldestSiblingWithDisplayL( aChildren, KBlock, oldestIndex );
    // Set next node's display to block
    if ( node && node == aFocusedNode )
        {
        if ( oldestIndex - 1 >= 0 )
            {
            node = aChildren[oldestIndex - 1];
            CXnDomStringPool* sp = node->DomNode()->StringPool();
            SetStringPropertyToNodeL( *sp, *node, KDisplay, KBlock );
            // find youngest sibling with display block
            TInt index( 0 );
            node = FindYoungestSiblingWithDisplayL( aChildren, KBlock, index );
            if ( node )
                {
                SetStringPropertyToNodeL( *sp, *node, KDisplay, KNone );
                }
            }
        else
            {
            if ( IsLoopingNodeL( node ) )
                {
                ScrollBoxToEndL( aChildren );
                }
            else
                {
                NotifyScrollNodeL( aChildren[0]->Parent(), ETrue, ETrue );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DoCompoundNodeEndKeyL
// If key towards end of list was pressed scroll to that direction
// -----------------------------------------------------------------------------
//
static void DoCompoundNodeEndKeyL(
    RPointerArray< CXnNode >& aChildren,
    CXnNode* aFocusedNode )
    {
    TInt index( 0 );
    TInt siblingCount( aChildren.Count() );
    // Notify scrollNode if aFocusedNode is first node of the list
    CXnNode* firstItem = aChildren[0];
    if ( aFocusedNode == firstItem && !IsLoopingNodeL( firstItem ) )
        {
        NotifyScrollNodeL( aChildren[0]->Parent(), ETrue, EFalse );
        }
    CXnNode* node = FindYoungestSiblingWithDisplayL( aChildren, KBlock, index );
    // Set next node's display to block
    if ( node && node == aFocusedNode )
        {
        if ( index < siblingCount - 1 )
            {
            node = aChildren[index + 1];
            CXnDomStringPool* sp = node->DomNode()->StringPool();
            SetStringPropertyToNodeL( *sp , *node, KDisplay, KBlock );
            // find oldest sibling with display block
            TInt oldestIndex( 0 );
            node = FindOldestSiblingWithDisplayL( aChildren, KBlock, oldestIndex );
            if ( node )
                {
                SetStringPropertyToNodeL( *sp, *node, KDisplay, KNone );
                }
            }
        else // we are end of list
            {
            if ( IsLoopingNodeL( node ) )
                {
                ScrollBoxToBeginningL( aChildren );
                }
            else
                {
                NotifyScrollNodeL( aChildren[0]->Parent(), EFalse, ETrue );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DoCompoundNodeScrollingL
// Process compound node key events
// -----------------------------------------------------------------------------
//
static void DoCompoundNodeScrollingL(
    CXnNode& aFocusedNode,
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return;
        }
    CXnNode* compoundNode = aFocusedNode.Parent();
    if ( IsCompoundNodeL( compoundNode ) )
        {
        RPointerArray< CXnNode >& children = compoundNode->Children();

        if ( aKeyEvent.iScanCode != EStdKeyLeftArrow  &&
             aKeyEvent.iScanCode != EStdKeyRightArrow &&
             aKeyEvent.iScanCode != EStdKeyDownArrow &&
             aKeyEvent.iScanCode != EStdKeyUpArrow )
            {
            return;
            }
        CXnNode* parent = aFocusedNode.Parent();
        if ( !parent )
            {
            return;
            }
        else
            {
            const TDesC8* direction(
                &XnPropertyNames::style::common::direction::KLTR );
            const TDesC8* orientation(
                &XnPropertyNames::style::common::block_progression::KTB );

            CXnProperty* directionProperty( parent->DirectionL() );

            if ( directionProperty )
                {
                direction = &directionProperty->StringValue();
                }

            CXnProperty* orientationProperty( parent->BlockProgressionL() );

            if ( orientationProperty )
                {
                orientation = &orientationProperty->StringValue();
                }

            if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
                 ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
                   *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
                {
                if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
                    {
                    DoCompoundNodeBeginKeyL( children, &aFocusedNode );
                    }
                if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
                    {
                    DoCompoundNodeEndKeyL( children, &aFocusedNode );
                    }
                }
            else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                      ( *orientation == XnPropertyNames::style::common::block_progression::KLR ||
                        *orientation == XnPropertyNames::style::common::block_progression::KRL ) )
                {
                if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
                    {
                    DoCompoundNodeEndKeyL( children, &aFocusedNode );
                    }
                if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
                    {
                    DoCompoundNodeBeginKeyL( children, &aFocusedNode );
                    }
                }
            else if ( *direction == XnPropertyNames::style::common::direction::KLTR &&
                      *orientation == XnPropertyNames::style::common::block_progression::KTB )
                {
                if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
                    {
                    DoCompoundNodeBeginKeyL( children, &aFocusedNode );
                    }
                else if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
                    {
                    DoCompoundNodeEndKeyL( children, &aFocusedNode );
                    }
                }
            else if ( *direction == XnPropertyNames::style::common::direction::KRTL &&
                      *orientation == XnPropertyNames::style::common::block_progression::KTB )
                {
                if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
                    {
                    DoCompoundNodeBeginKeyL( children, &aFocusedNode );
                    }
                else if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
                    {
                    DoCompoundNodeEndKeyL( children, &aFocusedNode );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// BuildTriggerNodeL
// Builds a trigger node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerNodeL(
    CXnUiEngine& aUiEngine,
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
    CXnDomPropertyValue* nameValue =
        CXnDomPropertyValue::NewL( aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CleanupStack::Pop( node );
    return node;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNodeImpl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNodeImpl* CXnNodeImpl::NewL( CXnType* aType )
    {
    CXnNodeImpl* self = new ( ELeave ) CXnNodeImpl;

    CleanupStack::PushL( self );
    self->ConstructL( aType );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::ConstructL( CXnType* aType )
    {
    iType = aType;
    iPropertyList = CXnPropertyList::NewL();
    iLayoutCapable = ETrue;
    iHandleTooltip = ETrue;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::CXnNodeImpl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNodeImpl::CXnNodeImpl()
    : iDropped( 0 ), iChildren( 1 ), iAdaptive( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::~CXnNodeImpl()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnNodeImpl::~CXnNodeImpl()
    {
    delete iPropertyList;
    delete iType;
    iPropertyPseudoClasses.Reset();
    iStates.Reset();
    iChildren.ResetAndDestroy();
    delete iLayoutPropertyCache;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetNode
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetNode( CXnNode& aNode )
    {
    iNode = &aNode;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Node
// -----------------------------------------------------------------------------
//
CXnNode* CXnNodeImpl::Node()
    {
    return iNode;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetRect
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetRect( const TRect& aRect )
    {
    iRect = aRect;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Rect
// -----------------------------------------------------------------------------
//
TRect CXnNodeImpl::Rect()
    {
    return AdjustRectIfNeeded(iRect);
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetBorderRect
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetBorderRect( const TRect& aRect )
    {
    iBorderRect = aRect;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderRect
// -----------------------------------------------------------------------------
//
TRect CXnNodeImpl::BorderRect()
    {
    return AdjustRectIfNeeded(iBorderRect);
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetNormalFlowBorderRect
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetNormalFlowBorderRect( const TRect& aRect )
    {
    iNormalFlowBorderRect = aRect;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::NormalFlowBorderRect
// -----------------------------------------------------------------------------
//
TRect CXnNodeImpl::NormalFlowBorderRect()
    {
    return AdjustRectIfNeeded(iNormalFlowBorderRect);
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetMarginRect
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetMarginRect( const TRect& aRect )
    {
    iMarginRect = aRect;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MarginRect
// -----------------------------------------------------------------------------
//
TRect CXnNodeImpl::MarginRect()
    {
    return AdjustRectIfNeeded(iMarginRect);
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetPaddingRect
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetPaddingRect( const TRect& aRect )
    {
    iPaddingRect = aRect;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PaddingRect
// -----------------------------------------------------------------------------
//
TRect CXnNodeImpl::PaddingRect()
    {
    return AdjustRectIfNeeded(iPaddingRect);
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Type()
// Returns control type.
// -----------------------------------------------------------------------------
//
CXnType* CXnNodeImpl::Type()
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Parent
// Get component parent
// -----------------------------------------------------------------------------
//
CXnNode* CXnNodeImpl::Parent() const
    {
    return iParent;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetParent
// Set component parent
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetParent( CXnNode& aParent )
    {
    iParent = &aParent;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::AddChildL
// Adds a child to this container.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::AddChildL( CXnNode* aChild )
    {
    TInt err = iChildren.Append( aChild );
    if ( err != KErrNone )
        {
        User::Leave( KXnErrAddChildToAreaFailed );
        }
    aChild->SetParent( *iNode );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Children
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >& CXnNodeImpl::Children()
    {
    return iChildren;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetPCDataL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetPCDataL( const TDesC8& aData )
    {
    iDomNode->SetPCDataL( aData );

    SetDirtyL( XnDirtyLevel::ERender );

    CXnDomStringPool* sp( iDomNode->StringPool() );

    CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( sp );
    CleanupStack::PushL( newValue );
    newValue->SetStringValueL( CXnDomPropertyValue::EString, KNullDesC8 );
    // create property
    CXnProperty* prop = CXnProperty::NewL(
        XnPropertyNames::common::KPCData, newValue, *sp );
    CleanupStack::Pop( newValue );
    CleanupStack::PushL( prop );

    InformPropertyChangeL( *iNode, prop );

    CleanupStack::PopAndDestroy( prop );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::GetPCData
// Set a property.
// -----------------------------------------------------------------------------
//
const TDesC8& CXnNodeImpl::GetPCData()
    {
    return iDomNode->PCData();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetHandleTooltip()
// Sets wheter to handle tooltips
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetHandleTooltip( TBool aFlag )
    {
    iHandleTooltip = aFlag;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ShowPopupsL()
// Searchs and shoes tooltips
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::ShowPopupsL( TRect aRect, TInt aSource )
    {
    if ( iHandleTooltip )
        {
        for ( TInt i = 0; i < iChildren.Count(); i++ )
            {
            CXnNode* node( iChildren[i] );

            if ( node->Type()->Type() == KToolTip )
                {
                CXnNodeAppIf& tooltipNode( node->AppIfL() );
                CXnPopup* popup( NULL );

                XnComponentInterface::MakeInterfaceL( popup, tooltipNode );

                if ( popup && aSource != XnEventSource::EStylus )
                    {
                    popup->ShowPopupL( aRect );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::HidePopupsL()
// Searchs and hides tooltips
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::HidePopupsL()
    {
    if ( iHandleTooltip )
        {
        for ( TInt i = 0; i < iChildren.Count(); i++ )
            {
            CXnNode* node( iChildren[i] );

            if ( node->Type()->Type() == KToolTip )
                {
                CXnNodeAppIf& tooltipNode( node->AppIfL() );
                CXnPopup* popup( NULL );

                XnComponentInterface::MakeInterfaceL( popup, tooltipNode );

                if ( popup )
                    {
                    popup->HidePopupL();
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetPropertyL
// Sets a property, informs property change and builds trigger.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetPropertyL( CXnProperty* aProperty, TBool aNotify )
    {
    TInt level( DoSetPropertyL( aProperty ) );

    if ( !aNotify || level == XnDirtyLevel::ENone )
        {
        // all done
        return;
        }

    InformPropertyChangeL( *iNode, aProperty );

    if ( iLayoutCapable && iNode->Parent() )
        {
        const TDesC8* value( NULL );
        const TDesC8& name( aProperty->Property()->Name() );
        TBool informChildren( EFalse );

        if ( name == XnPropertyNames::style::common::KDisplay )
            {
            value = &XnPropertyNames::style::common::display::KNone;
            informChildren = ETrue;
            }
        else if ( name == XnPropertyNames::style::common::KVisibility )
            {
            value = &XnPropertyNames::style::common::visibility::KHidden;
            informChildren = ETrue;
            }

        if ( value )
            {
            if ( *value == aProperty->StringValue() )
                {
                CXnNode* loseVisualisation(
                    BuildTriggerNodeL(
                        *iUiEngine,
                        XnPropertyNames::action::trigger::name::KLoseVisualisation ) );

                CleanupStack::PushL( loseVisualisation );

                CXnNode* focused( iUiEngine->FocusedNode() );

                for ( CXnNode* node = focused; node; node = node->Parent() )
                    {
                    if ( node == iNode )
                        {
                        // Focus is either in this node or
                        // somewhere in this node's childrens
                        focused->UnsetStateL(
                            XnPropertyNames::style::common::KFocus );

                        focused->UnsetStateL(
                            XnPropertyNames::style::common::KHold );

                        focused->UnsetStateL(
                            XnPropertyNames::style::common::KActive );

                        // Report losevisualisation to focused node if the node
                        // is 'visualisationaware'
                        CXnProperty* visaware = GetPropertyL(
                            XnPropertyNames::common::KVisualisationAware );

                        if ( visaware && visaware->StringValue() ==
                             XnPropertyNames::KTrue )
                            {
                            focused->ReportXuikonEventL( *loseVisualisation );
                            }

                        break;
                        }
                    }

                if ( focused != iNode )
                    {
                    // Report losevisualisation to this node if the node
                    // is 'visualisationaware'
                    CXnProperty* visaware = GetPropertyL(
                        XnPropertyNames::common::KVisualisationAware );
                    if ( visaware && visaware->StringValue() ==
                         XnPropertyNames::KTrue )
                        {
                        iNode->ReportXuikonEventL( *loseVisualisation );
                        }
                    }

                CleanupStack::PopAndDestroy( loseVisualisation );
                }
            else
                {
                if ( !iUiEngine->FocusedNode() )
                    {
                    CXnNode* node( IsNodeNavigableL( Node() ) );

                    if ( node )
                        {
                        // This node can be focused
                        iUiEngine->AddFocusCandidateL( node );
                        }
                    }
                }
            }

        if ( informChildren && ( iNode->Children().Count() > 0 ) )
            {
            //we have already done inform for the parent, so EFalse
            InformChildrenPropertyChangeL( *iNode, aProperty, EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetPropertyWithoutNotificationL
// Sets a property.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetPropertyWithoutNotificationL( CXnProperty* aProperty )
    {
    SetPropertyL( aProperty, EFalse );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetPropertyArrayL
// Sets an array of properties and after all the properties are set informs
// property change.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetPropertyArrayL(
    RPointerArray< CXnProperty >& aPropertyArray )
    {
    TBool informPropertyChange( EFalse );

    TInt count( aPropertyArray.Count() );

    iUiEngine->DisableRenderUiLC();

    for ( TInt i = 0; i < count; i++ )
        {
        CXnProperty* aProp( aPropertyArray[i] );

        const TDesC8& name( aProp->Property()->Name() );

        if ( name == XnPropertyNames::style::common::KDisplay ||
             name == XnPropertyNames::style::common::KVisibility )
            {
            SetPropertyL( aProp );
            }
        else
            {
            SetPropertyL( aProp, EFalse );
            informPropertyChange = ETrue;
            }
        }

    if ( informPropertyChange )
        {
        InformPropertyChangeL( *iNode, NULL );
        }

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::DoSetPropertyL
// Does the actual property update
// -----------------------------------------------------------------------------
//
TInt CXnNodeImpl::DoSetPropertyL( CXnProperty* aProperty )
    {
    TInt level( XnDirtyLevel::ENone );

    const TDesC8& name( aProperty->Property()->Name() );

    CXnProperty* prop( GetPropertyL( name ) );

    if ( prop && prop->EqualsL( *aProperty ) )
        {
        // No change in the property value
        delete aProperty;

        return level;
        }

    level = DirtyLevelFromPropertyL( *iNode, name );

    // Save the displayed state, before setting the property
    TBool isDisplayed( IsNodeDisplayedL( *iNode ) );

    CXnDomProperty::TPseudoClass pseudoClass( aProperty->Property()->PseudoClass() );

    if ( pseudoClass != CXnDomProperty::ENone )
        {
        InsertPropertyPseudoClassL( pseudoClass, iPropertyPseudoClasses );

        if ( IsStateSet( PseudoClassName( pseudoClass ) ) )
            {
            SetCachedProperty(
                aProperty,
                iLayoutPropertyCache,
                iName,
                iValue,
                iLabel,
                iInitialFocus,
                iClass,
                iId,
                iPath,
                iMaskPath,
                EFalse,
                EFalse );
            }
        }
    else
        {
        SetCachedProperty(
            aProperty,
            iLayoutPropertyCache,
            iName,
            iValue,
            iLabel,
            iInitialFocus,
            iClass,
            iId,
            iPath,
            iMaskPath,
            EFalse,
            EFalse );
        }

    iPropertyList->SetPropertyL( aProperty );

    TBool isDisplayedNow( IsNodeDisplayedL( *iNode ) );

    level = ( iAdaptive & XnAdaptive::EIgnoreDirty ) ? XnDirtyLevel::ENone : level;

    // This is from layout calculation point of view
    TBool displayed( ETrue );

    if ( isDisplayed && !isDisplayedNow )
        {
        // Node loses its visualisation (either display: none, or 
        // visibility: hidden) Make sure control is hidden
        CCoeControl* ctrl( iNode->Control() );

        if ( ctrl )
            {
            ctrl->MakeVisible( EFalse );
            }
        }
    else if ( !isDisplayed && isDisplayedNow )
        {
        // Node gains its visualisation (both display: block, and
        // visibility: visible)
        // Run gainvisualisation trigger, if node is 'visualisationaware'
        CXnProperty* visaware(
                GetPropertyL( XnPropertyNames::common::KVisualisationAware ) );

        if ( visaware && visaware->StringValue() == XnPropertyNames::KTrue )
            {
            CXnNode* gainVisualisation( BuildTriggerNodeL( *iUiEngine,
                XnPropertyNames::action::trigger::name::KGainVisualisation ) );
            CleanupStack::PushL( gainVisualisation );
            iNode->ReportXuikonEventL( *gainVisualisation );
            CleanupStack::PopAndDestroy( gainVisualisation );
            }
        }
    else if ( !isDisplayed && !isDisplayedNow )
        {
        // No change
        displayed = EFalse;
        }

    SetDirtyL( level, displayed );

    return level;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::InitializePropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::InitializePropertyL( CXnProperty* aProperty )
    {
    if ( aProperty->Property()->PseudoClass() != CXnDomProperty::ENone )
        {
        InsertPropertyPseudoClassL(
            aProperty->Property()->PseudoClass(), iPropertyPseudoClasses );

        SetCachedProperty(
            aProperty,
            iLayoutPropertyCache,
            iName,
            iValue,
            iLabel,
            iInitialFocus,
            iClass,
            iId,
            iPath,
            iMaskPath,
            ETrue,
            ETrue );
        }
    else
        {
        SetCachedProperty(
            aProperty,
            iLayoutPropertyCache,
            iName,
            iValue,
            iLabel,
            iInitialFocus,
            iClass,
            iId,
            iPath,
            iMaskPath,
            ETrue,
            EFalse );
        }

    iPropertyList->SetPropertyL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::GetPropertyL
// Gets a property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::GetPropertyL( const TDesC8& aKey )
    {
    CXnDomProperty* attribute = NULL;
    CXnProperty* property = iPropertyList->GetProperty( aKey );

    if ( property )
        {
        attribute = property->Property();
        CXnDomList& list = attribute->PropertyValueList();

        if ( list.Length() == 0 )
            {
            return NULL;
            }

        CXnDomPropertyValue* attributeValue =
            static_cast< CXnDomPropertyValue* >( list.Item( 0 ) );

        if ( !attributeValue )
            {
            return NULL;
            }

        if ( attributeValue->IsInheritIdent() && iParent )
            {
            // inherited values are queried from the parent
            property = iParent->GetPropertyL( aKey );
            }

        // otherwise just return the value
        return property;
        }

    // if not found, return NULL
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetStateL
// Set a pseudoclass
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetStateL( const TDesC8& aState, TInt aSource )    
    {
    if ( IsStateSet( aState ) )
        {
        return;
        }

    TBool focusReceived( EFalse );
    TBool activated( EFalse );
    TBool stateChanged( EFalse );

    if ( aState == XnPropertyNames::action::trigger::name::KFocus )
        {
        if ( !iUiEngine->AppUiAdapter().FocusShown() )
            {
            return;
            }
        
        focusReceived = DoReceiveFocusL( *iNode, *iUiEngine );

        if ( focusReceived )
            {
            // focused
            HidePopupsL();
            ShowPopupsL( iNode->Rect(), aSource );

            if ( IsStateSet( XnPropertyNames::action::trigger::name::KPassiveFocus ) )
                {
                iUiEngine->RemovePassiveFocusedNodeL( iNode );
                }

            stateChanged = focusReceived = iPropertyList->SetStateL( aState );            
            }
        }
    else if ( aState == XnPropertyNames::style::common::KPressedDown )
        {
        if ( IsStateSet( XnPropertyNames::action::trigger::name::KFocus ) )
            {
            iPropertyList->SetStateL( 
                XnPropertyNames::style::common::KPressedDown );
            iNode->SetDirtyL( XnDirtyLevel::ERender );            
            }
        }        
    else if ( aState == XnPropertyNames::style::common::KActive )
        {
        activated = ETrue;
        }
    else if ( aState == XnPropertyNames::action::trigger::name::KPassiveFocus )
        {
        // is focusable node
        if ( DoReceiveFocusL( *iNode, *iUiEngine ) )
            {
            // focused
            stateChanged = iPropertyList->SetStateL( aState );
            }
        }
    else
        {
        stateChanged = iPropertyList->SetStateL( aState );
        }

    if ( stateChanged )
        {
        CXnDomProperty::TPseudoClass pseudoClass( PseudoClassFromName( aState ) );

        if ( pseudoClass != CXnDomProperty::ENone )
            {
            iStates.Append( pseudoClass );
            
            // Ensure that pseudo class is defined
            InsertPropertyPseudoClassL( pseudoClass, iPropertyPseudoClasses );

            if ( IsPropertyPseudoClass( pseudoClass, iPropertyPseudoClasses ) )
                {
                EnableStatePropertiesL( *iNode, pseudoClass, iLayoutPropertyCache );

                InformPropertyChangeL( *iNode );
                }
            }
        
        if ( aState == XnPropertyNames::style::common::KPressedDown )            
            {
            iNode->SetDirtyL( XnDirtyLevel::ERender );
            }
        }

    if ( focusReceived && aState == XnPropertyNames::action::trigger::name::KFocus )
        {                
        iUiEngine->SetFocusedNodeL( iNode, aSource );
        }

    if ( activated )
        {
        iUiEngine->AppUiAdapter().ItemActivator().ActivateL( iNode );
        }

    if ( stateChanged )
        {
        iUiEngine->RenderUIL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IsStateSet
// Check whether a state is set or not
// -----------------------------------------------------------------------------
//
TBool CXnNodeImpl::IsStateSet( const TDesC8& aState )
    {
    return iPropertyList->IsStateSet( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetStateWithoutNotificationL
// Set a pseudoclass
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetStateWithoutNotificationL( const TDesC8& aState )
    {
    if ( IsStateSet( aState ) )
        {
        return;
        }

    TBool setState( ETrue );

    if ( aState == XnPropertyNames::action::trigger::name::KFocus )
        {
        if ( !iUiEngine->AppUiAdapter().FocusShown() )
            {
            return;
            }
        
        setState = DoReceiveFocusL( *iNode, *iUiEngine );
        }

    if ( setState && iPropertyList->SetStateL( aState ) )
        {
        CXnDomProperty::TPseudoClass pseudoClass( PseudoClassFromName( aState ) );

        if ( pseudoClass != CXnDomProperty::ENone )
            {
            iStates.Append( pseudoClass );

            // Ensure that pseudo class is defined
            InsertPropertyPseudoClassL( pseudoClass, iPropertyPseudoClasses );

            if ( IsPropertyPseudoClass( pseudoClass, iPropertyPseudoClasses ) )
                {
                EnableStatePropertiesL( *iNode, pseudoClass, iLayoutPropertyCache );
                }
            }

        if ( aState == XnPropertyNames::action::trigger::name::KFocus )
            {
            iUiEngine->SetFocusedNodeL( iNode );

            HidePopupsL();
            }

        iUiEngine->RenderUIL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::UnsetStateL
// Unset a pseudoclass
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::UnsetStateL( const TDesC8& aState )
    {
    if ( !IsStateSet( aState ) )
        {
        return;
        }

    if ( aState == XnPropertyNames::action::trigger::name::KFocus )
        {
        HidePopupsL();
        
        iPropertyList->UnsetState( 
            XnPropertyNames::style::common::KPressedDown );
        
        if ( iUiEngine->FocusedNode() == iNode )
            {
            iUiEngine->SetFocusedNodeL( NULL );
            }
        }

    TBool stateChanged( iPropertyList->UnsetState( aState ) );

    if ( stateChanged )
        {
        CXnDomProperty::TPseudoClass pseudoClass( PseudoClassFromName( aState ) );

        if ( IsPropertyPseudoClass( pseudoClass, iPropertyPseudoClasses ) )
            {
            DisableStatePropertiesL(
                *iNode, pseudoClass, iStates, iLayoutPropertyCache );

            InformPropertyChangeL( *iNode );
            }

        for ( TInt i = iStates.Count() - 1; i >= 0; --i )
            {
            if ( iStates[i] == pseudoClass )
                {
                iStates.Remove( i );
                }
            }
        
        if ( aState == XnPropertyNames::style::common::KPressedDown )
            {
            iNode->SetDirtyL( XnDirtyLevel::ERender );
            }        

        iUiEngine->RenderUIL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetUiEngine
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetUiEngine( CXnUiEngine& aEngine )
    {
    iUiEngine = &aEngine;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::UiEngine
// -----------------------------------------------------------------------------
//
CXnUiEngine* CXnNodeImpl::UiEngine()
    {
    return iUiEngine;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ReportXuikonEventL
// -----------------------------------------------------------------------------
//
TBool CXnNodeImpl::ReportXuikonEventL( CXnNode& aEventData, TInt aSource )
    {
    CXnProperty* prop( GetPropertyL( KActionsHandler ) );
    
    CXnNode* handler( NULL );
    
    iUiEngine->DisableRenderUiLC();
    
    if ( prop )
        {
        const TDesC8& value( prop->StringValue() );
        
        handler = iUiEngine->FindNodeByIdL( value, Namespace() );       
        }
    
    if ( handler && handler->Type()->Type() == KActionsHandler )
        {
        // Let <actionshandler> node to handle actions
        DoTriggerEventL( this, *iUiEngine, *iNode, 
                         *handler, aEventData, aSource );
        }    
    else
        {
        // Let this node to handle actions
        DoTriggerEventL( this, *iUiEngine, *iNode, 
                         *iNode, aEventData, aSource );
        }
    
    CleanupStack::PopAndDestroy(); // DisableRenderUiLC
           
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::OfferKeyEventL
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    DoTriggerKeyEventL( this, *iUiEngine, *iNode, aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetDropped
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetDropped( const TInt aDropped )
    {
    if ( Type()->Type() != KToolTip )
        {
        iDropped = aDropped;

        TInt count( iChildren.Count() );

        for ( TInt i = 0; i < count; i++ )
            {
            iChildren[i]->SetDropped( aDropped );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IsDropped
// -----------------------------------------------------------------------------
//
TInt CXnNodeImpl::IsDropped() const
    {
    return iDropped;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetLayoutCapable
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetLayoutCapable( const TBool aLayoutCapable )
    {
    iLayoutCapable = aLayoutCapable;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IsLayoutCapable
// -----------------------------------------------------------------------------
//
TBool CXnNodeImpl::IsLayoutCapable() const
    {
    return iLayoutCapable;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetRenderedL
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetRenderedL()
    {
    if ( !iLayoutCapable )
        {
        return;
        }

    CXnControlAdapter* control =
        static_cast< CXnControlAdapter* >( iNode->Control() );

    if ( control )
        {
        TBool displayed( IsNodeDisplayedL( *iNode, ETrue, EFalse ) );

        control->SetBlank( EFalse );

        if ( iDropped || !displayed )
            {
            // Need to check blank here
            CXnProperty* visibility( VisibilityL() );
            CXnProperty* display( DisplayL() );
            
            if ( visibility && visibility->StringValue() ==
                 XnPropertyNames::style::common::visibility::KBlank && 
                 ( !display || display->StringValue() 
                         == XnPropertyNames::style::common::display::KBlock ) )
                {
                control->SetBlank( ETrue );
                control->MakeVisible( EFalse );
                control->SetComponentsToInheritVisibility( EFalse );
                control->MakeVisible( ETrue );
                control->SetComponentsToInheritVisibility( ETrue );
                }
            else
                {
                if ( control->IsVisible() )
                    {
                    control->MakeVisible( EFalse );
                    }
                }
            }
        else
            {
            CCoeControl* parent( control->Parent() );

            TBool parentVisible( ETrue );

            if ( parent && !parent->IsVisible() )
                {
                parentVisible = EFalse;
                }

            if ( !control->IsVisible() && parentVisible )
                {
                TBool found( EFalse );

                for ( CXnNode* node = iNode; !found && node; node = node->Parent() )
                    {
                    if ( node->Type()->Type() == KToolTip )
                        {
                        // Tooltip control handles its visibility internally
                        found = ETrue;
                        }
                    }

                if ( !found )
                    {
                    control->MakeVisible( ETrue );
                    }
                }
            }
        }

    iRendered = ETrue;

    for ( TInt i = 0; i < iChildren.Count(); i++ )
        {
        iChildren[i]->SetRenderedL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetLaidOutL
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetLaidOutL()
    {
    if ( !iLayoutCapable )
        {
        return;
        }

    if ( iDropped || !IsNodeDisplayedL( *iNode ) )
        {
        return;
        }

    iLaidOut = ETrue;

    for ( TInt i = 0; i < iChildren.Count(); i++ )
        {
        iChildren[i]->SetLaidOutL();
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IsLaidOut
// -----------------------------------------------------------------------------
//
TBool CXnNodeImpl::IsLaidOut() const
    {
    return iLaidOut;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ClearRenderedAndLaidOut
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::ClearRenderedAndLaidOut()
    {
    if ( !iRendered && !iLaidOut )
        {
        return;
        }

    TRect empty;

    SetRect( empty );
    SetPaddingRect( empty );
    SetBorderRect( empty );
    SetNormalFlowBorderRect( empty );
    SetMarginRect( empty );

    iRendered = EFalse;
    iLaidOut = EFalse;

    if ( iLayoutCapable )
        {
        for ( TInt i = 0; i < iChildren.Count(); i++ )
            {
            iChildren[i]->ClearRenderedAndLaidOut();
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
XnComponentInterface::MXnComponentInterface* CXnNodeImpl::MakeInterfaceL(
    const TDesC8& /*aType*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// PropertyL
// Returns a property.
// -----------------------------------------------------------------------------
//
static CXnProperty* PropertyL( CXnProperty* aProperty,
    CXnProperty* ( CXnNode::* aFunc )(),
    TAny* aLayoutPropertyCache, CXnNode* aParent )
    {
    if ( aLayoutPropertyCache && aProperty )
        {
        CXnDomProperty* attribute = aProperty->Property();
        CXnDomList& list = attribute->PropertyValueList();

        if ( list.Length() == 0 )
            {
            return NULL;
            }

        CXnDomPropertyValue* attributeValue =
            static_cast< CXnDomPropertyValue* >( list.Item( 0 ) );

        if ( !attributeValue )
            {
            return NULL;
            }

        if ( attributeValue->IsInheritIdent() && aParent )
            {
            // inherited values are queried from the parent
            return ( aParent->*aFunc )();
            }

        // otherwise just return the value
        return aProperty;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::WidthL
// Returns width property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::WidthL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iWidth : NULL;
    return PropertyL( property, &CXnNode::WidthL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::HeightL
// Returns height property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::HeightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iHeight : NULL;
    return PropertyL( property, &CXnNode::HeightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MarginLeftL
// Returns margin-left property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MarginLeftL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ?
        iLayoutPropertyCache->iMarginLeft :
        NULL;
    return PropertyL( property, &CXnNode::MarginLeftL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MarginRightL
// Returns margin-right property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MarginRightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ?
        iLayoutPropertyCache->iMarginRight :
        NULL;
    return PropertyL( property, &CXnNode::MarginRightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderLeftL
// Returns border-left property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderLeftL()
    {
    CXnProperty* property = iLayoutPropertyCache ?
        iLayoutPropertyCache->iBorderLeft :
        NULL;
    return PropertyL( property, &CXnNode::BorderLeftL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderRightL
// Returns border-right property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderRightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ?
        iLayoutPropertyCache->iBorderRight :
        NULL;
    return PropertyL( property, &CXnNode::BorderRightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PaddingLeftL
// Returns padding-left property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PaddingLeftL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ?
        iLayoutPropertyCache->iPaddingLeft :
        NULL;
    return PropertyL( property, &CXnNode::PaddingLeftL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PaddingRightL
// Returns PaddingRightL property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PaddingRightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iPaddingRight : NULL;
    return PropertyL( property, &CXnNode::PaddingRightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MarginTopL
// Returns margin-top property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MarginTopL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMarginTop : NULL;
    return PropertyL( property, &CXnNode::MarginTopL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MarginBottomL
// Returns margin-bottom property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MarginBottomL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMarginBottom : NULL;
    return PropertyL( property, &CXnNode::MarginBottomL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderTopL
// Returns border-top property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderTopL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderTop : NULL;
    return PropertyL( property, &CXnNode::BorderTopL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderBottomL
// Returns border-bottom property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderBottomL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderBottom : NULL;
    return PropertyL( property, &CXnNode::BorderBottomL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PaddingTopL
// Returns padding-top property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PaddingTopL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iPaddingTop : NULL;
    return PropertyL( property, &CXnNode::PaddingTopL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PaddingBottomL
// Returns padding-bottom property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PaddingBottomL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iPaddingBottom : NULL;
    return PropertyL( property, &CXnNode::PaddingBottomL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderWidthL
// Returns border-width property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderWidthL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderWidth : NULL;
    return PropertyL( property, &CXnNode::BorderWidthL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BlockProgressionL
// Returns block-progression property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BlockProgressionL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBlockProgression : NULL;
    return PropertyL( property, &CXnNode::BlockProgressionL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::DirectionL
// Returns direction property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::DirectionL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iDirection : NULL;
    return PropertyL( property, &CXnNode::DirectionL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PositionL
// Returns position property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PositionL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iPosition : NULL;
    return PropertyL( property, &CXnNode::PositionL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MaxHeightL
// Returns max-height property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MaxHeightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMaxHeight : NULL;
    return PropertyL( property, &CXnNode::MaxHeightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MinHeightL
// Returns min-height property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MinHeightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMinHeight : NULL;
    return PropertyL( property, &CXnNode::MinHeightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MaxWidthL
// Returns max-width property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MaxWidthL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMaxWidth : NULL;
    return PropertyL( property, &CXnNode::MaxWidthL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MinWidthL
// Returns min-width property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MinWidthL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iMinWidth : NULL;
    return PropertyL( property, &CXnNode::MinWidthL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::DisplayL
// Returns display property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::DisplayL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iDisplay : NULL;
    return PropertyL( property, &CXnNode::DisplayL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::LeftL
// Returns left property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::LeftL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iLeft : NULL;
    return PropertyL( property, &CXnNode::LeftL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::RightL
// Returns right property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::RightL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iRight : NULL;
    return PropertyL( property, &CXnNode::RightL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::TopL
// Returns top property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::TopL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iTop : NULL;
    return PropertyL( property, &CXnNode::TopL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BottomL
// Returns bottom property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BottomL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBottom : NULL;
    return PropertyL( property, &CXnNode::BottomL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderLeftStyleL
// Returns border-left-style property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderLeftStyleL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderLeftStyle : NULL;
    return PropertyL( property, &CXnNode::BorderLeftStyleL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderRightStyleL
// Returns border-right-style property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderRightStyleL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderRightStyle : NULL;
    return PropertyL( property, &CXnNode::BorderRightStyleL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderTopStyleL
// Returns border-top-style property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderTopStyleL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderTopStyle : NULL;
    return PropertyL( property, &CXnNode::BorderTopStyleL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderBottomStyleL
// Returns border-bottom-style property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderBottomStyleL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderBottomStyle : NULL;
    return PropertyL( property, &CXnNode::BorderBottomStyleL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderStyleL
// Returns border-style property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderStyleL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderStyle : NULL;
    return PropertyL( property, &CXnNode::BorderStyleL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BorderImageL
// Returns border-image property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BorderImageL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBorderImage : NULL;
    return PropertyL( property, &CXnNode::BorderImageL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::DisplayPriorityL
// Returns display-priority property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::DisplayPriorityL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iDisplayPriority : NULL;
    return PropertyL( property, &CXnNode::DisplayPriorityL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::NameL
// Returns name property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::NameL()
    {
    return PropertyL( iName, &CXnNode::NameL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ValueL
// Returns value property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::ValueL()
    {
    return PropertyL( iValue, &CXnNode::ValueL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::VisibilityL
// Returns visibility property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::VisibilityL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iVisibility : NULL;
    return PropertyL( property, &CXnNode::VisibilityL, 
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::LabelL
// Returns label property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::LabelL()
    {
    return PropertyL( iLabel, &CXnNode::LabelL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::InitialFocusL
// Returns initial-focus property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::InitialFocusL()
    {
    return PropertyL( iInitialFocus, &CXnNode::InitialFocusL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ClassL
// Returns class property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::ClassL()
    {
    return PropertyL( iClass, &CXnNode::ClassL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IdL
// Returns id property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::IdL()
    {
    if ( iId )
        {
        CXnDomProperty* attribute = iId->Property();
        CXnDomList& list = attribute->PropertyValueList();
        if ( list.Length() == 0 )
            {
            return NULL;
            }
        CXnDomPropertyValue* attributeValue =
            static_cast< CXnDomPropertyValue* >( list.Item( 0 ) );
        if ( !attributeValue )
            {
            return NULL;
            }
        if ( attributeValue->IsInheritIdent()
            && iParent )
            {
            // inherited values are queried from the parent
            return iParent->IdL();
            }
        // otherwise just return the value
        return iId;
        }
    // if not found, return NULL
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::PathL
// Returns path property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::PathL()
    {
    return PropertyL( iPath, &CXnNode::PathL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::MaskPathL
// Returns maskpath property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::MaskPathL()
    {
    return PropertyL( iMaskPath, &CXnNode::MaskPathL, this, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::NavIndexL
// Returns nav-index property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::NavIndexL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iNavIndex : NULL;
    return PropertyL( property, &CXnNode::NavIndexL, 
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ZIndexL
// Returns z-index property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::ZIndexL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iZIndex : NULL;
    return PropertyL( property, &CXnNode::ZIndexL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BackgroundColorL
// Returns background-color property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BackgroundColorL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBackgroundColor : NULL;
    return PropertyL( property, &CXnNode::BackgroundColorL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::BackgroundImageL
// Returns background-image property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::BackgroundImageL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iBackgroundImage : NULL;
    return PropertyL( property, &CXnNode::BackgroundImageL,
        iLayoutPropertyCache, iParent );
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::FocusBackgroundL
// Returns focus-background property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNodeImpl::FocusBackgroundL()
    {
    CXnProperty* property =
        iLayoutPropertyCache ? iLayoutPropertyCache->iFocusBackground : NULL;

    return PropertyL( property, &CXnNode::FocusBackgroundL,
        iLayoutPropertyCache, iParent );
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::SetDomNode
// Sets DOM node
// ----------------------------------------------------------------------------
//
void CXnNodeImpl::SetDomNode( CXnDomNode* aDomNode )
    {
    iDomNode = aDomNode;
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::DomNode
// Returns the DOM node pointer
// ----------------------------------------------------------------------------
//
CXnDomNode* CXnNodeImpl::DomNode()
    {
    return iDomNode;
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::MeasureAdaptiveContentL
// Measures the adaptive content dimensions.
// ----------------------------------------------------------------------------
//
TSize CXnNodeImpl::MeasureAdaptiveContentL( const TSize& aAvailableSize )
    {
    TSize empty;

    TInt adaptive( IsAdaptive() );

    if ( aAvailableSize.iWidth <= 0 && ( adaptive & XnAdaptive::EWidth ) ||
         aAvailableSize.iHeight <= 0 && ( adaptive & XnAdaptive::EHeight ) )
        {
        // No available size, don't even try to measure
        return empty;
        }

    // node is displayed as default
    TBool displayed( ETrue );

    CXnProperty* displayProp( DisplayL() );

    if ( displayProp )
        {
        const TDesC8& value( displayProp->StringValue() );

        if ( value != XnPropertyNames::style::common::display::KBlock )
            {
            // Not displayed
            displayed = EFalse;
            }
        }

    if ( adaptive && iLayoutCapable && !iDropped && displayed )
        {
        CXnControlAdapter* adapter( iNode->Control() );

        // What is my size?
        if ( adapter )
            {
            TInt width( 0 );
            TInt height( 0 );

            TSize size = adapter->MeasureAdaptiveContentL( aAvailableSize );

            if ( adaptive & XnAdaptive::EWidth )
                {
                width = size.iWidth;
                }

            if ( adaptive & XnAdaptive::EHeight )
                {
                height = size.iHeight;
                }

            return TSize( width, height );
            }
        }

    return empty;
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::IsAdaptive
// Checks if the node have adaptive content
// ----------------------------------------------------------------------------
//
TInt CXnNodeImpl::IsAdaptive( TBool aIgnoreSizeFixed ) const
    {
    if ( !iAdaptive )
        {
        return XnAdaptive::ENone;
        }

    TInt adaptive( iAdaptive );

    if ( ( adaptive & XnAdaptive::ESizeFixed ) && !aIgnoreSizeFixed )
        {
        // When size is fixed, pretend non-adaptive
        return XnAdaptive::ENone;
        }

    // Remove measure control  flags
    adaptive &= ~XnAdaptive::EMeasure;
    adaptive &= ~XnAdaptive::ESizeFixed;

    return adaptive;
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::SetAdaptiveL
// Marks the node to have adaptive content
// ----------------------------------------------------------------------------
//
void CXnNodeImpl::SetAdaptiveL( const TInt aAdaptive )
    {
    TInt adaptive( aAdaptive );

    // Force adaptive nodes to be auto
    if ( adaptive & XnAdaptive::EMeasure )
        {
        if ( iAdaptive & XnAdaptive::EMeasure )
            {
            // Measure already pending
            return;
            }

        if ( iAdaptive & XnAdaptive::EWidth )
            {
            adaptive |= XnAdaptive::EWidth;
            }

        if ( iAdaptive & XnAdaptive::EHeight )
            {
            adaptive |= XnAdaptive::EHeight;
            }

        // Reset
        iAdaptive = XnAdaptive::ENone;
        }

    if ( ( adaptive & XnAdaptive::EWidth ) &&
           !( iAdaptive & XnAdaptive::EWidth ) )
        {
        iAdaptive |= XnAdaptive::EIgnoreDirty;

        SetStringPropertyToNodeL(
            *DomNode()->StringPool(), *Node(),
            XnPropertyNames::style::common::KWidth,
            XnPropertyNames::style::common::KAuto );

        // Width is adaptive
        iAdaptive |= XnAdaptive::EWidth;

        iAdaptive &= ~XnAdaptive::EIgnoreDirty;
        }

    if ( ( adaptive & XnAdaptive::EHeight ) &&
         !( iAdaptive & XnAdaptive::EHeight ) )
        {
        iAdaptive |= XnAdaptive::EIgnoreDirty;

        SetStringPropertyToNodeL(
            *DomNode()->StringPool(), *Node(),
            XnPropertyNames::style::common::KHeight,
            XnPropertyNames::style::common::KAuto );

        // Height is adaptive
        iAdaptive |= XnAdaptive::EHeight;

        iAdaptive &= ~XnAdaptive::EIgnoreDirty;
        }

    // Size is no more fixed
    iAdaptive &= ~XnAdaptive::ESizeFixed;

    // and it needs to be measured
    iAdaptive |= XnAdaptive::EMeasure;
    }

// ----------------------------------------------------------------------------
// CXnNodeImpl::FixAdaptiveSize
// Fixes the adaptive node's size
// ----------------------------------------------------------------------------
//
void CXnNodeImpl::FixAdaptiveSizeL( const TSize& aFixedSize )
    {
    // Is there measure pending
    if ( iAdaptive & XnAdaptive::EMeasure )
        {
        if ( iAdaptive & XnAdaptive::EWidth )
            {
            iAdaptive |= XnAdaptive::EIgnoreDirty;

            SetFloatPropertyToNodeL(
                *DomNode()->StringPool(), *Node(),
                XnPropertyNames::style::common::KWidth,
                aFixedSize.iWidth, CXnDomPropertyValue::EPx );

            iAdaptive &= ~XnAdaptive::EIgnoreDirty;
            }

        if ( iAdaptive & XnAdaptive::EHeight )
            {
            iAdaptive |= XnAdaptive::EIgnoreDirty;

            SetFloatPropertyToNodeL(
                *DomNode()->StringPool(), *Node(),
                XnPropertyNames::style::common::KHeight,
                aFixedSize.iHeight, CXnDomPropertyValue::EPx );

            iAdaptive &= ~XnAdaptive::EIgnoreDirty;
            }

        // Measure done
        iAdaptive &= ~XnAdaptive::EMeasure;
        // Size is now fixed
        iAdaptive |= XnAdaptive::ESizeFixed;
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetDirtyL
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::SetDirtyL( TInt aLevel, TBool aDisplayedState )
    {
    if( !aDisplayedState )
        {
        // check if node isn't displayed indeed (def. param. = EFalse)
        aDisplayedState = IsNodeDisplayedL( *iNode );
        }
    if ( iLayoutCapable )
        {
        if ( aLevel == XnDirtyLevel::ENone  )
            {
            // nothing to do
            return;
            }

        if ( aLevel == XnDirtyLevel::ELayoutAndRenderSiblings )
            {
            if ( iParent )
                {
                // Siblings needs to be laid out also
                RPointerArray< CXnNode >& siblings( iParent->Children() );

                for ( TInt i = 0; i < siblings.Count(); i++ )
                    {
                    CXnNode* sibling( siblings[i] );

                    if ( sibling == iNode )
                        {
                        sibling->SetDirtyL( XnDirtyLevel::ELayoutAndRender,
                            aDisplayedState );
                        }
                    else
                        {
                        sibling->SetDirtyL( XnDirtyLevel::ELayoutAndRender );
                        }
                    }
                }
            }
        else
            {
            if ( ( iParent || iNode == iUiEngine->RootNode() ) &&
                 iNode != iUiEngine->ActiveView() )
                {
                // Full redraw (without layout) can be done by setting rootnode
                // level to ERender
                if ( aLevel == XnDirtyLevel::ERender && !iLaidOut &&
                     iNode != iUiEngine->RootNode() )
                    {
                    aLevel = XnDirtyLevel::ELayoutAndRender;
                    }

                if ( aDisplayedState )
                    {
                    iUiEngine->AddDirtyNodeL( iNode, aLevel );
                    }
                else
                    {
                    ClearRenderedAndLaidOut();
                    }
                }
            }
        }
    else if ( iParent && iParent->Type()->Type() == KMenuBar )
        {
        // Mark menu to be refreshed
        iUiEngine->AddDirtyNodeL( iParent, XnDirtyLevel::ERender );
        }
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::RunFocusChangeL
// -----------------------------------------------------------------------------
//
TBool CXnNodeImpl::RunFocusChangeL( RPointerArray< CXnNode >& aFocusCandidates )
    {
    for ( TInt i = 0; i < aFocusCandidates.Count(); i++ )
        {
        CXnNode* node( aFocusCandidates[i] );

        if ( IsNodeNavigableL( *node ) )
            {                       
            if( node->ScrollableControl() )
                {
                node->ScrollableControl()->ShowItem( *node );
                }
            
            node->SetStateWithoutNotificationL(
                XnPropertyNames::style::common::KFocus );
            
            // All done
            return ETrue;
            }
        }

    // Focus change failed
    CXnNode* focused( iUiEngine->FocusedNode() );

    if ( focused )
        {
        // Is the currently focused node still navigable
        if ( !IsNodeNavigableL( *focused ) )
            {
            // No, remove focus
            focused->UnsetStateL( XnPropertyNames::style::common::KFocus );            
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ReorderNodesL
// -----------------------------------------------------------------------------
//
void CXnNodeImpl::ReorderNodesL( CXnNode* aSource, CXnNode* aTarget )
    {
    TInt target = -1;
    TInt source = -1;
    TInt childCount = iChildren.Count();
    RPointerArray< CXnNode > lockedNodes;
    CleanupClosePushL( lockedNodes );
    RArray< TInt > lockedNodeIndex;
    CleanupClosePushL( lockedNodeIndex );

    for ( TInt i = 0; i < childCount; i++ )
        {
        CXnNode* tmpNode = iChildren[i];
        CXnProperty* editable = tmpNode->GetPropertyL(
            XnPropertyNames::style::common::edit::KEditable );
        if ( editable && editable->StringValue() == XnPropertyNames::KFalse )
            {
            lockedNodes.AppendL( tmpNode );
            lockedNodeIndex.AppendL( i );
            continue;
            }

        if ( tmpNode == aSource )
            {
            source = i;
            }
        if ( tmpNode == aTarget )
            {
            target = i;
            }
        }
    if ( source != -1 && target != -1 )
        {
        TBool swap = EFalse;
        CXnPluginData* data( aTarget->UiEngine()->ViewManager()->
                ActiveViewData().Plugin( aTarget ) );
        if ( data && !data->Occupied() )
            {
            // target is empty widget, swap widget places
            swap = ETrue;
            }
        
        if ( target < source )
            {
            iChildren.InsertL( aSource, target );
            iChildren.Remove( source + 1 );
            if ( swap )
                {
                iChildren.Remove( target + 1 );
                iChildren.InsertL( aTarget, source );
                }
            }
        else if ( source < target )
            {
            iChildren.Remove( source );
            iChildren.InsertL( aSource, target );
            if ( swap )
                {
                iChildren.Insert( aTarget, source );
                iChildren.Remove( target );
                }
            }

        // put the locked nodes to the original places
        TInt lockedNodeCount = lockedNodes.Count();
        TInt lockedIndexCount = lockedNodeIndex.Count();
        if ( lockedNodeCount != lockedIndexCount )
            {
            return;
            }
        // First remove all the locked nodes
        for ( TInt j = 0; j < lockedNodeCount; j++ )
            {
            CXnNode* tmpLockedNode = lockedNodes[j];
            TInt ind = iChildren.FindL( tmpLockedNode );
            if ( KErrNotFound != ind )
                {
                iChildren.Remove( ind );
                }
            }

        //And then put them again to the original places
        for ( TInt j = 0; j < lockedNodeCount; j++ )
            {
            CXnNode* tmpLockedNode = lockedNodes[j];
            iChildren.InsertL( tmpLockedNode, lockedNodeIndex[j] );
            }
        }

    lockedNodes.Reset();
    lockedNodeIndex.Reset();
    CleanupStack::PopAndDestroy( 2 ); // lockedNodes, lockedNodeIndex
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::FindNextNodeL
// -----------------------------------------------------------------------------
//
CXnNode* CXnNodeImpl::FindNextNodeL(
    RPointerArray< CXnNode >& aArray,
    CXnNode& aNode,
    TInt aDirection )
    {
    CXnNode* node = NULL;
    if ( aDirection == EStdKeyRightArrow )
        {
        node = FindNextNodeFromRightL( aArray, aNode );
        }
    else if ( aDirection == EStdKeyLeftArrow )
        {
        node = FindNextNodeFromLeftL( aArray, aNode );
        }
    else if ( aDirection == EStdKeyUpArrow )
        {
        node = FindNextNodeFromAboveL( aArray, aNode );
        }
    else if ( aDirection == EStdKeyDownArrow )
        {
        node = FindNextNodeFromBelowL( aArray, aNode );
        }
    return node;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::Namespace
// -----------------------------------------------------------------------------
//
const TDesC8& CXnNodeImpl::Namespace()
    {
    return DomNode()->Namespace();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetScrollableControl()
// -----------------------------------------------------------------------------
//      
void CXnNodeImpl::SetScrollableControl( CXnScrollableControlAdapter* aScrollableControl )
    {
    iScrollableControl = aScrollableControl;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::ScrollableControl()
// -----------------------------------------------------------------------------
//      
CXnScrollableControlAdapter* CXnNodeImpl::ScrollableControl()
    {
    return iScrollableControl;
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::AdjustRectIfNeeded
// -----------------------------------------------------------------------------
//      
TRect CXnNodeImpl::AdjustRectIfNeeded( TRect aRect )
    {
    if(iScrollableControl) 
        {
        aRect.Move( iScrollableControl->DeltaPosition() );
        }
    return aRect;
    }
