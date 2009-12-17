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
#include <WidgetRegistryClient.h> // widget reqistry


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
    : CAsyncOneShot( EPriorityLow )
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
    // 1. mark all the existing widgets initially non-valid
    for( TInt i=0; i<iWidgetsList.WidgetDataCount(); ++i )
        iWidgetsList.WidgetData(i).SetValid( EFalse );

    // 2. load the widgets array
    MHsContentController& controller = iWmPlugin.ContentController();    
    CHsContentInfoArray* contentInfoArray = CHsContentInfoArray::NewL();
    CleanupStack::PushL( contentInfoArray );
    controller.WidgetListL( *contentInfoArray );
    
    // 3. load the widget order
    iWidgetOrder = CWmPersistentWidgetOrder::NewL( iWmPlugin.FileServer() );
    TRAPD( loadError, iWidgetOrder->LoadL() );

    // 4. loop through the content array and compare it against the existing
    // widget data.
    TInt widgetsAdded = 0;
    for ( TInt i=0; i < contentInfoArray->Array().Count(); i++ )
        {
        CHsContentInfo* contentInfo = contentInfoArray->Array()[i];

        // check if this widget exists.
        // if it does, keep the existing one
        // if it does not, add it
        CWmWidgetData* existingData = FindWidgetData( *contentInfo );
        if ( existingData )
            {
            // keep the existing one
            existingData->SetValid( ETrue );
            }
        else
            {
            // add a new one
            AddWidgetDataL( *contentInfo );
            ++widgetsAdded;
            }
        }

    // if widgets were added, extend the listbox
    if ( widgetsAdded )
        iWidgetsList.HandleItemAdditionL();
    
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

    // 6: cleanup
    CleanupStack::PopAndDestroy( contentInfoArray );

    // 7. redraw list
    if ( widgetsAdded > 0 || widgetsRemoved > 0 )
        {
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
        CHsContentInfo& aContentInfo )
    {
    if ( !iWidgetRegistry )
        {
        iWidgetRegistry = new (ELeave) RWidgetRegistryClientSession();
        User::LeaveIfError( iWidgetRegistry->Connect() );
        }
    
    CWmWidgetData* widgetData = CWmWidgetData::NewLC( 
            aContentInfo, iWidgetRegistry );
    widgetData->SetPersistentWidgetOrder( iWidgetOrder );
    widgetData->SetValid( ETrue );
    iWidgetsList.AddWidgetDataL( widgetData );
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
    
    // delete widget order
    delete iWidgetOrder;
    iWidgetOrder = NULL;
    }

// end of file

