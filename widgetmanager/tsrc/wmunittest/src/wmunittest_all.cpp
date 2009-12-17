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
* Description: list of all test blocks
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
#include "wmunittest.h"
#include "wmresourceloader.h"

// CONSTANTS
_LIT( KHexPrefix,     "0x" );
_LIT8( KWrtMime, "application/x-nokia-widget");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWmUnitTest::RunMethodL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {
        // general test blocks
        ENTRY( "HeapSet", CWmUnitTest::HeapSetL ),
        ENTRY( "LogHeap", CWmUnitTest::LogHeapL ),
        ENTRY( "HeapCheck", CWmUnitTest::HeapCheckL ),
        ENTRY( "SetMass", CWmUnitTest::SetMassL ),
        ENTRY( "ErrorPopup", CWmUnitTest::ErrorPopupL ),
        
		// install/uninstall wrt widget for test
		ENTRY( "InstallWrtWidget", CWmUnitTest::InstallWrtWidgetL ),
		ENTRY( "UninstallWrtWidget", CWmUnitTest::UninstallWrtWidgetL ),

        // test blocks for the plugin
        ENTRY( "PluginCreate", CWmUnitTest::PluginCreateL ),
        ENTRY( "PluginDelete", CWmUnitTest::PluginDeleteL ),
        ENTRY( "PluginActivate", CWmUnitTest::PluginActivateL ),
        ENTRY( "PluginDeactivate", CWmUnitTest::PluginDeactivateL ),
        ENTRY( "PluginWidgetsChanged", CWmUnitTest::PluginWidgetsChangedL ),

        // test blocks for the persistent widget order class
        ENTRY( "WidgetOrderCreate", CWmUnitTest::WidgetOrderCreateL ),
        ENTRY( "WidgetOrderDelete", CWmUnitTest::WidgetOrderDeleteL ),
        ENTRY( "WidgetOrderLoad", CWmUnitTest::WidgetOrderLoadL ),
        ENTRY( "WidgetOrderSave", CWmUnitTest::WidgetOrderSaveL ),
        ENTRY( "WidgetOrderCheckIsEmpty", CWmUnitTest::WidgetOrderCheckIsEmptyL ),
        ENTRY( "WidgetOrderUse", CWmUnitTest::WidgetOrderUseL ),
        
        // test blocks for the image converter
        ENTRY( "ImageConverterCreate", CWmUnitTest::ImageConverterCreateL ),
        ENTRY( "ImageConverterDelete", CWmUnitTest::ImageConverterDeleteL ),
        ENTRY( "ImageConverterConvert", CWmUnitTest::ImageConverterConvertL ),
        ENTRY( "ImageConverterWaitResult", CWmUnitTest::ImageConverterWaitResultL ),
        ENTRY( "ImageConverterConversionMethod", CWmUnitTest::ImageConverterConversionMethodL ),
        ENTRY( "CancelConvert", CWmUnitTest::CancelConvertL ),
        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::HeapSetL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::HeapSetL( CStifItemParser& /*aItem*/ )
    {
    User::Heap().AllocSize( iHeapSize );
    _RETURN("HeapSetL End", KErrNone);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::LogHeapL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::LogHeapL( CStifItemParser& aItem )
    {
    TPtrC inputStr;
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    User::LeaveIfError( aItem.GetNextString( inputStr ) );

    TInt size = 0;
    User::Heap().AllocSize( size );
    iLog->Log( _L("Heap usage, %S: %d"), &inputStr, size );
    _RETURN("LogHeapL End", KErrNone);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::HeapCheckL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::HeapCheckL( CStifItemParser& /*aItem*/ )
    {
    TInt ret = KErrNone;
    TInt heapSizeAtCheckpoint;
    User::Heap().AllocSize( heapSizeAtCheckpoint );
    if ( iHeapSize != heapSizeAtCheckpoint )
        {
        //ret = KErrTooBig; <-- not reliable
        _LOG("Warning: potential memory leak detected!!!");
        }
    _RETURN("HeapCheckL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::SetMassL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::SetMassL( CStifItemParser& aItem )
    {
    aItem.GetNextInt( iMass );
    _RETURN("SetMassL End", KErrNone);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::InstallWrtWidgetL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::InstallWrtWidgetL( CStifItemParser& aItem )
    {
	TInt ret = KErrNone;
    TPtrC inputStr;
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    User::LeaveIfError( aItem.GetNextString( inputStr ) );
    if ( inputStr.Length() == 0 ) User::Leave( KErrArgument );
	
	// install widget
    SwiUI::RSWInstSilentLauncher installer;
    User::LeaveIfError( installer.Connect() );
    CleanupClosePushL( installer );

    SwiUI::TInstallReqPckg installReqPckg;
    SwiUI::TInstallOptions options;
    TRequestStatus status = KRequestPending;
    
    options.iUpgrade = SwiUI::EPolicyAllowed;
    options.iOptionalItems = SwiUI::EPolicyAllowed;
    options.iOCSP = SwiUI::EPolicyAllowed;
    options.iIgnoreOCSPWarnings = SwiUI::EPolicyAllowed;
    options.iUntrusted = SwiUI::EPolicyAllowed;
    options.iPackageInfo = SwiUI::EPolicyAllowed;
    options.iCapabilities = SwiUI::EPolicyAllowed;
    options.iKillApp = SwiUI::EPolicyAllowed;
    options.iOverwrite = SwiUI::EPolicyAllowed;
    options.iDownload = SwiUI::EPolicyAllowed;
    options.iDrive = 'C';
    options.iLang = User::Language();
    options.iUsePhoneLang = ETrue;
    options.iUpgradeData = SwiUI::EPolicyAllowed;
    
    SwiUI::TInstallOptionsPckg optionsPkg( options );
    installer.SilentInstall( status , inputStr, optionsPkg );
    User::WaitForRequest( status );
    ret = status.Int();
	CleanupStack::PopAndDestroy( &installer );
    _RETURN("InstallWrtWidgetL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::UninstallWrtWidgetL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::UninstallWrtWidgetL( CStifItemParser& aItem )
    {
    TInt ret = KErrNone;
	TPtrC inputStr;
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    User::LeaveIfError( aItem.GetNextString( inputStr ) );
    if ( inputStr.Length() == 0 ) User::Leave( KErrArgument );

	TUid uid;
	TLex lex( inputStr );
	lex.SkipSpaceAndMark();
	TPtrC mtoken = lex.MarkedToken();
	TInt pos = mtoken.FindF( KHexPrefix );
	if ( pos == 0 )
		{
		TLex lex( mtoken.Mid( KHexPrefix().Length() ) );
		TUint id = 0;
		ret = lex.Val( id, EHex );
		uid = TUid::Uid( (TInt)id );
		}
	else
		{
		TInt id( 0 );
		ret = lex.Val( id );
		uid.iUid = id;
		}

	if ( ret == KErrNone )
		{
		// Uninstall widget
		SwiUI::RSWInstSilentLauncher installer;
		User::LeaveIfError( installer.Connect() );
		CleanupClosePushL( installer );
		
		TRequestStatus status = KRequestPending;
		SwiUI::TUninstallOptions optionsUninstall;
		optionsUninstall.iBreakDependency = SwiUI::EPolicyAllowed;
		optionsUninstall.iKillApp = SwiUI::EPolicyAllowed;
		SwiUI::TUninstallOptionsPckg uninstallOptionsPkg( optionsUninstall );
		
		installer.SilentUninstall( status, uid, uninstallOptionsPkg, KWrtMime );
		User::WaitForRequest( status );				
		CleanupStack::PopAndDestroy( &installer );
		}
	
    _RETURN("UninstallWrtWidgetL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ErrorPopup
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ErrorPopupL( CStifItemParser& /*aItem*/ )
    {
    CWmResourceLoader* res = CWmResourceLoader::NewL( *CEikonEnv::Static() );
    res->ErrorPopup(KErrNone);
    delete res;
    _RETURN("ErrorPopupL End", KErrNone);
    }
// End of File
