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
* Description:  UI State Listener
*
*/

// System includes
#include <ctsydomainpskeys.h>

// User includes
#include "xnappuiadapter.h"
#include "xnuistatelistener.h"

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnUiStateListener::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnUiStateListener* CXnUiStateListener::NewL( CXnAppUiAdapter& aAdapter  )    
    {
    CXnUiStateListener* self = CXnUiStateListener::NewLC( aAdapter );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnUiStateListener* CXnUiStateListener::NewLC( CXnAppUiAdapter& aAdapter )
    {
    CXnUiStateListener* self = new( ELeave ) CXnUiStateListener( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::CXnUiStateListener()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnUiStateListener::CXnUiStateListener( CXnAppUiAdapter& aAdapter ) 
    : iAppUiAdapter( aAdapter ), iLightStatus( CHWRMLight::ELightStatusUnknown )    
    {
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::~CXnUiStateListener()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnUiStateListener::~CXnUiStateListener()
    {
    iUiStateObservers.Reset();
    iResourceChangeObservers.Reset();
    
    iAppUiAdapter.StatusPane()->SetObserver( NULL );
    CCoeEnv::Static()->RemoveMessageMonitorObserver( *this );
    
    delete iLightObserver;
    delete iCallStateObserver;
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::ConstructL()
    {
    iLightObserver = CHWRMLight::NewL( this );
    
    iCallStateObserver = CXnPropertySubscriber::NewL( KPSUidCtsyCallInformation,
        KCTsyCallState, *this );
    
    iAppUiAdapter.StatusPane()->SetObserver( this );
    
    CCoeEnv::Static()->AddMessageMonitorObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::PrepareToExit()
// Prepares for application exit
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::PrepareToExit()
    {    
    iUiStateObservers.Reset();
    iResourceChangeObservers.Reset();
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::AddObserver()
// Adds MXnUiStateObserver
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::AddObserver( const MXnUiStateObserver& aObserver )
    {
    if ( iUiStateObservers.InsertInAddressOrder( &aObserver ) == KErrNone )
        {
        MXnUiStateObserver& observer( 
                const_cast< MXnUiStateObserver & >( aObserver) );
        
        if ( iForegroundStatus != MXnUiStateObserver::EUnknown )
            {
            observer.NotifyForegroundChanged( iForegroundStatus );
            }
                
        observer.NotifyInCallStateChaged( iInCallState );
        observer.NotifyLightStatusChanged( 
            iLightStatus == CHWRMLight::ELightOn ? ETrue : EFalse );
        }     
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::RemoveObserver()
// Removes MXnUiStateObserver
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::RemoveObserver( const MXnUiStateObserver& aObserver )
    {
    TInt index( iUiStateObservers.Find( &aObserver ) );
    
    if ( index != KErrNotFound )
        {
        iUiStateObservers.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::AddObserver()
// Adds MXnUiResourceChangeObserver 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::AddObserver( 
    const MXnUiResourceChangeObserver& aObserver )
    {
    iResourceChangeObservers.InsertInAddressOrder( &aObserver );
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::RemoveObserver()
// Removes MXnUiResourceChangeObserver
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::RemoveObserver( 
    const MXnUiResourceChangeObserver& aObserver )
    {
    TInt index( iResourceChangeObservers.Find( &aObserver ) );
    
    if ( index != KErrNotFound )
        {
        iResourceChangeObservers.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::HandleResourceChangeL()
// 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::HandleResourceChangeL( TInt aType )
    {   
    MXnUiStateObserver::TForegroundStatus status( iForegroundStatus );
    
    if ( aType == KEikMessageFadeAllWindows )
        {
        if ( iForegroundStatus != MXnUiStateObserver::EBackground )
            {
            iForegroundStatus = MXnUiStateObserver::EPartialForeground;
            }        
        }
    else if ( aType == KEikMessageUnfadeWindows )           
        {
        if ( iForegroundType == KAknFullOrPartialForegroundGained )
            {
            if ( iAppUiAdapter.IsPartialForeground() )
                {                       
                iForegroundStatus = MXnUiStateObserver::EPartialForeground;            
                }
            else
                {                       
                iForegroundStatus = MXnUiStateObserver::EForeground;            
                }                                  
            }
        }  
    
    if ( status != iForegroundStatus )
        {
        for ( TInt i = 0; i < iUiStateObservers.Count(); i++ )
            {
            iUiStateObservers[i]->
                NotifyForegroundChanged( iForegroundStatus );
            }
        }
            
    for ( TInt i = 0; i < iResourceChangeObservers.Count(); i++ )
        {
        iResourceChangeObservers[i]->NotifyResourceChanged( aType );
        }    
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::MonitorWsMessage()
// 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::MonitorWsMessage( const TWsEvent& aEvent )
    {
    TInt type( aEvent.Type() );

    MXnUiStateObserver::TForegroundStatus status( iForegroundStatus ); 
    
    if ( type == KAknFullOrPartialForegroundGained )
        {     
        iForegroundType = type;
        
        if ( iAppUiAdapter.IsPartialForeground() )
            {                       
            iForegroundStatus = MXnUiStateObserver::EPartialForeground;            
            }
        else
            {                       
            iForegroundStatus = MXnUiStateObserver::EForeground;            
            }                      
        }
    else if ( type == KAknFullOrPartialForegroundLost )        
        {
        iForegroundType = type;
        
        iForegroundStatus = MXnUiStateObserver::EBackground;
        }  
    else if ( type == EEventFocusLost )
        {
        iForegroundStatus = MXnUiStateObserver::EBackground;
        }
    else if ( type == EEventFocusGained )
        {
        if ( iForegroundType == KAknFullOrPartialForegroundGained )
            {
            if ( iAppUiAdapter.IsPartialForeground() )
                {                       
                iForegroundStatus = MXnUiStateObserver::EPartialForeground;            
                }
            else
                {                       
                iForegroundStatus = MXnUiStateObserver::EForeground;            
                }                                 
            }
        }
    
    if ( status != iForegroundStatus )
        {       
        for ( TInt i = 0; i < iUiStateObservers.Count(); i++ )
            {
            iUiStateObservers[i]->
                NotifyForegroundChanged( iForegroundStatus );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::HandleStatusPaneSizeChange()
// 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::HandleStatusPaneSizeChange()
    {              
    for ( TInt i = 0; i < iResourceChangeObservers.Count(); i++ )
        {
        iResourceChangeObservers[i]->NotifyStatusPaneSizeChanged();                         
        }        
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::LightStatusChanged()
// 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::LightStatusChanged( TInt aTarget, 
    CHWRMLight::TLightStatus aStatus ) 
    {
    if ( aTarget == CHWRMLight::EPrimaryDisplay )
        {
        if ( ( aStatus == CHWRMLight::ELightOn || 
             aStatus == CHWRMLight::ELightOff ) &&
             aStatus != iLightStatus )
            {
            iLightStatus = aStatus;
                            
            for ( TInt i = 0; i < iUiStateObservers.Count(); i++ )
                {
                iUiStateObservers[i]->
                    NotifyLightStatusChanged( 
                        iLightStatus == CHWRMLight::ELightOn ? ETrue : EFalse );
                }                                        
            }
        }    
    }

// -----------------------------------------------------------------------------
// CXnUiStateListener::PropertyChangedL()
// 
// -----------------------------------------------------------------------------
//
void CXnUiStateListener::PropertyChangedL( const TUint32 aKey, 
    const TInt aValue )
    {
    if ( aKey == KCTsyCallState )
        {
        TBool active( aValue > EPSCTsyCallStateNone );
        
        if( iInCallState != active )
            {
            iInCallState = active;
            
            for ( TInt i = 0; i < iUiStateObservers.Count(); i++ )
                {
                iUiStateObservers[i]->NotifyInCallStateChaged( iInCallState );
                }                                                  
            }
        }    
    }

// End of file
