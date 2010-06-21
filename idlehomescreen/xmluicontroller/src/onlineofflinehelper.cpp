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
#include <ai3xmlui.rsg>
#include <AknQueryDialog.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <cmmanagerkeys.h>
#include <AknGlobalConfirmationQuery.h>
#include <StringLoader.h> // StringLoader
#include <AknQueryDialog.h>
#include <ai3xmlui.rsg>
// User includes
#include <aifwstatehandler.h>
#include <aifwdefs.h>
#include <activeidle2domaincrkeys.h>
#include <hspublisherinfo.h>
#include "onlineofflinehelper.h"
#include "xmluicontroller.h"
#include "appui.h"
#include "ai3.hrh"
#include "globalqueryhandler.h"

// Constants
const TInt KOfflineProfileId( 5 );

using namespace AiXmlUiController;

#ifdef HS_NETWORK_MONITOR
#include <flogger.h>
_LIT( KLogFolder,"xnnetwork" );
_LIT( KLogDom, "networksettings.log" );

#define _LOG1( a ) RFileLogger::Write( \
    KLogFolder, KLogDom, EFileLoggingModeAppend, ( a ) );
#define _LOG2( a, b ) RFileLogger::WriteFormat( \
    KLogFolder, KLogDom, EFileLoggingModeAppend, ( a ), ( b ) )
#else
#define _LOG1
#define _LOG2
#endif

_LIT( KDisConnected, "disconnected");
_LIT( KConnected, "connected");

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
    
    // Update state manager
    SetOnline( iFlags.IsSet( EOnline ) );
         
    iCurrentNwStatus = ENWRegistrationUnknown;    
    iHomeNetwork = KErrNotFound;
    iRoamingNetwork = KErrNotFound;
    
    iNwSettingObserver = CCenRepObserver::NewL( this );
    
    iGlobalQueryHandler = CGlobalQueryHandler::NewL(iUiCtl);
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();

    }
      
