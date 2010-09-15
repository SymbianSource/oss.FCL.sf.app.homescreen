/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Implementation of the container control for WidgetManager application.
*
*/

// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikappui.h>
#include <widgetmanagerview.rsg>
#include <coemain.h>
#include <eikenv.h>
#include <barsread.h>
#include <AknUtils.h>
#include <barsread.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <AknIconArray.h>
#include <aknstyluspopupmenu.h> 
#include <avkon.loc>
#include <aknlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout.lag>
#include <AknsSkinInstance.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <aknenv.h>
#include <aknlists.h>
#include <aknpopup.h> 
#include <badesca.h> 
#include <eikclbd.h>
#include <aknsfld.h>
#include <AknsBasicBackgroundControlContext.h>
#include <apgcli.h>
#include <widgetmanager.mbg>
#include <avkon.rsg>
#include <coecobs.h>
#include <coecntrl.h>
#include <featmgr.h>     // FeatureManager
#include <hlplch.h>      // HlpLauncher
#include <csxhelp/hmsc.hlp.hrh>
#include <aisystemuids.hrh>
#include <babitflags.h>

#include "wmcommon.h"
#include "wmplugin.h"
#include "wmmaincontainer.h"
#include "wmmaincontainerview.h"
#include "wmresourceloader.h"
#include "widgetmanager.hrh"
#include "wmlistbox.h"
#include "wmpersistentwidgetorder.h"
#include "wmdetailsdlg.h"
#include "wmstore.h"
#include "wmwidgetloaderao.h"
#include "wmconfiguration.h"
#include "wminstaller.h"

#include <hscontentcontroller.h> // content control api
#include <hscontentinfoarray.h> // content control api

// CONSTANTS
const TInt KTextLimit = 40; // Text-limit for find-field
const TInt KMinWidgets = 1; // minimum number of widgets to show findpane

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }

// ---------------------------------------------------------
// CWmMainContainer::CWmMainContainer()
// ---------------------------------------------------------
//
CWmMainContainer::CWmMainContainer( CWmPlugin& aWmPlugin ) :
    iWmPlugin( aWmPlugin )
    {
    iWidgetsList = NULL;
    iFindbox = NULL;
    iFindPaneIsVisible = EFalse;
    iBgContext = NULL;
    iClosingDown = ETrue;
    }

// ---------------------------------------------------------
// CWmMainContainer::~CWmMainContainer()
// ---------------------------------------------------------
//
CWmMainContainer::~CWmMainContainer()
    {
    if ( iSelectedStore )
        {
        delete iSelectedStore;
        iSelectedStore = NULL;
        }
    TRAP_IGNORE(DeactivateFindPaneL(EFalse));
    
    if ( iWidgetLoader )
        {
        iWidgetLoader->SetObserver( NULL );
        delete iWidgetLoader;
        }

    RemoveCtrlsFromStack();
    Components().ResetAndDestroy();
    
    iWidgetsList = NULL;
    iFindbox = NULL;
    delete iBgContext;
    }

