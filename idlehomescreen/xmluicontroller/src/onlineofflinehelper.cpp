/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper for online/offline state handling
*
*/

// System includes
#include <NetworkHandlingProxy.h>
#include <CNWSession.h>
#include <MProfileEngine.h>
#include <CProfileChangeNotifyHandler.h>
#include <aifweventhandler.h>
#include <ai3xmlui.rsg>
#include <AknQueryDialog.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>

// User includes
#include <activeidle2domaincrkeys.h>
#include "onlineofflinehelper.h"
#include "xmluicontroller.h"
#include "appui.h"

// Constants
const TInt KOfflineProfileId( 5 );

using namespace AiXmlUiController;

// ============================ LOCAL FUNCTIONS ==============================

// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// COnlineOfflineHelper::NewL
// ---------------------------------------------------------------------------
//
COnlineOfflineHelper* COnlineOfflineHelper::NewL( CXmlUiController& aUiCtl )    
    {
    COnlineOfflineHelper* self = new ( ELeave )COnlineOfflineHelper( aUiCtl );                 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// ----------------------------------------------------------------------------
// COnlineOfflineHelper::COnlineOfflineHelper
// ----------------------------------------------------------------------------
//
COnlineOfflineHelper::COnlineOfflineHelper( CXmlUiController& aUiCtl )
    : iUiCtl( aUiCtl )
    {    
    }
        
// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ConstructL
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::ConstructL()
    { 
    //Create network handling engine session.
    iSession = CreateL( *this, iInfo );

    iHandler = CProfileChangeNotifyHandler::NewL( this );
    
    MProfileEngine* engine( CreateProfileEngineL() );
    
    if( engine->ActiveProfileId() == KOfflineProfileId )
        {
        iFlags.Set( EOfflineProfile );
        }
    
    engine->Release();
    
    TInt value( 0 );
    
    if ( iUiCtl.SettingsRepository().Get(  KAIWebStatus, value ) == KErrNone )
        {
        if( value && iFlags.IsClear( EOfflineProfile ) )
            {
            iFlags.Set( EOnline );
            }                    
        }
    
    // Update repository
    iUiCtl.SettingsRepository().Set(  KAIWebStatus, iFlags.IsSet( EOnline ) ); 
    
    iCurrentNwStatus = ENWRegistrationUnknown;    
    }
      
// ----------------------------------------------------------------------------
// COnlineOfflineHelper::~COnlineOfflineHelper
// ----------------------------------------------------------------------------
//
COnlineOfflineHelper::~COnlineOfflineHelper() 
    {
    delete iHandler;
    delete iSession;
    // Whether the user choice EOnline should be stored in cenrep ?
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ShowOnlineItem
// ----------------------------------------------------------------------------
//
TBool COnlineOfflineHelper::ShowOnlineItem() const
    {
    if ( iFlags.IsSet( EUtilizeOnline ) )
    	{
    	TInt value ( KErrNotFound );
    	if ( iUiCtl.SettingsRepository().Get(  KAIWebStatus, value ) == KErrNone )
    		{
    		return ( !value );
    		}
    	}
    return EFalse;
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ShowOfflineItem
// ----------------------------------------------------------------------------
//
TBool COnlineOfflineHelper::ShowOfflineItem() const
    {
    if ( iFlags.IsSet( EUtilizeOnline ) )
		{
		TInt value ( KErrNotFound );
		if ( iUiCtl.SettingsRepository().Get(  KAIWebStatus, value ) == KErrNone )
			{
			return ( value );
			}
		}
     return EFalse;
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ProcessOnlineStateL
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::ProcessOnlineStateL( 
    RPointerArray< CXnNodeAppIf >& aList )
    {
    _LIT( KOnlineOffline, "online_offline" );

    iFlags.Clear( EUtilizeOnline );
                   
    // Check if data plugins are using online_offline
    for ( TInt i = 0; i < aList.Count(); i++ )
        {
        TAiPublisherInfo info;
        
        iUiCtl.PublisherInfoL( *aList[i], info );
                         
        if( iUiCtl.FwEventHandler()->HasMenuItemL( info, KOnlineOffline() ) )
            {
            iFlags.Set( EUtilizeOnline );            
            break;                       
            }
        }    
    
    if( iFlags.IsSet( EUtilizeOnline ) )
        {
        TInt value( 0 );
        iUiCtl.SettingsRepository().Get( KAIWebStatus, value );
        
        if ( value )
        	{
            // Switch to online
        	SetOnlineL( ETrue );
        	}
        else
        	{
            // Switch to offline
        	SetOnlineL( EFalse );
        	}                
        }
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ProcessOnlineStateL
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::ProcessOnlineStateL( TBool aOnline )
    {
    // User has selected online/offline item from menu
    if( iFlags.IsSet( EUtilizeOnline ) )
    	{
    	 // Don't show R_YES_NO_HS_ONLINE query as user selected online
    	 if (aOnline )
			{
			iFlags.Set( EOnline );
			// Save state
			iUiCtl.SettingsRepository().Set( KAIWebStatus, ETrue );
			// Run state change.
			iUiCtl.FwEventHandler()->ProcessStateChange( EAifwOnline  );     
			}
    	 else
    		 {
    		 iFlags.Clear( EOnline );
    		 // Save state
			 iUiCtl.SettingsRepository().Set( KAIWebStatus, EFalse );
			 // Run state change.
			 iUiCtl.FwEventHandler()->ProcessStateChange( EAifwOffline  );
    		 }
    	}
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::SetOnline
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::SetOnlineL( TBool aOnline )    
    {
    
    // Save state
    iUiCtl.SettingsRepository().Set( KAIWebStatus, aOnline );

    if( aOnline )
        {
        // Run state change.
        iUiCtl.FwEventHandler()->ProcessStateChange( EAifwOnline  );        
        }
    else  
        {
		// Run state change.
		iUiCtl.FwEventHandler()->ProcessStateChange( EAifwOffline );
        }
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::HandleNetworkMessage
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::HandleNetworkMessage( const TNWMessages aMessage )
    {        
    if ( HasNetworkInfoChanged( aMessage ) )
        {
        // Interpret new nw state
        TRAP_IGNORE( InterpretNWMessageL( aMessage, iInfo ) );  
        }                       
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::InterpretNWMessage
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::InterpretNWMessageL( const TNWMessages aMessage, 
    const TNWInfo aNWInfo )
    {       
    switch ( aMessage )
        {
        case MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange:
        case MNWMessageObserver::ENWMessageCurrentHomeZoneMessage:
            {
            switch ( aNWInfo.iRegistrationStatus )
                {
                case ENWRegisteredRoaming:
                    if( iFlags.IsSet( EOnline ) )
                        {
                        // Process to offline state. 
                        // Don't change the user selection.
                        SetOnlineL ( EFalse );
                        // Show roaming notification
						CAknGlobalNote* note = CAknGlobalNote::NewLC();
						HBufC* msg( StringLoader::LoadLC( R_QTN_HS_AUTOMATIC_OFFLINE ) );
						note->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY); 
						note->ShowNoteL( EAknGlobalInformationNote, *msg );
						CleanupStack::PopAndDestroy( 2, note ); // msg    
                        }
                    
                    iCurrentNwStatus = aNWInfo.iRegistrationStatus;
                    break;
                case ENWRegisteredOnHomeNetwork:
                	// Reset to user selection
                	SetOnlineL(iFlags.IsSet( EOnline ) );
                	
                    iCurrentNwStatus = aNWInfo.iRegistrationStatus;
                    break;                     
                default:                        
                    // unknown state                    
                    iCurrentNwStatus = ENWRegistrationUnknown;
                    break;
                }
            }
        }       
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::HandleNetworkError
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::HandleNetworkError( const TNWOperation aOperation, 
    TInt /*aErrorCode*/ )
    {
     switch ( aOperation )
        {
        case MNWMessageObserver::ENWGetNetworkProviderName:
            iReceivedMessageFlags |= ENetworkProviderNameReceived;
            iReceivedMessageFlags &= ~ENetworkProviderNameOk;
            iInfo.iNPName.Zero();
            break;
        case MNWMessageObserver::ENWGetProgrammableOperatorName:
            iReceivedMessageFlags |= EProgrammableOperatorInfoReceived;
            iReceivedMessageFlags &= ~EProgrammableOperatorInfoReceivedOk;
            iInfo.iOperatorNameInfo.iName.Zero();
            break;
        case MNWMessageObserver::ENWGetServiceProviderName:
            iReceivedMessageFlags |= EServiceProviderNameReceived;
            iReceivedMessageFlags &= ~EServiceProviderNameOk;
            iInfo.iServiceProviderNameDisplayReq = RMobilePhone::KDisplaySPNNotRequired;
            iInfo.iSPName.Zero();
            iInfo.iPLMNField.Zero();
            break;
        default:
            break;
        }
    
    HandleNetworkMessage( TNWMessages( KErrGeneral ) );
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::HasNetworkInfoChanged
// ----------------------------------------------------------------------------
//
TBool COnlineOfflineHelper::HasNetworkInfoChanged( const TNWMessages aMsg )
    {
    TBool result( ETrue );
    
    // pass through
    if ( aMsg == MNWMessageObserver::ENWMessageCurrentHomeZoneMessage   ||
         aMsg == MNWMessageObserver::ENWMessageNetworkConnectionFailure ||
         aMsg == MNWMessageObserver::ENWMessageCurrentCellInfoMessage   ||
         aMsg == MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange )
        {
        return result;
        }
    
    result = ( iReceivedMessageFlags != iOldReceivedMessageFlags );
    
    if ( !result )
        {
         result = iInfo.iRegistrationStatus != iOldInfo.iRegistrationStatus;             
        }

    iOldReceivedMessageFlags = iReceivedMessageFlags;
    iOldInfo = iInfo;

    return result;
    }

// ---------------------------------------------------------------------------
// COnlineOfflineHelper::HandleActiveProfileEventL
// ---------------------------------------------------------------------------
//
void COnlineOfflineHelper::HandleActiveProfileEventL(
    TProfileEvent aProfileEvent, TInt aProfileId )                                                        
    {
    //Profile activated or modified.
    if ( aProfileEvent == EProfileNewActiveProfile )         
        {
        if( aProfileId == KOfflineProfileId )
            {
            iFlags.Set( EOfflineProfile );
            // Don't change the user selection.
            SetOnlineL( EFalse );
            }
        else
            {
            iFlags.Clear( EOfflineProfile );
            // Reset to user selection
            SetOnlineL(iFlags.IsSet( EOnline ) );
            }
        }
    
    delete iHandler;
    iHandler = NULL;
    
    iHandler = CProfileChangeNotifyHandler::NewL( this );        
    }

 // End of file

