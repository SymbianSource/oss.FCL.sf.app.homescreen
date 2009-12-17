/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Menu Framewrok ECom observer
*
*/

#ifndef C_ECOM_OBSERVER_H
#define C_ECOM_OBSERVER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class REComSession;

/**
 * Class defining callback interface.
 */

/**
 * MHNEcomInstallNotifierCallback is an interface contains function
 * executed when ECom plugin have changed. Interface is used by all
 * ECom observer implementations.
 *
 * @since S60 5.0
 * @ingroup group_mmextensions
 */
class MHNEcomInstallNotifierCallback
    {
public:
	
	/**
	 * Refreshes list of MultimediaMenu plugins implementations.
	 */
    virtual void EcomChangedL() = 0;    
    };

/**
 * Class for observing ECom server
 */

/**
 * CMMEcomObserver is a class represents ECom plugin. Class is
 * derived by all ECom plugin implemetations. Implementation
 * contains basic functionality related with ECom mechanism and
 * notification handling.
 *
 * @since S60 5.0
 * @ingroup group_mmextensions
 */
NONSHARABLE_CLASS( CMMEcomObserver ) : public CActive
    {
public:
    /**
     * Cancel and destroy.
     */
    ~CMMEcomObserver();

    /**
     * Two-phased constructor.
     * @param aCallback Pointer to ECom plugin callback.
     * @return Instance of the CMMEcomObserver class.
     */
    static CMMEcomObserver* NewL( MHNEcomInstallNotifierCallback* aCallback );

    /**
     * Two-phased constructor.
     * @param aCallback Pointer to ECom plugin callback.
     * @return Instance of the CMMEcomObserver class.
     */
    static CMMEcomObserver* NewLC( MHNEcomInstallNotifierCallback* aCallback );

private:
    /**
     * Default C++ constructor.
     */
    CMMEcomObserver();

    /**
     * Second-phase constructor.
     * @param aCallback Pointer to ECom plugin callback.
     */
    void ConstructL( MHNEcomInstallNotifierCallback* aCallback );

private:
    
    /**
     * From CActive. Handles completion.
     */
    void RunL();

    /**
     * Cancels active object.
     */
    void DoCancel();

    /**
     * Override to handle leaves from RunL(). Default implementation causes
     * the active scheduler to panic.
     * @param aError Currently error status.
     * @return Error code.
     */
    TInt RunError( TInt aError );

private:

    /**
     * ECom handler
     * Own.
     */
    REComSession* iSession;
    
    /**
     * Callback pointer.
     * Not own.
     */
    MHNEcomInstallNotifierCallback* iCallback;
    
    };

#endif // C_ECOM_OBSERVER_H
