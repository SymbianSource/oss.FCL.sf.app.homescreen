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
* Description:  Task list entry
 *
*/


#include "tsfswentry.h"
#include <s32strm.h>

// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CTsFswEntry* CTsFswEntry::NewL()
    {
    CTsFswEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CTsFswEntry* CTsFswEntry::NewLC()
    {
    CTsFswEntry* self = new ( ELeave ) CTsFswEntry;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::~CTsFswEntry
// --------------------------------------------------------------------------
//
CTsFswEntry::~CTsFswEntry()
    {
    delete iAppName;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetWgId
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetWgId( TInt aWgId )
    {
    iWgId = aWgId;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppUid
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetAppUid( const TUid& aUid )
    {
    iAppUid = aUid;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppNameL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetAppNameL( const TDesC& aAppName )
    {
    delete iAppName; iAppName = 0;
    iAppName = aAppName.AllocL();
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetScreenshotHandle
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetScreenshotHandle( TInt aHandle )
    {
    iScreenshotHandle = aHandle;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAlwaysShown
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetAlwaysShown( TBool aValue )
    {
    iAlwaysShown = aValue;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetSystemApp
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetSystemApp( TBool aValue )
    {
    iSystemApp = aValue;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetWidget
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetWidget( TBool aValue )
    {
    iWidget = aValue;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppIconHandles
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::SetAppIconHandles( TInt aBitmapHandle, TInt aMaskHandle )
    {
    iAppIconBitmapHandle = aBitmapHandle;
    iAppIconMaskHandle = aMaskHandle;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::WgId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CTsFswEntry::WgId() const
    {
    return iWgId;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppUid
// --------------------------------------------------------------------------
//
EXPORT_C TUid CTsFswEntry::AppUid() const
    {
    return iAppUid;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppName
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CTsFswEntry::AppName() const
    {
    return iAppName ? *iAppName : KNullDesC();
    }

// --------------------------------------------------------------------------
// CTsFswEntry::ScreenshotHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CTsFswEntry::ScreenshotHandle() const
    {
    return iScreenshotHandle;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AlwaysShown
// --------------------------------------------------------------------------
//
EXPORT_C TBool CTsFswEntry::AlwaysShown() const
    {
    return iAlwaysShown;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SystemApp
// --------------------------------------------------------------------------
//
EXPORT_C TBool CTsFswEntry::SystemApp() const
    {
    return iSystemApp;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::Widget
// --------------------------------------------------------------------------
//
EXPORT_C TBool CTsFswEntry::Widget() const
    {
    return iWidget;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconBitmapHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CTsFswEntry::AppIconBitmapHandle() const
    {
    return iAppIconBitmapHandle;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconMaskHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CTsFswEntry::AppIconMaskHandle() const
    {
    return iAppIconMaskHandle;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( iWgId );
    aStream.WriteInt32L( iAppUid.iUid );
    aStream << AppName();
    aStream.WriteInt32L( iScreenshotHandle );
    aStream.WriteInt32L( iAlwaysShown );
    aStream.WriteInt32L( iSystemApp );
    aStream.WriteInt32L( iWidget );
    aStream.WriteInt32L( iAppIconBitmapHandle );
    aStream.WriteInt32L( iAppIconMaskHandle );
    }

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::InternalizeL( RReadStream& aStream )
    {
    iWgId = aStream.ReadInt32L();
    iAppUid = TUid::Uid( aStream.ReadInt32L() );
    delete iAppName; iAppName = 0;
    iAppName = HBufC::NewL( aStream, KMaxTInt );
    iScreenshotHandle = aStream.ReadInt32L();
    iAlwaysShown = aStream.ReadInt32L();
    iSystemApp = aStream.ReadInt32L();
    iWidget = aStream.ReadInt32L();
    iAppIconBitmapHandle = aStream.ReadInt32L();
    iAppIconMaskHandle = aStream.ReadInt32L();
    }

// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::ExternalizeArrayL( RWriteStream& aStream,
        const RTsFswArray& aArray )
    {
    TInt count = aArray.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; ++i )
        {
        aArray[i]->ExternalizeL( aStream );
        }
    }

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CTsFswEntry::InternalizeArrayL( RReadStream& aStream,
        RTsFswArray& aArray )
    {
    aArray.ResetAndDestroy();
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; ++i )
        {
        CTsFswEntry* entry = CTsFswEntry::NewLC();
        entry->InternalizeL( aStream );
        aArray.AppendL( entry );
        CleanupStack::Pop( entry );
        }
    }


// end of file
