/*
* ==============================================================================
*  Name        : %name: previewprovider.cpp %
*  Part of     : Hg fast swap preview provider plugin
*  Description : Preview provider API
*  Version     : %version: sa1spcx1#3 %
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#include "hgpreviewprovider.h"
#include "hgpreviewobserver.h"
#include "previewmsg.h"
#include "previewproviderlogging.h"
#include <w32std.h>
#include <s32mem.h>
#include <e32property.h>

#define SZ( x ) ( sizeof( x ) / sizeof( TInt ) )

// CONSTANTS

// Publish & subscribe UID (akncapserver.exe SID so that it can define the keys)
const TUid KPSUidPreviewProvider = { 0x10207218 };

// P&S keys
const TUint32 KPSKeyWgId = 0x20016BE1;
const TUint32 KPSKeyFbsBitmapId = 0x20016BE2;
const TUint32 KPSKeyOperation = 0x20016BE3;

/**
 * Operation which will be done after P&S keys have changed.
 */
enum TPreviewOperation
    {
    EOperationNone,
    EOperationUnregister,
    EOperationBitmapUpdated
    };

/**
 * CPropertyListener
 */
NONSHARABLE_CLASS( CPropertyListener ) : public CActive
    {
public:
    static CPropertyListener* NewL( MHgFastSwapPreviewObserver& aObs );
    virtual ~CPropertyListener();
    
protected:
    CPropertyListener( MHgFastSwapPreviewObserver& aObs );
    void ConstructL();  

private: // From CActive
    void RunL();
    void DoCancel();
    
private:    
    void ReadPropertiesL();
    void DefinePropertyL( TInt aPSKey ); 
    void Subscribe();

private:
    MHgFastSwapPreviewObserver& iObs;
    RProperty iProperty;
   
    TInt iWgId;
    TInt iFbsBitmapId;
    
    TPreviewOperation iOperation;
    };

// --------------------------------------------------------------------------
// CPropertyListener::~CPropertyListener
// --------------------------------------------------------------------------
//
CPropertyListener::~CPropertyListener()
    {
    Cancel();
    iProperty.Close();
    }

// --------------------------------------------------------------------------
// CPropertyListener::CPropertyListener
// --------------------------------------------------------------------------
//
CPropertyListener::CPropertyListener( MHgFastSwapPreviewObserver& aObs ) : 
    CActive( CActive::EPriorityHigh ), iObs( aObs )
    {
    }

// --------------------------------------------------------------------------
// CPropertyListener::NewL
// --------------------------------------------------------------------------
//
CPropertyListener* CPropertyListener::NewL( MHgFastSwapPreviewObserver& aObs )
    {
    CPropertyListener* self = new ( ELeave ) CPropertyListener( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------
// CPropertyListener::RunL
// --------------------------------------------------------------------------
//
void CPropertyListener::RunL()
    {
    // Resubscribe before processing new value to prevent missing updates
    Subscribe();
    
    // Read all properties to member variables
    ReadPropertiesL();
    
    // Do the callback
    switch( iOperation )
        {
        case EOperationUnregister:
            {
            iObs.HandleFswPpApplicationUnregistered( iWgId );
            break;            
            }        
        case EOperationBitmapUpdated:
            {
            iObs.HandleFswPpApplicationChange( iWgId, iFbsBitmapId );
            break;
            }
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CPropertyListener::DoCancel
// --------------------------------------------------------------------------
//
void CPropertyListener::DoCancel()
    {
    iProperty.Cancel();
    }

// --------------------------------------------------------------------------
// CPropertyListener::ReadProperties
// --------------------------------------------------------------------------
//
void CPropertyListener::ReadPropertiesL()
    {
    // Read all properties
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyWgId, iWgId ) );
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyFbsBitmapId, iFbsBitmapId ) );
    TInt operation( 0 );
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyOperation, operation ) );
    iOperation = static_cast<TPreviewOperation>( operation );    
    }

// --------------------------------------------------------------------------
// CPropertyListener::DefinePropertyL
// --------------------------------------------------------------------------
//
void CPropertyListener::DefinePropertyL( TInt aPSKey )
    {
    TInt err = RProperty::Define( KPSUidPreviewProvider, aPSKey, RProperty::EInt );
    // Don't mind the already exists error
    if ( err != KErrAlreadyExists && err != KErrPermissionDenied )
        {
        User::LeaveIfError( err );
        }    
    }

