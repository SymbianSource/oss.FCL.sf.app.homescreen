/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class to listen SWI uninstall operation.
*
*/

#include "hscontentcontroluninstallmonitor.h"
#include <SWInstallerInternalPSKeys.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::NewL()
// ---------------------------------------------------------------------------
//
CHsContentControlUninstallMonitor* CHsContentControlUninstallMonitor::NewL( 
        MHsContentControlUninstallObserver& aObs )
    {
    CHsContentControlUninstallMonitor* self = 
        new ( ELeave ) CHsContentControlUninstallMonitor( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::~CHsContentControlUninstallMonitor()
// ---------------------------------------------------------------------------
//
CHsContentControlUninstallMonitor::~CHsContentControlUninstallMonitor()
    {
    Cancel();
    iSwUninstallKey.Close();
    }

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::DoCancel()
// ---------------------------------------------------------------------------
//
void CHsContentControlUninstallMonitor::DoCancel()
    {
    if ( IsActive() )
        {
        iSwUninstallKey.Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::RunL()
// ---------------------------------------------------------------------------
//
void CHsContentControlUninstallMonitor::RunL()
    {
    iSwUninstallKey.Subscribe( iStatus );
    SetActive();
    
    TInt value = 0;
    if( iSwUninstallKey.Get( value ) == KErrNone )
        {
        TUid uid( KNullUid );
        uid.iUid = value;
        iObs.HandleUninstallEvent( uid );
        }
    }

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::CHsContentControlUninstallMonitor()
// ---------------------------------------------------------------------------
//
CHsContentControlUninstallMonitor::CHsContentControlUninstallMonitor( 
    MHsContentControlUninstallObserver& aObs )
    : CActive( CActive::EPriorityStandard ),
    iObs( aObs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CHsContentControlUninstallMonitor::ConstructL()
// ---------------------------------------------------------------------------
//
void CHsContentControlUninstallMonitor::ConstructL()
    {
    if ( KErrNone == iSwUninstallKey.Attach( 
        KPSUidSWInstallerUiNotification, KSWInstallerUninstallation ) )
        {
        iSwUninstallKey.Subscribe( iStatus );
        SetActive();
        }
    }

// End of file
