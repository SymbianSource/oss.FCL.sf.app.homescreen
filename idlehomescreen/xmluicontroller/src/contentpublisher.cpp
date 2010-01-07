/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Publishing Service interface.
*
*/

#include "contentpublisher.h"
#include "globalqueryhandler.h"

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <AknQueryDialog.h>
#include <ai3xmlui.rsg>
#include "xmluicontroller.h"
#include <AknGlobalConfirmationQuery.h>
#include <StringLoader.h> // StringLoader

using namespace AiXmlUiController;

_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCPInterface, "IDataSource" );

_LIT8( KType, "type" );
_LIT( KCpData, "cp_data" );

_LIT8( KPublisherId, "publisher" );
_LIT8( KContentTypeId, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT8( KResults, "results" );

_LIT(KAI3HSPublisher,"ai3homescreen");
_LIT(KContentType,"wideimage");

_LIT8( KAPStatus, "apstatus");
_LIT(KNetwork, "network");

_LIT( KDisConnected, "disconnected");
_LIT( KConnected, "connected");

_LIT(KPublisher16, "publisher");


_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KDataMap, "data_map" );

_LIT8( KItem, "item" );
_LIT8( KAdd, "Add" );
_LIT8( KGetList, "GetList" );
_LIT8( KFilter,        "filter" );
_LIT8( KWidth, "width");
_LIT8( KHeight, "height");
_LIT8( KChangeInfo,    "change_info" );
// ============================ LOCAL FUNCTIONS ===============================


static void DoResetAndDestroy( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    reinterpret_cast< RCriteriaArray* >( aPtr )->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
// 
CContentPublisher* CContentPublisher::NewL(CXmlUiController& aUiController)
    {
    CContentPublisher* self = new (ELeave) CContentPublisher(aUiController);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//   
CContentPublisher::~CContentPublisher()
    {
    TRAP_IGNORE(UnRegisterListenerL());
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
    delete iGlobalQueryHandler;
    delete iGlobalConfirmationQuery;    
    }

// -----------------------------------------------------------------------------
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CContentPublisher::CContentPublisher(CXmlUiController& aUiController): iUiController( aUiController )
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
void CContentPublisher::ConstructL()
    {
    InitCpsInterfaceL();
    RegisterListenerL();
    iGlobalQueryHandler = CGlobalQueryHandler::NewL(iUiController);
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    
    }

// -----------------------------------------------------------------------------
// Publishes a given width and height to Content Publishing Service
// -----------------------------------------------------------------------------
//
void CContentPublisher::PublishSizeL( const TDesC& aElementName,
        TInt aWidth, TInt aHeight )
    {
    AddDataL( KAI3HSPublisher() ,KContentType(), aElementName,
            aWidth, KWidth, aHeight, KHeight );
    }

// -----------------------------------------------------------------------------
// Uses the CPS interface to add given data
// -----------------------------------------------------------------------------
//
void CContentPublisher::AddDataL( 
        const TDesC& aPublisherId, const TDesC& aContentType, const TDesC& aContentId,
        const TInt aData1, const TDesC8& aDataKey1,
        const TInt aData2, const TDesC8& aDataKey2 )
    {
    CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();

    TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
    cptype.PushL();
    
    inparam.AppendL( cptype );
    
    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
    
    // Add data to data map
    map->InsertL( aDataKey1,  TLiwVariant( aData1 ));
    map->InsertL( aDataKey2,  TLiwVariant( aData2 ));

    // Create content data map
    cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
    cpdatamap->InsertL( KContentTypeId, TLiwVariant( aContentType )); 
    cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
    cpdatamap->InsertL( KDataMap, TLiwVariant(map) );
            
    TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));     
    item.PushL(); 
       
    inparam.AppendL( item );
    
    iCpsInterface->ExecuteCmdL( KAdd , inparam, outparam);

    CleanupStack::PopAndDestroy( &item );
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( cpdatamap );
    CleanupStack::PopAndDestroy( &cptype );

    outparam.Reset();
    inparam.Reset();
    }

