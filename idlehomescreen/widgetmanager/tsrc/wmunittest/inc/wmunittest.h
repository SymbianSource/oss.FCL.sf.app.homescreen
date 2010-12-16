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
* Description: STIF testclass declaration
*
*/

#ifndef WMUNITTEST_H
#define WMUNITTEST_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <testclassassert.h>
#include <s32file.h> // RFs

#include "wmwidgetorderdata.h"
#include <hscontentcontroller.h>
#include "wminstaller.h"

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// MACROS FOR THE TEST CASES
#define _LOG(a) iLog->Log(_L(a))
#define _RETURN(a, b) iLog->Log(_L("\treturn: %d"),b); \
                     iLog->Log(_L(a)); \
                     return b; \

#define _CLEANUPCHECK_BEGIN \
    CBase* __check = new (ELeave) CBase(); \
    CleanupStack::PushL( __check );

#define _CLEANUPCHECK_END \
    CleanupStack::PopAndDestroy( __check );


// Logging path & file
_LIT( KWmUnitTestLogPath, "\\logs\\testframework\\wmunittest\\" ); 
_LIT( KWmUnitTestLogFile, "wmunittest.txt" ); 

// FORWARD DECLARATIONS
class CWmPlugin;
class CWmPersistentWidgetOrder;
class CWmImageConverter;
class CHsContentInfo;
class CHsContentInfoArray;
class RWidgetDataValues;
class CWmInstaller;

// CLASS DECLARATION
/**
*  test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS(CWmUnitTest) : public CScriptBase,
                                 public MHsContentController
    {
    public: // Constructors and destructor
        /** static constructor */
        static CWmUnitTest* NewL( CTestModuleIf& aTestModuleIf );
        /** desctructor */
        ~CWmUnitTest();

    private: // private stuff
        /** default constructor */
        CWmUnitTest( CTestModuleIf& aTestModuleIf );
        /** 2nd phase constructor */
        void ConstructL();
        /** version check */
        void SendTestClassVersion();

    public: // Test run API

        /** From CScriptBase Runs a script line. */
        virtual TInt RunMethodL( CStifItemParser& aItem );
    
    public: // from MHsContentController

        
        TInt WidgetListL( CHsContentInfoArray& aArray );
        TInt WidgetListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );
        TInt ViewListL( CHsContentInfoArray& aArray );
        TInt ViewListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );
        TInt AppListL( CHsContentInfoArray& aArray );
        TInt AddWidgetL( CHsContentInfo& aInfo );
        TInt RemoveWidgetL( CHsContentInfo& aInfo );
        TInt AddViewL( CHsContentInfo& aInfo );
        TInt RemoveViewL( CHsContentInfo& aInfo );
        TInt ActivateViewL( CHsContentInfo& aInfo );
        TInt ActivateAppL( CHsContentInfo& aInfo );
        TInt ActiveViewL( CHsContentInfo& aInfo );
        TInt ActiveAppL( CHsContentInfo& aInfo );
        
    private: // Test methods

        // GENERAL
        
        // sets heap reference value
        TInt HeapSetL( CStifItemParser& aItem );
        // Log heap usage to file
        TInt LogHeapL( CStifItemParser& aItem );
        // compares heap against reference, logs warning if no match
        TInt HeapCheckL( CStifItemParser& aItem );
        // sets the mass of data for next test case
        TInt SetMassL( CStifItemParser& aItem );
		// install and uninstall wrt widget for tests
		TInt InstallWrtWidgetL( CStifItemParser& aItem );
		TInt UninstallWrtWidgetL( CStifItemParser& aItem );
		// test error note
		TInt ErrorPopupL( CStifItemParser& aItem );
		
        // TESTING THE PLUGIN
        
        // creates the WM plugin
        TInt PluginCreateL( CStifItemParser& aItem );
        // deletes WM plugin
        TInt PluginDeleteL( CStifItemParser& aItem );        
        // calls Activate method for WM plugin
        TInt PluginActivateL( CStifItemParser& aItem );
        // calls Deactivate method for WM plugin
        TInt PluginDeactivateL( CStifItemParser& aItem );
        // calls NotifyWidgetListChanged method for WM plugin
        TInt PluginWidgetsChangedL( CStifItemParser& aItem );
        // changes test content
        TInt ChangeContentL( CStifItemParser& aItem );
        
        // TESTING THE WIDGET SORTING AND ORDER
        
        // creates WM persistent widget order object
        TInt WidgetOrderCreateL( CStifItemParser& aItem );
        // deletes WM persistent widget order object
        TInt WidgetOrderDeleteL( CStifItemParser& aItem );
        // tests WM persistent widget order's LOAD method
        TInt WidgetOrderLoadL( CStifItemParser& aItem );
        // tests WM persistent widget order's SAVE method
        TInt WidgetOrderSaveL( CStifItemParser& aItem );
        // tests WM persistent widget order's SAVE method
        TInt WidgetOrderCheckIsEmptyL( CStifItemParser& aItem );
        // calls WM persistent widget order's API methods
        TInt WidgetOrderUseL( CStifItemParser& aItem );

        // TESTING IMAGE CONVERSION ROUTINES
       
        // creates image converter object
        TInt ImageConverterCreateL( CStifItemParser& aItem );
        // deletes image converter object
        TInt ImageConverterDeleteL( CStifItemParser& aItem );
        // calls image converter's main conversion method
        TInt ImageConverterConvertL( CStifItemParser& aItem );

        // TESTING UNISTALL
        TInt UninstallL( CStifItemParser& aItem );
        TInt WaitUninstallToFinnishL( CStifItemParser& aItem );
        TInt UninstallUidL( CStifItemParser& aItem );
        TInt CleanUninstall( CStifItemParser& aItem );
        TInt WasUnistalledL( CStifItemParser& aItem );
        TInt CancelUninstall( CStifItemParser& aItem );
        
    private: // helper functions for test cases

        void CreateContentInfoArrayL( CHsContentInfoArray& aArray, TInt aCount );
        void CreateWidgetDataArrayL( ROrderArray& aArray, TInt aCount );
        TUid UidFromString( const TDesC8& aUidString ) const;
        void StartTimerL();
        static TInt Timeout( TAny* aPtr );
        
    private: // members used in testing
        
        // file server
        RFs iFs;

        // the WM plugin instance
        CWmPlugin* iWmPlugin;

        // the WM persistent widget order instance
        CWmPersistentWidgetOrder* iWmWidgetOrder;

        // the Image converter instance
        CWmImageConverter* iImageConverter;
        
        // amount of heap allocated in HeapSet
        TInt iHeapSize;
        
        // image converter response ready
        TBool iConversionReady;
        
        // image converter result
        TInt iConversionError;
        
        // switch for active schedular wait
        TBool iWaiting;

        // multipurpose test data count
        TInt iMass;

        // uninstall uid
        TUid iUninstallUid;
        
        // timer for waiting uninstall to finnish
        CPeriodic* iTimer;
        
        // requested changes to the content
        TInt iChangeIndex;
        TInt iChangeType;
        HBufC16* iChangeValue;
    };


#endif      // WMUNITTEST_H

// End of File