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
* widget manager plugin implementation
*
*/

#include <e32base.h>

#include "wminstaller.h"
#include "wmwidgetdata.h"


// CONSTANTS
_LIT8( KWrtMime, "application/x-nokia-widget");

// ---------------------------------------------------------
// CWmInstaller::NewL
// ---------------------------------------------------------
//
CWmInstaller* CWmInstaller::NewL()
    {
    CWmInstaller* self = CWmInstaller::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmInstaller::NewLC
// ---------------------------------------------------------
//
CWmInstaller* CWmInstaller::NewLC()
    {
    CWmInstaller* self = new ( ELeave ) CWmInstaller();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CWmInstaller::CWmInstaller
// ---------------------------------------------------------
//
CWmInstaller::CWmInstaller() : CActive( EPriorityStandard )
    {
    iUid = KNullUid;
    iIdle = NULL;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CWmInstaller::ConstructL
// ---------------------------------------------------------
//
void CWmInstaller::ConstructL()
    {
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------
// CWmInstaller::~CWmInstaller
// ---------------------------------------------------------
//
CWmInstaller::~CWmInstaller()
    {
    Cancel();

    if ( iIdle && iIdle->IsActive() )
        {
        iIdle->Cancel();
        }
    delete iIdle;
    }

// ---------------------------------------------------------
// CWmInstaller::DoCancel
// ---------------------------------------------------------
//
void CWmInstaller::DoCancel()
    {
    if ( IsActive() )
        {
        iInstaller.CancelAsyncRequest( 
                SwiUI::ERequestSilentUninstall );

        // close session
        iInstaller.Close();
        }
    }

// ---------------------------------------------------------
// CWmInstaller::RunL
// ---------------------------------------------------------
//
void CWmInstaller::RunL()
    {
    // close SWI session
    if ( iIdle && iIdle->IsActive() )
        {
        iIdle->Cancel();
        }
    iIdle->Start( TCallBack( CloseSwiSession, this ) );
    }

// ---------------------------------------------------------
// CWmInstaller::CloseSwiSession
// ---------------------------------------------------------
//
TInt CWmInstaller::CloseSwiSession( TAny* aPtr )
    {
    CWmInstaller* self = static_cast< CWmInstaller* >( aPtr );
    if ( self->iIdle->IsActive() )
      {
      self->iIdle->Cancel(); 
      }
    
    self->iUid = KNullUid;
    
    self->iInstaller.Close();
    return KErrNone;
    }

// ---------------------------------------------------------
// CWmInstaller::RunError
// ---------------------------------------------------------
//
TInt CWmInstaller::RunError(TInt /*aError*/)
    {   
    // close SWI session
    if ( iIdle && iIdle->IsActive() )
        {
        iIdle->Cancel();
        }
    iIdle->Start( TCallBack( CloseSwiSession, this ) );

    return KErrNone;
    }

// ---------------------------------------------------------
// CWmInstaller::UninstallL
// ---------------------------------------------------------
//
void CWmInstaller::UninstallL( CWmWidgetData* aData )
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        User::LeaveIfError( iInstaller.Connect() );
        iUid = aData->PublisherUid();
        SwiUI::TUninstallOptions optionsUninstall;
        optionsUninstall.iBreakDependency = SwiUI::EPolicyAllowed;
        optionsUninstall.iKillApp = SwiUI::EPolicyAllowed;
        SwiUI::TUninstallOptionsPckg uninstallOptionsPkg( optionsUninstall );
        iInstaller.SilentUninstall( iStatus, iUid, 
                                uninstallOptionsPkg, KWrtMime );
        
        aData->VisualizeUninstallL();
        SetActive();
        }
    }

// ---------------------------------------------------------
// CWmInstaller::Uid
// ---------------------------------------------------------
//
TUid CWmInstaller::UninstallUid()
    {
    if ( IsActive() )
        {
        return iUid;
        }
    else
        {
        return KNullUid;
        }
    }