// -----------------------------------------------------------------------------
// Uses the CPS interface to get data
// -----------------------------------------------------------------------------
//
void CContentPublisher::GetDataL( 
        const TDesC& aPublisherId, const TDesC& aContentType, const TDesC& aContentId,
        CLiwDefaultMap* aOutDataMap)
    {
    CLiwGenericParamList& inParamList  = iServiceHandler->InParamListL();
	CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
	
	TLiwGenericParam type( KType, TLiwVariant( KCpData ) );
	inParamList.AppendL( type );
	
	CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();

	// Create content data map
	cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
	cpdatamap->InsertL( KContentTypeId, TLiwVariant( aContentType )); 
	cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
	  
	//append filter to input param
	TLiwGenericParam item( KFilter, TLiwVariant( cpdatamap ));
	inParamList.AppendL( item );
    CleanupStack::PopAndDestroy( cpdatamap );
	 
    iCpsInterface->ExecuteCmdL( KGetList, inParamList, outParamList);

    type.Reset();
	item.Reset();
	inParamList.Reset();
	
	//extracts data map
	TInt pos = 0;
	outParamList.FindFirst( pos, KResults );
	if( pos != KErrNotFound )
		// results present
		{
		//extract iterator on results list
		TLiwVariant variant = (outParamList)[pos].Value();
		variant.PushL();
		CLiwIterable* iterable = variant.AsIterable();
		iterable->Reset();
		//get next result
		if( iterable->NextL( variant ) )
			{
			//extract content map
			CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
			variant.Get( *map );
			if( map->FindL( KDataMap, variant) )
				{
				 variant.Get( *aOutDataMap );
				}
			CleanupStack::PopAndDestroy( map );
			}
		iterable->Reset();
		CleanupStack::PopAndDestroy( &variant );
		variant.Reset();
		}
	outParamList.Reset();
    	
    }

