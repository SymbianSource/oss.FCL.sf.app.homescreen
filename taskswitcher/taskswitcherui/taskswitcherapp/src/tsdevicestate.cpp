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


#include "tsdevicestate.h"
#include "tsapplogging.h"

#include <AknUtils.h>
#include <hwrmdomainpskeys.h>

// -----------------------------------------------------------------------------
// CTsDeviceState::NewL
// -----------------------------------------------------------------------------
//
CTsDeviceState* CTsDeviceState::NewL()
    {
    CTsDeviceState* self = new ( ELeave ) CTsDeviceState;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsDeviceState::CTsDeviceState
// -----------------------------------------------------------------------------
//
CTsDeviceState::CTsDeviceState()
    {
    }

// -----------------------------------------------------------------------------
// CTsDeviceState::ConstructL
// -----------------------------------------------------------------------------
//
void CTsDeviceState::ConstructL()
    {
    iFlipStatusObserver = new ( ELeave ) CTsPropertyListener(KPSUidHWRM,	
		KHWRMFlipStatus, *this);
    // check if touch is enabled or not
    CheckTouchState();
    // are we in portrait or landscape
    CheckOrientation();
    // checks the qwerty input mode.
    CheckDeviceType();
    }

// -----------------------------------------------------------------------------
// CTsDeviceState::~CTsDeviceState
// -----------------------------------------------------------------------------
//
CTsDeviceState::~CTsDeviceState()
    {
    iObservers.Close();
    delete iFlipStatusObserver;
    }

// --------------------------------------------------------------------------
// CTsDeviceState::HandleResourceChange
// --------------------------------------------------------------------------
//
void CTsDeviceState::HandleResourceChange( TInt aType )
    {
    TSLOG_CONTEXT( HandleResourceChange, TSLOG_LOCAL );
    TSLOG2_IN( "aType = %d (active count = %d)", aType, iResChangeActiveCount );

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
                TSLOG0( TSLOG_INFO, "dyn layout var switch" );
                // This might be a screen orientation or touch-nontouch switch,
                // so let's check the situation and notify observers if needed.
                CheckTouchState();
                CheckOrientation();
                }
            else if ( aType == KAknsMessageSkinChange )
                {
                TSLOG0( TSLOG_INFO, "skin change" );
                NotifyObservers( MTsDeviceStateObserver::ESkin );
                }
            --iResChangeActiveCount;
            }
        }
        
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsDeviceState::PropertyChanged
// --------------------------------------------------------------------------
//
void CTsDeviceState::PropertyChanged(TUid aCategory, TUint aKey)
    {
    TSLOG_CONTEXT( PropertyChanged, TSLOG_LOCAL );
    TSLOG2_IN( "aKey = %d aNewValue = %d", aCategory, aKey );
    CheckDeviceType();
    TSLOG_OUT();
    }

// --------------------------------------------------------------------------
// CTsDeviceState::CheckTouchState
// --------------------------------------------------------------------------
//
void CTsDeviceState::CheckTouchState()
    {
    TSLOG_CONTEXT( CheckTouchState, TSLOG_LOCAL );
    TSLOG_IN();

    TTouchState oldValue = iTouchState;
    iTouchState = AknLayoutUtils::PenEnabled() ? ETouchEnabled : ETouchDisabled;
    if ( iTouchState != oldValue )
        {
        NotifyObservers( MTsDeviceStateObserver::ETouchState );
        }

    TSLOG1_OUT( "new value for iTouchState: %d", iTouchState );
    }

// --------------------------------------------------------------------------
// CTsDeviceState::CheckOrientation
// --------------------------------------------------------------------------
//
void CTsDeviceState::CheckOrientation()
    {
    TSLOG_CONTEXT( CheckOrientation, TSLOG_LOCAL );
    TSLOG_IN();

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect ( AknLayoutUtils::EScreen, rect );
    TOrientation oldValue = iOrientation;
    iOrientation = rect.Width() > rect.Height() ? ELandscape : EPortrait;
    if ( iOrientation != oldValue )
        {
        NotifyObservers( MTsDeviceStateObserver::EOrientation );
        }

    TSLOG1_OUT( "new value for iOrientation: %d", iOrientation );
    }

// --------------------------------------------------------------------------
// CTsDeviceState::DeviceType
// --------------------------------------------------------------------------
//
void CTsDeviceState::CheckDeviceType()
    {
    TSLOG_CONTEXT( CheckQwerty, TSLOG_LOCAL );
    TSLOG_IN();

    TInt oldDeviceType = iDeviceType;
    TInt value( 0 );
    if( RProperty::Get( KPSUidHWRM, KHWRMFlipStatus, value ) == KErrNone )
    	{
    	iDeviceType = (value == EPSHWRMFlipOpen ? EHybrid : EFullTouch);
    	}
    
    if( iDeviceType != oldDeviceType )
        {
        NotifyObservers( MTsDeviceStateObserver::EDeviceType);
        }

    TSLOG1_OUT( "new value for iDeviceType: %d", iDeviceType );
    }

// --------------------------------------------------------------------------
// CTsDeviceState::TouchState
// --------------------------------------------------------------------------
//
CTsDeviceState::TTouchState CTsDeviceState::TouchState() const
    {
    return iTouchState;
    }

// --------------------------------------------------------------------------
// CTsDeviceState::Orientation
// --------------------------------------------------------------------------
//
CTsDeviceState::TOrientation CTsDeviceState::Orientation() const
    {
    return iOrientation;
    }

// --------------------------------------------------------------------------
// CTsDeviceState::Qwerty
// --------------------------------------------------------------------------
//
CTsDeviceState::TDeviceType CTsDeviceState::DeviceType() const
    {
    return iDeviceType;
    }

// --------------------------------------------------------------------------
// CTsDeviceState::ObserverIdentity
// --------------------------------------------------------------------------
//
TBool CTsDeviceState::ObserverIdentity( const SObserver& aA,
        const SObserver& aB )
    {
    return aA.iObserver == aB.iObserver;
    }

// --------------------------------------------------------------------------
// CTsDeviceState::AddDeviceStateObserverL
// --------------------------------------------------------------------------
//
void CTsDeviceState::AddObserverL( MTsDeviceStateObserver& aObserver,
        TInt aMask )
    {
    iObservers.AppendL( SObserver( aMask, &aObserver ) );
    }

// --------------------------------------------------------------------------
// CTsDeviceState::RemoveObserver
// --------------------------------------------------------------------------
//
void CTsDeviceState::RemoveObserver( MTsDeviceStateObserver& aObserver )
    {
    for ( ; ; )
        {
        TInt pos = iObservers.Find( SObserver( MTsDeviceStateObserver::EAny,
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
// CTsDeviceState::NotifyObservers
// --------------------------------------------------------------------------
//
void CTsDeviceState::NotifyObservers(
        MTsDeviceStateObserver::TChangeType aType )
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
