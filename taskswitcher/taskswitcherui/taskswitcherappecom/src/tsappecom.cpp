/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Taskswitcher application ecom plugin
 *
*/

 
#include <e32property.h>
#include <apgtask.h>
#include <eikenv.h>

#include "tsappecomconst.hrh"
#include "tsappecom.h"

#include "tsappecomlogging.h"

// AknCapServer UID, used for P&S category
const TUid KTaskswitcherStateCategory = { 0x10207218 };

// Taskswitcher UI, used as P&S key
const TInt KTaskswitcherStateKey = 0x20016BF0;

// Values for Taskswitcher launching P&S
const TInt KTaskswitcherBackgroundValue = 1;
const TInt KTaskswitcherForegroundValue = KTaskswitcherBackgroundValue << 1;
const TInt KTaskswitcherShortAppKeyPressed = KTaskswitcherForegroundValue << 1;
const TInt KTaskswitcherLongAppKeyPressed = KTaskswitcherShortAppKeyPressed << 1;

// Taskswitcher application UID, for checking if taskswitcher is running
const TUid KTaskswitcherAppUidValue = { 0x20016BF0 };

// ---------------------------------------------------------------------------
// CTsEcomPlugin::CTsEcomPlugin()
// Default constructor for first phase construction.
// ---------------------------------------------------------------------------
//
CTsEcomPlugin::CTsEcomPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::NewL()
// Standard NewL.
// ---------------------------------------------------------------------------
//
CTsEcomPlugin* CTsEcomPlugin::NewL()
    {
    CTsEcomPlugin* self = new ( ELeave ) CTsEcomPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::ConstructL()
// 2nd phase construction.
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::ConstructL()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::ConstructL, TSLOG_LOCAL );
    TSLOG_IN();
    
    DefineTaskswitcherStateProperty();
    User::LeaveIfError( iApaLsSession.Connect() );
    
    TSLOG_OUT();
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::~CTsEcomPlugin()
// Destructor.
// ---------------------------------------------------------------------------
//
CTsEcomPlugin::~CTsEcomPlugin()
    {
    iApaLsSession.Close();
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::Show
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::Show()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::Show, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherBackgroundValue;
    value |= KTaskswitcherForegroundValue;
    SetTaskswitcherStateProperty( value );  
    
    TSLOG_OUT();    
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::Dismiss
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::Dismiss()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::Dismiss, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherForegroundValue;
    value |= KTaskswitcherBackgroundValue;
    SetTaskswitcherStateProperty( value );  
    
    TSLOG_OUT();     
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::HandleLongAppKeyPress
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::HandleLongAppKeyPress()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::Show, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherShortAppKeyPressed;
    value |= KTaskswitcherLongAppKeyPressed;
    SetTaskswitcherStateProperty( value );    
    
    TSLOG_OUT();    
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::HandleShortAppKeyPress
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::HandleShortAppKeyPress()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::Dismiss, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherLongAppKeyPressed;
    value |= KTaskswitcherShortAppKeyPressed;
    SetTaskswitcherStateProperty( value ); 
    
    TSLOG_OUT();     
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::IsVisible
// ---------------------------------------------------------------------------
//
TBool CTsEcomPlugin::IsVisible()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::IsVisible, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    if ( value & KTaskswitcherForegroundValue )
        {
        TSLOG_OUT();
        return ETrue;
        }
    TSLOG_OUT();
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::IsReady
// ---------------------------------------------------------------------------
//
TBool CTsEcomPlugin::IsReady()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::IsReady, TSLOG_LOCAL );
    TSLOG_IN();

    TBool ret = ETrue;    
    
    if( !IsTaskSwitcherStarted() )
        {
        ret = (KErrNone == StartTaskSwitcher()) ? ETrue : EFalse;
        }
    
    TSLOG1_OUT( "IsReady returns: %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CTsEcomPlugin::SetTaskswitcherStateProperty
// -----------------------------------------------------------------------------
//
void CTsEcomPlugin::SetTaskswitcherStateProperty( TInt aValue )
    {
    TSLOG_CONTEXT( CTsEcomPlugin::SetTaskswitcherShowProperty, TSLOG_LOCAL );
    TSLOG_IN();
    
    if ( RProperty::Set( 
            KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue ) != KErrNone )
        {
        DefineTaskswitcherStateProperty();
        TInt error = 
            RProperty::Set( KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue );
        if ( error != KErrNone )
            {
            TSLOG1( TSLOG_INFO, "RProperty::Set Error: %d", error ); 
            }        
        }
    
    TSLOG_OUT();    
    }

// -----------------------------------------------------------------------------
// CTsEcomPlugin::DefineTaskswitcherStateProperty
// -----------------------------------------------------------------------------
//
void CTsEcomPlugin::DefineTaskswitcherStateProperty()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::DefineTaskswitcherShowProperty, TSLOG_LOCAL );
    TSLOG_IN();
    
    TInt error = RProperty::Define( 
            KTaskswitcherStateCategory, KTaskswitcherStateKey, RProperty::EInt );
    if ( error != KErrNone )
        {
        TSLOG1( TSLOG_INFO, "RProperty::Define Error: %d", error );
        }              
    
    TSLOG_OUT();
    }

// -----------------------------------------------------------------------------
// CTsEcomPlugin::IsTaskSwitcherStarted
// -----------------------------------------------------------------------------
//
TBool CTsEcomPlugin::IsTaskSwitcherStarted()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::IsTaskSwitcherStarted, TSLOG_LOCAL );
    TSLOG_IN();
    
    TBool ret(EFalse);
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( eikonEnv )
        {
        TApaTaskList taskList( eikonEnv->WsSession() );
        TApaTask task = taskList.FindApp( KTaskswitcherAppUidValue ); 
        if ( task.Exists() )
            {
            ret = ETrue;
            }
        }
    TSLOG1( TSLOG_INFO, "TaskSwitcherStarted: %d", ret);
    TSLOG_OUT();
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsEcomPlugin::StartTaskSwitcher
// -----------------------------------------------------------------------------
//
TInt CTsEcomPlugin::StartTaskSwitcher()
    {
    TInt ret(0);
    TRAPD(err, ret = StartTaskSwitcherL() );
    if( KErrNone != err )
        {
        ret = err;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsEcomPlugin::StartTaskSwitcherL
// -----------------------------------------------------------------------------
//
TInt CTsEcomPlugin::StartTaskSwitcherL()
    {
    TSLOG_CONTEXT( CTsEcomPlugin::StartTaskSwitcher, TSLOG_LOCAL );
    TSLOG_IN();
    TInt ret(KErrNone);
    
    TApaAppInfo appInfo;
    TApaAppCapabilityBuf capabilityBuf;
    ret = iApaLsSession.GetAppInfo( appInfo, KTaskswitcherAppUidValue );
    if ( !ret )
        {
        TFileName appName = appInfo.iFullName;
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL( appName );
        cmdLine->SetCommandL( EApaCommandRun );
        ret = iApaLsSession.StartApp( *cmdLine );
        CleanupStack::PopAndDestroy( cmdLine );
        }
    
    TSLOG1( TSLOG_INFO, "RApaLsSession::StartApp returned with: %d", ret );
    TSLOG_OUT();
    return ret;
    }

// End of file 
