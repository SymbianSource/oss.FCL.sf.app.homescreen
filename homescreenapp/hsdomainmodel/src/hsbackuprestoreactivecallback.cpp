/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active callback for Backup/Restore observer.
*
*/


// INCLUDE FILES
#include <bautils.h>

#include "hsbackuprestoreactivecallback.h"
#include "hsbackuprestoreobserver.h"

//  CONSTANTS

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHsBURActiveCallback* CHsBURActiveCallback::NewL(HsBackupRestoreObserver* observer)
{
    CHsBURActiveCallback* self =
        new( ELeave ) CHsBURActiveCallback(observer);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHsBURActiveCallback::~CHsBURActiveCallback()
{
}

// ---------------------------------------------------------------------------
// Inform that all data has been backed up or restored.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::AllSnapshotsSuppliedL()
{
    // No implementation needed. Must not leave    
}

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::ReceiveSnapshotDataL(
    TDriveNumber /*aDrive*/, TDesC8& /*aBuffer*/, TBool /*aLastSection*/)
{
	// No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Make a guess about data size.
// ---------------------------------------------------------------------------
//
TUint CHsBURActiveCallback::GetExpectedDataSize(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::GetSnapshotDataL(
    TDriveNumber /*aDrive*/, TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Initialize for backup.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::InitialiseGetBackupDataL(
    TDriveNumber /*aDrive*/)
{
    iObserver->backupRestoreStarted();    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::GetBackupDataSectionL(
    TPtr8& /*aBuffer*/, TBool& aFinished)
{
    // No data for active backup
    aFinished = ETrue;
}

// ---------------------------------------------------------------------------
// Initialize restore.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::InitialiseRestoreBaseDataL(
    TDriveNumber /*aDrive*/ )
{
    // No implementation needed
    User::Leave( KErrNotSupported );    
}

// ---------------------------------------------------------------------------
// Run state machine for restore. Receive stream from BUR engine and turn it
// to file(s).
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::RestoreBaseDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/ )
{
    // No implementation needed
    User::Leave( KErrNotSupported );  
}

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::InitialiseRestoreIncrementDataL(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::RestoreIncrementDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// Called when restore is complete - sets data back to initial state.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::RestoreComplete(TDriveNumber /*aDrive*/)
{
    // No implementation needed
}

// ---------------------------------------------------------------------------
// Tidy up when operation is over.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::TerminateMultiStageOperation()
{
    // No implementation needed
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint CHsBURActiveCallback::GetDataChecksum(TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHsBURActiveCallback::CHsBURActiveCallback(HsBackupRestoreObserver* observer)
{
	iObserver = observer;
}

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHsBURActiveCallback::ConstructL()
{
}
 
// End of file
