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
* Description:  image provider for image file
*
*/


#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include "hnconvutils.h"
#include "hnglobals.h"
#include "hnattrimgprovsvgimage.h"
#include "SVGEngineInterfaceImpl.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvSvgImage::CHnAttrImgProvSvgImage()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvSvgImage::ConstructL( const TDesC& aFileName )
    {
    iBitmap = new (ELeave) CFbsBitmap();
    iMask = new (ELeave) CFbsBitmap();
    TFontSpec fontSpec;
       
    iDummyBitmap = new( ELeave ) CFbsBitmap;
    User::LeaveIfError( iDummyBitmap->Create( TSize( 0, 0 ), EGray2 ) );

    
    iSvgEngine = CSvgEngineInterfaceImpl::NewL( iDummyBitmap, this, fontSpec );
    iSvgEngine->SetBackgroundColor( 0 );
    iSvgEngine->SetDRMMode( EFalse );
    
    User::LeaveIfError( iSvgEngine->PrepareDom( aFileName, iHandle )->SystemErrorCode() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnAttrImgProvSvgImage::RenderIcon( TSize aDesiredIconSize )
    {   
    TInt err = iBitmap->Create( aDesiredIconSize, EColor64K );
    if ( err == KErrNone )
        {
        err = iMask->Create( aDesiredIconSize, EGray256 );
        }
    if ( err == KErrNone )
        {
        iSvgEngine->UseDom( iHandle, iBitmap, iMask );
        iSvgEngine->SetPreserveAspectRatio( NULL, ESvgPreserveAspectRatio_XmidYmid, 
                ESvgMeetOrSlice_Meet, ETrue);
        iSvgEngine->Start();
        }
    return err; 
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvSvgImage::~CHnAttrImgProvSvgImage()
    {
    if ( iSvgEngine )
    	{
    	iSvgEngine->UseDom( iHandle, NULL, NULL );
    	iSvgEngine->DeleteDom( iHandle );
    	}
    delete iDummyBitmap;
    delete iSvgEngine;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvSvgImage* CHnAttrImgProvSvgImage::NewL( 
        const TDesC& aFileName )
    {
    CHnAttrImgProvSvgImage* self = NewLC( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnAttrImgProvSvgImage* CHnAttrImgProvSvgImage::NewLC( 
        const TDesC& aFileName )
    {
    CHnAttrImgProvSvgImage* self = new( ELeave ) CHnAttrImgProvSvgImage;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CGulIcon* CHnAttrImgProvSvgImage::GetIcon( TSize* aDesiredIconSize )
    {
    TInt err = KErrNone;
    if ( !iIconHolder || !iIconHolder->GetGulIcon() )
        {
        TSize targetSize = aDesiredIconSize ?
            *aDesiredIconSize : TSize( KDefaultSvgIconSize, KDefaultSvgIconSize );
        err = RenderIcon( targetSize );
        if ( err == KErrNone )
            {
            TRAP( err, CreateIconL( iBitmap, iMask ) );
            }
        }
    else if ( aDesiredIconSize && iBitmap->SizeInPixels() != *aDesiredIconSize )
        {
        err = RenderIcon( *aDesiredIconSize );
        }
    
    return ( !err && iIconHolder ? iIconHolder->GetGulIcon() : NULL );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnIconHolder* CHnAttrImgProvSvgImage::GetIconHolder( TSize* aDesiredIconSize )
    {
    GetIcon( aDesiredIconSize );
    return iIconHolder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvSvgImage::UpdateScreen()
	{

	}
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CHnAttrImgProvSvgImage::ScriptCall( const TDesC& /* aScript */,
		CSvgElementImpl* /* aCallerElement */ )
	{
	return EFalse;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnAttrImgProvSvgImage::FetchImage( const TDesC& /* aUri */, 
		RFs& /* aSession */, RFile& /* aFileHandle */ )
	{
	return KErrNotSupported;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnAttrImgProvSvgImage::FetchFont( const TDesC& /* aUri */, 
		RFs& /* aSession */, RFile& /* aFileHandle */ )
	{
	return KErrNotSupported;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvSvgImage::UpdatePresentation(
		const TInt32&  /*aNoOfAnimation*/ )
	{
	
	}
