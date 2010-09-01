/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <xmlengelement.h> 
#include <xmlengdom.h>

#include "hnmdbasekey.h"
#include "hnmdmodel.h"
#include "hnservicehandler.h"
#include "hnmdservicecommand.h"
#include "hnitemid.h"
#include "hnmdnotifyrequest.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CHnMdNotifyRequest* CHnMdNotifyRequest::NewL( TXmlEngElement aElement, 
        const CHnItemId & aItemId, THnMdCommonPointers* aCmnPtrs )
        
    {
    CHnMdNotifyRequest* self = CHnMdNotifyRequest::NewLC(aElement,
            aItemId, aCmnPtrs );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequest* CHnMdNotifyRequest::NewLC( TXmlEngElement aElement,
        const CHnItemId & aItemId, THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdNotifyRequest* self = new (ELeave) CHnMdNotifyRequest( aCmnPtrs );
    CleanupStack::PushL(self);
    self->ConstructL( aElement, aItemId );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequest::~CHnMdNotifyRequest()
    {
    delete iItemId;
    delete iService;
    delete iSh;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdNotifyRequest::ConstructL( TXmlEngElement aElement, 
        const CHnItemId & aItemId )
    {
    ASSERT(aElement.Name() == KNotifyRequestElementName8 );

    iItemId = CHnItemId::NewL( aItemId.SuiteName(), aItemId.Id() );
    iService = CHnMdService::NewL( aElement );
    iSh = NULL;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequest::CHnMdNotifyRequest(THnMdCommonPointers* aCmnPtrs) 
    : iCmnPtrs(aCmnPtrs)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdNotifyRequest::RequestNotifyL( CLiwGenericParamList& aParams, 
        TInt aCmdOptions )
    {
    if ( iSh == NULL )
        {
        CLiwGenericParamList* constructor =
            iService->EvaluateConstructorL( aParams );
        CleanupStack::PushL(constructor);
        CLiwGenericParamList* command = iService->EvaluateCommandL( aParams );
        CleanupStack::PushL( command );
    
        // Create service
        iSh = CHnServiceHandler::NewL( 
                iService->GetServiceName(), 
                iService->GetInterfaceName(), 
                iService->GetCommand().GetCommandName(),
                EServiceModeAsynchronous,
                constructor, 
                command );
    
        CleanupStack::Pop( command );
        CleanupStack::Pop( constructor );
        }
    iSh->ExecuteL( this, aCmdOptions ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
TInt CHnMdNotifyRequest::HandleNotifyL( TInt /* aCmdId */, TInt aEventId ,
        CLiwGenericParamList& /* aEventParamList */,
        const CLiwGenericParamList& /* aInParamList */)
    {
    TInt err( KErrNone );
    
    if ( aEventId == KLiwEventInProgress )
        {
        //Filling in variant structure.
        CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
        CleanupStack::PushL( inParam );
           
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
            
        map->InsertL( KGenreName8, TLiwVariant(iItemId->SuiteName()) );
        map->InsertL( KItemName8, TLiwVariant(iItemId->Id()) );
        
        CLiwDefaultList* list = CLiwDefaultList::NewLC();   
        
        list->AppendL( TLiwVariant( map ) );
        inParam->AppendL(TLiwGenericParam( KFilterElementItem8, TLiwVariant(list)));
        CleanupStack::PopAndDestroy( list );
        CleanupStack::PopAndDestroy( map );
        
        iCmnPtrs->iModelEventObserver->HandleModelEventL( KReevaluateMdEvent, *inParam );
    
        CleanupStack::PopAndDestroy( inParam );
        }
    
    return err;
    }

