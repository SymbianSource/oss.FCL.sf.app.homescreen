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
* Description:  UI Engine class
*
*/

// User includes
#include "xnuiengine.h"
#include "xnuienginepluginif.h"
#include "xnuiengineappif.h"
#include "xnuiengineimpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnUiEngine::CXnUiEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnUiEngine::CXnUiEngine()
    {
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ConstructL
// Symbian 2nd phase constructor can leave
// -----------------------------------------------------------------------------
//
void CXnUiEngine::ConstructL( CXnAppUiAdapter& aAdapter )
    {
    iUiEngineImpl = CXnUiEngineImpl::NewL( *this, aAdapter );

    iUiEngineImpl->ConstructL();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CXnUiEngine* CXnUiEngine::NewL( CXnAppUiAdapter& aAdapter )
    {
    CXnUiEngine* self = new ( ELeave ) CXnUiEngine;

    CleanupStack::PushL( self );
    self->ConstructL( aAdapter );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::~CXnUiEngine
// Destructor
// -----------------------------------------------------------------------------
//
CXnUiEngine::~CXnUiEngine()
    {
    delete iUiEngineImpl;
    delete iPluginIf;
    delete iAppIf;
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AppUiAdapter
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnAppUiAdapter& CXnUiEngine::AppUiAdapter() const
    {
    return iUiEngineImpl->AppUiAdapter();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::RootNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::RootNode()
    {
    return iUiEngineImpl->RootNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::FindNodeByIdL(
        const TDesC& aAreaId, const TDesC& aNamespace )
    {
    return iUiEngineImpl->FindNodeByIdL( aAreaId, aNamespace );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByIdL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::FindNodeByIdL(
        const TDesC8& aAreaId, const TDesC8& aNamespace )
    {
    return iUiEngineImpl->FindNodeByIdL( aAreaId, aNamespace );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnPointerArray* CXnUiEngine::FindNodeByClassL( const TDesC& aClassId,
    const TDesC& aNamespace )
    {
    return iUiEngineImpl->FindNodeByClassL( aClassId, aNamespace );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FindNodeByClassL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnPointerArray* CXnUiEngine::FindNodeByClassL( const TDesC8& aClassId,
    const TDesC8& aNamespace )
    {
    return iUiEngineImpl->FindNodeByClassL( aClassId, aNamespace );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::RenderUIL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
void CXnUiEngine::RenderUIL( CXnNode* aNode )
    {
    iUiEngineImpl->LayoutUIL( aNode );
    iUiEngineImpl->RenderUIL( aNode );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::LayoutUIL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
void CXnUiEngine::LayoutUIL( CXnNode* aNode )
    {
    iUiEngineImpl->LayoutUIL( aNode );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::Resources
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CArrayPtrSeg< CXnResource >& CXnUiEngine::Resources()
    {
    return iUiEngineImpl->Resources();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::SetFocusedNodeL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
void CXnUiEngine::SetFocusedNodeL( CXnNode* aFocusedNode, TInt aSource )
    {
    iUiEngineImpl->SetFocusedNodeL( aFocusedNode, aSource );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::FocusedNode
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::FocusedNode()
    {
    return iUiEngineImpl->FocusedNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::IsEditMode
// Returns ETrue if the current mode is edit, otherwise EFalse.
// -----------------------------------------------------------------------------
//
TBool CXnUiEngine::IsEditMode()
    {
    return iUiEngineImpl->IsEditMode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ODT
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
CXnODT* CXnUiEngine::ODT()
    {
    return iUiEngineImpl->ODT();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::VerticalPixelValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
TInt CXnUiEngine::VerticalPixelValueL(
        CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngineImpl->VerticalPixelValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::HorizontalPixelValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
TInt CXnUiEngine::HorizontalPixelValueL(
        CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngineImpl->HorizontalPixelValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::VerticalTwipValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
TInt CXnUiEngine::VerticalTwipValueL(
        CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngineImpl->VerticalTwipValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::HorizontalTwipValueL
// Forwards the call to the ui engine implementation
// -----------------------------------------------------------------------------
//
TInt CXnUiEngine::HorizontalTwipValueL(
        CXnProperty* aValue, TInt aReferenceValue )
    {
    return iUiEngineImpl->HorizontalTwipValueL( aValue, aReferenceValue );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ActivateViewL
// Activate a view
// -----------------------------------------------------------------------------
//
void CXnUiEngine::ActivateViewL( CXnNode& /*aViewNode*/ )
    {    
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ViewManager
// -----------------------------------------------------------------------------
//
CXnViewManager* CXnUiEngine::ViewManager()
    {
    return iUiEngineImpl->ViewManager();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ActiveView
// Returns the active view
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::ActiveView()
    {
    return iUiEngineImpl->ActiveView();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::RefreshMenuL
// Refresh current menu
// -----------------------------------------------------------------------------
//
void CXnUiEngine::RefreshMenuL()
    {
    iUiEngineImpl->RefreshMenuL();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AddDirtyNodeL
// Add a dirty node
// -----------------------------------------------------------------------------
//
void CXnUiEngine::AddDirtyNodeL( CXnNode* aNode, TInt aLevel )
    {
    iUiEngineImpl->AddDirtyNodeL( aNode, aLevel );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::PluginIfL
// Get plugin interface
// -----------------------------------------------------------------------------
//
TXnUiEnginePluginIf& CXnUiEngine::PluginIfL()
    {
    if ( iPluginIf )
        {
        return *iPluginIf;
        }
    else
        {
        iPluginIf = new ( ELeave ) TXnUiEnginePluginIf( *this );
        return *iPluginIf;
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AppIfL
// Get application interface
// -----------------------------------------------------------------------------
//
TXnUiEngineAppIf& CXnUiEngine::AppIfL()
    {
    if ( iAppIf )
        {
        return *iAppIf;
        }
    else
        {
        iAppIf = new ( ELeave ) TXnUiEngineAppIf( *this );
        return *iAppIf;
        }
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ScreenDeviceSize
// Get the size of the current screen device
// -----------------------------------------------------------------------------
//
TSize CXnUiEngine::ScreenDeviceSize() const
    {
    return iUiEngineImpl->ScreenDeviceSize();        
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::IsDialogDisplaying
// Checks whether the dialog is displaying or not
// -----------------------------------------------------------------------------
//
TBool CXnUiEngine::IsDialogDisplaying()
    {
    return iUiEngineImpl->IsDialogDisplaying();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::IsMenuDisplaying
// Checks whether the menu is displaying or not
// -----------------------------------------------------------------------------
//
TBool CXnUiEngine::IsMenuDisplaying()
    {
    return iUiEngineImpl->IsMenuDisplaying();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::MenuBarNode
// Returns menubar node
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::MenuBarNode() const
    {
    return iUiEngineImpl->MenuBarNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::StylusPopupNode
// Returns stylus popup node
// -----------------------------------------------------------------------------
//
CXnNode* CXnUiEngine::StylusPopupNode() const
    {
    return iUiEngineImpl->StylusPopupNode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngine::AddPassiveFocusedNodeL( CXnNode* aNode )
    {
    iUiEngineImpl->AddPassiveFocusedNodeL( aNode );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::RemovePassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngine::RemovePassiveFocusedNodeL( CXnNode* aNode )
    {
    iUiEngineImpl->RemovePassiveFocusedNodeL( aNode );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AddPassiveFocusedNode
// -----------------------------------------------------------------------------
//
void CXnUiEngine::ClearPassiveFocusedNodesL()
    {
    iUiEngineImpl->ClearPassiveFocusedNodesL();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::SetClientRectL
// -----------------------------------------------------------------------------
//
void CXnUiEngine::SetClientRectL( TRect aRect, TBool aDrawNow )
    {
    return iUiEngineImpl->SetClientRectL( aRect, aDrawNow );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::ClientRect
// -----------------------------------------------------------------------------
//
TRect CXnUiEngine::ClientRect() const
    {
    return iUiEngineImpl->ClientRect();
    } 

// -----------------------------------------------------------------------------
// CXnUiEngine::SetNodeDroppedL
// -----------------------------------------------------------------------------
//
void CXnUiEngine::SetNodeDroppedL( CXnNode& aNode, TInt aDropped ) const
    {
    iUiEngineImpl->SetNodeDroppedL( aNode, aDropped );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::CheckAdaptiveContentL
// -----------------------------------------------------------------------------
//
void CXnUiEngine::CheckAdaptiveContentL(
    CXnNode& aNode, TSize aAvailableSize ) const
    {
    iUiEngineImpl->CheckAdaptiveContentL( aNode, aAvailableSize );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AddFocusCandidateL
// -----------------------------------------------------------------------------
//
void CXnUiEngine::AddFocusCandidateL( CXnNode* aNode )
    {
    iUiEngineImpl->AddFocusCandidateL( aNode );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::Editor
// -----------------------------------------------------------------------------
//
CXnEditor* CXnUiEngine::Editor() const
    {
    return iUiEngineImpl->Editor();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::EditMode
// -----------------------------------------------------------------------------
//
CXnEditMode* CXnUiEngine::EditMode()
    {
    return iUiEngineImpl->EditMode();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::Plugins
// -----------------------------------------------------------------------------
//
RPointerArray< CXnNode >* CXnUiEngine::Plugins()
    {
    return iUiEngineImpl->Plugins();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::HitTest
// -----------------------------------------------------------------------------
//
CXnHitTest& CXnUiEngine::HitTest() const
    {
    return iUiEngineImpl->HitTest();
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::PositionStylusPopupL
// -----------------------------------------------------------------------------
//
void CXnUiEngine::PositionStylusPopupL( CXnNode& aNode,
    CXnNode& aReference, const TPoint& aPosition )
    {
    iUiEngineImpl->PositionStylusPopupL( aNode, aReference, aPosition );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::GetThemeResource
// -----------------------------------------------------------------------------
//
TInt CXnUiEngine::GetThemeResource( const TDesC& aPath, RFile& aFile )
    {
    return iUiEngineImpl->GetThemeResource( aPath, aFile );
    }

// -----------------------------------------------------------------------------
// CXnUiEngine::AnalyseAddedWidgetL
// -----------------------------------------------------------------------------
//
TBool CXnUiEngine::AnalyseAddedWidgetL( CXnNode& aNode )
    {
    return iUiEngineImpl->AnalyseAddedWidgetL( aNode );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//               
void CXnUiEngine::DisableRenderUiLC()
    {
    iUiEngineImpl->DisableRenderUiLC();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//               
void CXnUiEngine::SetEventDispatcher( CXnKeyEventDispatcher* aDispatcher )
    {
    iUiEngineImpl->SetEventDispatcher( aDispatcher );
    }

#ifndef EKA2
GLDEF_C TInt  E32Dll( TDllReason )
    {
    return KErrNone;
    }
#endif
