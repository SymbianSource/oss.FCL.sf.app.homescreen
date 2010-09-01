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
* Description:  Preview provider API
*
*/


#include "tspreviewprovider.h"
#include "previewproviderlogging.h"
#include "tspreviewproviderclient.h"

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CTsFastSwapPreviewProvider* CTsFastSwapPreviewProvider::NewL(
        MTsFastSwapPreviewObserver& aObs)
    {
    CTsFastSwapPreviewProvider* p = new (ELeave) CTsFastSwapPreviewProvider();
    CleanupStack::PushL( p );
    p->ConstructL( aObs );
    CleanupStack::Pop( p );
    return p;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::~CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//    
EXPORT_C CTsFastSwapPreviewProvider::~CTsFastSwapPreviewProvider()
    {
    if ( iClient )
        {
        iClient->Destroy();
        }
    delete iClient;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::Register
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CTsFastSwapPreviewProvider::Register( TInt aWgId )
    {
    iClient->Register( aWgId );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::Unregister
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CTsFastSwapPreviewProvider::Unregister( TInt aWgId )    
    {
    iClient->Unregister( aWgId );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::ChangeScreen
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CTsFastSwapPreviewProvider::ChangeScreen( TInt aScreenId )
    {
    iClient->ChangeScreen( aScreenId );
    return KErrNone;
    }    

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::SetPreviewParam
// --------------------------------------------------------------------------
//
EXPORT_C TInt CTsFastSwapPreviewProvider::SetPreviewParam(const TSize& aSize,
        TDisplayMode aMode)
    {
    TSLOG_CONTEXT( SetPreviewParam, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt error( KErrNone );
    
    if ( aMode == ENone || aMode == EColor64K )
        {
        iClient->SetPreviewParam( aSize, aMode );        
        }
    else
        {
        error = KErrNotSupported;
        }
   
    TSLOG_OUT();
    return error;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::TakePreview
// --------------------------------------------------------------------------
//    
EXPORT_C TInt CTsFastSwapPreviewProvider::TakePreview()
    {
    // Reset timer for screensaver (dismiss screensaver)
    User::ResetInactivityTime(); 
           
    iClient->TakePreview();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::AckPreview
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFastSwapPreviewProvider::AckPreview( TInt aPreviewId )
    {
    iClient->AckPreview( aPreviewId );
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::CTsFastSwapPreviewProvider
// --------------------------------------------------------------------------
//    
CTsFastSwapPreviewProvider::CTsFastSwapPreviewProvider()
    {
    }

// --------------------------------------------------------------------------
// CTsFastSwapPreviewProvider::ConstructL
// --------------------------------------------------------------------------
//    
void CTsFastSwapPreviewProvider::ConstructL( MTsFastSwapPreviewObserver& aObs )
    {
    TSLOG_CONTEXT( ConstructL, TSLOG_LOCAL );
    TSLOG_IN();
    
    iClient = new ( ELeave ) CTsFastSwapPreviewProviderClient( aObs );
    iClient->ConstructL();
    
    TSLOG_OUT();
    }

// End of file