// ---------------------------------------------------------
// CWmMainContainer::NewL
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainer::NewL( 
    const TRect& aRect,
    CWmPlugin& aWmPlugin )
    {
    CWmMainContainer* self = CWmMainContainer::NewLC( 
        aRect, 
        aWmPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmMainContainer::NewLC
// ---------------------------------------------------------
//
CWmMainContainer* CWmMainContainer::NewLC( 
    const TRect& aRect,
    CWmPlugin& aWmPlugin )
    {
    CWmMainContainer* self = new (ELeave) CWmMainContainer( aWmPlugin );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ---------------------------------------------------------
// ScreenRect
// rectangle representing the screen
// ---------------------------------------------------------
//
TRect ScreenRect()
    {
    TRect screen;
    CCoeEnv* env = CCoeEnv::Static();
    if ( env )
        {
        CWsScreenDevice* screenDevice = env->ScreenDevice();
        if ( screenDevice )
            screen.SetRect( TPoint(0,0), screenDevice->SizeInPixels() );
        }
    return screen;
    }

// ---------------------------------------------------------
// CWmMainContainer::ConstructL
// ---------------------------------------------------------
//
void CWmMainContainer::ConstructL( 
        const TRect& aRect )
    {  
    // Initialize control array
    InitComponentArrayL();
    Components().SetControlsOwnedExternally( ETrue );

    // create the UI
    CreateWindowL();

    // background context
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgScreen, ScreenRect() , ETrue);
    
    // set up controls
    InitializeControlsL( aRect );

    // set size and activate
    SetRect( aRect );
    ActivateL();
    
    }

// ---------------------------------------------------------
// CWmMainContainer::SizeChanged
// ---------------------------------------------------------
//
void CWmMainContainer::SizeChanged()
    {
    CCoeControl::SizeChanged();
    iBgContext->SetRect( ScreenRect() );
    LayoutControls();
    }

// ---------------------------------------------------------
// CWmMainContainer::LayoutControls
// ---------------------------------------------------------
//
void CWmMainContainer::LayoutControls()
    {
    TRect rect( Rect() );

    // layout iWidgetsList
    if( iFindbox && iFindPaneIsVisible )
        {
        iWidgetsList->SetRect( rect );
        HandleFindSizeChanged();
        }
    else
        {
        TAknWindowLineLayout listPane;
        listPane.il = rect.iTl.iX;
        listPane.it = rect.iTl.iY;
        listPane.ib = 0;
        listPane.iH = rect.Height();
        listPane.iW = rect.Width();
        listPane.ir = ELayoutEmpty;
        AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
        }

    DrawDeferred();
    }

// ---------------------------------------------------------
// CWmMainContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );

    // This is a bug fix for ou1cimx1#217716 & ou1cimx1#217667.
    // For some weird reason homescreen is genarating one extra EEventKey 
    // when using bluetooth keyboard & backspace key. This if is to 
    // ignore that event. Extra event allways has iModifiers set to 
    // EModifierAutorepeatable, EModifierKeyboardExtend and 
    // EModifierNumLock.
    if ( aType == EEventKey && 
         aKeyEvent.iScanCode == EStdKeyBackspace &&
         aKeyEvent.iModifiers&EModifierKeyboardExtend &&
         aKeyEvent.iModifiers&EModifierNumLock &&
         aKeyEvent.iModifiers&EModifierAutorepeatable )
        {
        return EKeyWasConsumed;
        }

    // Handle search keyevent
    keyResponse = HandleSearchKeyEventL( aKeyEvent, aType );
   
    // Handle list keyevent
    if ( keyResponse == EKeyWasNotConsumed )
        {
        keyResponse = HandleListKeyEventL( aKeyEvent, aType );
        }
    
    // Update ui if needed 
    if ( keyResponse == EKeyWasConsumed )
        {
        DrawDeferred();
        }
    
    // Do not let UI framework forward the keys to child controls as
    // we have already done that.
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleSearchKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::HandleSearchKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    // open search field with alpha digit numbers
    if ( aType == EEventKeyDown && !iFindPaneIsVisible && 
            aKeyEvent.iScanCode < EStdKeyF1 &&
        TChar( aKeyEvent.iScanCode ).IsAlphaDigit() )
        {
        ActivateFindPaneL( EFalse );
        
        if ( iFindPaneIsVisible )
            {
            return EKeyWasConsumed;
            }
        }
    
    if ( iFindPaneIsVisible && aType == EEventKey )
        {
        if ( aKeyEvent.iScanCode == EStdKeyNo || aKeyEvent.iCode == EKeyNo )
            {
            DeactivateFindPaneL();
            return EKeyWasConsumed;
            }
        
        TBool needsRefresh( EFalse );
       
        if ( iFindbox->TextLength() == 0 
                && aKeyEvent.iScanCode == EStdKeyBackspace )
            {
            // if lenght is 0 and backspace is pressed AknFind will deactivate
            // searchbox so we don't want to pass this event to AknFind
            keyResponse = EKeyWasConsumed;
            }
        else
            {
            keyResponse = AknFind::HandleFindOfferKeyEventL( 
                    aKeyEvent, aType, this, 
                    iWidgetsList, iFindbox, ETrue, needsRefresh );
            }
        if ( needsRefresh )
            {
            DrawNow();
            }
        }

    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleListKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmMainContainer::HandleListKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    // pass key event except backpace or delete key event to widgets list if focused
    if ( ( aType == EEventKey ) && ( aKeyEvent.iCode == EKeyBackspace 
            || aKeyEvent.iCode == EKeyDelete ) )
        {
        if( CanDoUninstall() )
            {
            UninstallWidgetL();
            }
        else
            {
            CWmWidgetData* data = iWidgetsList->WidgetData();
            if( data != NULL )
                iWmPlugin.ResourceLoader().InfoPopupL(
                     R_QTN_WM_UNINST_NOT_ALLOWED, data->Name() );
            }
        keyResponse = EKeyWasConsumed;
        }
    else 
        {
        //passing to listbox handler
        keyResponse = iWidgetsList->OfferKeyEventL( 
                aKeyEvent, aType );
        }
    
    return keyResponse;
    }

// ---------------------------------------------------------
// CWmMainContainer::FindChildControlByPoint
// ---------------------------------------------------------
//
CCoeControl* CWmMainContainer::FindChildControlByPoint(
        const TPoint& aPoint )
    {
    CCoeControl* theControl = NULL;
    CCoeControlArray::TCursor cursor = Components().Begin();
    CCoeControl* c;
    while((c=cursor.Control<CCoeControl>()) != NULL )
        {
        if(c->IsVisible())
            {
            if ( c->Rect().Contains( aPoint ) ) theControl = c;
            }
        cursor.Next();
        }
    return theControl;
    }

// ---------------------------------------------------------
// CWmMainContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CWmMainContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !iClosingDown  )
        {
        TBool eatEvent( EFalse );
        TPointerEvent event( aPointerEvent );

        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
            {
            // Check if user clicked a child control
            CCoeControl* control = FindChildControlByPoint( aPointerEvent.iPosition );
            if ( control && !control->IsFocused() )
                {
                // remove focus from ALL other child controls.
                CCoeControlArray::TCursor cursor = Components().Begin();
                CCoeControl* c;
                while ((c=cursor.Control<CCoeControl>()) != NULL )
                    {
                    if ( c != control && c->IsFocused() ) c->SetFocus( EFalse );
                    cursor.Next();
                    }
                
                // Set focus to the control that was clicked
                control->SetFocus( ETrue );
                                
                // repaint
                DrawDeferred();
                }
            
            // stylus popup should not be opened when uninstalling. 
            // ou1cimx1#302973
            if ( control == iWidgetsList && iWidgetsList->IsFocused() )
                 {
                 TInt itemIndex = iWidgetsList->CurrentListBoxItemIndex();
                 TBool itemPointed = iWidgetsList->View()->XYPosToItemIndex(
                                         aPointerEvent.iPosition,
                                         itemIndex );
                 if ( itemIndex >= 0 && itemPointed )
                     {
                     CWmWidgetData& data = iWidgetsList->WidgetData( itemIndex );                
                     if ( &data && data.IsUninstalling() )
                         {
                         event.iType = TPointerEvent::EButton1Up;
                         eatEvent = ETrue;
                         }
                     }
                 }
            
            }
        
        // set downkey event to base class
        CCoeControl::HandlePointerEventL( aPointerEvent );
        
        // send key up event if selected widget is being uninstalled.
        // stylus popup shouldn't be displayed for this item.
        if ( eatEvent )
            {
            CCoeControl::HandlePointerEventL( event );
            }
        }
    }

