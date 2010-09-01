/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application key detecting helper class
*  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*
*/

#ifndef CMMAPPKEYHANLDER_H
#define CMMAPPKEYHANDLER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <w32std.h>

/**
 *  @ingroup group_matrixmenu
 *  
 *  CMmAppkeyHandler is a helper class. It is a low priority active object.
 *  It is used to detect quick and frequent usage of the application key.
 *  To detect this state the IsActive() method should be invoked. If IsActive() returns
 *  ETrue, this means there is heavy processing which blocks the active object completion,
 *  which may indicate that appkey is being frequently pressed.
 *  
 *  @since S60 v5.0
 */
class CMmAppkeyHandler : public CActive
	{
public:
    /**
     * Cancel and destroy.
     */
	~CMmAppkeyHandler();

    /**
     * Two-phased constructor.
     * @return instance of CMmAppkeyHandler.
     */
	static CMmAppkeyHandler* NewL();

    /**
     * Two-phased constructor.
     * @return instance of CMmAppkeyHandler.
     */
	static CMmAppkeyHandler* NewLC();

public:
	// New functions
	
    /**
     * Function for making the initial request.
     */
	void StartL();

private:
    /**
     * C++ constructor.
     */
	CMmAppkeyHandler();

    /**
     * Second-phase constructor.
     */
	void ConstructL();
	
private:
    /**
     * From CActive.
     * Handle completion.
     */
	void RunL();

    /**
     * How to cancel me.
     * Left without implementation because the AO completes itself in StartL;
     */
	void DoCancel();
    
	/**
     * Override to handle leaves from RunL(). Default implementation causes
     * the active scheduler to panic.
     * 
     * @param aError Error occurred;
     * @return Error code.
     */
	TInt RunError(TInt aError);

	};

#endif // CMMAPPKEYHANLDER_H
