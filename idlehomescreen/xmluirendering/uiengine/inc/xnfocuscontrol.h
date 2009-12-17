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
* Description:  Focus appearance drawer
*
*/


#ifndef _XNFOCUSCONTROL_H
#define _XNFOCUSCONTROL_H

// System includes
#include <e32base.h>

// User includes
#include "xninactivitymonitor.h"

// Forward declarations
class CXnAppUiAdapter;
class CWindowGc;

/**
 * Focus appearance drawer
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnFocusControl ) : public CBase,
    public MXnInactivityObserver
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnFocusControl* NewL( CXnAppUiAdapter& aAppUiAdapter );

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnFocusControl* NewLC( CXnAppUiAdapter& aAppUiAdapter );

    /**
     * Destructor
     */
    ~CXnFocusControl();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnFocusControl( CXnAppUiAdapter& aAppUiAdapter );
    
private:
    // from MXnInactivityObserver
    
    /**
     * @see MXnInactivityObserver
     */
    void InactivityTimerExpired();

public: 
    // new functions
    
    /**
     * Makes focus viisble
     * 
     * @since S60 5.0     
     * @param aVisible, ETrue visible, EFalse invisible
     */
    void MakeVisible( TBool aVisible );

    /**
     * Queries whether focus is visible
     * 
     * @since S60 5.0     
     * @return ETrue if focus visible, EFalse otherwise
     */    
    TBool IsVisible() const;
    
    /**
     * Draws the 1st state and 2nd state focus appearenace
     * 
     * @since S60 5.0
     * @param aRect Rect where to draw
     * @param aGc Context
     */
    void Draw( const TRect& aRect, CWindowGc& aGc ) const;

private:
    // new functions
    
    void DoMakeVisibleL( TBool aVisible );
    
private:
    // data
    
    /** AppUiAdapter, Not owned */
    CXnAppUiAdapter& iAppUiAdapter;
    /** Inactivity monitor, Owned */
    CXnInactivityMonitor* iMonitor;
    /** Flag to indicate whether it is allowed to show focus */
    TBool iVisible;
    /** Flag to indicate refusal */
    TBool iRefused;
    };

#endif // _XNFOCUSCONTROL_H