// ---------------------------------------------------------
// CWmMainContainer::InitializeControlsL
// ---------------------------------------------------------
//
void CWmMainContainer::InitializeControlsL( const TRect& /*aRect*/ )
    {
    // Create widget list box
    iWidgetsList = CWmListBox::NewL(
            iWmPlugin,
            TRect(),
            this );
    iWidgetsList->SetMopParent( this );
    AddControlL( iWidgetsList, EWidgetsList );
    iWidgetsList->ActivateL();
    iWidgetsList->SetListBoxObserver( this );
    
    // Create scroll bar.
    iWidgetsList->CreateScrollBarFrameL( ETrue );
    iWidgetsList->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iWidgetsList->UpdateScrollBarsL(); 
    
    // search field
    iFindbox = CAknSearchField::NewL( *this,
                        CAknSearchField::EAdaptiveSearch,
                        0, KTextLimit );
    AddControlL( iFindbox, EFindBox );
    iFindbox->AddAdaptiveSearchTextObserverL(this);
   
    StartLoadingWidgetsL();
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleWidgetListChanged
// ---------------------------------------------------------
//
void CWmMainContainer::HandleWidgetListChanged()
    {
    TRAP_IGNORE( StartLoadingWidgetsL(); );
    }

// ---------------------------------------------------------
// CWmMainContainer::StartLoadingWidgetsL
// ---------------------------------------------------------
//
void CWmMainContainer::StartLoadingWidgetsL()
    {
    if ( !iWidgetLoader )
        {
        // create the widget loader AO
        iWidgetLoader = CWmWidgetLoaderAo::NewL( iWmPlugin, *iWidgetsList );
        iWidgetLoader->SetObserver( this );
        }
    iWidgetLoader->StartLoading();
    }

// ---------------------------------------------------------
// CWmMainContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CWmMainContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( 
                            AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        
        // notify widgetlist
        iWidgetsList->HandleLayoutChanged();
        }
    }


