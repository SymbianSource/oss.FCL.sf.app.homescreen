/*
 * ===========================================================================
 *  Name        : hgfswentry.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Task list entry
 *  Version     : %version: 7 %
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

#include "hgfswentry.h"
#include <s32strm.h>

// --------------------------------------------------------------------------
// CHgFswEntry::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CHgFswEntry* CHgFswEntry::NewL()
    {
    CHgFswEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CHgFswEntry* CHgFswEntry::NewLC()
    {
    CHgFswEntry* self = new ( ELeave ) CHgFswEntry;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::~CHgFswEntry
// --------------------------------------------------------------------------
//
CHgFswEntry::~CHgFswEntry()
    {
    delete iAppName;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetWgId
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetWgId( TInt aWgId )
    {
    iWgId = aWgId;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetAppUid
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetAppUid( const TUid& aUid )
    {
    iAppUid = aUid;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetAppNameL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetAppNameL( const TDesC& aAppName )
    {
    delete iAppName; iAppName = 0;
    iAppName = aAppName.AllocL();
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetScreenshotHandle
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetScreenshotHandle( TInt aHandle )
    {
    iScreenshotHandle = aHandle;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetAlwaysShown
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetAlwaysShown( TBool aValue )
    {
    iAlwaysShown = aValue;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetSystemApp
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetSystemApp( TBool aValue )
    {
    iSystemApp = aValue;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetWidget
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetWidget( TBool aValue )
    {
    iWidget = aValue;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SetAppIconHandles
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::SetAppIconHandles( TInt aBitmapHandle, TInt aMaskHandle )
    {
    iAppIconBitmapHandle = aBitmapHandle;
    iAppIconMaskHandle = aMaskHandle;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::WgId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHgFswEntry::WgId() const
    {
    return iWgId;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::AppUid
// --------------------------------------------------------------------------
//
EXPORT_C TUid CHgFswEntry::AppUid() const
    {
    return iAppUid;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::AppName
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHgFswEntry::AppName() const
    {
    return iAppName ? *iAppName : KNullDesC();
    }

// --------------------------------------------------------------------------
// CHgFswEntry::ScreenshotHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHgFswEntry::ScreenshotHandle() const
    {
    return iScreenshotHandle;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::AlwaysShown
// --------------------------------------------------------------------------
//
EXPORT_C TBool CHgFswEntry::AlwaysShown() const
    {
    return iAlwaysShown;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::SystemApp
// --------------------------------------------------------------------------
//
EXPORT_C TBool CHgFswEntry::SystemApp() const
    {
    return iSystemApp;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::Widget
// --------------------------------------------------------------------------
//
EXPORT_C TBool CHgFswEntry::Widget() const
    {
    return iWidget;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::AppIconBitmapHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHgFswEntry::AppIconBitmapHandle() const
    {
    return iAppIconBitmapHandle;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::AppIconMaskHandle
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHgFswEntry::AppIconMaskHandle() const
    {
    return iAppIconMaskHandle;
    }

// --------------------------------------------------------------------------
// CHgFswEntry::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::ExternalizeL( RWriteStream& aStream )
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
// CHgFswEntry::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::InternalizeL( RReadStream& aStream )
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
// CHgFswEntry::ExternalizeArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::ExternalizeArrayL( RWriteStream& aStream,
        const RHgFswArray& aArray )
    {
    TInt count = aArray.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; ++i )
        {
        aArray[i]->ExternalizeL( aStream );
        }
    }

// --------------------------------------------------------------------------
// CHgFswEntry::InternalizeArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CHgFswEntry::InternalizeArrayL( RReadStream& aStream,
        RHgFswArray& aArray )
    {
    aArray.ResetAndDestroy();
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; ++i )
        {
        CHgFswEntry* entry = CHgFswEntry::NewLC();
        entry->InternalizeL( aStream );
        aArray.AppendL( entry );
        CleanupStack::Pop( entry );
        }
    }


// end of file
