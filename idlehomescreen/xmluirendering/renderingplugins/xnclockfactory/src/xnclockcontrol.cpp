/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock control
*
*/


// System includes
#include <e32std.h>         

// User includes
#include <debug.h>
#include "xncontroladapter.h"
#include "xnclockadapter.h"
#include "xnclockface.h"

#include "xnclockcontrol.h"

// Constants
static const TInt KIntervalTime( 60000000 ); // in microseconds


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnClockControl::CXnClockControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnClockControl::CXnClockControl( CXnClockAdapter* aAdapter,
                                  const TBool aFormatFromLocale,
                                  const TClockFormat aFormat )
    : iAdapter( aAdapter ), 
      iClockFormat( aFormat ),
      iFormatFromLocale ( aFormatFromLocale )      
    {
    }

// -----------------------------------------------------------------------------
// CXnClockControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClockControl::ConstructL()
    {           
    iTimer = CPeriodic::NewL( CActive::EPriorityHigh );
            
    SetFormatL( iFormatFromLocale, iClockFormat );   
    }

// -----------------------------------------------------------------------------
// CXnClockControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnClockControl* CXnClockControl::NewL( CXnClockAdapter* aAdapter,
                                        const TBool aFormatFromLocale,
                                        const TClockFormat aFormat )
    {
    CXnClockControl* self =
        new (ELeave) CXnClockControl( aAdapter, aFormatFromLocale, aFormat );
        
    CleanupStack::PushL( self );    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CXnClockControl::~CXnClockControl()
    {    
    delete iTimer;
    delete iFace;
    }

// -----------------------------------------------------------------------------
// CXnClockControl::UpdateDisplay
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockControl::UpdateDisplay()
    {
    if( !iFace )
        {
        return;
        }
            
    iAdapter->UpdateDisplay();
    }

// -----------------------------------------------------------------------------
// CXnClockControl::SetFormatL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockControl::SetFormatL( const TBool aFormatFromLocale, 
                                  const TClockFormat aFormat )
    {
    TClockFormat format( aFormat );
    
    if( aFormatFromLocale )
        {
        format = TLocale().ClockFormat();                
        }
        
    iFormatFromLocale = aFormatFromLocale;
        
    if( format == iClockFormat && iFace )
        {
        // Already correct face
        return;    
        }      
            
    iClockFormat = format;
    
    delete iFace;
    iFace = NULL;

    if( format == EClockAnalog )
        {
        iFace = CXnClockFaceAnalog::NewL();
        }
    else if( format == EClockDigital )
        {
        iFace = CXnClockFaceDigital::NewL();
        }       
    }

// -----------------------------------------------------------------------------
// CXnClockControl::Format
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TClockFormat CXnClockControl::Format() const
    {
    return iClockFormat;
    }

// -----------------------------------------------------------------------------
// CXnClockControl::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
void CXnClockControl::Draw( CWindowGc& aGc, const TRect& aRect )
    {
    // Ensure correct appearance
    TRAP_IGNORE( SetFormatL( iFormatFromLocale, iClockFormat ) );
    
    if( iFace && !aRect.IsEmpty() )
        {
        __PRINT( __DBG_FORMAT( "CXnClockControl::Draw: 0x%X" ), this );
                
        TTime homeTime;
        homeTime.HomeTime();

        TRAP_IGNORE( iFace->DrawL( *iAdapter, aGc, aRect, homeTime ) );
        }
    }

// -----------------------------------------------------------------------------
// CXnClockControl::TimerCallback
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXnClockControl::TimerCallback( TAny* aThis )
    {
    __PRINT( __DBG_FORMAT( "CXnClockControl::TimerCallback: 0x%X" ), aThis );    
        
    CXnClockControl* self = static_cast< CXnClockControl* >( aThis );

    // Update the clock display
    self->UpdateDisplay();

    // Adjust the timer delay if necessary
    TTime time;
    time.HomeTime();
    TDateTime dateTime( time.DateTime() );

    if( dateTime.Second() > 0 )
        {
        self->iTimer->Cancel();
        self->iTimer->After( KIntervalTime -
            1000000 * dateTime.Second() - dateTime.MicroSecond() );
        }

    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CXnClockControl::StartTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockControl::StartTimer()
    {
    if ( iTimer && !iTimer->IsActive() )
        {
        __PRINT( __DBG_FORMAT( "CXnClockControl::StartTimer: 0x%X" ), this );
        
        TTime time;
        time.HomeTime();
        TDateTime dateTime( time.DateTime() );
        TCallBack callBack( TimerCallback, this );

        iTimer->Start(
            TTimeIntervalMicroSeconds32(
                KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() ),
            TTimeIntervalMicroSeconds32( KIntervalTime ),
            callBack );
        }
    }

// -----------------------------------------------------------------------------
// CXnClockControl::StopTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockControl::StopTimer()
    {
    if ( iTimer && iTimer->IsActive() )
        {
        __PRINT( __DBG_FORMAT( "CXnClockControl::StopTimer: 0x%X" ), this );
                       
        iTimer->Cancel();
        }
    }

//  End of File
