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
    : CAsyncOneShot( EPriorityStandard )
    , iWmPlugin( aWmPlugin )
    , iWidgetsList( aTargetList )
    {
    iWidgetRegistry = NULL;
    iWidgetOrder = NULL;
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::ConstructL
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::~CWmWidgetLoaderAo
// ---------------------------------------------------------------------------
//
CWmWidgetLoaderAo::~CWmWidgetLoaderAo()
    {
    // cancel ongoing operation
    Cancel();

    // cleanup run data
    Cleanup();
    
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::StartLoading
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::StartLoading()
    {
    if ( IsActive() )
        {
        // cancel ongoing process
        Cancel();
        }
    Call();
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::RunL
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::RunL()
    {
    DoLoadWidgetsL();
    Cleanup();
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::RunError
// ---------------------------------------------------------------------------
//
TInt CWmWidgetLoaderAo::RunError( TInt /*aError*/ )
    {
    Cleanup();
    return KErrNone;
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::DoLoadWidgetsL
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::DoLoadWidgetsL()
    {
    // 1. load the widgets array
    MHsContentController& controller = iWmPlugin.ContentController();    
    CHsContentInfoArray* contentInfoArray = CHsContentInfoArray::NewL();
    CleanupStack::PushL( contentInfoArray );
    controller.WidgetListL( *contentInfoArray );
    
    // 2. load the widget order
    iWidgetOrder = CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
    TRAPD( loadError, iWidgetOrder->LoadL() );

    // 3. prepare the widget data list
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); ++i )
        {
        iWidgetsList.WidgetData(i).SetPersistentWidgetOrder( iWidgetOrder );
        iWidgetsList.WidgetData(i).SetValid( EFalse );
        }

    // 4. loop through the content array and compare it against the existing
    // widget data.
    TInt widgetsAdded = 0;
    TInt widgetsChanged = 0;
    while( contentInfoArray->Array().Count() > 0 )
        {
        CHsContentInfo* contentInfo = contentInfoArray->Array()[0];
        contentInfoArray->Array().Remove( 0 );

        // check if this widget exists.
        // if it does, keep the existing one
        // if it does not, add it
        CWmWidgetData* existingData = FindWidgetData( *contentInfo );
        if ( existingData )
            {
            // update existing widget data
            existingData->SetValid( ETrue );
            if ( existingData->ReplaceContentInfoL( contentInfo ) )
                {
                ++widgetsChanged;
                }
            }
        else
            {
            // add a new widget data
            AddWidgetDataL( contentInfo );
            ++widgetsAdded;
            }
        }

    // 5: finally, remove all UI widgets that during the loading process were
    // NOT marked as VALID. those widgets do not exist anymore.
    TInt widgetsRemoved = 0;
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); i++ )
        {
        if( !iWidgetsList.WidgetData(i).IsValid() )
            {
            iWidgetsList.RemoveWidgetData( i );
            ++widgetsRemoved;
            }
        }
    
    // update listbox
    if ( widgetsAdded > 0 )
        {
        iWidgetsList.HandleItemAdditionL();
        }
    
    if ( widgetsRemoved > 0 || widgetsAdded > 0 ||
        widgetsChanged > 0 )
        {
        iWidgetsList.DrawDeferred();
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
        iWidgetOrder->StoreL( iWidgetsList.WidgetDataArray() );
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
        if ( iWidgetsList.WidgetData(i).EqualsTo( aContentInfo ) )
            {
            data = &iWidgetsList.WidgetData(i);
            }
        }
    return data;
    }

// ---------------------------------------------------------
// CWmWidgetLoaderAo::AddWidgetDataL
// ---------------------------------------------------------
//
void CWmWidgetLoaderAo::AddWidgetDataL(
        CHsContentInfo* aContentInfo )
    {
    CleanupStack::PushL( aContentInfo );
    if ( !iWidgetRegistry )
        {
        iWidgetRegistry = new (ELeave) RWidgetRegistryClientSession();
        User::LeaveIfError( iWidgetRegistry->Connect() );
        }
    CleanupStack::Pop( aContentInfo );
    
    CWmWidgetData* widgetData = CWmWidgetData::NewLC( 
            iWidgetsList.LogoSize(),
            iWmPlugin.ResourceLoader(),
            aContentInfo, iWidgetRegistry );
    widgetData->SetPersistentWidgetOrder( iWidgetOrder );
    widgetData->SetValid( ETrue );
    iWidgetsList.AddWidgetDataL( widgetData, EFalse );
    CleanupStack::Pop( widgetData );
    }

// ---------------------------------------------------------------------------
// CWmWidgetLoaderAo::Cleanup
// ---------------------------------------------------------------------------
//
void CWmWidgetLoaderAo::Cleanup()
    {
    // disconnect widget registry
    if ( iWidgetRegistry )
        {
        iWidgetRegistry->Disconnect();
        iWidgetRegistry->Close();
        delete iWidgetRegistry;
        iWidgetRegistry = NULL;
        }
    
    // delete widget order and references to it
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); ++i )
        {
        iWidgetsList.WidgetData(i).SetPersistentWidgetOrder( NULL );
        }
    delete iWidgetOrder;
    iWidgetOrder = NULL;
    }

// end of file

