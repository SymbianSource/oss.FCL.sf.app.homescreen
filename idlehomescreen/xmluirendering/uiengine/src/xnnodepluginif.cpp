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


// User includes
#include "xnnodepluginif.h"
#include "xnnode.h"
#include "xnuiengine.h"
#include "xncomponentnodeimpl.h"
#include "xnproperty.h"
#include "xntype.h"
#include "xndomdocument.h"
#include "xnodt.h"

_LIT8( KFocus, "focus" );
_LIT8( KEdit, "edit" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNodePluginIf::CXnNodePluginIf()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNodePluginIf::CXnNodePluginIf( CXnNode& aNode )
    {
    iNode = &aNode;
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::ViewNodeImpl
// -----------------------------------------------------------------------------
//
EXPORT_C CXnViewNodeImpl* CXnNodePluginIf::ViewNodeImpl()
    {
    return iNode->ViewNodeImpl();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::ComponentNodeImpl
// -----------------------------------------------------------------------------
//
EXPORT_C CXnComponentNodeImpl* CXnNodePluginIf::ComponentNodeImpl()
    {
    return iNode->ComponentNodeImpl();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::IsDrawingAllowed
// -----------------------------------------------------------------------------
//
TBool CXnNodePluginIf::IsDrawingAllowed() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::Type()
// Returns control type.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnType* CXnNodePluginIf::Type()
    {
    return iNode->Type();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::Parent
// Get component parent
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodePluginIf* CXnNodePluginIf::ParentL() const
    {
    CXnNode* node = iNode->Parent();
    if ( !node )
        {
        return NULL;
        }
    return &( node->PluginIfL() );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::AddChildL
// Adds a child to this container.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::AddChildL( CXnNodePluginIf* aChild )
    {
    iNode->AddChildL( &( aChild->Node() ) );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::Children
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CXnNodePluginIf > CXnNodePluginIf::ChildrenL()
    {
    RPointerArray< CXnNode >& nodes = iNode->Children();
    RPointerArray< CXnNodePluginIf > pluginNodes;
    CleanupClosePushL( pluginNodes );

    TInt count = nodes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        pluginNodes.AppendL( &nodes[i]->PluginIfL() );
        }
    CleanupStack::Pop( &pluginNodes );
    // Compiler will generate bitwise copy ctor, thus
    // caller of this function must call reset or close for copied RPointerArrays,
    // which is very nasty thing to do...
    return pluginNodes;
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetPCDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetPCDataL( const TDesC8& aData )
    {
    iNode->SetPCDataL( aData );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::GetPCData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnNodePluginIf::GetPCData() const
    {
    return iNode->GetPCData();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetPropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetPropertyL( CXnProperty* aProperty )
    {
    iNode->SetPropertyL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetPropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetPropertyWithoutNotificationL( CXnProperty* aProperty )
    {
    iNode->SetPropertyWithoutNotificationL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::GetPropertyL( CXnProperty& /*aProperty*/ ) const
    {
    return NULL; // iNode->GetPropertyL( aProperty );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::GetPropertyL( const TDesC8& aKey ) const
    {
    return iNode->GetPropertyL( aKey );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetStateL
// Set a state
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetStateL( const TDesC8& aState )
    {
    iNode->SetStateL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::UnsetState
// Unset a pseudoclass
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::UnsetStateL( const TDesC8& aState )
    {
    iNode->UnsetStateL( aState );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::IsFocusedState
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnNodePluginIf::IsFocusedState()
    {
    return iNode->IsStateSet( KFocus );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::IsEditState
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnNodePluginIf::IsEditState()
    {
    return iNode->IsStateSet( KEdit );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetRect
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetRect( const TRect& aRect )
    {
    iNode->SetRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::Rect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodePluginIf::Rect()
    {
    return iNode->Rect();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetBorderRect
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetBorderRect( const TRect& aRect )
    {
    iNode->SetBorderRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::BorderRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodePluginIf::BorderRect()
    {
    return iNode->BorderRect();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetMarginRect
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetMarginRect( const TRect& aRect )
    {
    iNode->SetMarginRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::MarginRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodePluginIf::MarginRect()
    {
    return iNode->MarginRect();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetPaddingRect
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetPaddingRect( const TRect& aRect )
    {
    iNode->SetPaddingRect( aRect );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::PaddingRect
// -----------------------------------------------------------------------------
//
EXPORT_C TRect CXnNodePluginIf::PaddingRect()
    {
    return iNode->PaddingRect();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::UiEngine
// -----------------------------------------------------------------------------
//
EXPORT_C TXnUiEnginePluginIf* CXnNodePluginIf::UiEngineL()
    {
    return &( iNode->UiEngine()->PluginIfL() );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetLayoutCapable
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetLayoutCapable( const TBool aLayoutCapable )
    {
    iNode->SetLayoutCapable( aLayoutCapable );
    }

// -----------------------------------------------------------------------------
// Get internal node
// -----------------------------------------------------------------------------
//
CXnNode& CXnNodePluginIf::Node()
    {
    return *iNode;
    }

// -----------------------------------------------------------------------------
// Get app interface
// -----------------------------------------------------------------------------
//
EXPORT_C CXnNodeAppIf& CXnNodePluginIf::AppIfL()
    {
    return iNode->AppIfL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::MakeInterfaceL
// Create a component interface according to the given type.
// -----------------------------------------------------------------------------
//
EXPORT_C XnComponentInterface::MXnComponentInterface* CXnNodePluginIf::MakeInterfaceL(
    const TDesC8& aType )
    {
    return iNode->MakeInterfaceL( aType );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::DisplayL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::DisplayL()
    {
    return iNode->DisplayL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::VisibilityL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::VisibilityL()
    {
    return iNode->VisibilityL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::LabelL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::LabelL()
    {
    return iNode->LabelL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::IdL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::IdL()
    {
    return iNode->IdL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetHandleTooltip
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetHandleTooltip( TBool aFlag )
    {
    iNode->SetHandleTooltip( aFlag );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::ReportTriggerEventL(
    const TDesC8& aTriggerName,
    const TDesC8& aValueName,
    const TDesC8& aValue )
    {
    CXnNode* trigger = CXnNode::NewL();
    CleanupStack::PushL( trigger );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    trigger->SetImpl( impl );
    trigger->SetUiEngine( *iNode->UiEngine() );

    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        iNode->UiEngine()->ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );

    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue,
        *iNode->UiEngine()->ODT()->DomDocument().StringPool() );

    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    trigger->SetPropertyL( name );
    CleanupStack::Pop( name );

    CXnDomPropertyValue* indexValue = CXnDomPropertyValue::NewL(
        iNode->UiEngine()->ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( indexValue );
    indexValue->SetStringValueL( CXnDomPropertyValue::EString, aValue );

    CXnProperty* index = CXnProperty::NewL(
        aValueName,
        indexValue,
        *iNode->UiEngine()->ODT()->DomDocument().StringPool() );

    CleanupStack::Pop( indexValue );
    CleanupStack::PushL( index );
    trigger->SetPropertyL( index );
    CleanupStack::Pop( index );

    iNode->ReportXuikonEventL( *trigger, 0 );

    CleanupStack::PopAndDestroy( trigger );
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::PathL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::PathL()
    {
    return iNode->PathL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::MaskPathL
// -----------------------------------------------------------------------------
//
EXPORT_C CXnProperty* CXnNodePluginIf::MaskPathL()
    {
    return iNode->MaskPathL();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::SetDirtyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnNodePluginIf::SetDirtyL()
    {
    const TDesC8& type( iNode->Type()->Type() );

    if ( iNode->IsAdaptive( ETrue ) )
        {
        iNode->SetDirtyL( XnDirtyLevel::ELayoutAndRender );
        }
    else
        {
        iNode->SetDirtyL( XnDirtyLevel::ERender );
        }

    // If node's parent is tooltip, it must be relayouted because it will
    // grow based on childs
    CXnNode* parent( iNode->Parent() );

    if ( parent && parent->Type()->Type() == _L8( "tooltip" ) )
        {
        parent->SetDirtyL( XnDirtyLevel::ELayoutAndRender );
        }
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::Namespace
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnNodePluginIf::Namespace()
    {
    return iNode->Namespace();
    }

// -----------------------------------------------------------------------------
// CXnNodePluginIf::ControlL
// Gets the control associated to this node
// -----------------------------------------------------------------------------
//
EXPORT_C CXnControlAdapter* CXnNodePluginIf::Control() const
    {
    return iNode->Control();
    }
