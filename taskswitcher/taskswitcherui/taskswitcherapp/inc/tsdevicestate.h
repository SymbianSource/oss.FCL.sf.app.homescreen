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
* Description:  Device state (touch mode, screen orientation) handler
 *
*/


#ifndef TSDEVICESTATE_H_
#define TSDEVICESTATE_H_

#include <e32base.h>
#include "tsappui.h"
#include "tsproplistener.h"

/**
 * Interface for getting notifications about screen orientation
 * and/or touch state etc. changes.
 */
class MTsDeviceStateObserver
    {
public:
    /**
     * Type of change.
     */
    enum TChangeType
        {
        // screen orientation (portrait <=> landscape) has changed
        EOrientation = 0x01,
        // touch enabled/disabled status has changed
        ETouchState = 0x02,
        // skin has changed
        ESkin = 0x04,
        // input device mode change
        EDeviceType = 0x08,
        // all of the above
        EAny = 0xFF
        };

    /**
     * Called when a change, to which the observer is registered,
     * has happened.
     */
    virtual void HandleDeviceStateChanged( TChangeType aChangeType ) = 0;
    };

/**
 * Class for keeping track of screen orientation and touch enabled/disabled changes.
 */
class CTsDeviceState :
    public CBase,
    public MTsPropertyChangeObserver
    {
public:
    /**
     * Enumeration for TouchState().
     */
    enum TTouchState
        {
        ETouchEnabled,
        ETouchDisabled
        };

    /**
     * Enumeration for Orientation().
     */
    enum TOrientation
        {
        EPortrait,
        ELandscape
        };

    /**
     * Enumeration for DeviceType().
     */
    enum TDeviceType
        {
        EHybrid = 0,
        EFullTouch  = 1
        };

    /**
     * Creates a new instance.
     */
    static CTsDeviceState* NewL();

    /**
     * Destructor.
     */
    ~CTsDeviceState();

    /**
     * Returns the current state of touch awareness.
     */
    TTouchState TouchState() const;

    /**
     * Returns the current screen orientation.
     */
    TOrientation Orientation() const;

    /**
     * Returns the current device input type.
     */
    TDeviceType DeviceType() const;

    /**
     * Registers an observer.
     * @param   aObserver   ref to observer
     * (same observer can be added several times with different mask if needed)
     * @param   aMask       bitmask composed from TChangeType values
     * (when to notify the observer)
     */
    void AddObserverL( MTsDeviceStateObserver& aObserver, TInt aMask );

    /**
     * Deregisters the given observer.
     * @param   aObserver   ref to observer
     */
    void RemoveObserver( MTsDeviceStateObserver& aObserver );

    /**
     * Called from appui.
     */
    void HandleResourceChange( TInt aType );
    
    //From MTsPropertyChangeObserver
public:
    /**
     * Observer interface for getting notifications about a P&S property change.
     */
	virtual void PropertyChanged( TUid aCategory, TUint aKey );
private:
    /**
     * Constructor.
     */
    CTsDeviceState();
    
    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();

    /**
     * Checks if touch is enabled or not and performs
     * appropriate actions.
     */
    void CheckTouchState();

    /**
     * Checks the screen orientation and performs
     * appropriate actions.
     */
    void CheckOrientation();
    
    /**
     * Checks the device input type.
     */
    void CheckDeviceType();
    
    /**
     * Notifies all observers that are registered for the given type.
     */
    void NotifyObservers( MTsDeviceStateObserver::TChangeType aType );

    class SObserver;

    /**
     * Identity function to perform comparison between observer array items.
     */
    static TBool ObserverIdentity( const SObserver& aA, const SObserver& aB );

private:
    /**
     * Current touch enabled/disabled state.
     */
    TTouchState iTouchState;
    
    /**
     * Current screen orientation.
     */
    TOrientation iOrientation;
    
    /**
     * Current the device input type.
     */
    TDeviceType iDeviceType;
    
    /**
     * Observer array entry struct.
     */
    struct SObserver
        {
        TInt iMask;
        MTsDeviceStateObserver* iObserver; // not owned
        SObserver( TInt aMask, MTsDeviceStateObserver* aObserver )
            : iMask( aMask ), iObserver( aObserver ) { }
        };
        
    /**
     * Observer array.
     */
    RArray<SObserver> iObservers;

    /**
     * Counter to indicate that we have not yet returned
     * from a previous HandleResourceChange.
     */
    TInt iResChangeActiveCount;
    
    /**
     * Flip status change observer.
     */
    CTsPropertyListener* iFlipStatusObserver;
    };

#endif // TSDEVICESTATE_H_