// ---------------------------------------------------------
// CWmMainContainer::MopSupplyObject
// ---------------------------------------------------------
//
TTypeUid::Ptr CWmMainContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( 
                                            aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CWmMainContainer::Draw
// ---------------------------------------------------------
//
void CWmMainContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );
    }

// ---------------------------------------------------------
// CWmMainContainer::AddControlL
// ---------------------------------------------------------
//
void CWmMainContainer::AddControlL( 
                                CCoeControl* aControl, 
                                TInt aControlId )
    {
    Components().AppendLC( aControl, aControlId ); // Ownership transfered 
    CleanupStack::Pop(aControl);
    CAknView* view = iWmPlugin.ViewAppUi().View( 
            TUid::Uid(EWmMainContainerViewId) );
    if ( view )
        {
        iWmPlugin.ViewAppUi().AddToStackL( *view, aControl );
        }
    else
        {
        iWmPlugin.ViewAppUi().AddToStackL( aControl );
        }
    aControl->MakeVisible( ETrue );
    }

// ---------------------------------------------------------
// CWmMainContainer::IsLoadingWidgets
// ---------------------------------------------------------
//
TBool CWmMainContainer::IsLoadingWidgets()
    {    
    return ((iWidgetLoader && iWidgetLoader->IsLoading()) ? 
        ETrue : EFalse);
    }

