/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
*/

#include "tspreviewproviderclient.h"
#include "previewproviderlogging.h"
#include "tspropertylistener.h"
#include "previewmsg.h"

#include <s32mem.h>

#define SZ( x ) ( sizeof( x ) / sizeof( TInt ) )


// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::CTsFastSwapPreviewProviderClient
// --------------------------------------------------------------------------
//
CTsFastSwapPreviewProviderClient::CTsFastSwapPreviewProviderClient(
        MTsFastSwapPreviewObserver& aObs ) : iObs( aObs )
    {
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::~CTsFastSwapPreviewProviderClient
// --------------------------------------------------------------------------
//
CTsFastSwapPreviewProviderClient::~CTsFastSwapPreviewProviderClient()
    {
    delete iListener;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::ConstructL
// --------------------------------------------------------------------------
//
void CTsFastSwapPreviewProviderClient::ConstructL()
    {
    TSLOG_CONTEXT( ConstructL, TSLOG_LOCAL );
    TSLOG_IN();

    const TUid id = { 0x20016BEC };
    BaseConstructL( id, KNullDesC8 );
    iListener = CTsPropertyListener::NewL( iObs );
    
    TSLOG_OUT();        
    }    

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::Register
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::Register( TInt aWgId )
    {
    TSLOG_CONTEXT( Register, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::ERegister,
        aWgId
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::Unregister
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::Unregister( TInt aWgId )
    {
    TSLOG_CONTEXT( Unregister, TSLOG_LOCAL );
    TSLOG_IN();

    const TInt msg[] = {
        NPreviewMsg::EUnregister,
        aWgId
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::ChangeScreen
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::ChangeScreen( TInt aScreenNo )
    {
    TSLOG_CONTEXT( ChangeScreen, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::EChangeScreen,
        aScreenNo
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    TSLOG_OUT();     
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::SetPreviewParam
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::SetPreviewParam(const TSize& aSize,
        TDisplayMode aMode)
    {
    TSLOG_CONTEXT( SetPreviewParam, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::ESetPreviewParam,
        aSize.iWidth,
        aSize.iHeight,
        static_cast<TInt>(aMode)
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::TakePreview 
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::TakePreview()
    {
    TSLOG_CONTEXT( TakePreview, TSLOG_LOCAL );
    TSLOG_IN();
    
    TPckgC<TInt> buf(NPreviewMsg::ETakePreview);
    SendMessage(buf);
    Flush();
    
    TSLOG_OUT();      
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::AckPreview 
// --------------------------------------------------------------------------
//
void CTsFastSwapPreviewProviderClient::AckPreview( TInt aPreviwId )
    {
    TSLOG_CONTEXT( TakePreview, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt msg[] = { NPreviewMsg::EAckPreviewReady,
                         aPreviwId };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::HandleMessage
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProviderClient::HandleMessage( const TDesC8& aData )
    {
    TRAP_IGNORE(DoHandleMessageL(aData));
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::DoHandleMessageL
// --------------------------------------------------------------------------
//
void CTsFastSwapPreviewProviderClient::DoHandleMessageL( const TDesC8& aData )
    {
    TSLOG_CONTEXT( DoHandleMessageL, TSLOG_LOCAL );
    TSLOG_IN();
    
    RDesReadStream in( aData );
    TInt message = in.ReadInt32L();
    switch ( message )
        {
        case NPreviewMsg::EPreviewReady:
            {
            const TInt id = in.ReadInt32L();
            const TInt handle = in.ReadInt32L();    
            SetProperties( handle, id, EOperationBitmapUpdated );
            }
            break;
        case NPreviewMsg::EUnregisterReady:
            {
            const TInt id = in.ReadInt32L();
            const TInt handle = in.ReadInt32L();
            SetProperties( handle, id, EOperationUnregister );
            }
            break;
        case NPreviewMsg::EBitmapRotationNeeded90:
            {
            const TInt id = in.ReadInt32L();
            const TInt handle = in.ReadInt32L();
            SetProperties( handle, id, EOperationBitmapRotation90 );
            }
            break;
        case NPreviewMsg::EBitmapRotationNeeded270:
            {
            const TInt id = in.ReadInt32L();
            const TInt handle = in.ReadInt32L();
            SetProperties( handle, id, EOperationBitmapRotation270 );
            }
            break;
        }

    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::SetProperties
// --------------------------------------------------------------------------
//
void CTsFastSwapPreviewProviderClient::SetProperties(TInt aHandle, TInt aWgId,
        TInt aOp)
    {
    // Notify CPropertyListener via P&S
    RProperty::Set( KPSUidPreviewProvider, KPSKeyFbsBitmapId, aHandle );
    RProperty::Set( KPSUidPreviewProvider, KPSKeyOperation, aOp );
    // KPSKeyWgId should be notified first since that is being listened 
    // by CPropertyListener 
    RProperty::Set( KPSUidPreviewProvider, KPSKeyWgId, aWgId );    
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProviderClient::OnReplace
// --------------------------------------------------------------------------
//
void CTsFastSwapPreviewProviderClient::OnReplace()
    {
    }

// End of file
