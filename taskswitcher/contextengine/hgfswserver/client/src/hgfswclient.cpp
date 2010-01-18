/*
 * ===========================================================================
 *  Name        : hgfswclient.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Client API
 *  Version     : %version: 3 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#include "hgfswclient.h"
#include "hgfswclientimpl.h"

EXPORT_C CHgFswClient* CHgFswClient::NewL()
    {
    CHgFswClient* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CHgFswClient* CHgFswClient::NewLC()
    {
    CHgFswClient* self = new ( ELeave ) CHgFswClient;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CHgFswClient::CHgFswClient()
    {
    }

CHgFswClient::~CHgFswClient()
    {
    delete iImpl;
    }

void CHgFswClient::ConstructL()
    {
    iImpl = CHgFswClientImpl::NewL();
    }

EXPORT_C void CHgFswClient::GetContentL( RHgFswArray& aDst )
    {
    iImpl->GetContentL( aDst );
    }

EXPORT_C void CHgFswClient::Subscribe( MHgFswObserver& aObserver )
    {
    iImpl->Subscribe( aObserver );
    }
    
EXPORT_C void CHgFswClient::CancelSubscribe()
    {
    iImpl->CancelSubscribe();
    }

EXPORT_C void CHgFswClient::CloseApp( TInt aWgId )
    {
    iImpl->CloseApp( aWgId );
    }
    
EXPORT_C void CHgFswClient::SwitchToApp( TInt aWgId )
    {
    iImpl->SwitchToApp( aWgId );
    }

EXPORT_C TUid CHgFswClient::ForegroundAppUid( THgFswFgAppType aType )
    {
    return iImpl->ForegroundAppUid( aType );
    }


// end of file
