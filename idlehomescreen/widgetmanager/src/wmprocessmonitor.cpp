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
*
*/

#include <e32base.h>
#include <e32std.h>

#include "wmprocessmonitor.h"


// ---------------------------------------------------------
// CWmProcessMonitor::NewL
// ---------------------------------------------------------
//
CWmProcessMonitor* CWmProcessMonitor::NewL( )
    {
    CWmProcessMonitor* self = CWmProcessMonitor::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmProcessMonitor::NewLC
// ---------------------------------------------------------
//
CWmProcessMonitor* CWmProcessMonitor::NewLC()
    {
    CWmProcessMonitor* self = new ( ELeave ) CWmProcessMonitor();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CWmProcessMonitor::CWmProcessMonitor
// ---------------------------------------------------------
//
CWmProcessMonitor::CWmProcessMonitor() 
	: CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------
// CWmProcessMonitor::ConstructL
// ---------------------------------------------------------
//
void CWmProcessMonitor::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CWmProcessMonitor::~CWmProcessMonitor
// ---------------------------------------------------------
//
CWmProcessMonitor::~CWmProcessMonitor()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CWmProcessMonitor::DoCancel
// ---------------------------------------------------------
//
void CWmProcessMonitor::DoCancel()
    {
    if ( IsActive() )
        {
        iProcess.LogonCancel( iStatus );
        }
    iProcess.Close();
    }

// ---------------------------------------------------------
// CWmProcessMonitor::RunL
// ---------------------------------------------------------
//
void CWmProcessMonitor::RunL()
    {
    iProcess.Close();
    }

// ---------------------------------------------------------
// CWmProcessMonitor::RunError
// ---------------------------------------------------------
//
TInt CWmProcessMonitor::RunError(TInt /*aError*/)
    {   
    iProcess.Close();
    return KErrNone;
    }


// ---------------------------------------------------------
// CWmProcessMonitor::Monitor
// ---------------------------------------------------------
//
void CWmProcessMonitor::Monitor( RProcess& aProcess )
    {
    iProcess.SetHandle( aProcess.Handle() );
    iProcess.Logon( iStatus );
    SetActive();
    }