// ---------------------------------------------------------
// CWmMainContainer::WidgetSelected
// ---------------------------------------------------------
//
TBool CWmMainContainer::WidgetSelected() 
    {
    return (( iWidgetsList->IsFocused() || iWidgetsList->IsHighlightEnabled())? 
            ETrue : EFalse );
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoAdd
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoAdd()
    {
    TBool retVal = EFalse;
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( WidgetSelected() && data && !data->IsUninstalling() )
        {
        if ( data->HsContentInfo().CanBeAdded() )
            retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoUninstall
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoUninstall() 
    {
    TBool retVal( EFalse );
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( WidgetSelected() && data && !data->IsUninstalling() )
        {
        if ( data->WidgetType() == CWmWidgetData::ECps &&
            data->PublisherUid() != KNullUid && 
            data->WrtType() != CWmWidgetData::EUnIdentified )
            {
            retVal = ETrue;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoLaunch
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoLaunch() 
    {
    TBool retVal(EFalse);
    
    if ( WidgetSelected() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();
        if ( data && !data->IsUninstalling() )
            {
            if ( data->WidgetType() == CWmWidgetData::ECps &&
                data->PublisherUid() != KNullUid )
                {
                retVal = ETrue;
                }
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoFind
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoFind()
    {
    return !iFindPaneIsVisible;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoSort
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoSort()
    {
    return !iFindPaneIsVisible;
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoDetails
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoDetails()
    {
    return ( WidgetSelected() && iWidgetsList->WidgetData() 
            && !iWidgetsList->WidgetData()->IsUninstalling() );
    }

// ---------------------------------------------------------
// CWmMainContainer::CanDoHelp
// ---------------------------------------------------------
//
TBool CWmMainContainer::CanDoHelp()
    {
    return FeatureManager::FeatureSupported( KFeatureIdHelp );
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::AddWidgetToHomeScreenL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::AddWidgetToHomeScreenL()
    {
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( !iClosingDown )
        {
        if ( iFindbox && iFindPaneIsVisible )
            {
            DeactivateFindPaneL(EFalse);
            }
        
        if ( data && !data->IsUninstalling() )
            {
            // set add to homescreen to be executed later
            iWmPlugin.SetPostponedCommandL(
                        CWmPlugin::EAddToHomescreen,
                        data->HsContentInfo() );

            iWmPlugin.CloseView();
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::LaunchWidgetL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::LaunchWidgetL()
    {
    if ( CanDoLaunch() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();
        
        HBufC* param = KNullDesC().AllocLC(); // launch params
        
        RApaLsSession appArc;
        User::LeaveIfError( appArc.Connect() ); // connect to AppArc server
        CleanupClosePushL( appArc );
        TThreadId id;
        User::LeaveIfError( appArc.StartDocument(
            *param, data->PublisherUid(), id ) );
    
        CleanupStack::PopAndDestroy( &appArc );      
        CleanupStack::PopAndDestroy( param );
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::ActivateFindPaneL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::ActivateFindPaneL( TBool aActivateAdaptive )
    {
    if ( iFindbox && !iFindPaneIsVisible &&
            iWidgetsList->Model()->NumberOfItems() > KMinWidgets )
        {
        // set column filter flag
        TBitFlags32 bitFlag;
        bitFlag.ClearAll(); // clear all columns
        bitFlag.Assign( 1,1 ); // turn on column at index one
        iFindbox->SetListColumnFilterFlags( bitFlag );
        
        // enable filtering
        CAknFilteredTextListBoxModel* m = 
                static_cast <CAknFilteredTextListBoxModel*> ( iWidgetsList->Model() );
        if ( m )
            {
            m->CreateFilterL( iWidgetsList, iFindbox );
            if ( m->Filter() )
                {
                m->Filter()->SetParentControl( this );
                }
            }

        iFindbox->ActivateL();
        iFindbox->ResetL();
        iFindbox->SetSearchTextL( KNullDesC );
        iFindbox->SetSkinEnabledL( ETrue );
        iFindPaneIsVisible = ETrue;
        iFindbox->MakeVisible( ETrue );
        
        // layout listbox and findbox
        LayoutControls();        
        
        if ( aActivateAdaptive )
            {
            iFindbox->ShowAdaptiveSearchGrid();
            }
        iFindbox->SetFocus( ETrue );
        iWidgetsList->SetFindPaneIsVisible( ETrue );
        
        if ( AknLayoutUtils::MSKEnabled() )
            {
            // set soft key set
            CEikButtonGroupContainer* cbaGroup =
                CEikButtonGroupContainer::Current();            
            TInt cbaResourceId = ( ( iWmPlugin.Configuration().StoreCount() > 0 )?
                                        R_WM_SOFTKEYS_OPTIONS_CANCEL__STORE :
                                        R_AVKON_SOFTKEYS_OPTIONS_CANCEL__SELECT );            
            cbaGroup->SetCommandSetL( cbaResourceId );            
            cbaGroup->DrawNow();
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::DeactivateFindPaneL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::DeactivateFindPaneL(TBool aLayout)
    {
    if( iFindbox && iFindPaneIsVisible )
        {
        // notify search field we're shutting down
        TKeyEvent keyEvent = { EKeyNo, EStdKeyNo, 0, 0  };
        iFindbox->OfferKeyEventL( keyEvent, EEventKey );
        
        // reset findbox
        iFindbox->ResetL();

        // reset filter
        CAknFilteredTextListBoxModel* m = 
                static_cast <CAknFilteredTextListBoxModel*> ( iWidgetsList->Model() );
        if ( m && m->Filter() )
            {
            m->Filter()->ResetFilteringL();
            m->RemoveFilter();
            }
        
        iFindbox->MakeVisible( EFalse );        
        iFindPaneIsVisible = EFalse;        
        iWidgetsList->SetFindPaneIsVisible( EFalse );       
        

        // set soft key set
        CEikButtonGroupContainer* cbaGroup =
            CEikButtonGroupContainer::Current();
        TInt cbaResourceId = ( ( AknLayoutUtils::MSKEnabled() && 
                                 iWmPlugin.Configuration().StoreCount() > 0 )?
                                    R_WM_SOFTKEYS_OPTIONS_BACK__STORE :
                                    R_AVKON_SOFTKEYS_OPTIONS_BACK );
        cbaGroup->SetCommandSetL( cbaResourceId );
        
        if (aLayout) //no need to draw UI if exiting list
            {
            LayoutControls();
            cbaGroup->DrawNow();
    
            DrawNow();
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::SortListAlphabeticallyL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::SortListAlphabeticallyL()
    {
    if ( iWidgetsList )
        {
        iWidgetsList->DoSortToVisibleArray( CWmListBox::EAlphabetical );
        iWidgetsList->DoSortToOrderData( CWmListBox::EAlphabetical );
       
        //store changed list order
        CWmPersistentWidgetOrder* widgetOrder =
            CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
        CleanupStack::PushL( widgetOrder );
        widgetOrder->StoreL( iWidgetsList->OrderDataArray() );
        
        CleanupStack::PopAndDestroy( widgetOrder );
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::UninstallWidgetL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::UninstallWidgetL()
    {
    if ( CanDoUninstall() )
        {
        CWmWidgetData* data = iWidgetsList->WidgetData();
        if ( data )
            {
            if ( iWmPlugin.ResourceLoader().QueryPopupL( 
                    R_QTN_WM_UNINSTALL_WIDGET_QUERY, data->Name() ) )
                {
                
                TRAPD( err, iWmPlugin.WmInstaller().UninstallL( data ) );
                if ( err != KErrNone )
                    {
                    iWmPlugin.ResourceLoader().InfoPopupL( 
                            R_QTN_WM_UNINST_PROCESS_BUSY, KNullDesC );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::HandleStoreCommandL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::HandleStoreCommandL()
    {
    delete iSelectedStore;
    iSelectedStore = NULL;
    
    const RPointerArray<CWmConfItem>& storeConfArray = 
            iWmPlugin.Configuration().StoreConfArray();
    
    if ( storeConfArray.Count() == 1 )
        {   
        iSelectedStore = CWmStore::NewL( *storeConfArray[0], 
                iWmPlugin.ResourceLoader() );
        
        iSelectedStore->ExecuteL();
        }
    else if ( storeConfArray.Count() > 1 )
        {
        DisplayAndLaunchStoresL();
        }
    else
        {
        // do nothing
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::ShowHelpL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::ShowHelpL()
    {
    TUid fwUid = TUid::Uid( AI_UID3_AIFW_COMMON );
    // retrieve resolve host application help context
    CArrayFix<TCoeHelpContext>* helpContext =
        iEikonEnv->EikAppUi()->AppHelpContextL();

    // add the widget manager entry into the help context
    CleanupStack::PushL( helpContext );
    helpContext->InsertL( 0, TCoeHelpContext(
            fwUid, KWM_HLP_WIDGET_COLLECTION ) );
    CleanupStack::Pop( helpContext );

    // launch help
    HlpLauncher::LaunchHelpApplicationL(
          iEikonEnv->WsSession(),
          helpContext );
    helpContext = 0; // HlpLauncher takes ownership of helpContext
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::RemoveCtrlsFromStack
// ---------------------------------------------------------------------------
//
void CWmMainContainer::RemoveCtrlsFromStack()
    {
    for ( TInt i=0; i < CountComponentControls(); i++ ) 
        {
        CCoeControl* ctl = ComponentControl( i ) ;
        if ( ctl )
            {
            iWmPlugin.ViewAppUi().RemoveFromStack( ctl );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmMainContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CWmMainContainer::HandleListBoxEventL(
        CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( ( aEventType == EEventEnterKeyPressed ||
           aEventType == EEventItemSingleClicked ) 
           && !iClosingDown )    
        {
        AddWidgetToHomeScreenL();
        }
    }

// ----------------------------------------------------
// CWmMainContainer::LaunchDetailsDialogL
// ----------------------------------------------------
//
void CWmMainContainer::LaunchDetailsDialogL()
    {
    CWmWidgetData* data = iWidgetsList->WidgetData();
    if ( data )
        {       
        // Find out if HS is full
        TBool hsFull = ETrue;
        MHsContentController& controller = iWmPlugin.ContentController();
        CHsContentInfo* activeView = CHsContentInfo::NewLC();
        if ( controller.ActiveViewL( *activeView ) == KErrNone )
            {
            hsFull = activeView->IsFull();
            }
        CleanupStack::PopAndDestroy( activeView );
        
        CWmDetailsDlg* dlg = CWmDetailsDlg::NewL(
                data->Name(), 
                data->Description(), 
                !hsFull,
                data->HsContentInfo().IconPath(),
                *iWidgetsList->DefaultLogo(),
                *iWidgetsList->DefaultMask()
                );
        
        if ( dlg && dlg->ExecuteLD() == ECbaAddToHs )
            {
            AddWidgetToHomeScreenL();
            }
        }
    }

// ----------------------------------------------------
// CWmMainContainer::SetClosingDown
// ----------------------------------------------------
//
void CWmMainContainer::SetClosingDown( TBool aClosingDown )
    {
    iClosingDown = aClosingDown;
    }

// ----------------------------------------------------
// CWmMainContainer::ClosingDown
// ----------------------------------------------------
//
TBool CWmMainContainer::ClosingDown()
    {
    return iClosingDown;
    }

// ----------------------------------------------------
// CWmMainContainer::HandleFindSizeChanged
// ----------------------------------------------------
//
void CWmMainContainer::HandleFindSizeChanged()
    {    
    TAknWindowLineLayout findPane = 
            AknLayoutScalable_Avkon::find_pane().LayoutLine();

    TAknWindowLineLayout listPane;

    TRect listRect = iWidgetsList->Rect();
    listPane.il = listRect.iTl.iX;
    listPane.it = listRect.iTl.iY;
    listPane.ib = 0;
    listPane.iH = TUint16( listRect.Height() - ( findPane.iH - 1 ) );
    listPane.iW = listRect.Width();
    listPane.ir = ELayoutEmpty;
    
    // set findpane width equal to listwidth
    findPane.iW = listPane.iW;

    TRect rect = Rect();
    AknLayoutUtils::LayoutControl( iFindbox, rect, findPane );
    AknLayoutUtils::LayoutControl( iWidgetsList, rect, listPane );
    
    iWidgetsList->DrawNow();
    }

// ----------------------------------------------------
// CWmMainContainer::WmListBox
// ----------------------------------------------------
//
CWmListBox& CWmMainContainer::WmListBox()
    {
    return *iWidgetsList;
    }

// ----------------------------------------------------
// CWmMainContainer::FocusChanged
// ----------------------------------------------------
//
void CWmMainContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );

    if ( iFindbox && iFindPaneIsVisible && 
        !iFindbox->IsFocused() && IsFocused() )
        {
        // set focus to find pane if its lost ( ou1cimx1#308019 )
        iFindbox->SetFocus( ETrue );
        }
    }

// ----------------------------------------------------
// CWmMainContainer::AdaptiveSearchTextChanged
// ----------------------------------------------------
//
void CWmMainContainer::AdaptiveSearchTextChanged( CAknSearchField* /*aSearchField*/ )
    {
    // fix for ou1cimx1#376818. aknfind will call drawDeferred for lisbox. 
    // We need to be sure that button area is also drawn.
    DrawDeferred();
    }


// ----------------------------------------------------
// CWmMainContainer::LoadDone
// ----------------------------------------------------
//
void CWmMainContainer::LoadDoneL( TBool aWidgetListChanged )
    {
    if ( aWidgetListChanged && iFindbox && iFindPaneIsVisible )
        {
        CAknFilteredTextListBoxModel* m = 
                static_cast <CAknFilteredTextListBoxModel*> ( iWidgetsList->Model() );
        if ( m && m->Filter() )
            {
            m->Filter()->HandleItemArrayChangeL();
            }
        }
    }

// ----------------------------------------------------------
// CWmMainContainer::CleanupIconArray
// ----------------------------------------------------------
//  
void CWmMainContainer::CleanupIconArray( TAny* aIconArray )
    {
    CArrayPtrFlat<CGulIcon>* icons = (CArrayPtrFlat<CGulIcon>*)aIconArray;
    icons->ResetAndDestroy();
    delete icons;
    }

// ----------------------------------------------------
// CWmMainContainer::DisplayAndLaunchStoresL
// ----------------------------------------------------
// 
void CWmMainContainer::DisplayAndLaunchStoresL()
    {
    // prepare stores
    RPointerArray<CWmStore> storesArray;
    CleanupResetAndDestroyPushL( storesArray );
        
    const RPointerArray<CWmConfItem>& storeConfArray = 
            iWmPlugin.Configuration().StoreConfArray();
    
    for( TInt i = 0; i < storeConfArray.Count(); i++ )
        {
        CWmStore* store = CWmStore::NewL( *storeConfArray[i], 
                iWmPlugin.ResourceLoader() );
        CleanupStack::PushL( store );
        storesArray.AppendL( store );
        CleanupStack::Pop( store );
        }
    
    CEikFormattedCellListBox* listBox = 
                new (ELeave) CAknDoubleLargeGraphicPopupMenuStyleListBox();
    CleanupStack::PushL( listBox );
    
    CAknPopupList* popupList = CAknPopupList::NewL( 
            listBox, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT, 
            AknPopupLayouts::EMenuDoubleLargeGraphicWindow );

    CleanupStack::PushL( popupList );
    
    listBox->ConstructL( popupList, EAknListBoxMenuList );
    listBox->CreateScrollBarFrameL(ETrue);
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    
    // set title
    HBufC* title = StringLoader::LoadLC( R_QTN_WM_STORELIST_TITLE );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy(); // title
   
    // format str
    _LIT( KFormatStr , "%d\t%S" );
    
    // items array
    CDesCArray* itemArray = 
            static_cast<CDesCArray*>( listBox->Model()->ItemTextArray( ) );

    // create icons array
    CArrayPtr<CGulIcon>* iconarray = new (ELeave) CArrayPtrFlat<CGulIcon>( 2 );
    CleanupStack::PushL( TCleanupItem( CleanupIconArray, iconarray) );
    
    for ( TInt i=0; i < storesArray.Count(); i++ )
        {
        // prepare icon
        CGulIcon* icon = CGulIcon::NewL( storesArray[i]->StoreIcon(),  
                                         storesArray[i]->StoreMask() );
        CleanupStack::PushL( icon );
        icon->SetBitmapsOwnedExternally( ETrue );
        iconarray->AppendL( icon );
        CleanupStack::Pop( icon );
        
        // add store to items array
        HBufC* buf = HBufC::NewLC( 
                storesArray[i]->Heading().Length() + 
                KFormatStr().Length() );

        TPtr ptr = buf->Des();
        ptr.Format( KFormatStr(), i, &storesArray[i]->Heading() );
        itemArray->AppendL( ptr );
        CleanupStack::PopAndDestroy( buf );
        }

    CleanupStack::Pop(); // iconarray
    listBox->ItemDrawer()->ColumnData()->SetIconArrayL( iconarray );
    
    TBool result = popupList->ExecuteLD();
    CleanupStack::Pop(); // popupList ( deleted in ExecuteLD)
    
    TInt selection = listBox->CurrentItemIndex();
    CleanupStack::PopAndDestroy(); // listBox
    
    if ( result && selection >= 0 && 
        selection < storesArray.Count() )
        {        
        iSelectedStore = storesArray[selection];
        if ( iSelectedStore )
            {
            storesArray.Remove( selection );
            iSelectedStore->ExecuteL();
            }
        }

    CleanupStack::PopAndDestroy(); //storesArray
    }

// End of File

