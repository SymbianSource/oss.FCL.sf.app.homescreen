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
* Active object to load widgets into list
*
*/

// INCLUDE FILES
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmpersistentwidgetorder.h"
#include "wmlistbox.h"
#include "wmwidgetloaderao.h"
#include "wminstaller.h"
#include "wmwidgetorderdata.h"

#include <hscontentcontroller.h> // content control api
#include <hscontentinfoarray.h> // content control api
#include <widgetregistryclient.h> // widget reqistry
#include <avkon.rsg> // avkon resources
#include <barsread.h> // TResourceReader

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::NewL
// ---------------------------------------------------------------------------
//
CWmWidgetLoaderAo* CWmWidgetLoaderAo::NewL(
        CWmPlugin& aWmPlugin,
        CWmListBox& aTargetList )
    {
    CWmWidgetLoaderAo* self = new (ELeave) CWmWidgetLoaderAo(
            aWmPlugin, aTargetList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::NewL
// ---------------------------------------------------------------------------
//
CWmWidgetLoaderAo::CWmWidgetLoaderAo(
        CWmPlugin& aWmPlugin,
        CWmListBox& aTargetList )
    : CActive( EPriorityNormal )
    , iWmPlugin( aWmPlugin )
    , iWidgetsList( aTargetList )
    {
    iWidgetRegistry = NULL;
    iWidgetOrder = NULL;
    iLoading = EFalse;
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::ConstructL
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::~CWmWidgetLoaderAo
// ---------------------------------------------------------------------------
//
CWmWidgetLoaderAo::~CWmWidgetLoaderAo()
    {
    // cancel ongoing operation
    Cancel();

    // cleanup and close session
    Cleanup();
    CloseSession();
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::StartLoading
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::StartLoading()
    {
    if ( IsActive() )
        {
        return; // already active                
        }

    // call itself
    iStatus = KRequestPending;
    TRequestStatus *pS = (&iStatus);
    User::RequestComplete( pS, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::RunL
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::RunL()
    {
    DoLoadWidgetsL();
    Cleanup();
    CloseSession();
    if ( iObserver )
        {
        iObserver->LoadDoneL( iWidgetListChanged );
        }
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::RunError
// ---------------------------------------------------------------------------
//
TInt CWmWidgetLoaderAo::RunError( TInt /*aError*/ )
    {
    Cleanup();
    CloseSession();
    return KErrNone;
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::DoCancel
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::DoCancel()
    {
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::DoLoadWidgetsL
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::DoLoadWidgetsL()
    {
    iWidgetListChanged = EFalse;
    // Check if unistallation is ongoing for for some widget
    // iUninstallUid is null no uninstallation is ongoing
    iUninstallUid = iWmPlugin.WmInstaller().UninstallUid();
    
    // connect to widget registry
    OpenSessionL();
    
    // 1. load the widgets array
    MHsContentController& controller = iWmPlugin.ContentController();    
    CHsContentInfoArray* contentInfoArray = CHsContentInfoArray::NewL();
    CleanupStack::PushL( contentInfoArray );
    controller.WidgetListL( *contentInfoArray );
    
    // 2. load the widget order
    if ( iWidgetOrder ) { Cleanup(); }    
    iWidgetOrder = CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
    TRAPD( loadError, iWidgetOrder->LoadL() );
    
    // 3. prepare the widget data array & sort order array
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); ++i )
        {
        iWidgetsList.WidgetData(i, ETrue ).SetPersistentWidgetOrder( iWidgetOrder );
        iWidgetsList.WidgetData(i, ETrue ).SetValid( EFalse );
        }
    for( TInt i = 0; i < iWidgetsList.OrderDataArray().Count(); ++i )
        {
        iWidgetsList.OrderData(i)->SetPersistentWidgetOrder( iWidgetOrder );
        }
    
    // 4. loop through the content array and compare it against the existing
    // widget data.
    TInt widgetsAdded = 0;
    TInt widgetsChanged = 0;
    iLoading = ETrue;
    while( contentInfoArray->Array().Count() > 0 )
        {
        CHsContentInfo* contentInfo = contentInfoArray->Array()[0];
        if ( !contentInfo ) { break; }
        contentInfoArray->Array().Remove( 0 );

        // check if this widget exists.
        // if it does, keep the existing one
        // if it does not, add it
        CWmWidgetData* existingData = FindWidgetData( *contentInfo );
        if ( existingData )
            {
            // update existing visible widget data
            existingData->SetValid( ETrue );
            if ( existingData->ReplaceContentInfo( contentInfo ) )
                {
                // Update name to order array if name changed
                for ( TInt i=0; i < iWidgetsList.OrderDataArray().Count(); i++ )
                    {
                    CWmWidgetOrderData* order = iWidgetsList.OrderData(i);
                    if ( order->EqualsTo( 
                            existingData->Uid(), existingData->PublisherId() ) )
                        {
                        order->UpdateName( existingData->Name() );
                        }
                    }
                ++widgetsChanged;
                }
            }
        else
            {
            // add a new widget data
            TRAP_IGNORE( AddWidgetDataL( contentInfo, widgetsAdded ); );
            }
        }

    // 5: finally, remove all UI widgets that during the loading process were
    // NOT marked as VALID. those widgets do not exist anymore.
    TInt widgetsRemoved = 0;
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); i++ )
        {
        if( !iWidgetsList.WidgetData(i, ETrue ).IsValid() )
            {
            iWidgetsList.RemoveWidgetData( i, ETrue );
            ++widgetsRemoved;
            }
        }
    
    // update listbox
    if ( widgetsAdded > 0 && !iWidgetsList.IsFindPaneIsVisible() )
        {
        iWidgetsList.HandleItemAdditionL();
        }
    
    if ( widgetsRemoved > 0 || widgetsAdded > 0 ||
        widgetsChanged > 0 )
        {
        if ( !iWidgetsList.IsFindPaneIsVisible() )
            {
            iWidgetsList.DrawDeferred();
            }
        iWidgetListChanged = ETrue;
        }
    // 6: cleanup
    CleanupStack::PopAndDestroy( contentInfoArray );

    // 7. check list empty condition
    if ( iWidgetsList.WidgetDataCount() == 0 )
        {
        TResourceReader rr;
        CEikonEnv::Static()->CreateResourceReaderLC(
                rr, R_AVKON_LISTBOX_DEFAULT_EMPTY_TEXT );
        TPtrC empty= rr.ReadTPtrC();
        iWidgetsList.View()->SetListEmptyTextL( empty );
        CleanupStack::PopAndDestroy();
        iWidgetsList.DrawDeferred();
        }
    
    // 8. store list order if necessary
    if ( loadError != KErrNone || widgetsAdded > 0 || widgetsRemoved > 0 )
        {
        TRAP_IGNORE( iWidgetOrder->StoreL( iWidgetsList.OrderDataArray() ); );
        }
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::FindWidgetDataL
// ---------------------------------------------------------
//
CWmWidgetData* CWmWidgetLoaderAo::FindWidgetData(
        CHsContentInfo& aContentInfo )
    {
    CWmWidgetData* data = NULL;
    for( TInt i=0; i<iWidgetsList.WidgetDataCount() && !data; ++i )
        {
        if ( iWidgetsList.WidgetData(i, ETrue ).EqualsTo( aContentInfo ) )
            {
            data = &iWidgetsList.WidgetData(i, ETrue );
            }
        }
    return data;
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::OpenSessionL
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::OpenSessionL()
    {
    if ( !iWidgetRegistry )
        {
        iWidgetRegistry = new (ELeave) RWidgetRegistryClientSession();
        TInt err = iWidgetRegistry->Connect();
        if ( KErrNone != err )
            {
            delete iWidgetRegistry;
            iWidgetRegistry = NULL;
            User::Leave( err );
            }
        }
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::CloseSession
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::CloseSession()
    {
    // disconnect widget registry
    if ( iWidgetRegistry )
        {
        iWidgetRegistry->Disconnect();
        iWidgetRegistry->Close();
        delete iWidgetRegistry;
        iWidgetRegistry = NULL;
        }
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::AddWidgetDataL
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::AddWidgetDataL(
        CHsContentInfo* aContentInfo, TInt& aCount )
    {
    CleanupStack::PushL( aContentInfo );
            
    // Becouse we show only widgets that can be added we need two arrays
    // to maintain order data and visible data. 
    // All widgets are added to listbox's iOrderDataArray and 
    // widgets that CanBeAdded are added also to iVisibleWidgetArray.
    CWmWidgetOrderData* order = CWmWidgetOrderData::NewLC(
            aContentInfo->PublisherId(), 
            UidFromString( aContentInfo->Uid() ), 
            aContentInfo->Name(),
            iWidgetOrder
            );
    
    // add to order data
    iWidgetsList.AddOrderDataL( order );
    CleanupStack::Pop( order );
    
    if ( aContentInfo->CanBeAdded() )
        {
        CWmWidgetData* widgetData = CWmWidgetData::NewL(
                iWidgetsList.LogoSize(),
                iWmPlugin.ResourceLoader(),
                aContentInfo, iWidgetRegistry );

        // widgetdata has taken ownership of contentinfo
        CleanupStack::Pop( aContentInfo );
        
        CleanupStack::PushL( widgetData );
        
        widgetData->SetPersistentWidgetOrder( iWidgetOrder );
        widgetData->SetValid( ETrue );
        
        // start uninstall animation if this widget
        // is currently been unistalled
        if ( iUninstallUid != KNullUid &&
            iUninstallUid == widgetData->PublisherUid() )
            {
            widgetData->VisualizeUninstallL();
            }
        // add to visible data
        iWidgetsList.AddWidgetDataL( widgetData, EFalse );
        CleanupStack::Pop( widgetData );
        aCount++;
        }
    else
        {
        //delete aContentInfo when it's not added to iVisibleWidgetArray
        CleanupStack::PopAndDestroy( aContentInfo );
        }
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::Cleanup
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::Cleanup()
    {
    iLoading = EFalse;
    
    // delete widget order and references to it
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); ++i )
        {
        iWidgetsList.WidgetData(i, ETrue ).SetPersistentWidgetOrder( NULL );
        }
    delete iWidgetOrder;
    iWidgetOrder = NULL;
    }

// ----------------------------------------------------
// CWmWidgetData::UidFromString
// ----------------------------------------------------
//
TUid CWmWidgetLoaderAo::UidFromString( const TDesC8& aUidString ) const
    {
    TUid uid( KNullUid );
    const TInt KHexPrefixLength = 2;
    if ( aUidString.Length() > KHexPrefixLength )
        {
        TUint id = 0;
        TLex8 lex( aUidString.Mid( KHexPrefixLength ) );
        if ( lex.Val( id, EHex ) == KErrNone )
            {
            uid.iUid = (TInt32)id;
            }
        }
    return uid;
    }

// ----------------------------------------------------
// CWmWidgetData::IsLoading
// ----------------------------------------------------
//
TBool CWmWidgetLoaderAo::IsLoading()
    {
    return iLoading;
    }

// ----------------------------------------------------
// CWmWidgetData::SetObserver
// ----------------------------------------------------
//
void CWmWidgetLoaderAo::SetObserver( MWmWidgetloaderObserver* aObserver )
    {
    iObserver = aObserver;
    }

// end of file

