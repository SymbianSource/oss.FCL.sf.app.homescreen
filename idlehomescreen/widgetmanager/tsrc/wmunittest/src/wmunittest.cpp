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
* Description: This file contains testclass implementation.
*
*/

// INCLUDE FILES
#include <stiftestinterface.h>
#include <settingserverclient.h>

#include "wmunittest.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWmUnitTest::CWmUnitTest
// -----------------------------------------------------------------------------
//
CWmUnitTest::CWmUnitTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    iChangeValue = NULL;
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ConstructL
// -----------------------------------------------------------------------------
//
void CWmUnitTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KWmUnitTestLogPath, 
            KWmUnitTestLogFile,
            CStifLogger::ETxt,
            CStifLogger::EFile,
            EFalse );
    
    SendTestClassVersion();
    
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::NewL
// -----------------------------------------------------------------------------
//
CWmUnitTest* CWmUnitTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CWmUnitTest* self = new (ELeave) CWmUnitTest( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::~CWmUnitTest
// -----------------------------------------------------------------------------
//
CWmUnitTest::~CWmUnitTest()
    { 
    // Delete logger
    delete iLog; 
    // close file server
    iFs.Close();
    }

//-----------------------------------------------------------------------------
// CWmDeploymentClientTest::SendTestClassVersion
//-----------------------------------------------------------------------------
//
void CWmUnitTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("wmunittest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL() - polymorphic Dll entry point.
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    return ( CScriptBase* ) CWmUnitTest::NewL( aTestModuleIf );
    }


//  End of File
