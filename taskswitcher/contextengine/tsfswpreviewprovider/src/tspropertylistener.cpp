/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*/

#include "tspropertylistener.h"
#include "previewproviderlogging.h"



// --------------------------------------------------------------------------
// CTsPropertyListener::~CTsPropertyListener
// --------------------------------------------------------------------------
//
CTsPropertyListener::~CTsPropertyListener()
    {
    Cancel();
    iProperty.Close();
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::CTsPropertyListener
// --------------------------------------------------------------------------
//
CTsPropertyListener::CTsPropertyListener( MTsFastSwapPreviewObserver& aObs ) : 
    CActive( CActive::EPriorityHigh ), iObs( aObs )
    {
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::NewL
// --------------------------------------------------------------------------
//
CTsPropertyListener* CTsPropertyListener::NewL( MTsFastSwapPreviewObserver& aObs )
    {
    CTsPropertyListener* self = new ( ELeave ) CTsPropertyListener( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::RunL
// --------------------------------------------------------------------------
//
void CTsPropertyListener::RunL()
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
        case EOperationBitmapRotation90:
            {
            iObs.HandleFswPpApplicationBitmapRotation( iWgId, ETrue );
            break;
            }
        case EOperationBitmapRotation270:
            {
            iObs.HandleFswPpApplicationBitmapRotation( iWgId, EFalse );
            break;
            }
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::DoCancel
// --------------------------------------------------------------------------
//
void CTsPropertyListener::DoCancel()
    {
    iProperty.Cancel();
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::ReadProperties
// --------------------------------------------------------------------------
//
void CTsPropertyListener::ReadPropertiesL()
    {
    // Read all properties
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyWgId, iWgId ) );
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyFbsBitmapId, iFbsBitmapId ) );
    TInt operation( 0 );
    User::LeaveIfError( iProperty.Get( KPSUidPreviewProvider, KPSKeyOperation, operation ) );
    iOperation = static_cast<TPreviewOperation>( operation );    
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::DefinePropertyL
// --------------------------------------------------------------------------
//
void CTsPropertyListener::DefinePropertyL( TInt aPSKey )
    {
    TInt err = RProperty::Define( KPSUidPreviewProvider, aPSKey, RProperty::EInt );
    // Don't mind the already exists error
    if ( err != KErrAlreadyExists && err != KErrPermissionDenied )
        {
        User::LeaveIfError( err );
        }    
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::Subscribe
// --------------------------------------------------------------------------
//
void CTsPropertyListener::Subscribe()
    {
    iProperty.Subscribe( iStatus );
    SetActive();  
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::ConstructL
// --------------------------------------------------------------------------
//
void CTsPropertyListener::ConstructL()
    {
    TSLOG_CONTEXT( ConstructL, TSLOG_LOCAL );
    TSLOG_IN();
    
    DefinePropertyL( KPSKeyWgId );
    DefinePropertyL( KPSKeyFbsBitmapId );
    DefinePropertyL( KPSKeyOperation );
    
    User::LeaveIfError( iProperty.Attach( KPSUidPreviewProvider, KPSKeyWgId ) );
    CActiveScheduler::Add( this );
    
    // initial subscription
    Subscribe();
    
    TSLOG_OUT();
    }

// End of file

