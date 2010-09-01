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
* Description:  Inactivity monitor
*
*/


#ifndef _NXINACTIVITYMONITOR_H
#define _NXINACTIVITYMONITOR_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations

/**
 * User inactivity observer
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( MXnInactivityObserver )
    {
public:
    /**
     * Notifies inactivity timer is expired
     *
     * @since S60 5.0
     */    
    virtual void InactivityTimerExpired() = 0;
    };

/**
 * User inactivity monitor
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnInactivityMonitor ) : public CTimer 
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnInactivityMonitor* NewL( MXnInactivityObserver& aObserver );

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnInactivityMonitor* NewLC( MXnInactivityObserver& aObserver );

    /**
     * Destructor
     */
    ~CXnInactivityMonitor();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnInactivityMonitor( MXnInactivityObserver& aObserver );

private:
    // from CTimer
    
    void RunL();
    
public:
    // new functions
    
    /**
     * Starts inactivity monitoring 
     *
     * @since S60 5.0
     */
    void Start();

    /**
     * Stops inactivity monitoring 
     *
     * @since S60 5.0
     */    
    void Stop();
        
private:
    // data
    /** Inactivity observer, Not owned */
    MXnInactivityObserver& iObserver;
    };

#endif // _NXINACTIVITYMONITOR_H
