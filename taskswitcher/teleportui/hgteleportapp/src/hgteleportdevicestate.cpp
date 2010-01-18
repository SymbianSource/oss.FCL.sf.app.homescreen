/*
 * ============================================================================
 *  Name        : hgteleportdevicestate.cpp
 *  Part of     : Hg Teleport
 *  Description : Device state (touch mode, screen orientation) handler
 *  Version     : %version: sa1spcx1#14 %
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

#include "hgteleportdevicestate.h"
#include <AknUtils.h>
#include <hwrmdomainpskeys.h>

#include "hgteleportapplogging.h"


// -----------------------------------------------------------------------------
// CHgTeleportDeviceState::NewL
// -----------------------------------------------------------------------------
//
CHgTeleportDeviceState* CHgTeleportDeviceState::NewL()
    {
    CHgTeleportDeviceState* self = new ( ELeave ) CHgTeleportDeviceState;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgTeleportDeviceState::CHgTeleportDeviceState
// -----------------------------------------------------------------------------
//
CHgTeleportDeviceState::CHgTeleportDeviceState()
    {
    }

// -----------------------------------------------------------------------------
// CHgTeleportDeviceState::ConstructL
// -----------------------------------------------------------------------------
//
void CHgTeleportDeviceState::ConstructL()
    {
    iFlipStatusObserver = new ( ELeave ) CHgPropertyListener(KPSUidHWRM,	
		KHWRMFlipStatus, *this);
    // check if touch is enabled or not
    CheckTouchState();
    // are we in portrait or landscape
    CheckOrientation();
    // checks the qwerty input mode.
    CheckDeviceType();
    }

// -----------------------------------------------------------------------------
// CHgTeleportDeviceState::~CHgTeleportDeviceState
// -----------------------------------------------------------------------------
//
CHgTeleportDeviceState::~CHgTeleportDeviceState()
    {
    iObservers.Close();
    delete iFlipStatusObserver;
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::HandleResourceChange
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::HandleResourceChange( TInt aType )
    {
    HGLOG_CONTEXT( HandleResourceChange, HGLOG_LOCAL );
    HGLOG2_IN( "aType = %d (active count = %d)", aType, iResChangeActiveCount );

    // increase function entrance count
    ++iResChangeActiveCount;
    // if we are still in a previous call then do nothing else
    if ( iResChangeActiveCount == 1 )
        {
        // the active count may increase during execution of the body of the loop
        // (if some observers have active scheduler waits, for example)
        while ( iResChangeActiveCount > 0 )
            {
            if ( aType == KEikDynamicLayoutVariantSwitch )
                {
                HGLOG0( HGLOG_INFO, "dyn layout var switch" );
                // This might be a screen orientation or touch-nontouch switch,
                // so let's check the situation and notify observers if needed.
                CheckTouchState();
                CheckOrientation();
                }
            else if ( aType == KAknsMessageSkinChange )
                {
                HGLOG0( HGLOG_INFO, "skin change" );
                NotifyObservers( MHgDeviceStateObserver::ESkin );
                }
            --iResChangeActiveCount;
            }
        }
        
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::PropertyChanged
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::PropertyChanged(TUid aCategory, TUint aKey)
    {
    HGLOG_CONTEXT( PropertyChanged, HGLOG_LOCAL );
    HGLOG2_IN( "aKey = %d aNewValue = %d", aCategory, aKey );
    CheckDeviceType();
    HGLOG_OUT();
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::CheckTouchState
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::CheckTouchState()
    {
    HGLOG_CONTEXT( CheckTouchState, HGLOG_LOCAL );
    HGLOG_IN();

    TTouchState oldValue = iTouchState;
    iTouchState = AknLayoutUtils::PenEnabled() ? ETouchEnabled : ETouchDisabled;
    if ( iTouchState != oldValue )
        {
        NotifyObservers( MHgDeviceStateObserver::ETouchState );
        }

    HGLOG1_OUT( "new value for iTouchState: %d", iTouchState );
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::CheckOrientation
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::CheckOrientation()
    {
    HGLOG_CONTEXT( CheckOrientation, HGLOG_LOCAL );
    HGLOG_IN();

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect ( AknLayoutUtils::EScreen, rect );
    TOrientation oldValue = iOrientation;
    iOrientation = rect.Width() > rect.Height() ? ELandscape : EPortrait;
    if ( iOrientation != oldValue )
        {
        NotifyObservers( MHgDeviceStateObserver::EOrientation );
        }

    HGLOG1_OUT( "new value for iOrientation: %d", iOrientation );
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::DeviceType
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::CheckDeviceType()
    {
    HGLOG_CONTEXT( CheckQwerty, HGLOG_LOCAL );
    HGLOG_IN();

    TInt oldDeviceType = iDeviceType;
    TInt value( 0 );
    if( RProperty::Get( KPSUidHWRM, KHWRMFlipStatus, value ) == KErrNone )
    	{
    	iDeviceType = (value == EPSHWRMFlipOpen ? EHybrid : EFullTouch);
    	}
    
    if( iDeviceType != oldDeviceType )
        {
        NotifyObservers( MHgDeviceStateObserver::EDeviceType);
        }

    HGLOG1_OUT( "new value for iDeviceType: %d", iDeviceType );
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::TouchState
// --------------------------------------------------------------------------
//
CHgTeleportDeviceState::TTouchState CHgTeleportDeviceState::TouchState() const
    {
    return iTouchState;
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::Orientation
// --------------------------------------------------------------------------
//
CHgTeleportDeviceState::TOrientation CHgTeleportDeviceState::Orientation() const
    {
    return iOrientation;
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::Qwerty
// --------------------------------------------------------------------------
//
CHgTeleportDeviceState::TDeviceType CHgTeleportDeviceState::DeviceType() const
    {
    return iDeviceType;
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::ObserverIdentity
// --------------------------------------------------------------------------
//
TBool CHgTeleportDeviceState::ObserverIdentity( const SObserver& aA,
        const SObserver& aB )
    {
    return aA.iObserver == aB.iObserver;
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::AddDeviceStateObserverL
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::AddObserverL( MHgDeviceStateObserver& aObserver,
        TInt aMask )
    {
    iObservers.AppendL( SObserver( aMask, &aObserver ) );
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::RemoveObserver
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::RemoveObserver( MHgDeviceStateObserver& aObserver )
    {
    for ( ; ; )
        {
        TInt pos = iObservers.Find( SObserver( MHgDeviceStateObserver::EAny,
            &aObserver ), ObserverIdentity );
        if ( pos >= 0 )
            {
            iObservers.Remove( pos );
            }
        else
            {
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CHgTeleportDeviceState::NotifyObservers
// --------------------------------------------------------------------------
//
void CHgTeleportDeviceState::NotifyObservers(
        MHgDeviceStateObserver::TChangeType aType )
    {
    for ( TInt i = 0, ie = iObservers.Count(); i != ie; ++i )
        {
        const SObserver& obs( iObservers[i] );
        if ( obs.iMask & aType )
            {
            obs.iObserver->HandleDeviceStateChanged( aType );
            }
        }
    }


// end of file
