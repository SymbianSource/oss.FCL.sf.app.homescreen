/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Internal class definition for WidgetUI application.
*
*/


#include "tscpsnotifier.h"
#include "tsfswengine.h"
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <liwcommon.h>
#include <liwcommon.hrh>

_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCPInterface, "IDataSource" );

_LIT8( KType, "type" );
_LIT( KCpData, "cp_data" );

_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );

_LIT8( KGetList, "GetList");
_LIT8( KDataMap, "data_map" );
_LIT8( KWidgetId, "widget_id");
_LIT8( KBitmapHandle, "bitmap_handle");

_LIT( KWidgetUi, "widget_ui");
_LIT( KScreenshot, "screenshot");

_LIT8( KFilter, "filter" );
_LIT8( KResults,      "results");
_LIT8( KOperation, "operation" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT( KUpdate, "update" );
_LIT( K16Add, "add" );
_LIT( KAddUpdate, "add:update" );
_LIT8( KChangeInfo, "change_info" );

_LIT( KAll, "all");

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTSCpsNotifier::NewL
// --------------------------------------------------------------------------
//
CTSCpsNotifier* CTSCpsNotifier::NewL(CTsFswEngine& aEngine )
    {
    CTSCpsNotifier* self = new (ELeave) CTSCpsNotifier(aEngine);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::~CTSCpsNotifier
// --------------------------------------------------------------------------
//
CTSCpsNotifier::~CTSCpsNotifier()
    {
    TRAP_IGNORE(ExecuteRegistrationCommandL( KLiwOptCancel ));
    
    if( iCpsInterface )
        {
        iCpsInterface->Close();
        iCpsInterface = NULL;
        }
    if( iServiceHandler )
        {
        iServiceHandler->Reset();
        delete iServiceHandler;
        iServiceHandler = NULL;
        }
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::CTSCpsNotifier
// --------------------------------------------------------------------------
//
CTSCpsNotifier::CTSCpsNotifier(CTsFswEngine& aEngine):
    iEngine(aEngine)
    {
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::ConstructL
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::ConstructL()
    {
    InitCpsInterfaceL();
    ExecuteRegistrationCommandL( 0 );
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::WidgetFilterLC
// --------------------------------------------------------------------------
//
CLiwDefaultMap* CTSCpsNotifier::WidgetFilterLC()
    {
    //Create filter criteria for requested entries in form of LIW map:
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant(  KWidgetUi ));
    filter->InsertL( KContentType, TLiwVariant( KScreenshot ));
    return filter;
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::ExecuteRegistrationCommandL
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::ExecuteRegistrationCommandL( 
    TUint aOption )
    {
    CLiwDefaultMap* filter = WidgetFilterLC();
    filter->InsertL( KContentId, TLiwVariant( KAll ));
    filter->InsertL( KOperation, TLiwVariant( KAddUpdate ));
    
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
        
    // Fill in input list for RequestNotification command
    inParamList.AppendL( TLiwGenericParam( KType, TLiwVariant( KCpData )));
    inParamList.AppendL( TLiwGenericParam( KFilter, TLiwVariant( filter )));
        
    iCpsInterface->ExecuteCmdL( 
                KRequestNotification,
                inParamList,
                outParamList,
                aOption,
                this );
    
    outParamList.Reset();
    inParamList.Reset(); 
    CleanupStack::PopAndDestroy( filter );
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::HandleNotifyL
// --------------------------------------------------------------------------
//
TInt CTSCpsNotifier::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    TInt pos(0);
    aEventParamList.FindFirst( pos, KChangeInfo );
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = ( aEventParamList )[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();

        TPtrC operation;
        // Iter through list content
        for (TInt i = 0; i < changeMapsList->Count(); ++i)
            {
             if ( changeMapsList->AtL( i, variant )) 
                 {
                 const CLiwMap* changeMap  = variant.AsMap();
                 // Check what triggered a notification
                 if ( changeMap->FindL( KOperation, variant ))
                     {
                     operation.Set( variant.AsDes());
                     }
                 if (!operation.Compare(KUpdate) || !operation.Compare(K16Add))
                     {
                     HandleWidgetUpdateL( *changeMap );
                     }
                 }
            }
        CleanupStack::PopAndDestroy( &variant );
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::InitCpsInterfaceL
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::InitCpsInterfaceL()
    {
    if( !iServiceHandler )
        {
        iServiceHandler = CLiwServiceHandler::NewL();
        }
    
    CLiwGenericParamList& inParam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParam = iServiceHandler->OutParamListL();
    
    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewLC( KLiwCmdAsStr, KCPInterface,
        KCPService  );
    crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
    
    RCriteriaArray a;
    a.AppendL(crit);    
    iServiceHandler->AttachL( a );
	CleanupStack::Pop(crit);
    iServiceHandler->ExecuteServiceCmdL( *crit, inParam, outParam ); 
    delete crit;
    a.Reset();
        
    // find service interface
    TInt pos = 0;
    MLiwInterface* msgInterface = NULL;
    outParam.FindFirst( pos, KCPInterface );
    if ( pos != KErrNotFound ) 
        {
        msgInterface = (outParam)[pos].Value().AsInterface(); 
        }
    
    outParam.Reset();
    inParam.Reset();    
    iCpsInterface = msgInterface;
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::HandleWidgetUpdateL
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::HandleWidgetUpdateL(const CLiwMap& aChangeMap)
    {
    //ectract content_id which is widget bundled name
    TPtrC contentid;
    TLiwVariant contentIdVar;
    contentIdVar.PushL();
    if (aChangeMap.FindL(KContentId, contentIdVar))
        {
        contentid.Set( contentIdVar.AsDes());
        }
    
    TInt widgetId(0);
    TInt bitmapHandle(0);
    GetWidgetDataL(contentid, widgetId, bitmapHandle);
    iEngine.HandleWidgetUpdateL(widgetId, bitmapHandle );
    CleanupStack::PopAndDestroy( &contentIdVar );
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::GetWidgetData
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::GetWidgetDataL(const TDesC& aContentId, TInt& aWidgetId,
        TInt& aBitmapHandle)
    {
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();

    TLiwGenericParam type( KType, TLiwVariant( KCpData ) );
    inParamList->AppendL( type );
    
    //append filter to input param
    CLiwDefaultMap* filter = WidgetFilterLC();
    filter->InsertL(KContentId, TLiwVariant(aContentId));
    TLiwGenericParam item( KFilter, filter);
    inParamList->AppendL( item );
     
    // It is assumed that interface is already initiated
    iCpsInterface->ExecuteCmdL( KGetList, *inParamList, *outParamList);

    type.Reset();
    item.Reset();
    inParamList->Reset();
    
    //extracts data map
    TInt pos = 0;
    outParamList->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        // results present
        {
        //extract iterator on results list
        TLiwVariant variant = (*outParamList)[pos].Value();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        
        //get next result
        if( iterable->NextL( variant ) )
            {
            //extract content map
            CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
            variant.Get( *map );
            if( map && map->FindL( KDataMap, variant) )
                {
                CLiwDefaultMap *dataMap = CLiwDefaultMap::NewLC();
                variant.Get( *dataMap );
                if( dataMap )
                    {
                    GetWidgetDataFromDataMapL(*dataMap, aWidgetId, aBitmapHandle);
                    }
                CleanupStack::PopAndDestroy( dataMap );
                }
            variant.Reset();
            CleanupStack::PopAndDestroy( map );
            }
        iterable->Reset();
        }
    outParamList->Reset();
    CleanupStack::PopAndDestroy( filter );
    }

// --------------------------------------------------------------------------
// CTSCpsNotifier::GetWidgetDataFromDataMap
// --------------------------------------------------------------------------
//
void CTSCpsNotifier::GetWidgetDataFromDataMapL(const CLiwMap& aDataMap, 
        TInt& aWidgetId, TInt& aBitmapHandle)
    {
    TLiwVariant bitmapHandleVar;
    bitmapHandleVar.PushL();
    if( aDataMap.FindL( KBitmapHandle, bitmapHandleVar ))
        {
        aBitmapHandle = bitmapHandleVar.AsTInt32();
        }
    CleanupStack::PopAndDestroy(&bitmapHandleVar);

    TLiwVariant widgetIdVar;
    widgetIdVar.PushL();
    if( aDataMap.FindL( KWidgetId, widgetIdVar ))
        {
        aWidgetId = widgetIdVar.AsTInt32();
        }
    CleanupStack::PopAndDestroy(&widgetIdVar);
    }

// END OF FILE


    

