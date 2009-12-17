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
* Description:  Implementation for xuikon clock.
*
*/


// SYSTEM INCLUDE FILES
#include <e32std.h>         // for RChangeNotifier

// USER INCLUDE FILES
#include "xncontroladapter.h"
#include "xnclockadapter.h"
#include "xnclockface.h"

#include "xnclockcontrol.h"


// LOCAL CONSTANTS AND MACROS
static const TInt KIntervalTime( 60000000 ); // in microseconds


// MODULE DATA STRUCTURES
class CXnClockChangeHandler : public CActive
    {
    public: // Constructor and destructor
        static CXnClockChangeHandler* NewL(
            CXnClockControl& aClient )
            {
            CXnClockChangeHandler* self =
                new (ELeave) CXnClockChangeHandler( aClient );
            CleanupStack::PushL( self );
            self->ConstructL();
            CleanupStack::Pop( self );
            return self;
            }

        virtual ~CXnClockChangeHandler()
            {
            Cancel();
            iChangeNotifier.Close();
            }

    private: // From CActive
        void DoCancel()
            {
            iChangeNotifier.LogonCancel();
            // Return value is ignored.
            }

        void RunL()
            {
            if( iStatus.Int() & ( EChangesLocale | EChangesSystemTime ) )
                {
                iClient.TimeOrLocaleChanged();
                }

            User::LeaveIfError( iChangeNotifier.Logon( iStatus ) );
            SetActive();
            }

    private: // Private constructors
        void ConstructL()
            {
            User::LeaveIfError( iChangeNotifier.Create() );
            User::LeaveIfError( iChangeNotifier.Logon( iStatus ) );
            SetActive();
            }

        CXnClockChangeHandler( CXnClockControl& aClient )
            : CActive( EPriorityStandard ), iClient( aClient )
            {
            CActiveScheduler::Add( this );
            }

    private: // Data
        RChangeNotifier  iChangeNotifier;
        CXnClockControl& iClient;
    };

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

    iHandler = CXnClockChangeHandler::NewL( *this );
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
// -----------------------------------------------------------------------------
//
CXnClockControl::~CXnClockControl()
    {
    delete iHandler;
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
// CXnClockControl::TimeOrLocaleChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnClockControl::TimeOrLocaleChanged()
    {
    TLocale locale;

    if( iFormatFromLocale && ( locale.ClockFormat() != iClockFormat ) )
        {
        TRAP_IGNORE( SetFormatL( iFormatFromLocale, locale.ClockFormat() ) );        
        }
        
    UpdateDisplay();        
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
void CXnClockControl::Draw( CWindowGc& aGc, const TRect& aRect ) const
    {
    if( iFace && !aRect.IsEmpty() )
        {
        TTime homeTime;
        homeTime.HomeTime();

        TRAP_IGNORE( iFace->DrawL( *iAdapter, aGc, aRect, homeTime ) );
        
        // Ensure timer is active
        const_cast< CXnClockControl* >( this )->StartTimer();
        }
    }

// -----------------------------------------------------------------------------
// CXnClockControl::TimerCallback
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXnClockControl::TimerCallback( TAny* aThis )
    {
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
        iTimer->Cancel();
        }
    }

//  End of File
