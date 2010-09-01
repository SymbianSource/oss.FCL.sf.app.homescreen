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

#ifndef __WMPROCESSMONITOR_
#define __WMPROCESSMONITOR_

//includes
#include <e32base.h>

// Forward declaration
class RProcess;


/**
 * CWmProcessMonitor 
 * Monitoring running process
 */
NONSHARABLE_CLASS( CWmProcessMonitor ) : public CActive
    {
public:
    /**
     * Two-phased constructors.
     */
    static CWmProcessMonitor* NewL();
    static CWmProcessMonitor* NewLC();
    
    /** Destructor */
    ~CWmProcessMonitor();
    
private:    
    /** constructor */
    CWmProcessMonitor();
    
    /** 2nd phase constructor */
    void ConstructL();
    
protected: // implementation of CActive
    /**
     * Implements cancellation of an outstanding request.
     * 
     * @see CActive::DoCancel
     */
    void DoCancel();
    
    /**
     * Handles an active object's request completion event.
     * 
     * @see CActive::RunL
     */
    void RunL();
    
    /**
     * RunError
     * 
     * @see CActive::RunError
     */
    TInt RunError(TInt aError);
    
public:
	/**
	 * Starts monitoring process
	 */
    void Monitor( RProcess& aProcess );
    
private:
    
	/** process to monitor*/
    RProcess iProcess;
    };

#endif // __WMPROCESSMONITOR_
