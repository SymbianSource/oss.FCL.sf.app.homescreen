/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Home screen Backup/Restore observer.
*
*/

#include <connect/sbdefs.h>
#include "hsdatabase.h"
#include "hsbackuprestoreobserver.h"
#include "hsbackuprestoreactivecallback.h"

using namespace conn;

// -----------------------------------------------------------------------------
// CHsBackupRestoreObserver::CHsBackupRestoreObserver
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHsBackupRestoreObserver::CHsBackupRestoreObserver() : CActive(EPriorityNormal)
{    
	iLastType = conn::EBURUnset;
    iProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey);
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CHsBackupRestoreObserver::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CHsBackupRestoreObserver::ConstructL()
{

}

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::NewL
// ---------------------------------------------------------------------------
//
CHsBackupRestoreObserver* CHsBackupRestoreObserver::NewL()
{
    CHsBackupRestoreObserver* self = new (ELeave)
    CHsBackupRestoreObserver();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
}

// ---------------------------------------------------------------------------
// CHsBackupRestoreObserver::~CHsBackupRestoreObserver
// ---------------------------------------------------------------------------
//
CHsBackupRestoreObserver::~CHsBackupRestoreObserver()
{
    Cancel();
    iProperty.Close();        
    delete iCallBack;
    delete iActiveBackupClient;
}

// ---------------------------------------------------------------------------
// Handles changes in backup state.
// ---------------------------------------------------------------------------
//
void CHsBackupRestoreObserver::HandleBackupStateL( const TInt aValue )
{
    const TInt type = aValue & conn::KBURPartTypeMask;

    // Test if the device is going into backup or restore mode, and we are
    // required to participate.
    if ( ( type == conn::EBURBackupFull || type == conn::EBURRestoreFull ) ||
         ( type == conn::EBURBackupPartial || type == conn::EBURRestorePartial ) )
    {
        if ( !iCallBack )
        {
            iCallBack = CHsBRActiveCallback::NewL();
        }

        if ( !iActiveBackupClient )
        {
            iActiveBackupClient = conn::CActiveBackupClient::NewL( iCallBack );

            if ( ( type == conn::EBURBackupPartial || 
                   type == conn::EBURRestorePartial ) &&
                 !iActiveBackupClient->DoesPartialBURAffectMeL() )
            {
                delete iCallBack;
                iCallBack = NULL;
                delete iActiveBackupClient;
                iActiveBackupClient = NULL;
                return;
            }
            else if(type == conn::EBURRestorePartial || type == conn::EBURRestoreFull)
            {
            	iCallBack->StartRestore();
            }
        }

        iActiveBackupClient->ConfirmReadyForBURL( KErrNone );
    }
    else
    {
        if ( type == conn::EBURNormal )
        {
            if ( (iLastType == conn::EBURBackupFull || iLastType == conn::EBURBackupPartial) ||
            	 (iLastType == conn::EBURRestorePartial || iLastType == conn::EBURRestoreFull) )
            {
                iCallBack->FinishBackupRestore();
            }         
            // delete once back to normal.
            delete iCallBack;
            iCallBack = NULL;
            delete iActiveBackupClient;
            iActiveBackupClient = NULL;
        }
    }

    iLastType = type;
}

// ---------------------------------------------------------------------------
// Subsribes notifications of backup/restore p&s key.
// ---------------------------------------------------------------------------
//
void CHsBackupRestoreObserver::SubscribePSKey()
{
    Cancel();

    iProperty.Subscribe( iStatus );
    SetActive();
}
    
// ---------------------------------------------------------------------------
// CHsBackupRestoreObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CHsBackupRestoreObserver::DoCancel()
{
    iProperty.Cancel( );
}

// ---------------------------------------------------------------------------
// CHsBackupRestoreObserver::RunL
// ---------------------------------------------------------------------------
//
void CHsBackupRestoreObserver::RunL()
{   
    if ( iStatus.Int() == KErrNone )
    {
        TInt currentValue = KErrNone;
        iProperty.Get( currentValue );

        HandleBackupStateL( currentValue );
    }
    // Re-subscribe notifications.
    SubscribePSKey();
}    

// ---------------------------------------------------------------------------
// CHsBackupRestoreObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CHsBackupRestoreObserver::RunError( TInt /*aError*/)
{
    // No need to do anything      
    return KErrNone;
}
    
// End of File
