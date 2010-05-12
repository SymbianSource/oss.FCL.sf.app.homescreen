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
* Description:   Utility class to listen SWI operations.
*
*/

#ifndef HSCONTENTCONTROLSWILISTENER_H
#define HSCONTENTCONTROLSWILISTENER_H

// System includes
#include <e32base.h>
#include <e32property.h>

// User includes
#include "hscontentcontrolswiobserver.h"

// Forward declarations

// Class declaration
/** 
* @class CHsContentControlSwiListener
* 
* @brief An instance of class CHsContentControlSwiListener which listens for 
*  SWI operations (installation, uninstallation , restore).
*  
* @lib hscontentcontrol.lib
*/
NONSHARABLE_CLASS( CHsContentControlSwiListener ) : public CActive
    {
public:     // constructors and destructor
    static CHsContentControlSwiListener* NewL( 
                MHsContentControlSwiObserver& aObs );
    ~CHsContentControlSwiListener();

protected:  // from CActive
    void DoCancel();
    void RunL();

private:    // new functions
   CHsContentControlSwiListener( 
            MHsContentControlSwiObserver& aObs );
    void ConstructL();

private:     // data
    MHsContentControlSwiObserver& iObs;
    RProperty iSwInstallKey;
    };

#endif // HSCONTENTCONTROLSWILISTENER_H

