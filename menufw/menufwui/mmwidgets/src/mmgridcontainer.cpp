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
*  Version     : %version: MM_72 % << Don't touch! Updated by Synergy at check-out.
*  Version     : %version: MM_72 % << Don't touch! Updated by Synergy at check-out.
*
*/


#include "mmgridcontainer.h"
#include "mmgridmodel.h"
#include "mmlistboxmodel.h"
#include "mmmarqueeadapter.h"
#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <layoutmetadata.cdl.h>
#include "mmhighlighttimer.h"
#include "hnsuitemodel.h"
#include "mmgrid.h"
#include "mmwidgetsconstants.h"
#include "hnglobals.h"
#include "mmpostevaluationprocessor.h"
#include "mmlistboxitemdrawer.h"
#include "mmdraweranimator.h"

// -----------------------------------------------------------------------------
// CMmGridContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmGridContainer* CMmGridContainer::NewLC( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aLibrary )
    {        
    CMmGridContainer* self = new( ELeave ) CMmGridContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aObjectProvider, aLibrary );
    return self;
    }

// -----------------------------------------------------------------------------
// CMmGridContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmGridContainer* CMmGridContainer::NewL( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aLibrary )
    {        
    CMmGridContainer* self = NewLC( aRect, aObjectProvider, aLibrary);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMmGridContainer::CMmGridContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmGridContainer::CMmGridContainer()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CAknGrid* CMmGridContainer::Grid()
    { 
    return iGrid;
    }
// -----------------------------------------------------------------------------
// CMmGridContainer::~CMmGridContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CMmGridContainer::~CMmGridContainer()
    {  
    delete iGrid;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
THnSuiteWidgetType CMmGridContainer::WidgetType()
	{
	return EGridWidget;
	}
// -----------------------------------------------------------------------------
// CMmGridContainer::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//    
void CMmGridContainer::ConstructL( const TRect& aRect, MObjectProvider* aObjectProvider,
        CMmTemplateLibrary* aTemplateLibrary )
    {
    CMmWidgetContainer::ConstructL();
    
    SetMopParent( aObjectProvider );
    CreateWindowL(); // Creates window.     
    iWidget = CreateGridL(aTemplateLibrary);
    iWidget->SetListBoxObserver( this );
    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates window. ( Ready to draw )     
    SetupDrawer();
    SetHighlightVisibilityL( !AknLayoutUtils::PenEnabled() );	
    iPostProcessor = CMmPostEvaluationProcessor::NewL( *iDrawer );
    }

// -----------------------------------------------------------------------------
// CMmGridContainer::CreateGridL()
// Constructs listbox from resource, creates scroll bar and sets empty
// list background text. 
// -----------------------------------------------------------------------------
//
CMmGrid* CMmGridContainer::CreateGridL( CMmTemplateLibrary* aTemplateLibrary )
    {
    iGrid = CMmGrid::NewL( this,
    		EAknListBoxSelectionList | EAknListBoxLoopScrolling | EAknListBoxDisableHighlight , aTemplateLibrary );
    iGrid->SetContainerWindowL( *this );
    iGrid->CreateScrollBarFrameL( ETrue );
    iGrid->SetPrimaryScrollingType(
    			CAknGridView::EScrollFollowsItemsAndLoops );
    iGrid->SetSecondaryScrollingType(
    			CAknGridView::EScrollFollowsGrid );
    iGrid->ScrollBarFrame()->SetScrollBarVisibilityL( 
    		CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
    iGrid->ScrollBarFrame()->DrawBackground( EFalse, EFalse );
    return iGrid;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CMmGridContainer::SizeChanged()
    {
    CMmWidgetContainer::SizeChanged();
    if ( iGrid )
    	{
//    	iGrid->SetRect( Rect() );
        iGrid->SetupLayout();
        }
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetEditModeL( TBool aIsEditMode )
    {
	if (aIsEditMode && !AknLayoutUtils::PenEnabled() )
		{
		iGrid->SetPrimaryScrollingType(
				CAknGridView::EScrollStops );
		iGrid->SetSecondaryScrollingType(
				CAknGridView::EScrollStops );
        }
    else
    	{
    	iGrid->SetPrimaryScrollingType(
    	    			CAknGridView::EScrollFollowsItemsAndLoops );
    	iGrid->SetSecondaryScrollingType(
    	    			CAknGridView::EScrollFollowsGrid );
    	    
    	}
	
	// In EditMode we allow Avkon to redraw scrollbar background
	// to prevent scrollbar flicking.
	// When edit mode is disabled, this redrawing causes performance problems 
	// (and disabling it does not produce erroneous side-effects).
	if( aIsEditMode )
		{
	    iGrid->ScrollBarFrame()->DrawBackground( EFalse, ETrue );
		}
	else
		{
	    iGrid->ScrollBarFrame()->DrawBackground( EFalse, EFalse );
		}
	
    CMmWidgetContainer::SetEditModeL( aIsEditMode );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmGridContainer::SetEmptyTextL(const TDesC& aText)
    {
    iGrid->SetEmptyGridTextL( aText );  
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmGridContainer::SetDefaultHighlightL(  TBool aRedraw )
    {  
    CAknGridView* view = (CAknGridView*) iGrid->View();
    
    if ( view )
        {
        TInt defaultHighlight( 0 );
        TSize layout = TSize( view->NumberOfColsInView(),
        		view->NumberOfRowsInView() );
        
        if ( Layout_Meta_Data::IsLandscapeOrientation() || FlipOpen() )
            {
            defaultHighlight = LandscapeOrientationDefaultHighlight( layout );
            }
        else
            {
            defaultHighlight = PortraitOrientationDefaultHighlight( layout );
            }
        
        SetManualHighlightL( defaultHighlight + DefaultHighlightOffset(), aRedraw );
        }
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CMmGridContainer::LandscapeOrientationDefaultHighlight( TSize aLayout )
	{
	TInt defaultHighlight( 0 );
	
    if ( aLayout == TSize(MmGrid::K4By3LayoutX, MmGrid::K4By3LayoutY) 
    		&& iGrid->Model()->NumberOfItems() >= MmGrid::K4By3Threshold )
        {
        defaultHighlight = MmGrid::K4By3DefaultHighlight;
        }
    else if ( aLayout == TSize(MmGrid::K5By4LayoutX, MmGrid::K5By4LayoutY) 
    		&& iGrid->Model()->NumberOfItems() >= MmGrid::K5By4Threshold )
        {
        defaultHighlight = MmGrid::K5By4DefaultHighlight;
        }
    // When flip is opened, the phone layout is not changed immediately,
    // so the phone might still be in portrait mode for a couple of secs.
    // However, we have to set the correct default item immediately, and
    // the correct item is the one for landscape mode, even if the phone
    // is still in portrait mode. On variant switch 3x4 layout will be
    // replaced by 4x3 layout, while 4x5 layout will change into 5x4.
    else if ( aLayout == TSize( MmGrid::K3By4LayoutX, MmGrid::K3By4LayoutY )
            && iGrid->Model()->NumberOfItems() >= MmGrid::K4By3Threshold )
        {
        defaultHighlight = MmGrid::K4By3DefaultHighlight;
        }
    else if ( aLayout == TSize( MmGrid::K4By5LayoutX, MmGrid::K4By5LayoutY )
            && iGrid->Model()->NumberOfItems() >= MmGrid::K5By4Threshold )
        {
        defaultHighlight = MmGrid::K5By4DefaultHighlight;
        }
    
    return defaultHighlight;
	}

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CMmGridContainer::PortraitOrientationDefaultHighlight( TSize aLayout )
	{
	TInt defaultHighlight( 0 );
	
	if ( aLayout == TSize(MmGrid::K4By5LayoutX, MmGrid::K4By5LayoutY ) 
			&& iGrid->Model()->NumberOfItems() >= MmGrid::K4By5Threshold )
		{
		defaultHighlight = MmGrid::K4By5DefaultHighlight;
		}
	else if ( aLayout == TSize(MmGrid::K3By4LayoutX, MmGrid::K3By4LayoutY) 
			&& iGrid->Model()->NumberOfItems() >= MmGrid::K3By4Threshold )
		{
		defaultHighlight = MmGrid::K3By4DefaultHighlight;
		}
	
    return defaultHighlight;
	}

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
TInt CMmGridContainer::DefaultHighlightOffset()
	{
	TInt topVisibleItemIndex = iGrid->View()->TopItemIndex();
	TInt hiddenPixels = -iGrid->View()->ItemOffsetInPixels();
	
	if ( iGrid->View()->ItemIsPartiallyVisible( topVisibleItemIndex ) 
		&& hiddenPixels > iGrid->View()->ItemSize().iHeight / 2 )
		{
		topVisibleItemIndex += ColumnsInCurrentView();
		}
		
	return topVisibleItemIndex;
	}

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
CMmListBoxModel* CMmGridContainer::GetMmModel()
    {
    return iGrid->MmModel()->MmListBoxModel();
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetItemDrawerAndViewBgContext(
		CAknsBasicBackgroundControlContext * aBgContext )
	{
	iGrid->SetItemDrawerAndViewBgContext (aBgContext);
	}

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetupWidgetLayoutL()
	{
	User::LeaveIfNull( iGrid );
	iGrid->SetupLayout();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetSuiteModelL( CHnSuiteModel* aModel )
    {
    CMmWidgetContainer::SetSuiteModelL( aModel );
    if ( aModel )
        {
        aModel->SetActiveL( !AknLayoutUtils::PenEnabled() );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetHighlightVisibilityL( TBool aVisible )
    {    
    CMmWidgetContainer::SetHighlightVisibilityL( aVisible );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmGridContainer::HandleItemAdditionL()
	{
	iGrid->HandleItemAdditionL();
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmGridContainer::HandleItemRemovalL()
	{
	
	GetMmModel()->HandleSuiteEventL( ESuiteItemsRemoved, GetMmModel()->GetSuiteModel() );
	ValidateWidgetCurrentItemIndex();
	iDrawer->RemoveFloatingItems();
	if( IsEditMode() )
		{
		iDrawer->GetAnimator()->SetNextRedrawToWholeScreen();
		}
	
	CacheWidgetPosition();
	iGrid->HandleItemRemovalL();
	RestoreWidgetPosition();
	CacheWidgetPosition();
	
	if ( iCurrentHighlight != iGrid->CurrentItemIndex() )
		{
		iCurrentHighlight = iGrid->CurrentItemIndex();
		if( !iCurrentHighlight && !iGrid->Model()->NumberOfItems() )
			{
			iCurrentHighlight = KErrNotFound ;
			}
		CHnSuiteModel* suiteModel = GetSuiteModelL();
		if ( suiteModel )
		    {
		    suiteModel->SetSuiteHighlightL( iCurrentHighlight );
		    }
		}
	static_cast<CMmListBoxItemDrawer*>( iGrid->ItemDrawer() )->TrimCacheSize(
            GetMmModel()->NumberOfItems() );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmGridContainer::FlipStateChangedL()
	{
//	if ( !IsEditMode() )
//		{
//		if ( FlipOpen() )
//			{
//			if ( iTimer )
//				{
//				delete iTimer;
//				iTimer = NULL;
//				}
//			SetHighlightVisibilityL( ETrue );
//			if ( iCurrentHighlight == KErrNotFound )
//			    {
//			    SetDefaultHighlightL( ETrue );
//			    }
//			}
//		else
//			{
//			if ( AknLayoutUtils::PenEnabled() && !iTimer )
//				{
//				iTimer = CMmHighlightTimer::NewL( this );
//				}
//			SetHighlightVisibilityL( EFalse );
//			DrawNow();
//			}
//		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmGridContainer::ColumnsInCurrentView()
	{
	CAknGridView* view ( (CAknGridView*) iGrid->View() );
	return view->NumberOfColsInView();
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmGridContainer::RowsInCurrentView()
    {
    CAknGridView* view ( (CAknGridView*) iGrid->View() );
    return view->NumberOfRowsInView();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmGridContainer::DrawView()
    {
    iGrid->DrawView();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmGridContainer::SetVerticalItemOffset( TInt aOffset )
	{
	iGrid->SetVerticalItemOffset( aOffset );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmGridContainer::VerticalItemOffset() const
	{
	return iGrid->VerticalItemOffset();
	}

//----------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmGridContainer::UpdateViewScrollBarThumbs()
	{
	iGrid->UpdateScrollBarThumbs();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmGridContainer::ItemIsVisible( TInt aItemIndex ) const
    {
    CListBoxView* v = iGrid->View(); 
    TRect itemRect( v->ItemPos( aItemIndex ), v->ItemSize( aItemIndex ) );
    TRect viewRect = v->ViewRect();
    TBool isVisible = EFalse;
    if ( itemRect.Intersects( viewRect ) )
        {
        TRect intersection = itemRect;
        intersection.Intersection( viewRect );
        isVisible = intersection.Height() > 1 && intersection.Width() > 1;
        }
    return isVisible;
    }

// End of File