// --------------------------------------------------------------------------
// CPropertyListener::Subscribe
// --------------------------------------------------------------------------
//
void CPropertyListener::Subscribe()
    {
    iProperty.Subscribe( iStatus );
    SetActive();  
    }

// --------------------------------------------------------------------------
// CPropertyListener::ConstructL
// --------------------------------------------------------------------------
//
void CPropertyListener::ConstructL()
    {
    HGLOG_CONTEXT( ConstructL, HGLOG_LOCAL );
    HGLOG_IN();
    
    DefinePropertyL( KPSKeyWgId );
    DefinePropertyL( KPSKeyFbsBitmapId );
    DefinePropertyL( KPSKeyOperation );
    
    User::LeaveIfError( iProperty.Attach( KPSUidPreviewProvider, KPSKeyWgId ) );
    CActiveScheduler::Add( this );
    
    // initial subscription
    Subscribe();
    
    HGLOG_OUT();    
    }

/**
 * CHgFastSwapPreviewProviderClient
 */
NONSHARABLE_CLASS( CHgFastSwapPreviewProviderClient ) : public CWsGraphic
    {
public:
    CHgFastSwapPreviewProviderClient( MHgFastSwapPreviewObserver& aObs );
    void ConstructL();
    void Register( TInt aWgId );
    void Unregister( TInt aWgId );
    void ChangeScreen( TInt aScreenNo );
    void SetPreviewParam( const TSize& aSize, TDisplayMode aMode );
    void TakePreview();
    void AckPreview( TInt aPreviewId );
    virtual ~CHgFastSwapPreviewProviderClient();
private:
    void HandleMessage( const TDesC8& aData );
    void OnReplace();
private:
    void DoHandleMessageL( const TDesC8& aData );
    void SetProperties( TInt aHandle, TInt aWgId, TInt aOp );    
private:
    MHgFastSwapPreviewObserver& iObs;
    CPropertyListener* iListener;
    };


// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::CHgFastSwapPreviewProviderClient
// --------------------------------------------------------------------------
//
CHgFastSwapPreviewProviderClient::CHgFastSwapPreviewProviderClient(
        MHgFastSwapPreviewObserver& aObs ) : iObs( aObs )
    {
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::~CHgFastSwapPreviewProviderClient
// --------------------------------------------------------------------------
//
CHgFastSwapPreviewProviderClient::~CHgFastSwapPreviewProviderClient()
    {
    delete iListener;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::ConstructL
// --------------------------------------------------------------------------
//
void CHgFastSwapPreviewProviderClient::ConstructL()
    {
    HGLOG_CONTEXT( ConstructL, HGLOG_LOCAL );
    HGLOG_IN();

    const TUid id = { 0x20016BEC };
    BaseConstructL( id, KNullDesC8 );
    iListener = CPropertyListener::NewL( iObs );
    
    HGLOG_OUT();        
    }    

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::Register
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::Register( TInt aWgId )
    {
    HGLOG_CONTEXT( Register, HGLOG_LOCAL );
    HGLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::ERegister,
        aWgId
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::Unregister
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::Unregister( TInt aWgId )
    {
    HGLOG_CONTEXT( Unregister, HGLOG_LOCAL );
    HGLOG_IN();

    const TInt msg[] = {
        NPreviewMsg::EUnregister,
        aWgId
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::ChangeScreen
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::ChangeScreen( TInt aScreenNo )
    {
    HGLOG_CONTEXT( ChangeScreen, HGLOG_LOCAL );
    HGLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::EChangeScreen,
        aScreenNo
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    HGLOG_OUT();     
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::SetPreviewParam
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::SetPreviewParam(const TSize& aSize,
        TDisplayMode aMode)
    {
    HGLOG_CONTEXT( SetPreviewParam, HGLOG_LOCAL );
    HGLOG_IN();
    
    const TInt msg[] = {
        NPreviewMsg::ESetPreviewParam,
        aSize.iWidth,
        aSize.iHeight,
        static_cast<TInt>(aMode)
        };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::TakePreview 
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::TakePreview()
    {
    HGLOG_CONTEXT( TakePreview, HGLOG_LOCAL );
    HGLOG_IN();
    
    TPckgC<TInt> buf(NPreviewMsg::ETakePreview);
    SendMessage(buf);
    Flush();
    
    HGLOG_OUT();      
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::AckPreview 
// --------------------------------------------------------------------------
//
void CHgFastSwapPreviewProviderClient::AckPreview( TInt aPreviwId )
    {
    HGLOG_CONTEXT( TakePreview, HGLOG_LOCAL );
    HGLOG_IN();
    
    const TInt msg[] = { NPreviewMsg::EAckPreviewReady,
                         aPreviwId };
    TPckgC<TInt[SZ(msg)]> buf(msg);
    SendMessage(buf);
    Flush();
    
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::HandleMessage
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProviderClient::HandleMessage( const TDesC8& aData )
    {
    TRAP_IGNORE(DoHandleMessageL(aData));
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::DoHandleMessageL
// --------------------------------------------------------------------------
//
void CHgFastSwapPreviewProviderClient::DoHandleMessageL( const TDesC8& aData )
    {
    HGLOG_CONTEXT( DoHandleMessageL, HGLOG_LOCAL );
    HGLOG_IN();
    
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
        }

    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProviderClient::SetProperties
// --------------------------------------------------------------------------
//
void CHgFastSwapPreviewProviderClient::SetProperties(TInt aHandle, TInt aWgId,
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
// CHgFastSwapPreviewProviderClient::OnReplace
// --------------------------------------------------------------------------
//
void CHgFastSwapPreviewProviderClient::OnReplace()
    {
    }
    
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CHgFastSwapPreviewProvider* CHgFastSwapPreviewProvider::NewL(
        MHgFastSwapPreviewObserver& aObs)
    {
    CHgFastSwapPreviewProvider* p = new (ELeave) CHgFastSwapPreviewProvider();
    CleanupStack::PushL( p );
    p->ConstructL( aObs );
    CleanupStack::Pop( p );
    return p;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::~CHgFastSwapPreviewProvider
// --------------------------------------------------------------------------
//    
EXPORT_C CHgFastSwapPreviewProvider::~CHgFastSwapPreviewProvider()
    {
    if ( iClient )
        {
        iClient->Destroy();
        }
    delete iClient;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::Register
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CHgFastSwapPreviewProvider::Register( TInt aWgId )
    {
    iClient->Register( aWgId );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::Unregister
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CHgFastSwapPreviewProvider::Unregister( TInt aWgId )    
    {
    iClient->Unregister( aWgId );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::ChangeScreen
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CHgFastSwapPreviewProvider::ChangeScreen( TInt aScreenId )
    {
    iClient->ChangeScreen( aScreenId );
    return KErrNone;
    }    

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::SetPreviewParam
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHgFastSwapPreviewProvider::SetPreviewParam(const TSize& aSize,
        TDisplayMode aMode)
    {
    HGLOG_CONTEXT( SetPreviewParam, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt error( KErrNone );
    
    if ( aMode == ENone || aMode == EColor64K )
        {
        iClient->SetPreviewParam( aSize, aMode );        
        }
    else
        {
        error = KErrNotSupported;
        }
   
    HGLOG_OUT();
    return error;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::TakePreview
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CHgFastSwapPreviewProvider::TakePreview()
    {
    // Reset timer for screensaver (dismiss screensaver)
    User::ResetInactivityTime(); 
           
    iClient->TakePreview();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::AckPreview
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFastSwapPreviewProvider::AckPreview( TInt aPreviewId )
    {
    iClient->AckPreview( aPreviewId );
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::CHgFastSwapPreviewProvider
// --------------------------------------------------------------------------
//    
CHgFastSwapPreviewProvider::CHgFastSwapPreviewProvider()
    {
    }

// --------------------------------------------------------------------------
// CHgFastSwapPreviewProvider::ConstructL
// --------------------------------------------------------------------------
//    
void CHgFastSwapPreviewProvider::ConstructL( MHgFastSwapPreviewObserver& aObs )
    {
    HGLOG_CONTEXT( ConstructL, HGLOG_LOCAL );
    HGLOG_IN();
    
    iClient = new ( ELeave ) CHgFastSwapPreviewProviderClient( aObs );
    iClient->ConstructL();
    
    HGLOG_OUT();
    }

// End of file

