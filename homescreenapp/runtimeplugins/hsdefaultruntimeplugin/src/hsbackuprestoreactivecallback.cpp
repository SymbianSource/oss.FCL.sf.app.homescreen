/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Widget's active callback in WidgetBackupRestore.
*
*/


// INCLUDE FILES
#include <bautils.h>

#include "hsbackuprestoreactivecallback.h"
#include "hsdatabase.h"

//  CONSTANTS

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHsBRActiveCallback* CHsBRActiveCallback::NewL()
{
    CHsBRActiveCallback* self =
        new( ELeave ) CHsBRActiveCallback();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHsBRActiveCallback::~CHsBRActiveCallback()
{
}

// ---------------------------------------------------------------------------
// Backup or restore has been finished.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::FinishBackupRestore()
{
	// allow database usage
	OpenDataBaseConnection();
	// TODO: Load widgets
}

// ---------------------------------------------------------------------------
// Backup or restore has been started.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::StartRestore()
{
	// allow database usage
	CloseDataBaseConnection();
	// TODO: Load widgets
}

// ---------------------------------------------------------------------------
// Inform that all data has been backed up or restored.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::AllSnapshotsSuppliedL()
{
    // No implementation needed. Must not leave    
}

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::ReceiveSnapshotDataL(
    TDriveNumber /*aDrive*/, TDesC8& /*aBuffer*/, TBool /*aLastSection*/)
{
	// No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Make a guess about data size.
// ---------------------------------------------------------------------------
//
TUint CHsBRActiveCallback::GetExpectedDataSize(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::GetSnapshotDataL(
    TDriveNumber /*aDrive*/, TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Initialize for backup.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::InitialiseGetBackupDataL(
    TDriveNumber /*aDrive*/)
{
    // Prevent database usage
    CloseDataBaseConnection();
    // TODO: Unload widgets
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::GetBackupDataSectionL(
    TPtr8& /*aBuffer*/, TBool& aFinished)
{
    // No data for active backup
    aFinished = ETrue;
}

// ---------------------------------------------------------------------------
// Initialize restore.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::InitialiseRestoreBaseDataL(
    TDriveNumber /*aDrive*/ )
{
    // No implementation needed
    //User::Leave( KErrNotSupported );
    CloseDataBaseConnection();
}

// ---------------------------------------------------------------------------
// Run state machine for restore. Receive stream from BUR engine and turn it
// to file(s).
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::RestoreBaseDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/ )
{
    // No implementation needed
    CloseDataBaseConnection();
}

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::InitialiseRestoreIncrementDataL(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::RestoreIncrementDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Called when restore is complete - sets data back to initial state.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::RestoreComplete(TDriveNumber /*aDrive*/)
{
    // No implementation needed
}

// ---------------------------------------------------------------------------
// Tidy up when operation is over.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::TerminateMultiStageOperation()
{
    // No implementation needed
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint CHsBRActiveCallback::GetDataChecksum(TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHsBRActiveCallback::CHsBRActiveCallback()
{
}

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::ConstructL()
{
}

// ---------------------------------------------------------------------------
// CHsBRActiveCallback::CloseDataBaseConnection()
//
// Close database connection.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::CloseDataBaseConnection()
{
    HsDatabase *db = HsDatabase::instance();
    db->setDataBaseBlocked(true);
    db->close();
}

// ---------------------------------------------------------------------------
// CHsBRActiveCallback::OpenDataBaseConnection()
//
// Open database connection.
// ---------------------------------------------------------------------------
//
void CHsBRActiveCallback::OpenDataBaseConnection()
{
    HsDatabase *db = HsDatabase::instance();
    db->setDataBaseBlocked(false);
    db->open();
}   
// End of file
