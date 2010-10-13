/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*  Version     : %version: MM_66 % << Don't touch! Updated by Synergy at check-out.
*
*/


#include <aknlists.h> 
#include <barsread.h> 

#include <eikclb.h> 
#include <StringLoader.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>

#include "mmlistbox.h"
#include "mmlistboxmodel.h"
#include "mmmarqueeadapter.h"
#include "hnsuitemodel.h"
#include "mmlistboxcontainer.h"
#include "mmlistboxitemdrawer.h"
#include "mmwidgetsconstants.h"
#include "mmtemplatelibrary.h"
#include "mmpostevaluationprocessor.h"
#include "mmdraweranimator.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CListboxNumberContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmListBoxContainer* CMmListBoxContainer::NewLC( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary )
    {        
    CMmListBoxContainer* self = new( ELeave ) CMmListBoxContainer();
    CleanupStack::PushL( self );	
    self->ConstructL( aRect, aObjectProvider, aTemplateLibrary );
    return self;
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CListboxNumberContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmListBoxContainer* CMmListBoxContainer::NewL( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary )
    {        
    CMmListBoxContainer* self = NewLC( aRect, aObjectProvider, aTemplateLibrary );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CListboxNumberContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmListBoxContainer::CMmListBoxContainer()
    {       
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::~CListboxNumberContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CMmListBoxContainer::~CMmListBoxContainer()
    {  
    delete iListBox;
    delete iMarqueeAdapter;
    }

// -----------------------------------------------------------------------------
// CMmListBoxContainer::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//    
void CMmListBoxContainer::ConstructL( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary )
    {
    CMmWidgetContainer::ConstructL();
    
    SetMopParent( aObjectProvider );
    CreateWindowL(); // Creates window.
    iWidget = CreateListboxL( aTemplateLibrary );
    iWidget->SetListBoxObserver( this );
    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates window. ( Ready to draw )       
    SetupDrawer();
//    SetHighlightVisibilityL( ETrue );
    iMarqueeAdapter = CMmMarqueeAdapter::NewL();
    iDrawer->SetMarqueeAdapter( iMarqueeAdapter );
    iListBox->SetMarqueeAdapter( iMarqueeAdapter );
    iPostProcessor = CMmPostEvaluationProcessor::NewL( *iDrawer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmListBoxContainer::DrawView()
    {
    iListBox->DrawView();
    }

// -----------------------------------------------------------------------------
// CMmListBoxContainer::CreateListboxL()
// Constructs listbox from resource, creates scroll bar and sets empty
// list background text. 
// -----------------------------------------------------------------------------
//
CMmListBox* CMmListBoxContainer::CreateListboxL( 
    CMmTemplateLibrary* aTemplateLibrary  )
    {
    iListBox = CMmListBox::NewL( this, EAknListBoxSelectionList 
    		| EAknListBoxLoopScrolling, aTemplateLibrary );
    iListBox->SetContainerWindowL( *this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
    		CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );  
    iListBox->ScrollBarFrame()->DrawBackground( EFalse, EFalse );
    return iListBox;
    }
 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
THnSuiteWidgetType CMmListBoxContainer::WidgetType()
	{
	return EListWidget;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmListBoxContainer::SetDefaultHighlightL(  TBool aRedraw )
    {  
    TInt defaultHighlight = Widget()->TopItemIndex();
    if ( !ItemIsFullyVisible( defaultHighlight ) 
    		&& defaultHighlight < NumberOfItems() - 1 )
    	{
    	defaultHighlight++;
    	}
    	
    if (defaultHighlight >= 0 )
    	{
    	SetManualHighlightL( defaultHighlight, aRedraw );
    	}
    }
// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmListBoxContainer::SetEmptyTextL(const TDesC& aText)
    {
    iListBox->View()->SetListEmptyTextL( aText );  
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxContainer::SetEditModeL( TBool aIsEditMode )
    {
	if (aIsEditMode && !AknLayoutUtils::PenEnabled() )
		{
		iListBox->ClearListFlag( EAknListBoxLoopScrolling );
        }
    else
    	{
		iListBox->SetListFlag( EAknListBoxLoopScrolling ); 
    	}
    CMmWidgetContainer::SetEditModeL( aIsEditMode );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmListBoxModel* CMmListBoxContainer::GetMmModel()
    {
    return iListBox->MmModel();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmListBoxContainer::SetupWidgetLayoutL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxContainer::SetItemDrawerAndViewBgContext(
		CAknsBasicBackgroundControlContext * aBgContext )
	{
	iListBox->SetItemDrawerAndViewBgContext (aBgContext);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxContainer::SetSuiteModelL( CHnSuiteModel* aModel )
    {
    CMmWidgetContainer::SetSuiteModelL( aModel );
	iMarqueeAdapter->StopMarqueeDrawing();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxContainer::SetHighlightVisibilityL( TBool aVisible )
    {
        CMmWidgetContainer::SetHighlightVisibilityL( aVisible );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CMmListBoxContainer::PointInItemReorderAreaL(
            TInt aItemIndex, TPoint aPoint )
    {
    TRect itemRect = GetItemRectL( aItemIndex );
    itemRect.Shrink( 0, MmEffects::KShiftRatio * itemRect.Height() );
    return itemRect.Contains( aPoint );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmListBoxContainer::HandleItemAdditionL()
	{
	iListBox->HandleItemAdditionL();
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmListBoxContainer::HandleItemRemovalL()
	{
	GetMmModel()->HandleSuiteEventL( ESuiteItemsRemoved, GetMmModel()->GetSuiteModel() );
	iCurrentHighlight = GetSuiteModelL()->GetSuiteHighlight();
	ValidateWidgetCurrentItemIndex();
	
	iDrawer->RemoveFloatingItems();
	if( IsEditMode() )
		{
		iDrawer->GetAnimator()->SetNextRedrawToWholeScreen();
		}
	
	iListBox->HandleItemRemovalL();
    
    CHnSuiteModel* suiteModel = GetSuiteModelL();
	suiteModel->SetSuiteHighlightL( iCurrentHighlight );
	static_cast<CMmListBoxItemDrawer*>( iListBox->ItemDrawer() )->TrimCacheSize(
	        GetMmModel()->NumberOfItems() );
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmListBoxContainer::ColumnsInCurrentView()
	{
	return 1;
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmListBoxContainer::RowsInCurrentView()
    {
    return iListBox->View()->NumberOfItemsThatFitInRect(
            iListBox->View()->ViewRect());
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmListBoxContainer::SetVerticalItemOffset( TInt aOffset )
	{
	iListBox->SetVerticalItemOffset( aOffset );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmListBoxContainer::VerticalItemOffset() const
	{
	return iListBox->VerticalItemOffset();
	}

//----------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmListBoxContainer::UpdateViewScrollBarThumbs()
	{
	iListBox->UpdateScrollBarThumbs();
	}
// End of File
