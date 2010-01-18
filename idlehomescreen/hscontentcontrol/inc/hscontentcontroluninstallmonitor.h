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

#ifndef HSCONTENTCONTROLUNINSTALLMONITOR_H
#define HSCONTENTCONTROLUNINSTALLMONITOR_H

// System includes
#include <e32base.h>
#include <e32property.h>

// User includes
#include "hscontentcontroluninstallobserver.h"

// Forward declarations

// Class declaration
/** 
* @class CHsContentControlUninstallMonitor
* 
* @brief An instance of class CHsContentControlUninstallMonitor which listens for 
*  uninstall event from SWI.
*  
* @lib hscontentcontrol.lib
*/
NONSHARABLE_CLASS( CHsContentControlUninstallMonitor ) : public CActive
    {
public:     // constructors and destructor
    static CHsContentControlUninstallMonitor* NewL( 
                MHsContentControlUninstallObserver& aObs );
    ~CHsContentControlUninstallMonitor();

protected:  // from CActive
    void DoCancel();
    void RunL();
    
private:    // new functions
    CHsContentControlUninstallMonitor( 
            MHsContentControlUninstallObserver& aObs );
    void ConstructL();

private:     // data
    MHsContentControlUninstallObserver& iObs;
    RProperty iSwUninstallKey;
    };

#endif // HSCONTENTCONTROLUNINSTALLMONITOR_H

