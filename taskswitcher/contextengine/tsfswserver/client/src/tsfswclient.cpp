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
* Description:  Client API
 *
*/


#include "tsfswclient.h"
#include "tsfswclientimpl.h"

// --------------------------------------------------------------------------
// CTsFswClient::NewL
// --------------------------------------------------------------------------
//    
EXPORT_C CTsFswClient* CTsFswClient::NewL()
    {
    CTsFswClient* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswClient::NewLC
// --------------------------------------------------------------------------
//    
EXPORT_C CTsFswClient* CTsFswClient::NewLC()
    {
    CTsFswClient* self = new ( ELeave ) CTsFswClient;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswClient::CTsFswClient
// --------------------------------------------------------------------------
//    
CTsFswClient::CTsFswClient()
    {
    }

// --------------------------------------------------------------------------
// CTsFswClient::~CTsFswClient
// --------------------------------------------------------------------------
//    
CTsFswClient::~CTsFswClient()
    {
    delete iImpl;
    }

// --------------------------------------------------------------------------
// CTsFswClient::ConstructL
// --------------------------------------------------------------------------
//    
void CTsFswClient::ConstructL()
    {
    iImpl = CTsFswClientImpl::NewL();
    }

// --------------------------------------------------------------------------
// CTsFswClient::GetContentL
// --------------------------------------------------------------------------
//    
EXPORT_C void CTsFswClient::GetContentL( RTsFswArray& aDst )
    {
    iImpl->GetContentL( aDst );
    }

// --------------------------------------------------------------------------
// CTsFswClient::Subscribe
// --------------------------------------------------------------------------
//    
EXPORT_C void CTsFswClient::Subscribe( MTsFswObserver& aObserver )
    {
    iImpl->Subscribe( aObserver );
    }

// --------------------------------------------------------------------------
// CTsFswClient::CancelSubscribe
// --------------------------------------------------------------------------
//    
EXPORT_C void CTsFswClient::CancelSubscribe()
    {
    iImpl->CancelSubscribe();
    }

// --------------------------------------------------------------------------
// CTsFswClient::CloseApp
// --------------------------------------------------------------------------
//    
EXPORT_C void CTsFswClient::CloseApp( TInt aWgId )
    {
    iImpl->CloseApp( aWgId );
    }
    
// --------------------------------------------------------------------------
// CTsFswClient::SwitchToApp
// --------------------------------------------------------------------------
//    
EXPORT_C void CTsFswClient::SwitchToApp( TInt aWgId )
    {
    iImpl->SwitchToApp( aWgId );
    }

// --------------------------------------------------------------------------
// CTsFswClient::ForegroundAppUid
// --------------------------------------------------------------------------
//    
EXPORT_C TUid CTsFswClient::ForegroundAppUid( TTsFswFgAppType aType )
    {
    return iImpl->ForegroundAppUid( aType );
    }


// end of file