// -----------------------------------------------------------------------------
// Gets the CPS messaging interface
// -----------------------------------------------------------------------------
//
void CContentPublisher::InitCpsInterfaceL()
    {
    if( !iServiceHandler )
        {
        iServiceHandler = CLiwServiceHandler::NewL();
        }
    
    CLiwGenericParamList& inParam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParam = iServiceHandler->OutParamListL();
    
    RCriteriaArray a;
    TCleanupItem item( DoResetAndDestroy, &a );
    CleanupStack::PushL( item );
    
    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewLC( KLiwCmdAsStr, KCPInterface,
        KCPService  );
    crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
    
    a.AppendL(crit);    
    
    CleanupStack::Pop( crit );
    
    iServiceHandler->AttachL(a);
    iServiceHandler->ExecuteServiceCmdL( *crit, inParam, outParam ); 
    
    CleanupStack::PopAndDestroy(); // item;
        
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

void CContentPublisher::RegisterListenerL()
	{
	CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
	CLiwGenericParamList& outparam =  iServiceHandler->OutParamListL();
	CLiwDefaultMap*  reqFilter = CLiwDefaultMap::NewLC();
	
	// Listen all the information(contentId) from publisher 
	reqFilter->InsertL( KPublisherId, TLiwVariant( KAI3HSPublisher ));
	reqFilter->InsertL( KContentTypeId, TLiwVariant(KPublisher16 ));
	
	// Type of the registry  (content registry)
	inparam.AppendL(TLiwGenericParam(KType, TLiwVariant(KCpData)));
	// fill in input list for RequestNotification command
	inparam.AppendL(TLiwGenericParam(KFilter, TLiwVariant(reqFilter)));
   
	TRAP_IGNORE( iCpsInterface->ExecuteCmdL( 
			KRequestNotification,
				inparam,
				outparam,
				0,
				this ) );
	
	outparam.Reset();
	inparam.Reset();
	CleanupStack::PopAndDestroy(reqFilter );  
	}

void CContentPublisher::UnRegisterListenerL()
	{
	CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
	CLiwGenericParamList& outparam =  iServiceHandler->OutParamListL();
 
	// Unregister the reuqest notification 
	TRAP_IGNORE( iCpsInterface->ExecuteCmdL( 
			KRequestNotification,
				inparam,
				outparam,
				KLiwOptCancel,
				this ) ); 
	
	outparam.Reset();
	inparam.Reset();
	}

TInt CContentPublisher::HandleNotifyL(
        TInt aErrorCode,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/ )
   {
   TInt pos( 0 );
   aEventParamList.FindFirst(pos, KChangeInfo);
   if (pos != KErrNotFound)
    {
    // Get list of maps
    TLiwVariant variant = (aEventParamList)[pos].Value();
    variant.PushL();
    const CLiwList* changeMapsList = variant.AsList();
    // Iter through list content
    for (TInt i = 0; i < changeMapsList->Count() ; ++i)
	    {
 		if ( changeMapsList->AtL(i, variant) ) 
 			{
			const CLiwMap* map  = variant.AsMap();
			if( map->FindL( KPublisherId, variant) )
				 {
				 HBufC* publisher = variant.AsDes().AllocLC();
				 if ( publisher && ( publisher->Des() == KAI3HSPublisher ) 
					  && map->FindL( KContentTypeId, variant) )
					{
					 HBufC* contentType = variant.AsDes().AllocLC();
					 if ( contentType && ( contentType->Des() ==  KPublisher16 )
						  && map->FindL( KContentId, variant ) )
						 {
						 HBufC* contentId = variant.AsDes().AllocLC();
						 if ( contentId )
							 {
							 HandlePublisherInfoL( *contentId );
							 }
						 CleanupStack::PopAndDestroy( contentId );
						 }
					 CleanupStack::PopAndDestroy( contentType );
					}
			  CleanupStack::PopAndDestroy( publisher );
				 }
			}
	   }
    CleanupStack::PopAndDestroy(&variant); 
    }
   return aErrorCode;
   }

void CContentPublisher::HandlePublisherInfoL( const TDesC& aPubInfo)
	{
	//Currently HS will handle only network status update form publisher
	if ( aPubInfo == KNetwork )
		{
		CLiwDefaultMap *outDataMap = CLiwDefaultMap::NewLC();
		GetDataL( KAI3HSPublisher(), KPublisher16, KNetwork(), outDataMap );
		TLiwVariant variant;
		variant.PushL();
		if ( outDataMap->FindL(KAPStatus, variant))
			{
			HBufC16* netStatus = variant.AsDes().AllocLC();
            if ( netStatus->Des() == KDisConnected() )
                {
                ShowGlobalQueryL(R_QTN_HS_DISABLE_NETWORK, EFalse );
                }
            else if ( netStatus->Des() == KConnected() )
                {
                ShowGlobalQueryL(R_QTN_HS_SWITCH_ONLINE, ETrue );
                }
			CleanupStack::PopAndDestroy( netStatus );
			}
		CleanupStack::PopAndDestroy( &variant );
		CleanupStack::PopAndDestroy(outDataMap);
		}
    }


void CContentPublisher::ShowGlobalQueryL( TInt aResourceId, TBool aSetOnline )
    {
    if ( !iGlobalQueryHandler->IsActive() )
        {
        HBufC* confirmationText = StringLoader::LoadLC(aResourceId);
        iGlobalConfirmationQuery->ShowConfirmationQueryL(
                iGlobalQueryHandler->iStatus,
            *confirmationText, 
            R_AVKON_SOFTKEYS_YES_NO);
        iGlobalQueryHandler->SetOnlineParamAndActivate(aSetOnline);
        CleanupStack::PopAndDestroy(); //confirmationText
        }
    }
// END OF FILE
