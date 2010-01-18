/*
 * ============================================================================
 *  Name        : hgteleportdevicestate.h
 *  Part of     : Hg Teleport
 *  Description : Device state (touch mode, screen orientation) handler
 *  Version     : %version: sa1spcx1#11 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 * Template version: 4.2
 */

#ifndef HGTELEPORTDEVICESTATE_H_
#define HGTELEPORTDEVICESTATE_H_

#include <e32base.h>
#include "hgteleportappui.h"
#include "hgproplistener.h"

/**
 * Interface for getting notifications about screen orientation
 * and/or touch state etc. changes.
 */
class MHgDeviceStateObserver
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
class CHgTeleportDeviceState :
    public CBase,
    public MHgPropertyChangeObserver
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
    static CHgTeleportDeviceState* NewL();

    /**
     * Destructor.
     */
    ~CHgTeleportDeviceState();

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
    void AddObserverL( MHgDeviceStateObserver& aObserver, TInt aMask );

    /**
     * Deregisters the given observer.
     * @param   aObserver   ref to observer
     */
    void RemoveObserver( MHgDeviceStateObserver& aObserver );

    /**
     * Called from appui.
     */
    void HandleResourceChange( TInt aType );
    
    //From MHgPropertyChangeObserver
public:
    /**
     * Observer interface for getting notifications about a P&S property change.
     */
	virtual void PropertyChanged( TUid aCategory, TUint aKey );
private:
    /**
     * Constructor.
     */
    CHgTeleportDeviceState();
    
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
    void NotifyObservers( MHgDeviceStateObserver::TChangeType aType );

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
        MHgDeviceStateObserver* iObserver; // not owned
        SObserver( TInt aMask, MHgDeviceStateObserver* aObserver )
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
    CHgPropertyListener* iFlipStatusObserver;
    };

#endif // HGTELEPORTDEVICESTATE_H_
