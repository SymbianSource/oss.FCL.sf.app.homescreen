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
* Description:
*
*/

//INCLUDE FILES
#include <e32svr.h>
#include <stdlib.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <bautils.h>
#include <aknsskininstance.h>
#include <swinstapi.h>
#include <swinstdefs.h>
#include <coemain.h>
#include <EIKENV.H> 
#include <widgetregistryclient.h> // widgetreqistry
#include "wmresourceloader.h"

#include "hscontentinfo.h"
#include "wmplugin.h"
#include "wmwidgetdata.h"
#include "wmunittest.h"
#include "wminstaller.h"
#include "wmmainContainer.h"
#include "wmlistbox.h"
 

// -----------------------------------------------------------------------------
// CWmUnitTest::UninstallL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::UninstallL( CStifItemParser& aItem )
    {
    TInt ret = KErrNone;
    CWmWidgetData* data = 
            iWmPlugin->iWmMainContainer->iWidgetsList->WidgetData();
    iUninstallUid = data->PublisherUid();
    iWmPlugin->WmInstaller().UninstallL( data );
    
    _RETURN("UninstallL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::CleanUninstall
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::CleanUninstall( CStifItemParser& /*aItem*/ )
    {
    if ( iTimer )
        {
        if ( iTimer->IsActive() )
            iTimer->Cancel();

        delete iTimer;
        iTimer = NULL;
        }
    _RETURN("CleanUninstall End", KErrNone );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::UninstallUidL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::UninstallUidL( CStifItemParser& /*aItem*/ )
    {
    TInt ret = KErrNone;

    TUid installUid = iWmPlugin->WmInstaller().UninstallUid(); 
    if ( installUid != iUninstallUid )
        {
        ret = KErrNotFound;
        }
    _RETURN("UninstallUidL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WaitUninstallToFinnishL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WaitUninstallToFinnishL( CStifItemParser& /*aItem*/ )
    {
    TInt ret = KErrNone;
    
    if ( iWmPlugin->WmInstaller().IsActive() )
        {
        StartTimerL();
        CActiveScheduler::Start();
        
        if ( iWmPlugin->WmInstaller().UninstallUid() != KNullUid ||
                iWmPlugin->WmInstaller().IsActive() )
            {
            ret = KErrGeneral;
            }
        }
    _RETURN("WaitUninstallToFinnishL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WasUnistalled
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WasUnistalledL( CStifItemParser& aItem )
    {
    TInt ret = KErrNone;
    TPtrC id;
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString( id );
        
    RWidgetRegistryClientSession widgetRegistry;
    User::LeaveIfError( widgetRegistry.Connect() );
    CleanupClosePushL( widgetRegistry );
    
    TInt uid = widgetRegistry.GetWidgetUidL( id );
    
    if ( uid != KErrNone )
        {
        ret = KErrGeneral;
        }
    CleanupStack::PopAndDestroy( &widgetRegistry );
    
    _RETURN("WasUnistalled End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::CancelUninstall
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::CancelUninstall( CStifItemParser& aItem )
    {
    TInt ret = KErrNone;
    iWmPlugin->WmInstaller().Cancel();
    
    TBool active = iWmPlugin->WmInstaller().IsActive();
    if ( active )
        {
        ret = KErrGeneral;
        }
    _RETURN("CancelUninstall End", ret );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::StartTimer
// -----------------------------------------------------------------------------
//
void CWmUnitTest::StartTimerL()
    {
    if ( !iTimer )
        iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    
    TTimeIntervalMicroSeconds32 delay( 4*1000*1000 ); // 2 sec
    iTimer->Start( delay, delay, TCallBack( Timeout, this ) );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::Timeout
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::Timeout( TAny* aPtr )
    {
    CWmUnitTest* self = static_cast<CWmUnitTest*>( aPtr );
    if ( self->iWmPlugin->WmInstaller().IsActive() )
        {
        return 1;
        }
    else
        {
        self->iTimer->Cancel();
        CActiveScheduler::Stop();
        return 0;
        }
    }
