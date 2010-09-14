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
* Description:  Preview provider ECOM plugin
*
*/


#include <bldvariant.hrh>
#ifdef SYMBIAN_BUILD_GCE
#define NGA
#endif

#include "previewprovidercrp.h"
#include "previewmsg.h"
#include "previewprovidercrplogging.h"
#include <ecom/implementationproxy.h>
#include <s32mem.h> //RDesReadStream

#ifdef NGA
#include <graphics/wsscreendevice.h>
#endif

// CONSTANTS
const TInt KImpId( 0x20016BEC );
const TInt64 KMinTimeForOrientationSwitch = 500000; // 0.5 second

// --------------------------------------------------------------------------
// CPreviewProviderCRP::CreateL
// --------------------------------------------------------------------------
//
CWsGraphicDrawer* CPreviewProviderCRP::CreateL()
    {
    CPreviewProviderCRP* crp = new (ELeave) CPreviewProviderCRP();
    CleanupStack::PushL( crp );
    crp->ConstructL();
    CleanupStack::Pop( crp );
    return crp;
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::ConstructL
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::ConstructL()
    {
    iLastWgIdRedraw = ETrue;
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::~CPreviewProviderCRP
// --------------------------------------------------------------------------
//    
CPreviewProviderCRP::~CPreviewProviderCRP()    
    {
    Env().UnregisterEventHandler( this );
    iScreenshots.ResetAndDestroy();
    iWgIds.Close();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::DoDraw
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::DoDraw( MWsGc& /*aGc*/, const TRect& /*aRect*/, 
    const TDesC8& /*aData*/) const
    {
    //draws nothing
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::HandleMessage
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::HandleMessage( const TDesC8& aData )
    {
    TRAP_IGNORE( DoHandleMessageL( aData ) );
    }
    
// --------------------------------------------------------------------------
// CPreviewProviderCRP::DoHandleMessageL
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::DoHandleMessageL( const TDesC8& aData )
    {  
    TSLOG_CONTEXT( DoHandleMessageL, TSLOG_LOCAL );
    TSLOG_IN();
    
    RDesReadStream in( aData );
    switch( in.ReadInt32L() )
        {
        case NPreviewMsg::ERegister:
            {
            const TInt id = in.ReadInt32L();
            Register( id );
            }
            break;
        case NPreviewMsg::EUnregister:
            {
            const TInt id = in.ReadInt32L();
            Unregister( id );
            }
            break;
        case NPreviewMsg::EChangeScreen:
            {
            const TInt screen = in.ReadInt32L();
            if ( Env().ScreenCount() )
                {
                iScreen = screen;
                }
            }
            break;
        case NPreviewMsg::ESetPreviewParam:
            iScreenshotSize.iWidth = in.ReadInt32L();
            iScreenshotSize.iHeight = in.ReadInt32L();
            iScreenshotMode = static_cast<TDisplayMode>( in.ReadInt32L() );
            break;
        case NPreviewMsg::ETakePreview:
            ScreenshotL( FALSE );
            break;
        case NPreviewMsg::EAckPreviewReady:
            RemoveScreenshot( in.ReadInt32L() );
            break;
        }
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::ConstructL
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::ConstructL( MWsGraphicDrawerEnvironment& aEnv,
    const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/ )
    {
    TSLOG_CONTEXT( ConstructL, TSLOG_LOCAL );
    TSLOG_IN();
    
    BaseConstructL( aEnv, aId, aOwner );
    aEnv.RegisterEventHandler( this, this, TWservCrEvent::EWindowGroupChanged |
                                           TWservCrEvent::EDeviceOrientationChanged |
                                           TWservCrEvent::EScreenDrawing );
    iScreenChangedTime = 0;
    
    TSLOG_OUT();
    }
    
// --------------------------------------------------------------------------
// CPreviewProviderCRP::DoHandleEvent
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::DoHandleEvent( const TWservCrEvent& aEvent )
    {
    TSLOG_CONTEXT( DoHandleEvent, TSLOG_LOCAL );
    TSLOG_IN();
    
    if ( aEvent.Type() == TWservCrEvent::EWindowGroupChanged )
        {
        const TInt wgId = aEvent.WindowGroupIdentifier();
        if ( iWgIds.FindInOrder( iPrevId ) >= 0 ||
            ( iPrevId == 0 &&  iPrevReg != 0 ) )
            {
            TTime currTime;
            currTime.HomeTime();
            TTimeIntervalMicroSeconds micSecondsFrom = currTime.MicroSecondsFrom( iScreenChangedTime );
            if ( micSecondsFrom.Int64() > KMinTimeForOrientationSwitch )
                {
                if ( iLastWgIdRedraw )
                    {
                    TRAP_IGNORE( ScreenshotL( FALSE ) );
                    }
                }
            else
                {
                // Reset time to allow screenshot taking on next wg change
                iScreenChangedTime = 0;
                }
            iPrevReg = 0;
            }
        iPrevId = wgId;
        iLastWgIdRedraw = EFalse;
        }
    else if ( aEvent.Type() == TWservCrEvent::EDeviceOrientationChanged )
        {
        iScreenChangedTime.HomeTime();
        if ( iLastWgIdRedraw )
            {
            TRAP_IGNORE( 
                ScreenshotL( TRUE ); 
                );
            }
        }
    else if ( aEvent.Type() == TWservCrEvent::EScreenDrawing )
        {
        iLastWgIdRedraw = ETrue;
        }
    
    TSLOG_OUT();
    }
    
// --------------------------------------------------------------------------
// CPreviewProviderCRP::ScaleComplete
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::ScaleCompleteL( const CFbsBitmap& aBitmap, 
                    TBool aRotation )
    {
    TSLOG_CONTEXT( ScaleComplete, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt msgType = NPreviewMsg::EPreviewReady;
    if ( aRotation )
        {
        msgType = iClockwiseRot ? 
                NPreviewMsg::EBitmapRotationNeeded90 : 
                NPreviewMsg::EBitmapRotationNeeded270;
        }
    
    const TInt msg[] = {
            msgType,
            iPrevId?iPrevId:iPrevReg,
            aBitmap.Handle()
            };
    TPckgC<TInt[sizeof(msg) / sizeof(TInt)]> buf(msg);
    User::LeaveIfError( SendMessage( buf ) );
    TSLOG3( TSLOG_INFO, "size = %dx%d handle = %d",
        aBitmap.SizeInPixels().iWidth,
        aBitmap.SizeInPixels().iHeight,
        aBitmap.Handle() );
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::UnregisterComplete
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::UnregisterComplete( TInt aWgId )
    {
    TSLOG_CONTEXT( UnregisterComplete, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt msg[] = {
            NPreviewMsg::EUnregisterReady,
            aWgId,
            0
            };
    TPckgC<TInt[sizeof(msg) / sizeof(TInt)]> buf(msg);
    SendMessage(buf);
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::Register
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::Register( TInt aWgId )
    {
    TSLOG_CONTEXT( Register, TSLOG_LOCAL );
    TSLOG_IN();
    
    iPrevReg = aWgId;
    iWgIds.InsertInOrder( aWgId );
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::Unregister
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::Unregister( TInt aWgId )
    {
    TSLOG_CONTEXT( Unregister, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TInt index = iWgIds.FindInOrder( aWgId );
    if ( index >= 0 )
        {
        iWgIds.Remove( index );
        }
    
    UnregisterComplete( aWgId );
    
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::ScreenShotL
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::ScreenshotL( TBool aRotation )
    {
    CFbsBitmap* screenshot = new (ELeave)CFbsBitmap();
    CleanupStack::PushL( screenshot );
    ScreenshotL( *screenshot );
    ScaleCompleteL( *screenshot, aRotation );
    iScreenshots.InsertL( screenshot, iScreenshots.Count() );
    CleanupStack::Pop( screenshot );
    CheckOverflow();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::ScreenShotL
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::ScreenshotL(CFbsBitmap& aBitmap)
    {
    TSLOG_CONTEXT( ScreenShot, TSLOG_LOCAL );
    TSLOG_IN();

    if( 0 > iScreen || Env().ScreenCount() <= iScreen )
        {
        //screen offset out of range. skip request
        User::Leave(KErrArgument);
        }
    // Find the screen resolution
    MWsScreenConfig* screenConfig = 
        Env().Screen( iScreen )->ObjectInterface<MWsScreenConfig>();    
    
    const TSize sz = screenConfig->ScreenModeSizeInPixels();

    // Get the screen buffer (containing screenshot data)
    MWsFrontBuffer* screenFront = 
        Env().Screen( iScreen )->ObjectInterface<MWsFrontBuffer>();
    
    // With NGA we do not support downscaling of the screenshots.
    // So use the size of the screen if no screenshot size has been explicitly
    // set or if the old method for taking screenshots is not available.
    if ( iScreenshotSize.iWidth == 0 || 
         iScreenshotSize.iHeight == 0 || 
         !screenFront )
        {
        iScreenshotSize = sz;
        }
    
    // Mark direction for possible rotation
    iClockwiseRot = iScreenshotSize.iWidth > iScreenshotSize.iHeight;
    
    // Use the the same DisplayMode as for the source image
    // so override the display mode, ignoring any requests.
    iScreenshotMode = screenConfig->DisplayMode();
    
    TSLOG3( TSLOG_INFO, "iShotSize: %dx%d mode: %d",
        iScreenshotSize.iWidth, iScreenshotSize.iHeight, iScreenshotMode );
    
    aBitmap.Reset();
    User::LeaveIfError( aBitmap.Create( iScreenshotSize, iScreenshotMode ) );
  
    // Check if front buffer is available.
    // Will always be NULL with NGA.
    if ( screenFront )
        {
        TSLOG0( TSLOG_INFO, "non-NGA, using front buffer" );
        FrontBufferScreenShot( aBitmap, *screenFront, sz, screenConfig->Stride() );
        }
#ifdef NGA
    else
        {
        TSLOG0( TSLOG_INFO, "NGA, using CopyScreenToBitmapL" );
        MWsScreenDevice* screenDevice = static_cast<MWsScreenDevice*>(
            Env().Screen( iScreen )->ResolveObjectInterface(
                MWsScreenDevice::EWsObjectInterfaceId ) );
        User::LeaveIfNull( screenDevice );
        screenDevice->CopyScreenToBitmapL( &aBitmap, iScreenshotSize );
        }
#endif
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::RemoveScreenshot
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::RemoveScreenshot( TInt aBitmapHandle )
    {
    for( int iter(0); iter < iScreenshots.Count(); ++iter )
        {
        if( iScreenshots[iter]->Handle() == aBitmapHandle )
            {
            delete iScreenshots[iter];
            iScreenshots.Remove(iter);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::CheckOverflow
// --------------------------------------------------------------------------
//
void CPreviewProviderCRP::CheckOverflow()
    {
    TInt overflow( iScreenshots.Count() - KMaxShots);
    while( 0 < overflow-- )
        {
        delete iScreenshots[0];
        iScreenshots.Remove(0);
        }
    }

// --------------------------------------------------------------------------
// CPreviewProviderCRP::FrontBufferScreenShot
// --------------------------------------------------------------------------
//    
void CPreviewProviderCRP::FrontBufferScreenShot( CFbsBitmap& aBitmap, 
                                                 MWsFrontBuffer& aFront, 
                                                 const TSize& aSrcSize, 
                                                 TInt aSrcStride )
    {
    TSLOG_CONTEXT( CPreviewProviderCRP::FrontBufferScreenShot, TSLOG_LOCAL );
    TSLOG_IN();
    
    const TUint8* src = static_cast<const TUint8*> ( aFront.GetBits() );
    TSLOG1( TSLOG_INFO, "src address = [%d]", src );
        
    TInt srcstride( aSrcStride );
    TInt srcwidth( aSrcSize.iWidth );
    TInt srcheight( aSrcSize.iHeight ); 
    TInt dststride( aBitmap.DataStride() );
    TInt dstheight( iScreenshotSize.iHeight );
    TInt dstwidth( iScreenshotSize.iWidth );
    TInt stepdst( dststride / dstwidth );
    TInt stepsrc( srcstride / srcwidth );
    // scale must be TInt type
    TInt scaleX( aSrcSize.iWidth / iScreenshotSize.iWidth );
    TInt scaleY( aSrcSize.iHeight / iScreenshotSize.iHeight );
    
    aBitmap.LockHeap();
    
    TUint8* dst = ( TUint8* ) aBitmap.DataAddress();
    
    TInt minWidth = Min( srcwidth, dstwidth );
    TInt minHeight = Min( srcheight, dstheight );
    TInt minStep = Min( stepsrc, stepdst );
    
    for ( TInt y = minHeight; y > 0; y-- )
         {
         for ( TInt x = minWidth; x > 0; x-- )
             {
             Mem::Copy( dst, src, minStep );
             dst += stepdst;
             src += scaleX*stepdst;
             }
         src += srcstride - scaleX * stepdst * minWidth;
         src += ( scaleY - 1 ) * srcstride;
         }
         
    aBitmap.UnlockHeap();
    TSLOG_OUT();
    }
    
// --------------------------------------------------------------------------
// KImplementationTable
// --------------------------------------------------------------------------
//    
LOCAL_C const TImplementationProxy KImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KImpId, CPreviewProviderCRP::CreateL)
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// --------------------------------------------------------------------------
//    
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
    return KImplementationTable;
    }


// End of file
