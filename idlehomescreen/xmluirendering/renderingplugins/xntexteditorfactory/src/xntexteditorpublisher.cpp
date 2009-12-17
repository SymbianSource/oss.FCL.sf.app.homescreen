/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CPS Publishing for text editor.
*
*/

#include "xntexteditorpublisher.h"

#include "xntexteditoradapter.h"
#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>
#include <eikedwin.h>
#include <eikedwin.h>
#include <utf.h> 


//#include <StringLoader.h> // StringLoader

_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCPInterface, "IDataSource" );

_LIT8( KType, "type" );
_LIT( KCpData, "cp_data" );

_LIT8( KPublisherId, "publisher" );
_LIT8( KContentTypeId, "content_type" );
_LIT8( KContentId, "content_id" );

_LIT(KAI3HSPublisher, "ai3homescreen");
_LIT(KContentType, "editorinput");

_LIT8( KData, "data");

_LIT8( KDataMap, "data_map" );

_LIT8( KItem, "item" );
_LIT8( KAdd, "Add" );
_LIT8( KDelete, "Delete" );

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
CXnTextEditorPublisher* CXnTextEditorPublisher::NewL( CXnTextEditorAdapter& aAdapter,
    const TDesC8& aNodeId )
    {
    CXnTextEditorPublisher* self = new (ELeave) CXnTextEditorPublisher( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//   
CXnTextEditorPublisher::~CXnTextEditorPublisher()
    {
    if( iNodeId )
        {
        TRAP_IGNORE( RemoveDataFromCpsL( KAI3HSPublisher, KContentType, iNodeId->Des() ) );
        }
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
    CEikEdwin* editor = iAdapter.Editor();
    if( editor )
        {
        editor->RemoveEdwinObserver( this );    
        }
    delete iNodeId;
    delete iTextBuffer;
    }

// -----------------------------------------------------------------------------
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnTextEditorPublisher::CXnTextEditorPublisher( CXnTextEditorAdapter& aAdapter ) : 
    iAdapter( aAdapter )
    {
    }
    
// -----------------------------------------------------------------------------
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
void CXnTextEditorPublisher::ConstructL( const TDesC8& aNodeId )
    {
    iNodeId = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aNodeId );
    InitCpsInterfaceL();
    CEikEdwin* editor = iAdapter.Editor();
    if( editor )
        {
        editor->SetEdwinObserver( this );  
        TInt len = editor->MaxLength();
        iTextBuffer = HBufC::NewL( len );
        }
    }

// -----------------------------------------------------------------------------
// Uses the CPS interface to add given data
// -----------------------------------------------------------------------------
//
void CXnTextEditorPublisher::AddDataToCpsL( const TDesC& aPublisherId, 
    const TDesC& aContentType, const TDesC& aContentId, 
    const TDesC8& aDataKey, const TDesC& aData )
    {
    if( iCpsInterface )
        {
        CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
        CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();
    
        TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
        cptype.PushL();
        
        inparam.AppendL( cptype );
        
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
        
        // Add data to data map
        map->InsertL( aDataKey,  TLiwVariant( aData ));
    
        // Create content data map
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
        cpdatamap->InsertL( KContentTypeId, TLiwVariant( aContentType )); 
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        cpdatamap->InsertL( KDataMap, TLiwVariant(map) );
                
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));     
        item.PushL(); 
           
        inparam.AppendL( item );
        
        iCpsInterface->ExecuteCmdL( KAdd, inparam, outparam);
    
        CleanupStack::PopAndDestroy( 4, &cptype ); // &item, map, cpdatamap,
    
        outparam.Reset();
        inparam.Reset();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CXnTextEditorPublisher::RemoveDataFromCpsL( const TDesC& aPublisherId, 
    const TDesC& aContentType, const TDesC& aContentId )
    {
    if( iCpsInterface && iServiceHandler )
        {   
        CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
        CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();

        TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
        cptype.PushL();
        inparam.AppendL( cptype );
        
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
        cpdatamap->InsertL( KContentTypeId, TLiwVariant( aContentType )); 
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));
        item.PushL(); 
        inparam.AppendL( item );
                
        iCpsInterface->ExecuteCmdL( KDelete, inparam, outparam );

        CleanupStack::PopAndDestroy( 3, &cptype ); // cpdatamap, &item
        outparam.Reset();
        inparam.Reset();
        }
    }

// -----------------------------------------------------------------------------
// Gets the CPS messaging interface
// -----------------------------------------------------------------------------
//
void CXnTextEditorPublisher::InitCpsInterfaceL()
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

// -----------------------------------------------------------------------------
// CXnTextEditorPublisher::HandleEdwinEventL
// -----------------------------------------------------------------------------
//
void CXnTextEditorPublisher::HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType)
    {
    if( aEventType == MEikEdwinObserver::EEventTextUpdate)
        {
        TPtr bufferDes = iTextBuffer->Des();
        bufferDes.Zero();
        aEdwin->GetText( bufferDes );
        PublishTextL( bufferDes );
        }         
    }

// -----------------------------------------------------------------------------
// CXnTextEditorPublisher::PublishTextL
// -----------------------------------------------------------------------------
//
void CXnTextEditorPublisher::PublishTextL( const TDesC& aText )
    {      
    AddDataToCpsL( KAI3HSPublisher, KContentType, iNodeId->Des(), KData, aText );    
    }

// END OF FILE

