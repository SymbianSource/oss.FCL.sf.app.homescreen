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
* Description:  Node of the layout tree
*
*/

// INCLUDES
#include "xnnode.h"
#include "xnnodepluginif.h"
#include "xnnodeappif.h"
#include "xnnodeimpl.h"
#include "xnviewsnodeimpl.h"
#include "xnviewnodeimpl.h"
#include "xncomponentnodeimpl.h"
#include "xnproperty.h"
#include "xntype.h"
#include "xncomponent.h"
#include "xnscrollablecontroladapter.h"

_LIT8( KView, "view" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNode::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNode* CXnNode::NewL()
    {
    CXnNode* self = new ( ELeave ) CXnNode;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnNode::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNode::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnNode::CXnNode()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNode::CXnNode()
    {
    }

// -----------------------------------------------------------------------------
// CXnNode::~CXnNode()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnNode::~CXnNode()
    {
    delete iImpl;
    delete iPluginIf;
    delete iAppIf;
    }

// -----------------------------------------------------------------------------
// CXnNode::Impl
// -----------------------------------------------------------------------------
//
CXnNodeImpl* CXnNode::Impl()
    {
    return iImpl;
    }

// -----------------------------------------------------------------------------
// CXnNode::RootNodeImpl
// -----------------------------------------------------------------------------
//
CXnNodeImpl* CXnNode::RootNodeImpl()
    {
    return iRootNodeImpl;
    }

// -----------------------------------------------------------------------------
// CXnNode::ViewNodeImpl
// -----------------------------------------------------------------------------
//
CXnViewNodeImpl* CXnNode::ViewNodeImpl()
    {
    return iViewNodeImpl;
    }

// -----------------------------------------------------------------------------
// CXnNode::ComponentNodeImpl
// -----------------------------------------------------------------------------
//
CXnComponentNodeImpl* CXnNode::ComponentNodeImpl()
    {
    return iComponentNodeImpl;
    }

// -----------------------------------------------------------------------------
// CXnNode::SetImpl
// -----------------------------------------------------------------------------
//
void CXnNode::SetImpl( CXnNodeImpl* aImpl )
    {
    if ( aImpl )
        {
        delete iImpl;
        iImpl = aImpl;
        iImpl->SetNode( *this );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetRootNodeImpl
// -----------------------------------------------------------------------------
//
void CXnNode::SetRootNodeImpl( CXnNodeImpl* aRootNodeImpl )
    {
    if ( aRootNodeImpl )
        {
        delete iImpl;
        iRootNodeImpl = aRootNodeImpl;
        iImpl = iRootNodeImpl;
        iImpl->SetNode( *this );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetViewsNodeImpl
// -----------------------------------------------------------------------------
//
void CXnNode::SetViewsNodeImpl( CXnViewsNodeImpl* aViewsNodeImpl )
    {
    if ( aViewsNodeImpl )
        {
        delete iImpl;
        iViewsNodeImpl = aViewsNodeImpl;
        iImpl = iViewsNodeImpl;
        iImpl->SetNode( *this );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetViewNodeImpl
// -----------------------------------------------------------------------------
//
void CXnNode::SetViewNodeImpl( CXnViewNodeImpl* aViewNodeImpl )
    {
    if ( aViewNodeImpl )
        {
        delete iImpl;
        iViewNodeImpl = aViewNodeImpl;
        iImpl = iViewNodeImpl;
        iImpl->SetNode( *this );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetComponentNodeImpl
// -----------------------------------------------------------------------------
//
void CXnNode::SetComponentNodeImpl( CXnComponentNodeImpl* aComponentNodeImpl )
    {
    if ( aComponentNodeImpl )
        {
        delete iImpl;
        iComponentNodeImpl = aComponentNodeImpl;
        iImpl = iComponentNodeImpl;
        iImpl->SetNode( *this );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetDropped
// -----------------------------------------------------------------------------
//
void CXnNode::SetDropped( const TInt aDropped )
    {
    iImpl->SetDropped( aDropped );
    }

// -----------------------------------------------------------------------------
// CXnNode::IsDropped
// -----------------------------------------------------------------------------
//
TInt CXnNode::IsDropped() const
    {
    return iImpl->IsDropped();
    }

// -----------------------------------------------------------------------------
// CXnNode::Type()
// Returns control type.
// -----------------------------------------------------------------------------
//
CXnType* CXnNode::Type()
    {
    return iImpl->Type();
    }

// -----------------------------------------------------------------------------
// CXnNode::Parent
// Get component parent
// -----------------------------------------------------------------------------
//
CXnNode* CXnNode::Parent() const
    {
    return iImpl->Parent();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetParent
// Set component parent
// -----------------------------------------------------------------------------
//
void CXnNode::SetParent( CXnNode& aParent )
    {
    iImpl->SetParent( aParent );
    }

// -----------------------------------------------------------------------------
// CXnNode::AddChildL
// Adds a child to this container.
// -----------------------------------------------------------------------------
//
void CXnNode::AddChildL( CXnNode* aChild )
    {
    if ( aChild )
        {
        iImpl->AddChildL( aChild );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::Children
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >& CXnNode::Children()
    {
    return iImpl->Children();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetPCDataL
// -----------------------------------------------------------------------------
void CXnNode::SetPCDataL( const TDesC8& aData )
    {
    iImpl->SetPCDataL( aData );
    }

// -----------------------------------------------------------------------------
// CXnNode::GetPCData
// -----------------------------------------------------------------------------
const TDesC8& CXnNode::GetPCData() const
    {
    return iImpl->GetPCData();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetPropertyWithoutNotificationL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnNode::SetPropertyWithoutNotificationL( CXnProperty* aProperty )
    {
    if ( aProperty )
        {
        iImpl->SetPropertyWithoutNotificationL( aProperty );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetPropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnNode::SetPropertyL( CXnProperty* aProperty )
    {
    if ( aProperty )
        {
        iImpl->SetPropertyL( aProperty );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::SetPropertyArrayL
// Set a property array.
// -----------------------------------------------------------------------------
//
void CXnNode::SetPropertyArrayL( RPointerArray< CXnProperty >* aPropertyArray )
    {
    if ( aPropertyArray )
        {
        iImpl->SetPropertyArrayL( *aPropertyArray );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::InitializePropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnNode::InitializePropertyL( CXnProperty* aProperty )
    {
    if ( aProperty )
        {
        iImpl->InitializePropertyL( aProperty );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnNode::GetPropertyL( const TDesC8& aKey ) const
    {
    return iImpl->GetPropertyL( aKey );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetStateL
// Set a state
// -----------------------------------------------------------------------------
//
void CXnNode::SetStateL( const TDesC8& aState, TInt aSource )
    {
    iImpl->SetStateL( aState, aSource );
    }

// -----------------------------------------------------------------------------
// CXnNode::IsStateSet
// Check whether a state is set or not
// -----------------------------------------------------------------------------
//
TBool CXnNode::IsStateSet( const TDesC8& aState )
    {
    return iImpl->IsStateSet( aState );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetStateWithoutNotificationL
// Set a state
// -----------------------------------------------------------------------------
//
void CXnNode::SetStateWithoutNotificationL( const TDesC8& aState )
    {
    iImpl->SetStateWithoutNotificationL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNode::UnsetState
// Unset a pseudoclass
// -----------------------------------------------------------------------------
//
void CXnNode::UnsetStateL( const TDesC8& aState )
    {
    iImpl->UnsetStateL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetRect
// -----------------------------------------------------------------------------
//
void CXnNode::SetRect( const TRect& aRect )
    {
    iImpl->SetRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNode::Rect
// -----------------------------------------------------------------------------
//
TRect CXnNode::Rect()
    {
    return iImpl->Rect();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetBorderRect
// -----------------------------------------------------------------------------
//
void CXnNode::SetBorderRect( const TRect& aRect )
    {
    iImpl->SetBorderRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNode::BorderRect
// -----------------------------------------------------------------------------
//
TRect CXnNode::BorderRect()
    {
    return iImpl->BorderRect();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetNormalFlowBorderRect
// -----------------------------------------------------------------------------
//
void CXnNode::SetNormalFlowBorderRect( const TRect& aRect )
    {
    iImpl->SetNormalFlowBorderRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNode::NormalFlowBorderRect
// -----------------------------------------------------------------------------
//
TRect CXnNode::NormalFlowBorderRect()
    {
    return iImpl->NormalFlowBorderRect();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetMarginRect
// -----------------------------------------------------------------------------
//
void CXnNode::SetMarginRect( const TRect& aRect )
    {
    iImpl->SetMarginRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNode::MarginRect
// -----------------------------------------------------------------------------
//
TRect CXnNode::MarginRect()
    {
    return iImpl->MarginRect();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetPaddingRect
// -----------------------------------------------------------------------------
//
void CXnNode::SetPaddingRect( const TRect& aRect )
    {
    iImpl->SetPaddingRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNode::PaddingRect
// -----------------------------------------------------------------------------
//
TRect CXnNode::PaddingRect()
    {
    return iImpl->PaddingRect();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetUiEngine
// -----------------------------------------------------------------------------
//
void CXnNode::SetUiEngine( CXnUiEngine& aEngine )
    {
    iImpl->SetUiEngine( aEngine );
    }

// -----------------------------------------------------------------------------
// CXnNode::UiEngine
// -----------------------------------------------------------------------------
//
CXnUiEngine* CXnNode::UiEngine()
    {
    return iImpl->UiEngine();
    }

// -----------------------------------------------------------------------------
// CXnNode::ReportXuikonEventL
// -----------------------------------------------------------------------------
//
TBool CXnNode::ReportXuikonEventL( CXnNode& aEventData, TInt aSource )
    {
    return iImpl->ReportXuikonEventL( aEventData, aSource );
    }

// -----------------------------------------------------------------------------
// CXnNode::OfferKeyEventL
// -----------------------------------------------------------------------------
//
void CXnNode::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    iImpl->OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetLayoutCapable
// -----------------------------------------------------------------------------
//
void CXnNode::SetLayoutCapable( const TBool aLayoutCapable )
    {
    iImpl->SetLayoutCapable( aLayoutCapable );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetLayoutCapable
// -----------------------------------------------------------------------------
//
TBool CXnNode::IsLayoutCapable() const
    {
    return iImpl->IsLayoutCapable();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetRenderedL
// -----------------------------------------------------------------------------
//
void CXnNode::SetRenderedL()
    {
    iImpl->SetRenderedL();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::SetLaidOutL
// -----------------------------------------------------------------------------
//
void CXnNode::SetLaidOutL()
    {
    iImpl->SetLaidOutL();
    }

// -----------------------------------------------------------------------------
// CXnNodeImpl::IsLaidOut
// -----------------------------------------------------------------------------
//
TBool CXnNode::IsLaidOut() const
    {
    return iImpl->IsLaidOut();
    }

// -----------------------------------------------------------------------------
// CXnNode::ClearRenderedAndLaidOut
// -----------------------------------------------------------------------------
//
void CXnNode::ClearRenderedAndLaidOut()
    {
    iImpl->ClearRenderedAndLaidOut();
    }

// -----------------------------------------------------------------------------
// CXnNode::PluginIfL
// Get node plugin interface
// -----------------------------------------------------------------------------
//
CXnNodePluginIf& CXnNode::PluginIfL()
    {
    if ( iPluginIf )
        {
        return *iPluginIf;
        }
    else
        {
        iPluginIf = new ( ELeave ) CXnNodePluginIf( *this );
        return *iPluginIf;
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::AppIfL
// Get node plugin interface
// -----------------------------------------------------------------------------
//
CXnNodeAppIf& CXnNode::AppIfL()
    {
    if ( iAppIf )
        {
        return *iAppIf;
        }
    else
        {
        iAppIf = new ( ELeave ) CXnNodeAppIf( *this );
        return *iAppIf;
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
XnComponentInterface::MXnComponentInterface* CXnNode::MakeInterfaceL(
    const TDesC8& aType )
    {
    return iImpl->MakeInterfaceL( aType );
    }

CXnProperty* CXnNode::WidthL()
    {
    return iImpl->WidthL();
    }

CXnProperty* CXnNode::HeightL()
    {
    return iImpl->HeightL();
    }

CXnProperty* CXnNode::MarginLeftL()
    {
    return iImpl->MarginLeftL();
    }

CXnProperty* CXnNode::MarginRightL()
    {
    return iImpl->MarginRightL();
    }

CXnProperty* CXnNode::BorderLeftL()
    {
    return iImpl->BorderLeftL();
    }

CXnProperty* CXnNode::BorderRightL()
    {
    return iImpl->BorderRightL();
    }

CXnProperty* CXnNode::PaddingLeftL()
    {
    return iImpl->PaddingLeftL();
    }

CXnProperty* CXnNode::PaddingRightL()
    {
    return iImpl->PaddingRightL();
    }

CXnProperty* CXnNode::MarginTopL()
    {
    return iImpl->MarginTopL();
    }

CXnProperty* CXnNode::MarginBottomL()
    {
    return iImpl->MarginBottomL();
    }

CXnProperty* CXnNode::BorderTopL()
    {
    return iImpl->BorderTopL();
    }

CXnProperty* CXnNode::BorderBottomL()
    {
    return iImpl->BorderBottomL();
    }

CXnProperty* CXnNode::PaddingTopL()
    {
    return iImpl->PaddingTopL();
    }

CXnProperty* CXnNode::PaddingBottomL()
    {
    return iImpl->PaddingBottomL();
    }

CXnProperty* CXnNode::BorderWidthL()
    {
    return iImpl->BorderWidthL();
    }

CXnProperty* CXnNode::BlockProgressionL()
    {
    return iImpl->BlockProgressionL();
    }

CXnProperty* CXnNode::DirectionL()
    {
    return iImpl->DirectionL();
    }

CXnProperty* CXnNode::PositionL()
    {
    return iImpl->PositionL();
    }

CXnProperty* CXnNode::MaxHeightL()
    {
    return iImpl->MaxHeightL();
    }

CXnProperty* CXnNode::MinHeightL()
    {
    return iImpl->MinHeightL();
    }

CXnProperty* CXnNode::MaxWidthL()
    {
    return iImpl->MaxWidthL();
    }

CXnProperty* CXnNode::MinWidthL()
    {
    return iImpl->MinWidthL();
    }

CXnProperty* CXnNode::DisplayL()
    {
    return iImpl->DisplayL();
    }

CXnProperty* CXnNode::VisibilityL()
    {
    return iImpl->VisibilityL();
    }

CXnProperty* CXnNode::LeftL()
    {
    return iImpl->LeftL();
    }

CXnProperty* CXnNode::RightL()
    {
    return iImpl->RightL();
    }

CXnProperty* CXnNode::TopL()
    {
    return iImpl->TopL();
    }

CXnProperty* CXnNode::BottomL()
    {
    return iImpl->BottomL();
    }

CXnProperty* CXnNode::BorderLeftStyleL()
    {
    return iImpl->BorderLeftStyleL();
    }

CXnProperty* CXnNode::BorderRightStyleL()
    {
    return iImpl->BorderRightStyleL();
    }

CXnProperty* CXnNode::BorderTopStyleL()
    {
    return iImpl->BorderTopStyleL();
    }

CXnProperty* CXnNode::BorderBottomStyleL()
    {
    return iImpl->BorderBottomStyleL();
    }

CXnProperty* CXnNode::BorderStyleL()
    {
    return iImpl->BorderStyleL();
    }

CXnProperty* CXnNode::BorderImageL()
    {
    return iImpl->BorderImageL();
    }

CXnProperty* CXnNode::DisplayPriorityL()
    {
    return iImpl->DisplayPriorityL();
    }

CXnProperty* CXnNode::NameL()
    {
    return iImpl->NameL();
    }

CXnProperty* CXnNode::ValueL()
    {
    return iImpl->ValueL();
    }

CXnProperty* CXnNode::LabelL()
    {
    return iImpl->LabelL();
    }

CXnProperty* CXnNode::InitialFocusL()
    {
    return iImpl->InitialFocusL();
    }

CXnProperty* CXnNode::ClassL()
    {
    return iImpl->ClassL();
    }

CXnProperty* CXnNode::IdL()
    {
    return iImpl->IdL();
    }
CXnProperty* CXnNode::PathL()
    {
    return iImpl->PathL();
    }
CXnProperty* CXnNode::MaskPathL()
    {
    return iImpl->MaskPathL();
    }

CXnProperty* CXnNode::NavIndexL()
    {
    return iImpl->NavIndexL();
    }

CXnProperty* CXnNode::ZIndexL()
    {
    return iImpl->ZIndexL();
    }

CXnProperty* CXnNode::BackgroundColorL()
    {
    return iImpl->BackgroundColorL();
    }

CXnProperty* CXnNode::BackgroundImageL()
    {
    return iImpl->BackgroundImageL();
    }

CXnProperty* CXnNode::FocusBackgroundL()
    {
    return iImpl->FocusBackgroundL();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetDomNode
// Sets the DOM node pointer
// -----------------------------------------------------------------------------
//
void CXnNode::SetDomNode( CXnDomNode* aDomNode )
    {
    if ( aDomNode )
        {
        iImpl->SetDomNode( aDomNode );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::DomNode
// Returns the DOM node pointer
// -----------------------------------------------------------------------------
//
CXnDomNode* CXnNode::DomNode()
    {
    return iImpl->DomNode();
    }

// -----------------------------------------------------------------------------
// CXnNode::SetHandleTooltip
// Sets handle tooltip flag.
// -----------------------------------------------------------------------------
//
void CXnNode::SetHandleTooltip( TBool aFlag )
    {
    iImpl->SetHandleTooltip( aFlag );
    }

// -----------------------------------------------------------------------------
// CXnNode::MeasureAdaptiveContentL
// Measures the adaptive content dimensions.
// -----------------------------------------------------------------------------
//
TSize CXnNode::MeasureAdaptiveContentL( const TSize& aAvailableSize )
    {
    return iImpl->MeasureAdaptiveContentL( aAvailableSize );
    }

// -----------------------------------------------------------------------------
// CXnNode::IsAdaptive
// Checks if the node has adaptive content
// -----------------------------------------------------------------------------
//
TInt CXnNode::IsAdaptive( TBool aIgnoreSizeFixed ) const
    {
    return iImpl->IsAdaptive( aIgnoreSizeFixed );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetAdaptiveL
// Marks the node as content based.
// -----------------------------------------------------------------------------
//
void CXnNode::SetAdaptiveL( const TInt aAdaptive )
    {
    iImpl->SetAdaptiveL( aAdaptive );
    }

// -----------------------------------------------------------------------------
// CXnNode::FixAdaptiveSizeL
// Marks the node as content based.
// -----------------------------------------------------------------------------
//
void CXnNode::FixAdaptiveSizeL( const TSize& aFixedSize )
    {
    iImpl->FixAdaptiveSizeL( aFixedSize );
    }

// -----------------------------------------------------------------------------
// CXnNode::SetDirtyL
// Marks the node dirty
// -----------------------------------------------------------------------------
//
void CXnNode::SetDirtyL( TInt aLevel, TBool aDisplayedState )
    {
    iImpl->SetDirtyL( aLevel, aDisplayedState );
    }

// -----------------------------------------------------------------------------
// CXnNode::RunFocusChangeL
// Runs focus change
// -----------------------------------------------------------------------------
//
TBool CXnNode::RunFocusChangeL( RPointerArray< CXnNode >& aFocusCandidates )
    {
    return iImpl->RunFocusChangeL( aFocusCandidates );
    }

// -----------------------------------------------------------------------------
// CXnNode::ReorderNodesL
// -----------------------------------------------------------------------------
//
void CXnNode::ReorderNodesL( CXnNode* aSource, CXnNode* aTarget )
    {
    if ( aSource && aTarget )
        {
        iImpl->ReorderNodesL( aSource, aTarget );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::FindViewNode
// -----------------------------------------------------------------------------
//
CXnNode* CXnNode::FindViewNode()
    {
    CXnNode* tmp = NULL;
    tmp = Parent();
    while( tmp )
        {
        if ( tmp->Type()->Type() == KView )
            {
            break;
            }
        else
            {
            tmp = tmp->Parent();
            }
        }
    return tmp;
    }

// -----------------------------------------------------------------------------
// CXnNode::ShowTooltipsL
// -----------------------------------------------------------------------------
//
void CXnNode::ShowTooltipsL()
    {
    if ( IsStateSet( XnPropertyNames::action::trigger::name::KFocus ) )
        {
        iImpl->HidePopupsL();
        iImpl->ShowPopupsL( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CXnNode::HideTooltipsL
// -----------------------------------------------------------------------------
//
void CXnNode::HideTooltipsL()
    {
    iImpl->HidePopupsL();
    }

// -----------------------------------------------------------------------------
// CXnNode::Namespace
// -----------------------------------------------------------------------------
//
const TDesC8& CXnNode::Namespace()
    {
    return iImpl->Namespace();
    }

// -----------------------------------------------------------------------------
// CXnNode::ControlL
// -----------------------------------------------------------------------------
//
CXnControlAdapter* CXnNode::Control() const
    {
    CXnComponent* component( NULL );

    if ( iViewNodeImpl )
        {
        component = iViewNodeImpl->Component();
        }
    else if ( iComponentNodeImpl )
        {
        component = iComponentNodeImpl->Component();
        }

    if ( component )
        {
        return component->ControlAdapter();
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnNode::SetScrollableControl()
// -----------------------------------------------------------------------------
//      
void CXnNode::SetScrollableControl( CXnScrollableControlAdapter* aScrollableControl )
    {
    iImpl->SetScrollableControl( aScrollableControl );
    }

// -----------------------------------------------------------------------------
// CXnNode::ScrollableControl()
// -----------------------------------------------------------------------------
//      
CXnScrollableControlAdapter* CXnNode::ScrollableControl()
    {
    return iImpl->ScrollableControl();
    }

