/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Subscribes properties from P&S
*
*/

#ifndef CMMNOMEMORY_H
#define CMMNOMEMORY_H

#include <e32base.h>	// For CActive, link against: euser.lib

/**

 *  An active object for generating a message.
 *
 *  It generates an appropriate message for user
 *  when Matrix Menu is out of memory.
 *
 *  @since S60 v5.0
 *  @ingroup group_matrixmenu
 */
NONSHARABLE_CLASS( CMmNoMemory ) : public CActive
    {
public:
    ~CMmNoMemory();

    static CMmNoMemory* NewL();

    static CMmNoMemory* NewLC();

public:
    // New functions
	
    /*
     * Submit a service request.
     * It completes on iStatus to generate an event.
     */
    void Start();

private:
    CMmNoMemory();

    void ConstructL();

private:
    // From CActive
    
    /**
     * Handle completion.
     * It leaves (KErrNoMemory) to generate an appropriate message.
     */
    void RunL();

    void DoCancel();

    TInt RunError(TInt aError);

private:
    };

#endif // CMMNOMEMORY_H
