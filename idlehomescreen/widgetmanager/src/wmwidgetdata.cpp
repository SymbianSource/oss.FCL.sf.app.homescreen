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
#include <StringLoader.h>
#include <eikenv.h>

#include "wmwidgetdata.h"
#include "wmwidgetdataobserver.h"
#include "wmpersistentwidgetorder.h"
#include "wmcommon.h"
#include <widgetmanagerview.rsg>

// ---------------------------------------------------------
// CWmWidgetData::NewL
// ---------------------------------------------------------
//
CWmWidgetData* CWmWidgetData::NewL( 
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    CWmWidgetData* self = CWmWidgetData::NewLC( 
            aHsContentInfo, aRegistryClientSession );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetData::NewLC
// ---------------------------------------------------------
//
CWmWidgetData* CWmWidgetData::NewLC( 
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    CWmWidgetData* self = new ( ELeave ) CWmWidgetData();
    CleanupStack::PushL(self);
    self->ConstructL( aHsContentInfo, aRegistryClientSession );
    return self;
    }

// ---------------------------------------------------------
// CWmWidgetData::CWmWidgetData
// ---------------------------------------------------------
//
CWmWidgetData::CWmWidgetData()
    {
    iIdle = NULL;
    iLogoImage = NULL;    
    iLogoImageMask = NULL;
    iHsContentInfo = NULL;
    iWidgetType = CWmWidgetData::EUnknown;
    iInstallAnimationIndex = KErrNotFound;
    iPublisherUid = KNullUid;
    iLogoSize = TSize( 0, 0 );
    }

// ---------------------------------------------------------
// CWmWidgetData::ConstructL
// ---------------------------------------------------------
//
void CWmWidgetData::ConstructL( 
        CHsContentInfo* aHsContentInfo,
        RWidgetRegistryClientSession* aRegistryClientSession )
    {
    InitL( aHsContentInfo, aRegistryClientSession );

    // start decoding the icon
    iImageConverter = CWmImageConverter::NewL( this );
    iIdle = CIdle::NewL( CActive::EPriorityLow );
    iIdle->Start( TCallBack( HandleAsyncIconString, this ) );
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

    // take ownership of the content info
    iHsContentInfo = aHsContentInfo;

    // get publisher uid from widget registry
    FetchPublisherUidL( aRegistryClientSession );

    }

// ---------------------------------------------------------
// CWmWidgetData::~CWmWidgetData
// ---------------------------------------------------------
//
CWmWidgetData::~CWmWidgetData()
    {
    if ( iIdle && iIdle->IsActive() )
        {
        iIdle->Cancel();
        }
    delete iIdle;
    SetObserver( NULL );
    delete iLogoImage;
    delete iLogoImageMask;
    delete iImageConverter;
    delete iHsContentInfo;
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
        FireDataChanged();
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::HandleIconStringL
// ---------------------------------------------------------
//
void CWmWidgetData::HandleIconStringL( const TDesC& aIconStr )
    {
    HBufC* iconStr = NULL;
    if ( aIconStr.Length() == 0 && 
        iPublisherUid != KNullUid )
        {
        // workaround for wrt widgets icon
        _LIT( KUidTag, "uid(0x%x)" );
        const TInt KLength = 32;
        iconStr = HBufC::NewLC( KLength );
        iconStr->Des().Format( KUidTag, iPublisherUid.iUid );
        }
    else
        {
        iconStr = aIconStr.AllocLC();
        }

    TSize size( iLogoSize );
    if ( iLogoImage ) { size = iLogoImage->SizeInPixels(); }
    if ( size != iLogoSize &&
        iLogoSize.iWidth > 0 && iLogoSize.iHeight > 0 )
        {
        size = iLogoSize;
        }
    iImageConverter->HandleIconStringL( 
            size.iWidth, size.iHeight, *iconStr );
    
    CleanupStack::PopAndDestroy( iconStr );    
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
            RWidgetRegistryClientSession* aRegistryClientSession )
    {
    if ( iPublisherUid == KNullUid && 
        PublisherId() != KNullDesC &&
        iWidgetType != CWmWidgetData::ENative &&
        aRegistryClientSession )
        {
        TInt widgetUid = aRegistryClientSession->GetWidgetUidL( PublisherId() );
        if ( widgetUid != 0 )
            {
            // WRT widget
            iPublisherUid = TUid::Uid( widgetUid );
            
            HBufC* desc = StringLoader::LoadLC( R_QTN_WM_WIDGET_DETAILS_WRT, 
                    CEikonEnv::Static() );
            iHsContentInfo->SetDescriptionL( *desc );
            CleanupStack::PopAndDestroy( desc );
            }
        else
            {
            iPublisherUid = KNullUid;
            }
        }
    }
// ----------------------------------------------------
// CWmWidgetData::SetLogoSize
// ----------------------------------------------------
//
void CWmWidgetData::SetLogoSize( const TSize& aSize )
    {
    iLogoSize = aSize;
    if ( iImageConverter )
        {
        iImageConverter->SetLogoSize( aSize );
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::HandleAsyncIconString
// ---------------------------------------------------------
//
TInt CWmWidgetData::HandleAsyncIconString( TAny* aPtr )
    {
    CWmWidgetData* self = static_cast< CWmWidgetData* >( aPtr );    
    if ( self->iIdle->IsActive() )
      { 
      self->iIdle->Cancel(); 
      }
    TRAP_IGNORE( self->HandleIconStringL( 
            self->HsContentInfo().IconPath() ); );
    return KErrNone;
    }

// ---------------------------------------------------------
// CWmWidgetData::ReCreateLogo
// ---------------------------------------------------------
//
void CWmWidgetData::ReCreateLogo( const TSize& aSize )
    {
    iLogoSize = aSize;
    
    delete iLogoImage;
    iLogoImage = NULL;
    delete iLogoImageMask;
    iLogoImageMask = NULL;

    if ( iIdle && !iIdle->IsActive() )
        {
        // start decoding the icon
        iIdle->Start( TCallBack( HandleAsyncIconString, this ) );
        }
    }

// ---------------------------------------------------------
// CWmWidgetData::ReplaceContentInfoL
// ---------------------------------------------------------
//
TBool CWmWidgetData::ReplaceContentInfoL(
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

    // re-init the object
    InitL( aHsContentInfo, NULL );

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
// End of file

