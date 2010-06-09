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
* Description:  Image decoder
*
*/

// System includes
#include <imageconversion.h>

// User includes
#include "xncomponent.h"
#include "xnnodepluginif.h"
#include "xnuienginepluginif.h"
#include "xnbitmapadapter.h"
#include "xnimagedecoder.h"

using namespace ContentAccess;

// Constants
    
// ============================ LOCAL FUNCTIONS ================================
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnImageDecoder::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnImageDecoder* CXnImageDecoder::NewL( CXnBitmapAdapter& aAdapter, RFs& aFs ) 
    {
    CXnImageDecoder* self = CXnImageDecoder::NewLC( aAdapter, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnImageDecoder* CXnImageDecoder::NewLC( CXnBitmapAdapter& aAdapter, RFs& aFs )
    {
    CXnImageDecoder* self = new ( ELeave ) CXnImageDecoder( aAdapter, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::~CXnImageDecoder()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnImageDecoder::~CXnImageDecoder()
    {    
    Cancel();
    
    Cleanup();
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::CXnImageDecoder()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnImageDecoder::CXnImageDecoder( CXnBitmapAdapter& aAdapter, RFs& aFs )
    : CActive( CActive::EPriorityHigh ), iAdapter( aAdapter ), iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnImageDecoder::ConstructL()
    {                     
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::RunL()
// 
// -----------------------------------------------------------------------------
//
void CXnImageDecoder::RunL()
    {   
    // Takes ownership of iBitmap and iMask
    iAdapter.SetContentBitmaps( iBitmap, iMask );
    
    iBitmap = NULL;
    iMask = NULL;
    
    delete iDecoder;
    iDecoder = NULL;
    
    iAdapter.Component()->Node()->UiEngineL()->RenderUIL();
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::RunError()
// 
// -----------------------------------------------------------------------------
//
TInt CXnImageDecoder::RunError( TInt /*aError*/ )
    {
    Cleanup();
    
    // Set NULL bitmaps
    TRAP_IGNORE( RunL() );
    
    // Ignore
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::DoCancel()
// 
// -----------------------------------------------------------------------------
//
void CXnImageDecoder::DoCancel()
    {
    Cleanup();
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::DecodeL()
// 
// -----------------------------------------------------------------------------
//
void CXnImageDecoder::DecodeL( TFileName& aFilename )
    {     
    Cancel();
    
    TInt options( 
        CImageDecoder::EOptionAlwaysThread | 
        CImageDecoder::EPreferFastDecode );
         
    iDecoder = CImageDecoder::FileNewL( iFs, aFilename, EView, 
        ( CImageDecoder::TOptions ) options ); 
    
    TFrameInfo frameInfo( iDecoder->FrameInfo( 0 ) );
    
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    
    User::LeaveIfError( bitmap->Create( 
        frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode ) );
                             
    if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
        {
        CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( mask );
        
        User::LeaveIfError( mask->Create( frameInfo.iOverallSizeInPixels,                
            ( frameInfo.iFlags & TFrameInfo::EAlphaChannel ) ? EGray256 : EGray2 ) );
        
        iDecoder->Convert( &iStatus, *bitmap, *mask );
        
        CleanupStack::Pop( mask );
        iMask = mask;
        }
    else
        {
        iDecoder->Convert( &iStatus, *bitmap );               
        }
    
    CleanupStack::Pop( bitmap );
    iBitmap = bitmap;
        
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CXnImageDecoder::Cleanup()
// 
// -----------------------------------------------------------------------------
//
void CXnImageDecoder::Cleanup()
    {
    if ( iDecoder )
        {
        iDecoder->Cancel();        
        }
    
    delete iDecoder;
    iDecoder = NULL;
    
    delete iBitmap;
    iBitmap = NULL;
    
    delete iMask;
    iMask = NULL;      
    }

// End of file