// ----------------------------------------------------------------------------
// COnlineOfflineHelper::~COnlineOfflineHelper
// ----------------------------------------------------------------------------
//
COnlineOfflineHelper::~COnlineOfflineHelper() 
    {
    delete iHandler;
    delete iSession;
    delete iNwSettingObserver;
    delete iGlobalQueryHandler;
    delete iGlobalConfirmationQuery; 
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::CurrentCellularDataUsageChangedL
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::CurrentCellularDataUsageChangedL(const TInt aValue)
    {
    if ( iCurrentNwStatus == ENWRegisteredOnHomeNetwork )
        {
        iHomeNetwork = aValue;
        }
    else if ( iCurrentNwStatus == ENWRegisteredRoaming )
        {
        iRoamingNetwork = aValue;
        }
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ShowOnlineItem
// ----------------------------------------------------------------------------
//
TBool COnlineOfflineHelper::ShowOnlineItem() const
    {    
    TInt value ( KErrNotFound );
    if ( iUiCtl.SettingsRepository().Get(  KAIWebStatus, value ) == KErrNone )
        {
        return ( !value );
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ShowOfflineItem
// ----------------------------------------------------------------------------
//
TBool COnlineOfflineHelper::ShowOfflineItem() const
    {
    TInt value ( KErrNotFound );
    if ( iUiCtl.SettingsRepository().Get(  KAIWebStatus, value ) == KErrNone )
        {
        return ( value );
        }
     return EFalse;
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::ProcessOnlineStateL
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::ProcessOnlineStateL( TBool aOnline )
    {
    // User has selected online/offline item from menu
    // Don't show R_YES_NO_HS_ONLINE query as user selected online
    if ( aOnline )
        {
        iFlags.Set( EOnline );
        if ( iCurrentNwStatus == ENWRegisteredRoaming )
            {
            // When user allready is in foreing network and sets
            // HS online we want to disable automatic offline setting and not
            // show note QTN_HS_AUTOMATIC_OFFLINE
            iFlags.Set( ERoamingShown );
            }
        }
    else
        {
        iFlags.Clear( EOnline );
        }
     
    SetOnline( aOnline );
    }

// ----------------------------------------------------------------------------
// COnlineOfflineHelper::SetOnline
// ----------------------------------------------------------------------------
//
void COnlineOfflineHelper::SetOnline( TBool aOnline )    
    {    
    // Save state
    iUiCtl.SettingsRepository().Set( KAIWebStatus, aOnline );

    // Run state change.
    iUiCtl.FwStateHandler()->ChangePluginState( 
            aOnline ? EAiFwOnline : EAiFwOffline );                    
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
                    _LOG1( _L(" Roaming Network Activated "));
                    iCurrentNwStatus = aNWInfo.iRegistrationStatus;
                    CurrentNetworkSetting();
                    if( ( iRoamingNetwork == ECmCellularDataUsageConfirm
                          || iRoamingNetwork == ECmCellularDataUsageDisabled )
                          && iFlags.IsSet( EOnline )
                          && iFlags.IsClear( ERoamingShown ) )
                        {
                        // Process to offline state. 
                        // Don't change the user selection.
                        SetOnline( EFalse );
                        // Show roaming notification
						CAknGlobalNote* note = CAknGlobalNote::NewLC();
						HBufC* msg( StringLoader::LoadLC( 
						        R_QTN_HS_AUTOMATIC_OFFLINE ) );
						note->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY); 
						note->ShowNoteL( EAknGlobalInformationNote, *msg );
						CleanupStack::PopAndDestroy( 2, note ); // msg
						
						// set roming shown flag. We want to show 
						// romign note only once while user stays in foreign
						// network. If user sets the HS to online this note is 
						// not showed again and HS will stay online in foreign
						// network.
						iFlags.Set( ERoamingShown );
                        }
                    break;

                case ENWRegisteredOnHomeNetwork:
                    _LOG1( _L(" Home Network Activated "));
                    if ( iFlags.IsSet( ERoamingShown  ) )
                        {
                        // Clear ERoamingShown when user returns to home 
                        // network.
                        iFlags.Clear( ERoamingShown );
                        }
                    
                    iCurrentNwStatus = aNWInfo.iRegistrationStatus;
                    CurrentNetworkSetting();
                    if( iHomeNetwork == ECmCellularDataUsageAutomatic )
                        {
                        // Reset to user selection
                        SetOnline( iFlags.IsSet( EOnline ) );
                        }
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
            SetOnline( EFalse );
            }
        else
            {
            iFlags.Clear( EOfflineProfile );
            // Reset to user selection
            SetOnline( iFlags.IsSet( EOnline ) );
            }
        }
    
    delete iHandler;
    iHandler = NULL;
    
    iHandler = CProfileChangeNotifyHandler::NewL( this );        
    }

// ---------------------------------------------------------------------------
// COnlineOfflineHelper::CurrentNetworkSetting
// ---------------------------------------------------------------------------
//
void COnlineOfflineHelper::CurrentNetworkSetting() 
    {    
    CRepository* repository( NULL );
    
    TRAP_IGNORE( repository = CRepository::NewL( KCRUidCmManager ) )
    
    if ( repository )
        {        
        TInt value( 0 );
        TInt err( repository->Get( KCurrentCellularDataUsage, value ) );

        if ( err == KErrNone )
            {
            if ( iCurrentNwStatus == ENWRegisteredRoaming )
                {
                iRoamingNetwork = value;
                _LOG2( _L("Roaming Network Setting <%d>"), value ); 
                }
            else if  ( iCurrentNwStatus == ENWRegisteredOnHomeNetwork )
                {
                iHomeNetwork = value;
                _LOG2( _L("Home Network Setting <%d>"), value );
                }            
            }    
        }
    delete repository;
    }

// ---------------------------------------------------------------------------
// COnlineOfflineHelper::HandleConnectionQueryL
// ---------------------------------------------------------------------------
//
void COnlineOfflineHelper::HandleConnectionQueryL( const TDesC& aConnection)
    {
    if ( aConnection == KDisConnected() && ShowOfflineItem() ) 
        {
        ShowGlobalQueryL(R_QTN_HS_DISABLE_NETWORK, EFalse );
        }
    else if ( aConnection == KConnected() && ShowOnlineItem() )
        {
        ShowGlobalQueryL(R_QTN_HS_SWITCH_ONLINE, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// COnlineOfflineHelper::ShowGlobalQueryL
// ---------------------------------------------------------------------------
//
void COnlineOfflineHelper::ShowGlobalQueryL( TInt aResourceId, TBool aSetOnline )
    {
    if ( !iGlobalQueryHandler->IsActive() )
        {
        HBufC* confirmationText = StringLoader::LoadLC(aResourceId);
        iGlobalConfirmationQuery->ShowConfirmationQueryL(
                iGlobalQueryHandler->iStatus,
            *confirmationText, 
            R_AVKON_SOFTKEYS_YES_NO);
        iGlobalQueryHandler->SetOnlineParamAndActivate(aSetOnline);
        CleanupStack::PopAndDestroy(); //confirmationText
        }
    }

 // End of file
