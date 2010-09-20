/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for server side session object of
*              Screensaver Display Control Server
*
*/

#include <power_save_display_mode.h>
#include <hal.h>
#include <coreapplicationuisdomainpskeys.h>
#include <e32property.h>

#include "snsrdisplaycontrolsession.h"
#include "snsrdisplaycontrolcommon.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSnsrDisplayControlSession
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlSession::CSnsrDisplayControlSession() : 
    CSession2()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSnsrDisplayControlSession::ConstructL()
    {
    iPowerSave = CPowerSaveDisplayMode::NewL();
    
    // Create pixel buffer that can hold the full screen picture of our 
    // display device.
    TInt x = 0;
    TInt y = 0;
    HAL::Get(HALData::EDisplayXPixels, x);
    HAL::Get(HALData::EDisplayYPixels, y);
    iPowerSavePixelBuffer = HBufC::NewL(x*y);
    iPowerSavePixelBuffer->Des().FillZ();
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSnsrDisplayControlSession* CSnsrDisplayControlSession::NewL()
    {
    CSnsrDisplayControlSession* self = new (ELeave) CSnsrDisplayControlSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ~CSnsrDisplayControlSession
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlSession::~CSnsrDisplayControlSession()
    {
    delete iPowerSave;
    delete iPowerSavePixelBuffer;
    }

// ---------------------------------------------------------------------------
// ServiceL
// ---------------------------------------------------------------------------
//
void CSnsrDisplayControlSession::ServiceL( const RMessage2& aMessage )
    {
    TInt func = aMessage.Function();
    
    TInt err( KErrNone );
    
    switch ( func )
        {
        case ESnsrDispCtrlSrvDisplayOff:
            {
            // Disable touch
            HAL::Set( HALData::EPenState, 0 );
            
            // Lights off
            err = RProperty::Set(KPSUidCoreApplicationUIs, KLightsSSForcedLightsOn, 0);
            break;
            }
        case ESnsrDispCtrlSrvDisplayLowPower:
            {
            // Disable touch
            HAL::Set( HALData::EPenState, 0 );
            
            // Set display mode
            TInt startRow = aMessage.Int0();
            TInt endRow = aMessage.Int1();
            // TODO: for now, we pass a zero-filled pixel buffer to power save API.
            // This works fine with our reference hardware but some types of displays
            // might require passing the actual screen contents in this buffer.
            TUint16* ptr = const_cast<TUint16*>( iPowerSavePixelBuffer->Ptr() );
            err = iPowerSave->Set(startRow, endRow, ptr);
            break;
            }
        case ESnsrDispCtrlSrvDisplayFullPower:
            {
            // Enable touch
            HAL::Set( HALData::EPenState, 1 );
            
            // Set display mode
            err = iPowerSave->Exit();
            
            // Lights on
            err = RProperty::Set(KPSUidCoreApplicationUIs, KLightsSSForcedLightsOn, 30) || err;
            break;
            }
        default:
            {
            err = KErrNotSupported;
            }
        }
    
    aMessage.Complete(err);
    }

