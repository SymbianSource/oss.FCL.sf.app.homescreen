/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Description:
* CWmWidgetData implementation.
*
*/


// INCLUDE FILES
#include <fbs.h>
#include <bautils.h>
#include <s32file.h>
#include <hscontentinfo.h>
#include <widgetregistryclient.h> // widgetreqistry
#include <eikenv.h>
#include <avkon.mbg>
#include <avkon.rsg>
#include <AknsDrawUtils.h>
#include <AknBitmapAnimation.h>
#include <barsread.h>
#include <widgetmanagerview.rsg>

#include "wmwidgetdata.h"
#include "wmwidgetdataobserver.h"
#include "wmpersistentwidgetorder.h"
#include "wmresourceloader.h"
#include "wmcommon.h"

// ---------------------------------------------------------
// CWmWidgetData::NewL
// ---------------------------------------------------------
//
CWmWidgetData* CWmWidgetData::NewL(
        const TSize& aLogoSize,
        CWmResourceLoader& aWmResourceLoader,
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    CWmWidgetData* self = CWmWidgetData::NewLC( 
            aLogoSize, aWmResourceLoader,
            aHsContentInfo, aRegistryClientSession );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetData::NewLC
// ---------------------------------------------------------
//
CWmWidgetData* CWmWidgetData::NewLC(
        const TSize& aLogoSize,
        CWmResourceLoader& aWmResourceLoader,
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    CWmWidgetData* self = new ( ELeave ) CWmWidgetData( 
            aLogoSize, aWmResourceLoader );
    CleanupStack::PushL(self);
    self->ConstructL( aHsContentInfo, aRegistryClientSession );
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetData::CWmWidgetData
// ---------------------------------------------------------
//
CWmWidgetData::CWmWidgetData( const TSize& aLogoSize, 
        CWmResourceLoader& aWmResourceLoader )
    : iWmResourceLoader( aWmResourceLoader )
    {
    iLogoImage = NULL;    
    iLogoImageMask = NULL;
    iHsContentInfo = NULL;
    iWidgetType = CWmWidgetData::EUnknown;
    iWrtType = CWmWidgetData::EUnIdentified;
    iPublisherUid = KNullUid;
    iLogoSize = aLogoSize;
    iAnimationTimer = NULL;
    iTimeoutTimer = NULL;
    iAnimationIndex = 0;
    iAsyncUninstalling = EFalse;
    iFireLogoChanged = EFalse;
    iMdcaPoint = NULL;
    iWidgetName = NULL;
    }

// ---------------------------------------------------------
// CWmWidgetData::ConstructL
// ---------------------------------------------------------
//
void CWmWidgetData::ConstructL( 
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    // start decoding the icon
    iImageConverter = CWmImageConverter::NewL( this );
    iWait = new (ELeave) CActiveSchedulerWait();
    iAnimationTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iTimeoutTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    InitL( aHsContentInfo, aRegistryClientSession );
    
    // start logo handling
    iImageConverter->SetLogoSize( iLogoSize );
    HandleIconString( HsContentInfo().IconPath() );
    }

// ---------------------------------------------------------
// CWmWidgetData::InitL
// ---------------------------------------------------------
//
void CWmWidgetData::InitL(
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    // analyse the widget type
    if ( aHsContentInfo->Type() == KContentTemplate )
        {
        iWidgetType = CWmWidgetData::ECps; // wrt widgets included in this gategory
        }
    else if ( aHsContentInfo->Type() == KContentWidget )
        {
        iWidgetType = CWmWidgetData::ENative;
        }

    // get publisher uid from widget registry
    FetchPublisherUidL( aHsContentInfo->PublisherId(), 
                        aRegistryClientSession );
    
    // create iMdcaPoint for listbox    
    _LIT( KFormatStr, "0\t%S");
    iMdcaPoint = HBufC::NewL( 
            aHsContentInfo->Name().Length() + KFormatStr().Length() );
    iMdcaPoint->Des().Format( KFormatStr(), &aHsContentInfo->Name() );

    // take ownership of the content info
    iHsContentInfo = aHsContentInfo;
    }

// ---------------------------------------------------------
// CWmWidgetData::~CWmWidgetData
// ---------------------------------------------------------
//
CWmWidgetData::~CWmWidgetData()
    {
    if ( iTimeoutTimer && 
        iTimeoutTimer->IsActive() )
        {
        iTimeoutTimer->Cancel();
        }
    delete iTimeoutTimer;

    delete iWidgetName;
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    delete iWait;
    SetObserver( NULL );
    DestroyAnimData();
    delete iAnimationTimer;
    delete iLogoImage;
    delete iLogoImageMask;
    delete iImageConverter;
    delete iHsContentInfo;
    delete iMdcaPoint;
    }

// ---------------------------------------------------------
// CWmWidgetData::SetObserver
// ---------------------------------------------------------
//
void CWmWidgetData::SetObserver( MWmWidgetDataObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CWmWidgetData::SetObserver
// ---------------------------------------------------------
//
void CWmWidgetData::SetPersistentWidgetOrder(
        const CWmPersistentWidgetOrder* aPersistentWidgetOrder )
    {
    iPersistentWidgetOrder = aPersistentWidgetOrder;
    }

// ---------------------------------------------------------
// CWmWidgetData::EqualsTo
// ---------------------------------------------------------
//
TBool CWmWidgetData::EqualsTo( CHsContentInfo& aContentInfo )
    {
    return (
        HsContentInfo().Uid() == aContentInfo.Uid() &&
        HsContentInfo().PublisherId() == aContentInfo.PublisherId() );
    }

// ---------------------------------------------------------
// CWmWidgetData::CompareByName
// ---------------------------------------------------------
//
TInt CWmWidgetData::CompareByName( 
        const CWmWidgetData& aDataOne, const CWmWidgetData& aDataTwo )
    {
    // negate the result for ascending alphabetical order
    TInt result = aDataOne.Name().CompareC( aDataTwo.Name() );
    return result;
    }

// ---------------------------------------------------------
// CWmWidgetData::CompareByPersistentWidgetOrder
// ---------------------------------------------------------
//
TInt CWmWidgetData::CompareByPersistentWidgetOrder( 
        const CWmWidgetData& aDataOne, const CWmWidgetData& aDataTwo )
    {
    TInt result;
    const CWmPersistentWidgetOrder* order = aDataOne.iPersistentWidgetOrder;
    if ( order && !order->IsEmpty() )
        {
        result = order->IndexOf( aDataOne ) - order->IndexOf( aDataTwo );
        }
    else
        {
        // fallback: if persistent widget order is not available
        // or it is empty (this is the case on first start, or if the persistent
        // file is corrupted or deleted) -> order widgets by name.
        result = CompareByName( aDataOne, aDataTwo );
        }
    return result;
    }

// ---------------------------------------------------------
// CWmWidgetData::NotifyCompletion
// ---------------------------------------------------------
//
void CWmWidgetData::NotifyCompletion( TInt aError )
    {
    delete iLogoImage;
    iLogoImage = NULL;
    delete iLogoImageMask;
    iLogoImageMask = NULL;
    if ( KErrNone != aError )
        {
        // no image available. Do nothing.
        }
    else
        {
        iLogoImage = iImageConverter->Bitmap();
        iLogoImageMask = iImageConverter->Mask();

        if ( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
        if ( iFireLogoChanged ) 
            {
            iFireLogoChanged = EFalse;
            FireDataChanged();
            }
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::HandleIconString
// ---------------------------------------------------------
//
void CWmWidgetData::HandleIconString( const TDesC& aIconStr )
    {
    HBufC* iconStr = NULL;
    if ( aIconStr.Length() == 0 && 
        iPublisherUid != KNullUid )
        {
        // workaround for wrt widgets icon
        _LIT( KUidTag, "uid(0x%x)" );
        const TInt KLength = 32;
        TBuf<KLength> uidBuf;
        uidBuf.Format( KUidTag, iPublisherUid.iUid );
        iconStr = uidBuf.Alloc();
        }
    else
        {
        iconStr = aIconStr.Alloc();
        }
    
    // nothing to do if no iconstr
    if ( !iconStr ){ return; }

    TSize size( iLogoSize );
    if ( iLogoImage ) { size = iLogoImage->SizeInPixels(); }
    if ( size != iLogoSize &&
        iLogoSize.iWidth > 0 && iLogoSize.iHeight > 0 )
        {
        size = iLogoSize;
        }
    TInt err = iImageConverter->HandleIconString( 
            size.iWidth, size.iHeight, *iconStr );
    
    delete iconStr;
    iconStr = NULL;
    
    // handle result
    if ( KErrNone == err && iWait &&
        IsPrepairingLogo() && iTimeoutTimer )
        {
        iTimeoutTimer->Cancel();
        const TInt tickInterval = 200000;        
        iTimeoutTimer->Start(
                tickInterval,tickInterval,TCallBack(TimeoutTick, this));
        iWait->Start();
        }
    else if ( KErrNone != err && iFireLogoChanged )
        {
        FireDataChanged(); // draw default icon
        iFireLogoChanged = EFalse;
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::FireDataChanged
// ---------------------------------------------------------
//
void CWmWidgetData::FireDataChanged()
    {
    if ( iObserver )
        {
        iObserver->HandleWidgetDataChanged( this );
        }
    }

// ----------------------------------------------------
// CWmWidgetData::UidFromString
// ----------------------------------------------------
//
TUid CWmWidgetData::UidFromString( const TDesC8& aUidString ) const
    {
    TUid uid( TUid::Null() );
    const TInt KHexPrefixLength = 2;
    if ( aUidString.Length() > KHexPrefixLength )
        {
        TUint id = 0;
        TLex8 lex( aUidString.Mid( KHexPrefixLength ) );
        if ( lex.Val( id, EHex ) == KErrNone )
            {
            uid.iUid = (TInt32)id;
            }
        }
    return uid;
    }

// ----------------------------------------------------
// CWmWidgetData::PublisherUid
// ----------------------------------------------------
//
TUid CWmWidgetData::PublisherUid()
    {    
    return iPublisherUid;
    }

// ----------------------------------------------------
// CWmWidgetData::FetchPublisherUidL
// ----------------------------------------------------
//
void CWmWidgetData::FetchPublisherUidL( 
            const TDesC& aPublisherId,
            RWidgetRegistryClientSession* aRegistryClientSession )
    {
    if ( iPublisherUid == KNullUid && 
        aPublisherId != KNullDesC &&
        iWidgetType != CWmWidgetData::ENative &&
        aRegistryClientSession )
        {
        TInt widgetUid = aRegistryClientSession->GetWidgetUidL( aPublisherId );
        if ( widgetUid != 0 )
            {
            // WRT widget
            iPublisherUid = TUid::Uid( widgetUid );
            
            // wrt widget type  
            CWidgetPropertyValue* value( NULL );
            value = aRegistryClientSession->GetWidgetPropertyValueL( 
                    iPublisherUid, ENokiaWidget );
            if ( value && *value == 1 )
                {
                iWrtType = CWmWidgetData::EWgz;
                }
            else if ( value && *value == 2 )
                {
                iWrtType = CWmWidgetData::EWgt;
                }
            else
                {
                iWrtType = CWmWidgetData::EUnIdentified;
                }

            delete value;
            value = NULL;

            }
        else
            {
            iPublisherUid = KNullUid;
            }
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::Description
// ---------------------------------------------------------
//
const TDesC& CWmWidgetData::Name() const
    {
    return ( iAsyncUninstalling ? 
        iWmResourceLoader.Uninstalling() :
        iHsContentInfo->Name() );
    }

// ---------------------------------------------------------
// CWmWidgetData::Description
// ---------------------------------------------------------
//
const TDesC& CWmWidgetData::Description() const
    {
    if ( iHsContentInfo->Description().Length() <= 0 &&
        &iWmResourceLoader )
        {
        return ( ( iPublisherUid != KNullUid ) ? 
            iWmResourceLoader.WrtDescription() : 
            iWmResourceLoader.NoDescription() );
        }
    return iHsContentInfo->Description(); 
    }

// ---------------------------------------------------------
// CWmWidgetData::ReCreateLogo
// ---------------------------------------------------------
//
void CWmWidgetData::ReCreateLogo( const TSize& aSize )
    {    
    delete iLogoImage;
    iLogoImage = NULL;
    delete iLogoImageMask;
    iLogoImageMask = NULL;
    
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }

    iFireLogoChanged = ETrue;
    iLogoSize = aSize;
    HandleIconString( HsContentInfo().IconPath() );
    }

// ---------------------------------------------------------
// CWmWidgetData::ReplaceContentInfo
// ---------------------------------------------------------
//
TBool CWmWidgetData::ReplaceContentInfo(
        CHsContentInfo* aHsContentInfo )
    {
    TBool sameAppearance = (
            iHsContentInfo->Name() == aHsContentInfo->Name() &&
            iHsContentInfo->Description() == aHsContentInfo->Description() &&
            iHsContentInfo->CanBeAdded() == aHsContentInfo->CanBeAdded() );
    TBool sameLogo = (
            iHsContentInfo->IconPath() == aHsContentInfo->IconPath() );

    // delete the old content info
    delete iHsContentInfo;
    iHsContentInfo = NULL;
    
    // re-init the object, take care about leave.
    TRAPD( err, InitL( aHsContentInfo, NULL ); );
    if ( KErrNone != err )
        {
        delete aHsContentInfo;
        aHsContentInfo = NULL;
        }
   
    if ( !sameAppearance )
        {
        // fire change event -> widget redrawn
        FireDataChanged();
        }
    
    if ( !sameLogo )
        {
        // re-convert image
        // change event will be fired later when bitmap is ready
        ReCreateLogo( iLogoSize );
        }

    return !( sameAppearance && sameLogo );
    }

// ---------------------------------------------------------
// CWmWidgetData::IsPrepairingLogo
// ---------------------------------------------------------
//
TBool CWmWidgetData::IsPrepairingLogo()
    {
    TBool prepairing( EFalse );            
    if ( !iLogoImage )
        {
        prepairing = iImageConverter->IsProcessing();
        }
    return prepairing;
    }

// ---------------------------------------------------------
// CWmWidgetData::VisualizeUninstall
// ---------------------------------------------------------
//
void CWmWidgetData::VisualizeUninstallL()
    {   
    DestroyAnimData();
    PrepairAnimL();

    iAsyncUninstalling = ETrue;
    iAnimationIndex = 0;
    const TInt tickInterval = 400000;
    iAnimationTimer->Start(
            tickInterval,tickInterval,TCallBack(AnimationTick, this));
    }

// ---------------------------------------------------------
// CWmWidgetData::PrepairAnimL
// ---------------------------------------------------------
//
void CWmWidgetData::PrepairAnimL()
    {
    TInt resourceId = R_QGN_GRAF_WAIT_BAR_ANIM;
    CAknBitmapAnimation* aknAnimation = CAknBitmapAnimation::NewL();
    CleanupStack::PushL( aknAnimation );
    aknAnimation->SetScaleModeForAnimationFrames(EAspectRatioNotPreserved);
    TAknsItemID iid;
    iid.Set(EAknsMajorAvkon, resourceId );
    if ( !aknAnimation ->ConstructFromSkinL( iid ) )
        {
        TResourceReader readerForAnimation;
        CCoeEnv::Static()->CreateResourceReaderLC( readerForAnimation,resourceId );
        aknAnimation->ConstructFromResourceL( readerForAnimation );
        CleanupStack::PopAndDestroy();
        }
    TInt endFrame = aknAnimation ->BitmapAnimData()->FrameArray().Count()-1;
    for ( TInt i=0; i<=endFrame; i++ )
        {
        aknAnimation ->BitmapAnimData()->FrameArray().At(i)->SetBitmapsOwnedExternally( ETrue );
        CFbsBitmap* bitmap  = aknAnimation ->BitmapAnimData()->FrameArray().At(i)->Bitmap();
        CFbsBitmap* bitmapMask = aknAnimation ->BitmapAnimData()->FrameArray().At(i)->Mask();
        if ( bitmap && bitmapMask )
            {
            iUninstallAnimIcons.AppendL( bitmap );
            iUninstallAnimIcons.AppendL( bitmapMask );
            }
        }
    CleanupStack::PopAndDestroy( aknAnimation );
    }

// ---------------------------------------------------------
// CWmWidgetData::DestroyAnimData
// ---------------------------------------------------------
//
void CWmWidgetData::DestroyAnimData()
    {
    if ( iAnimationTimer && iAnimationTimer->IsActive() )
        {
        iAnimationTimer->Cancel();
        }
        
    for( TInt i=0; i < iUninstallAnimIcons.Count(); i++ )
        {
        CFbsBitmap* bitmap = iUninstallAnimIcons[i];
        delete bitmap; bitmap = NULL;
        }
    iUninstallAnimIcons.Close();    
    }

// ---------------------------------------------------------
// CWmWidgetData::AnimationTick
// ---------------------------------------------------------
//
TInt CWmWidgetData::AnimationTick( TAny* aPtr )
    {
    CWmWidgetData* self = static_cast< CWmWidgetData* >( aPtr );
    self->iAnimationIndex += 2;
    if ( self->iAnimationIndex >= self->iUninstallAnimIcons.Count() -1 )
        {
        self->iAnimationIndex = 0; // restart from beginging
        }
    self->FireDataChanged();
    return 1;
    }

// ---------------------------------------------------------
// CWmWidgetData::TimeoutTick
// ---------------------------------------------------------
//
TInt CWmWidgetData::TimeoutTick( TAny* aPtr )
    {
    CWmWidgetData* self = static_cast< CWmWidgetData* >( aPtr );
    self->iTimeoutTimer->Cancel();
    if ( self->iWait && self->iWait->IsStarted() )
        {
        self->iWait->AsyncStop();
        }
    return 1;
    }

// ---------------------------------------------------------
// CWmWidgetData::AnimationBitmap
// ---------------------------------------------------------
//
const CFbsBitmap* CWmWidgetData::AnimationBitmap( const TSize& aSize )
    {
    CFbsBitmap* bitmap = NULL;
    if ( iUninstallAnimIcons.Count() )
        {
        TInt index = iAnimationIndex;
        if ( index >= iUninstallAnimIcons.Count() - 1 )
            {
            index = 0;
            }
        bitmap = iUninstallAnimIcons[index];
        if ( bitmap->SizeInPixels() != aSize )
            {
            AknIconUtils::SetSize( bitmap, aSize, 
                    EAspectRatioNotPreserved );
            }               
        }
    return bitmap;
    }

// ---------------------------------------------------------
// CWmWidgetData::AnimationMask
// ---------------------------------------------------------
//
const CFbsBitmap* CWmWidgetData::AnimationMask( const TSize& aSize )
    {
    CFbsBitmap* mask = NULL;
    if ( iUninstallAnimIcons.Count() )
        {
        TInt index = iAnimationIndex+1;
        if ( index >= iUninstallAnimIcons.Count() )
            {
            index = 1;
            }
        mask = iUninstallAnimIcons[index];
        if ( mask && mask->SizeInPixels() != aSize )
            {
            AknIconUtils::SetSize( mask, aSize,
                    EAspectRatioNotPreserved );
            }
        }    
    return mask;
    }

// ---------------------------------------------------------
// CWmWidgetData::IsUninstalling
// ---------------------------------------------------------
//
TBool CWmWidgetData::IsUninstalling()
    {
    return iAsyncUninstalling;
    }

// ---------------------------------------------------------
// CWmWidgetData::MdcaPoint
// ---------------------------------------------------------
//
const TDesC& CWmWidgetData::MdcaPoint() const
    {
    if ( iMdcaPoint )
        return *iMdcaPoint;
    else
        return KNullDesC;
    }

// ---------------------------------------------------------
// CWmWidgetData::StopUninstallAnimation
// ---------------------------------------------------------
//
void CWmWidgetData::StopUninstallAnimation()
    {
    iAsyncUninstalling = EFalse;
    DestroyAnimData();
    FireDataChanged(); //redraw
    }

// End of file

