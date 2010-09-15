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
* Implementation of the store for WidgetManager
*
*/

// INCLUDE FILES
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknBidiTextUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <gulicon.h>
#include <avkon.mbg>
#include <widgetmanager.mbg>
#include <apgcli.h>
#include <apgtask.h>
#include <widgetregistryclient.h> // widgetreqistry
#include <bacline.h>
#include <escapeutils.h> 
#include <browserlauncher.h>
#include <centralrepository.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "wmimageconverter.h"
#include "wmstore.h"
#include "wmcommon.h"
#include "wmplugin.h"
#include "wmresourceloader.h"
#include "wmmaincontainer.h"
#include "wmconfiguration.h"
#include "wmprocessmonitor.h"

// CONSTANTS

// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CWmStore::NewL
// ---------------------------------------------------------
//
CWmStore* CWmStore::NewL( CWmConfItem& aWmConfItem, 
        CWmResourceLoader& aResourceLoader )
	{
 	CWmStore* self = new (ELeave) CWmStore( aWmConfItem );
 	CleanupStack::PushL( self );    
    self->ConstructL( aResourceLoader );    
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------
// CWmStore::~CWmStore
// ---------------------------------------------------------
//
CWmStore::~CWmStore()
    {
    delete iIcon;
    delete iMask;
        
    // if MAknIconFileProvider was used to create image from icon string
    // then it'll try accessing imageconverter after bitmap deletion
    // for de-reference open file count, so it should be deleted last.
    delete iImageConverter;
    delete iProcessMonitor;
    delete iBrowserLauncher;
    }

// ---------------------------------------------------------
// CWmStore::CWmStore
// ---------------------------------------------------------
//
CWmStore::CWmStore( CWmConfItem& aWmConfItem )
    : iWmConfItem( aWmConfItem )
    {
    iIcon = NULL;
    iMask = NULL;
	iProcessMonitor = NULL;
	iBrowserLauncher = NULL;
    }

// ---------------------------------------------------------
// CWmStore::ConstructL
// ---------------------------------------------------------
//
void CWmStore::ConstructL( CWmResourceLoader& aResourceLoader )
    {
    // start image converter for the icon
    iImageConverter = CWmImageConverter::NewL();
    
    // get size of icon for double large style list
    TRect rect;
    TAknLayoutRect layout;
    
    layout.LayoutRect( rect,
        AknLayoutScalable_Avkon::list_double_large_graphic_pane_g1( 0 ).LayoutLine() );

    TSize iconSize( layout.Rect().Width(),
                    layout.Rect().Height() );
    
    // create icon
    TInt err = iImageConverter->HandleIconString(
            iconSize,
            iWmConfItem.Icon(),
            iIcon, 
            iMask,
            ETrue ); // force scaling for non scaleble icons
    
    if ( err != KErrNone )
        {
        // lets use basket icon
        // skin( 0x101f86e3 0x23f6 ):mif( z:\resource\apps\widgetmanager.mif 16388 16389 )
        _LIT( KSkinMifIconFormat, "skin( 0x%x 0x%x ):mif( %S %d %d )");
        const TInt KMaxIconDescriptorLength = 256;
        TBuf<KMaxIconDescriptorLength> buf;
        buf.Format( KSkinMifIconFormat(),
                 EAknsMajorGeneric, EAknsMinorGenericQgnPropLmShopping,
                 &aResourceLoader.IconFilePath(),
                 EMbmWidgetmanagerQgn_menu_shopping,
                 EMbmWidgetmanagerQgn_menu_shopping_mask );
        
        iImageConverter->HandleIconString(
            iconSize,
            buf,
            iIcon, 
            iMask,
            EFalse ); // force scaling for non scaleble icons
        }
    }

// ---------------------------------------------------------
// CWmStore::StoreIcon
// ---------------------------------------------------------
//
CFbsBitmap* CWmStore::StoreIcon()
    {
    return iIcon;
    }

// ---------------------------------------------------------
// CWmStore::StoreMask
// ---------------------------------------------------------
//
CFbsBitmap* CWmStore::StoreMask()
    {
    return iMask;
    }

// ---------------------------------------------------------
// CWmStore::Heading
// ---------------------------------------------------------
//
const TDesC& CWmStore::Heading()
    {
    return iWmConfItem.Title();
    }

// ---------------------------------------------------------
// CWmStore::Order
// ---------------------------------------------------------
//
TInt CWmStore::Order() const
    {
    return iWmConfItem.Order();
    }

// ---------------------------------------------------------
// CWmStore::ExecuteL
// ---------------------------------------------------------
//
void CWmStore::ExecuteL()
    {
    if ( iWmConfItem.Id() == EOvi )
        {
        if ( !iProcessMonitor )
            {
            iProcessMonitor = CWmProcessMonitor::NewL();
            }
        // if process monitor is active laucher was allready started.
        // this is for ignoring multiple presses
        if ( !iProcessMonitor->IsActive() )
            {
            RunOviL();
            }
        }
    else if ( iWmConfItem.Id() >= EStore1 && iWmConfItem.Id() < ELastItem )
        {
        // if this leaves it means we need to start browser
        // becouse application info was not given
        TRAPD( err, RunApplicationL() )
        if ( err != KErrNone )        
            {
            StartBrowserL( iWmConfItem.BrowserUrl() );
            }
        } 
    else
        {
        // Not supported
        User::Leave( KErrGeneral );
        }
    }
   
// ---------------------------------------------------------
// CWmStore::RunOviL
// ---------------------------------------------------------
//
void CWmStore::RunOviL()
    {
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    
    //get app info
    TApaAppInfo appInfo;
    if ( session.GetAppInfo( appInfo, iWmConfItem.AppUid() ) == KErrNone )
        {
        // Form parameter
        HBufC* param = HBufC::NewLC( iWmConfItem.AppParam().Length() );
        param->Des().Copy( iWmConfItem.AppParam() );
        
        // do the launch
        RProcess process;
        User::LeaveIfError( process.Create( appInfo.iFullName, *param ) );
    
        iProcessMonitor->Monitor( process );
        process.Resume();
    
        CleanupStack::PopAndDestroy( param );
        }
    
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------
// CWmStore::StartBrowserL
// ---------------------------------------------------------
//
void CWmStore::StartBrowserL( const TDesC& aUrl )
    {
    // already running/or request launch or url is empty
    if ( iBrowserLauncher || !aUrl.Length() ) { return; } 

    _LIT( KUrlPrefix, "4 ");
    HBufC* param = HBufC::NewLC( aUrl.Length() + KUrlPrefix().Length() );
    param->Des().Append( KUrlPrefix );
    param->Des().Append( aUrl );
        
    // Create browser launcher
    CBrowserLauncher* launcher = CBrowserLauncher::NewLC();

    // Asynchronous operation to launch the browser with given URL
    launcher->LaunchBrowserEmbeddedL( 
            *param, NULL, this, NULL );
    
    CleanupStack::Pop( launcher ); 
    iBrowserLauncher = launcher;
    launcher = NULL;

    CleanupStack::PopAndDestroy( param );
    }

// ---------------------------------------------------------
// CWmStore::HandleServerAppExit
// ---------------------------------------------------------
//
void CWmStore::HandleServerAppExit( TInt aReason )
    {
    MAknServerAppExitObserver::HandleServerAppExit( aReason );
    delete iBrowserLauncher;
    iBrowserLauncher = NULL;
    }

// ---------------------------------------------------------
// CWmStore::RunApplicationL
// ---------------------------------------------------------
//
void CWmStore::RunApplicationL()
    {
    CWmConfiguration::TStoreAppType type = iWmConfItem.AppType();
    if ( type == CWmConfiguration::EUnknown ||
        type >= CWmConfiguration::EReserved )
        {
        // Leave if not found
        User::Leave( KErrArgument );
        }
    
    switch( type )
        {
        case CWmConfiguration::ES60:
        case CWmConfiguration::EQt:    
            {
            StartProcessL( iWmConfItem.AppUid(), 
                    iWmConfItem.AppParam() );
            }
            break;
        case CWmConfiguration::ECwrt:
            {
            StartWidgetL( iWmConfItem.AppUid(), 
                    iWmConfItem.AppParam() );
            }
            break;
        case CWmConfiguration::EWrt:
            {
            StartWidgetL( iWmConfItem.AppUid(), KNullDesC );
            }
            break;
        }
    }

// ---------------------------------------------------------
// CWmStore::StartProcessL
// ---------------------------------------------------------
//
void CWmStore::StartProcessL( TUid aUid, const TDesC& aParam )
    {
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    
    //get app info
    TApaAppInfo appInfo;
    User::LeaveIfError( session.GetAppInfo( appInfo, aUid ) );
    // do the launch
    RProcess process;
    User::LeaveIfError( process.Create( appInfo.iFullName, aParam ) );
    process.Resume();
    
    CleanupStack::PopAndDestroy( &session );
    }

// ---------------------------------------------------------
// CWmStore::StartWidgetL
// ---------------------------------------------------------
//
void CWmStore::StartWidgetL( TUid aAppUid, const TDesC& aParams )
    {
    if ( aAppUid == KNullUid )
        User::Leave( KErrArgument );
    
    HBufC* params = aParams.AllocLC();
    
    RApaLsSession appArc;
    User::LeaveIfError( appArc.Connect() );
    CleanupClosePushL( appArc );
    
    TThreadId threadId;
    User::LeaveIfError( appArc.StartDocument( *params, aAppUid, threadId ) );
    
    CleanupStack::PopAndDestroy( &appArc );      
    CleanupStack::PopAndDestroy( params );
    }

// End of file
