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
#include <eikenv.h>

#include "wminstaller.h"
#include "wmwidgetdata.h"
#include "wmplugin.h"


// CONSTANTS
_LIT8( KWrtMime, "application/x-nokia-widget");

/* 
  Note: 
  same mime type as above is used for wgz & wgt in wk9.
  \ext\mw\cwrt\app\platform\s60\WidgetRecognizer\
  Uninstalltion will fail with KErrNotFound for now.
*/
_LIT8( KCWrtMime, "application/widget");

// ---------------------------------------------------------
// CWmInstaller::NewL
// ---------------------------------------------------------
//
CWmInstaller* CWmInstaller::NewL( CWmPlugin& aWmPlugin )
    {
    CWmInstaller* self = CWmInstaller::NewLC( aWmPlugin );
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmInstaller::NewLC
// ---------------------------------------------------------
//
CWmInstaller* CWmInstaller::NewLC( CWmPlugin& aWmPlugin )
    {
    CWmInstaller* self = new ( ELeave ) CWmInstaller( aWmPlugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CWmInstaller::CWmInstaller
// ---------------------------------------------------------
//
CWmInstaller::CWmInstaller( CWmPlugin& aWmPlugin ) :
    CActive( EPriorityStandard ),
    iWmPlugin( aWmPlugin )
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
    delete iMime;
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
    // if error has occurred, stop uninstallation animation.
    if ( iStatus.Int() != KErrNone )
        {
        CWmWidgetData* widget = 
                iWmPlugin.GetUninstalledWidgetByUid( iUid );
        if ( widget )
            {
            widget->StopUninstallAnimationL();
            }
        
        // display error note
        CEikonEnv::Static()->HandleError( iStatus.Int() );
        }
    
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
    else if ( !aData || aData->PublisherUid() == KNullUid ||
        aData->WrtType() == CWmWidgetData::EUnIdentified )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        delete iMime;
        iMime = NULL;
        User::LeaveIfError( iInstaller.Connect() );
        iMime = ( ( aData->WrtType() == CWmWidgetData::EWgt ) ?
            KCWrtMime().AllocL() : KWrtMime().AllocL() );
        iUid = aData->PublisherUid();
        SwiUI::TUninstallOptions optionsUninstall;
        optionsUninstall.iBreakDependency = SwiUI::EPolicyAllowed;
        optionsUninstall.iKillApp = SwiUI::EPolicyAllowed;
        SwiUI::TUninstallOptionsPckg uninstallOptionsPkg( optionsUninstall );
        iInstaller.SilentUninstall( iStatus, iUid, 
                                uninstallOptionsPkg, *iMime );
        
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
