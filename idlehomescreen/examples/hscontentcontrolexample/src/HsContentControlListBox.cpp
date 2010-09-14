/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Control example
*
*/


//  Includes
#include <aknlists.h>
#include <akncontext.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <aknstyluspopupmenu.h>

#include <aknsinglestyletreelist.h>
#include <aknsbasicbackgroundcontrolcontext.h>

#include <hsccapiclient.h>
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>

#include "HsContentControlListBox.h"

//  Constants
const TInt KFlags( CAknSingleStyleTreeList::EPersistent | 
        CAknSingleStyleTreeList::EExpanded );

_LIT( KActivate, "Activate" );

enum TTabs
    {
    ETabFirst = 0,
    ETabActiveConfiguration = ETabFirst,
    ETabConfigurations,
    ETabViews,
    ETabWidgets,
    ETabLast = ETabWidgets 
    };

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHsContentControlListBox::CHsContentControlListBox
//
// -----------------------------------------------------------------------------
//
CHsContentControlListBox::CHsContentControlListBox()
    {
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::~CHsContentControlListBox
//
// -----------------------------------------------------------------------------
//
CHsContentControlListBox::~CHsContentControlListBox()
    {    
    delete iHsCcApiClient;
    delete iCallback;
    delete iPopup;
    delete iListBox;
    delete iBgContext;
    delete iArray;    
    delete iDecorator;
    }
  
// -----------------------------------------------------------------------------
// CHsContentControlListBox::NewL
//
// -----------------------------------------------------------------------------
//
CHsContentControlListBox* CHsContentControlListBox::NewL( const TRect& aRect,  
    const CCoeControl* aParent, MEikCommandObserver* aCommandObserver )          
    {
    CHsContentControlListBox* self = 
        CHsContentControlListBox::NewLC( aRect, aParent, aCommandObserver ); 
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::NewLC
//
// -----------------------------------------------------------------------------
//
CHsContentControlListBox* CHsContentControlListBox::NewLC( const TRect& aRect,          
    const CCoeControl* aParent, MEikCommandObserver* aCommandObserver )         
    {
    CHsContentControlListBox* self = new ( ELeave ) CHsContentControlListBox();
    CleanupStack::PushL( self );
    
    self->ConstructL( aRect, aParent, aCommandObserver );
    
    return self;
    }
            
// -----------------------------------------------------------------------------
// CHsContentControlListBox::ConstructL
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::ConstructL( const TRect& aRect,          
    const CCoeControl* aParent, MEikCommandObserver* /*aCommandObserver*/ )         
    {
    iHsCcApiClient = CHsCcApiClient::NewL( this );
    
    TCallBack cb( PopulateList, this );
    
    iCallback = new ( ELeave ) CAsyncCallBack( cb, CActive::EPriorityStandard );
    
    if ( !aParent )
        {
        CreateWindowL();
        }
    else
        {
        SetContainerWindowL( *aParent );
        }
    
    InitializeControlsL();

    SetRect( aRect );
               
    ActivateL();
    
    PopulateListL();
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::CountComponentControls
//
// -----------------------------------------------------------------------------
//
TInt CHsContentControlListBox::CountComponentControls() const
    {
    return 1;
    }
                
// -----------------------------------------------------------------------------
// CHsContentControlListBox::CHsContentControlListBox
//
// -----------------------------------------------------------------------------
//
CCoeControl* CHsContentControlListBox::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iListBox;
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::OfferKeyEventL
//
// -----------------------------------------------------------------------------
//
TKeyResponse CHsContentControlListBox::OfferKeyEventL(  
    const TKeyEvent& aKeyEvent, TEventCode aType )         
    {    
    if ( iListBox->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )         
        {
        return EKeyWasConsumed;
        }
    
    return CCoeControl::OfferKeyEventL( aKeyEvent, aType );
    }
                
// -----------------------------------------------------------------------------
// CHsContentControlListBox::HandleResourceChange
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    SetRect( iAvkonAppUi->ClientRect() );
    }
                
// -----------------------------------------------------------------------------
// CHsContentControlListBox::SizeChanged
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::SizeChanged()
    {
    CCoeControl::SizeChanged();

    LayoutControls();    
    }
                
// -----------------------------------------------------------------------------
// CHsContentControlListBox::MopSupplyObject
//
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CHsContentControlListBox::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::HandleTreeListEvent
//
// -----------------------------------------------------------------------------
//
TInt CHsContentControlListBox::HandleTreeListEvent(       
    CAknTreeList& /*aList*/,
    TAknTreeItemID aItem,
    TEvent aEvent )
    {        
    if ( aEvent == EItemSelected )
        {        
        if ( !iListBox->IsLeaf( aItem ) && !iListBox->IsMarked( aItem ) )
            {
            return KErrNone;
            }
                
        const TWsEvent& event( iCoeEnv->LastEvent() );        
        
        if ( event.Type() == EEventPointer )
            {                              
            TInt x, y;            
        
            TPointerEvent& pointer( *event.Pointer() );
            
            x = pointer.iParentPosition.iX;
            y = pointer.iParentPosition.iY;

            delete iPopup;
            iPopup = NULL;            
        
            iPopup = CAknStylusPopUpMenu::NewL( this, TPoint( x, y ) );             
                           
            if ( iTabIndex == ETabConfigurations )
                {
                iSelectedItem = aItem;
                
                iPopup->AddMenuItemL( KActivate, 1 );
                
                iPopup->SetPosition( TPoint( x, y ), 
                    CAknStylusPopUpMenu::EPositionTypeRightBottom );
                
                iPopup->ShowMenu();
                }
            }
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::ProcessCommandL
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::ProcessCommandL( TInt aCommandId )
    {  
    TAknTreeItemID item( iSelectedItem );
    
    iSelectedItem = KAknTreeIIDNone;
    
    if ( item == KAknTreeIIDNone || aCommandId == KErrCancel )
        {
        return;
        }
           
    // Resolve selected item
    if ( iTabIndex == ETabConfigurations )
        {
        TAknTreeItemID id = iListBox->Child( KAknTreeIIDRoot, 0 );
        
        TInt count( iListBox->ChildCount( id ) );
        
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iListBox->Child( id, i ) == item )
                {
                CHsContentInfo* info( iArray->Array()[i] );
                
                TRAP_IGNORE( iHsCcApiClient->ActivateAppL( *info ) );
                
                break;
                }
            }
        }       
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::SetEmphasis
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::SetEmphasis( CCoeControl* /*aMenuControl*/, 
    TBool /*aEmphasis*/ )
    {         
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::TabChangedL
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::TabChangedL( TInt aIndex )
    {
    if ( aIndex != iTabIndex )
        {
        iTabIndex = aIndex;
        
        iDecorator->SetScrollButtonDimmed( 
            CAknNavigationDecorator::ERightButton, EFalse );
        
        iDecorator->SetScrollButtonDimmed( 
            CAknNavigationDecorator::ELeftButton, EFalse );
        
        if ( iTabIndex == ETabFirst )
            {
            iDecorator->SetScrollButtonDimmed( 
                CAknNavigationDecorator::ELeftButton, ETrue );    
            }
        else if ( iTabIndex == ETabLast )
            {
            iDecorator->SetScrollButtonDimmed( 
                CAknNavigationDecorator::ERightButton, ETrue );    
            }
        
        iCallback->Cancel();
        iCallback->CallBack();        
        }    
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::NotifyWidgetListChanged
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::NotifyWidgetListChanged()
    {
    iCallback->Cancel();
    iCallback->CallBack();
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::NotifyViewListChanged
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::NotifyViewListChanged()
    {
    iCallback->Cancel();
    iCallback->CallBack();
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::NotifyAppListChanged
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::NotifyAppListChanged()
    {
    iCallback->Cancel();
    iCallback->CallBack();
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::InitializeControlsL
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::InitializeControlsL()
    {
    iListBox = CAknSingleStyleTreeList::NewL( *this );
    iListBox->SetContainerWindowL( *this );    
    iListBox->AddObserverL( this );    
    iListBox->SetFocus( ETrue );
    
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgScreen, TRect(), ETrue );
    
    CEikStatusPane* sp( iAvkonAppUi->StatusPane() );
    
    TUid naviPaneUid( TUid::Uid( EEikStatusPaneUidNavi ) );
                                 
    CAknNavigationControlContainer* naviPane = 
        static_cast< CAknNavigationControlContainer* >( sp->ControlL( naviPaneUid ) ); 
    
    iDecorator = naviPane->ResourceDecorator();
            
    iDecorator->MakeScrollButtonVisible( ETrue );
    
    iDecorator->SetScrollButtonDimmed( 
        CAknNavigationDecorator::ELeftButton, ETrue );

    iDecorator->SetScrollButtonDimmed( 
        CAknNavigationDecorator::ERightButton, EFalse );
    
    iTabGroup = static_cast< CAknTabGroup* > ( iDecorator->DecoratedControl() );    
    iTabGroup->SetObserver( this );       
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::LayoutControls
//
// -----------------------------------------------------------------------------
//
void CHsContentControlListBox::LayoutControls()
    {
    iListBox->SetRect( Rect() );
    
    TRect rect;
    
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );     
    
    iBgContext->SetRect( rect );
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::PopulateListL
//
// -----------------------------------------------------------------------------
//    
void CHsContentControlListBox::PopulateListL()
    {
    if ( !iListBox->IsEmpty( KAknTreeIIDRoot ) )
        {
        // Remove all
        iListBox->RemoveItem( KAknTreeIIDRoot, EFalse );
        }
    
    delete iArray;
    iArray = NULL;

    if ( iTabIndex == ETabActiveConfiguration )
        {       
        CHsContentInfo* activeApp = CHsContentInfo::NewL();
        CleanupStack::PushL( activeApp );
        
        // Get active homescreen configuration
        iHsCcApiClient->ActiveAppL( *activeApp );

        _LIT( KActiveConfiguration, "Active configuration" );
        
        TAknTreeItemID root = iListBox->AddNodeL( 
            KAknTreeIIDRoot, KActiveConfiguration, KFlags, EFalse );
                       
        CHsContentInfoArray* viewList = CHsContentInfoArray::NewL();
        CleanupStack::PushL( viewList );
        
        // Get view list from the active configuration
        iHsCcApiClient->ViewListL( *activeApp, *viewList );
        
        _LIT( KView, "View " );
        
        for ( TInt j = 0; j < viewList->Array().Count(); j++ )
            {
            TBuf< 32 > viewname;        
            viewname = KView();
            viewname.AppendNum( j + 1 );
            
            CHsContentInfo* view( viewList->Array()[j] );
            
            TAknTreeItemID parent = iListBox->AddNodeL( 
                root, viewname, KFlags, EFalse );
    
            CHsContentInfoArray* widgetList = CHsContentInfoArray::NewL();
            CleanupStack::PushL( widgetList );
    
            // Get widget list from the view 
            iHsCcApiClient->WidgetListL( *view, *widgetList );
            
            for ( TInt k = 0; k < widgetList->Array().Count(); k++ )
                {
                CHsContentInfo* widget( widgetList->Array()[k] );
                
                iListBox->AddLeafL( parent, widget->Name(), KFlags, EFalse );            
                }
            
            CleanupStack::PopAndDestroy( widgetList );
            }   
        
        CleanupStack::PopAndDestroy( viewList );
        
        CleanupStack::PopAndDestroy( activeApp );    
        }
    else if ( iTabIndex == ETabConfigurations )
        {
        _LIT( KConfigurations, "Configurations" );
        
        TAknTreeItemID root = iListBox->AddNodeL( 
            KAknTreeIIDRoot, KConfigurations, KFlags, EFalse );
                   
        iArray = CHsContentInfoArray::NewL();
        
        // Get all homescreen configurations 
        iHsCcApiClient->AppListL( *iArray );
        
        _LIT( KConfiguration, "Configuration " );
        
        for ( TInt i = 0; i < iArray->Array().Count(); i++ )
            {
            TBuf< 32 > confname;
            confname = KConfiguration();
            confname.AppendNum( i + 1 );
            
            CHsContentInfo* app( iArray->Array()[i] );
                        
            iListBox->AddLeafL( root, confname, KFlags, EFalse );            
            }
        }
    
    else if ( iTabIndex == ETabViews )
        {
        _LIT( KViews, "Views" );
        
        TAknTreeItemID root = iListBox->AddNodeL( 
            KAknTreeIIDRoot, KViews, KFlags, EFalse );
    
        iArray = CHsContentInfoArray::NewL();        
    
        // Get all homescreen views
        iHsCcApiClient->ViewListL( *iArray );
        
        _LIT( KView, "View " );
        
        for ( TInt i = 0; i < iArray->Array().Count(); i++ )
            {
            TBuf< 32 > viewname;        
            viewname = KView();
            viewname.AppendNum( i + 1 );
        
            CHsContentInfo* view( iArray->Array()[i] );
            
            iListBox->AddLeafL( root, viewname, KFlags, EFalse );                 
            }
        }

    else if ( iTabIndex == ETabWidgets )
        {
        _LIT( KWidgets, "Widgets" );
        
        TAknTreeItemID root = iListBox->AddNodeL( 
            KAknTreeIIDRoot, KWidgets, KFlags, EFalse );
    
        iArray = CHsContentInfoArray::NewL();
            
        // Get all homescreen widgets
        iHsCcApiClient->WidgetListL( *iArray );
        
        for ( TInt i = 0; i < iArray->Array().Count(); i++ )
            {
            CHsContentInfo* widget( iArray->Array()[i] );
            
            iListBox->AddLeafL( root, widget->Name(), KFlags, EFalse );                 
            }
        }
    
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CHsContentControlListBox::PopulateList
//
// -----------------------------------------------------------------------------
//    
/*static*/ TInt CHsContentControlListBox::PopulateList( TAny* aAny )
    {
    CHsContentControlListBox* self = 
        static_cast< CHsContentControlListBox* >( aAny );
    
    TRAP_IGNORE( self->PopulateListL() );       
    
    return KErrNone;
    }

// End of file
