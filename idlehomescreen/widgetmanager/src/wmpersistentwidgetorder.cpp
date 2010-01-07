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
* Defines an ordered list of widget id's with persistence capability
*
*/

// INCLUDES
#include <s32file.h> // RFile
#include <s32std.h> // streams
#include <driveinfo.h>
#include <sysutil.h> 

#include "wmwidgetdata.h"
#include "wmpersistentwidgetorder.h"

// CONSTANTS
_LIT( KStoreFileName, "wmlistorder.dat" );


// ---------------------------------------------------------
// CWmPersistentWidgetOrder::NewL
// ---------------------------------------------------------
//
CWmPersistentWidgetOrder* CWmPersistentWidgetOrder::NewL( RFs& aFs )
    {
    CWmPersistentWidgetOrder* self =
        new (ELeave) CWmPersistentWidgetOrder( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::CWmPersistentWidgetOrder
// ---------------------------------------------------------
//
CWmPersistentWidgetOrder::CWmPersistentWidgetOrder( RFs& aFs )
    : iFs( aFs )
    {
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::ConstructL
// ---------------------------------------------------------
//
void CWmPersistentWidgetOrder::ConstructL()
    {
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::~CWmPersistentWidgetOrder
// ---------------------------------------------------------
//
CWmPersistentWidgetOrder::~CWmPersistentWidgetOrder()
    {
    CleanupArray();
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::LoadL
// ---------------------------------------------------------
//
void CWmPersistentWidgetOrder::LoadL()
    {
    // 1. empty the in-mempory storage
    CleanupArray();
    // 2. create stream for reading data from a file
    TFileName storeFileName;
    GetStoreFileNameL( storeFileName );
    CPermanentFileStore* fileStore = NULL;
    fileStore = CPermanentFileStore::OpenL(
            iFs, storeFileName, EFileRead );
    CleanupStack::PushL( fileStore );
    RStoreReadStream reader;
    reader.OpenLC( *fileStore, fileStore->Root() );
    // 3. read all contents from the stream
    TInt arrayCount = reader.ReadInt32L();
    while( iTagArray.Count() < arrayCount )
        {
        TInt32 uid = reader.ReadInt32L();
        TInt32 publisherIdLen = reader.ReadInt32L();
        HBufC16* publisherId = HBufC16::NewLC(publisherIdLen);
        TPtr16 publisherIdPtr = publisherId->Des();
        reader.ReadL( publisherIdPtr, publisherIdLen );
        iTagArray.AppendL( Tag( uid, publisherId ) );
        CleanupStack::Pop( publisherId );
        }
    // 4. cleanup
    CleanupStack::PopAndDestroy( &reader );
    CleanupStack::PopAndDestroy( fileStore );
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::StoreL
// ---------------------------------------------------------
//
void CWmPersistentWidgetOrder::StoreL( const RWidgetDataValues& aArray )
    {
    // 1. create stream for storing the data to a file
    TFileName storeFileName;
    GetStoreFileNameL( storeFileName );
    CPermanentFileStore* fileStore = CPermanentFileStore::ReplaceLC(
            iFs, storeFileName, EFileWrite );
    fileStore->SetTypeL( KPermanentFileStoreLayoutUid );
    RStoreWriteStream writer;
    TStreamId id = writer.CreateLC( *fileStore );
    // 2. write all content to the stream
    writer.WriteInt32L( aArray.Count() );
    for( TInt i=0; i<aArray.Count(); ++i )
        {
        TInt32 uid = aArray[i]->Uid().iUid;
        const TDesC16& publisherId = aArray[i]->HsContentInfo().PublisherId();
        writer.WriteInt32L( uid );
        writer.WriteInt32L( publisherId.Length() );
        writer.WriteL( publisherId, publisherId.Length() );
        }
    // 3. check available space and commit the stream
    TInt streamsize = writer.Sink()->SizeL();
    TBool belowCriticalLevel = SysUtil::DiskSpaceBelowCriticalLevelL(
            &iFs, streamsize, EDriveC );
    if( !belowCriticalLevel )
        {
        writer.CommitL();
        fileStore->SetRootL(id);
        fileStore->CommitL();        
        }
    // 4. cleanup
    CleanupStack::PopAndDestroy( &writer );
    CleanupStack::PopAndDestroy( fileStore );
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::CleanupArray
// ---------------------------------------------------------
//
void CWmPersistentWidgetOrder::CleanupArray()
    {
    for( TInt i=0; i<iTagArray.Count(); ++i )
        {
        delete iTagArray[i].iPublisherId;
        iTagArray[i].iPublisherId = 0;
        }
    iTagArray.Close();
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::GetStoreFileNameL
// ---------------------------------------------------------
//
void CWmPersistentWidgetOrder::GetStoreFileNameL(
        TDes& aPathBuf )
    {
    // get default drive for phone memory
    TInt driveNum = EDriveC;
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
        DriveInfo::EDefaultPhoneMemory, driveNum ) );
    // make sure the directory exists
    iFs.CreatePrivatePath( driveNum ); // ignore errors
    // build the store path
    TChar driveLetter;
    User::LeaveIfError( iFs.DriveToChar( driveNum, driveLetter ) );
    aPathBuf.Append( driveLetter );
    aPathBuf.Append( _L(":") );
    TFileName privatePath;
    User::LeaveIfError( iFs.PrivatePath( privatePath ) );
    aPathBuf.Append( privatePath );
    aPathBuf.Append( KStoreFileName );
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::IndexOf
// ---------------------------------------------------------
//
TInt CWmPersistentWidgetOrder::IndexOf( const CWmWidgetData& aWidgetData ) const
    {
    TInt found = KErrNotFound;
    for( TInt i=0; i<iTagArray.Count() && found<0; ++i )
        {
        if ( iTagArray[i].Matches( aWidgetData ) )
            found = i;
        }
    return found;
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::IsEmpty
// ---------------------------------------------------------
//
TBool CWmPersistentWidgetOrder::IsEmpty() const
    {
    return iTagArray.Count() == 0;
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::Tag::Tag
// ---------------------------------------------------------
//
CWmPersistentWidgetOrder::Tag::Tag(
        TInt32 aUid, HBufC16* aPublisherId )
    {
    iUid = aUid;
    iPublisherId = aPublisherId;
    }

// ---------------------------------------------------------
// CWmPersistentWidgetOrder::Tag::Matches
// ---------------------------------------------------------
//
TBool CWmPersistentWidgetOrder::Tag::Matches(
        const CWmWidgetData& aWidgetData ) const
    {
    return ( aWidgetData.Uid().iUid == iUid &&
            aWidgetData.HsContentInfo().PublisherId() == *iPublisherId );
    }


// end of file
