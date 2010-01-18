/*
 * ===========================================================================
 *  Name        : hgteleportappecom.cpp
 *  Part of     : Teleport
 *  Description : Teleport application ecom plugin
 *  Version     : %version:  sa1spcx1#8.1.2 %
 *
 *  Copyright © 2009 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */
 
#include <e32property.h>
#include <apgtask.h>
#include <eikenv.h>

#include "hgteleportappecomconst.hrh"
#include "hgteleportappecom.h"

#include "hgteleportappecomlogging.h"

// AknCapServer UID, used for P&S category
const TUid KTeleportStateCategory = { 0x10207218 };

// Teleport UI, used as P&S key
const TInt KTeleportStateKey = 0x20016BF0;

// Values for Teleport launching P&S
const TInt KTeleportBackgroundValue = 1;
const TInt KTeleportForegroundValue = KTeleportBackgroundValue << 1;
const TInt KTeleportShortAppKeyPressed = KTeleportForegroundValue << 1;
const TInt KTeleportLongAppKeyPressed = KTeleportShortAppKeyPressed << 1;

// Teleport application UID, for checking if teleport is running
const TUid KTeleportAppUidValue = { 0x20016BF0 };

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::CHgTeleportEcomPlugin()
// Default constructor for first phase construction.
// ---------------------------------------------------------------------------
//
CHgTeleportEcomPlugin::CHgTeleportEcomPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::NewL()
// Standard NewL.
// ---------------------------------------------------------------------------
//
CHgTeleportEcomPlugin* CHgTeleportEcomPlugin::NewL()
    {
    CHgTeleportEcomPlugin* self = new ( ELeave ) CHgTeleportEcomPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::ConstructL()
// 2nd phase construction.
// ---------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::ConstructL()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::ConstructL, HGLOG_LOCAL );
    HGLOG_IN();
    
    DefineTeleportStateProperty();
    
    HGLOG_OUT();
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::~CHgTeleportEcomPlugin()
// Destructor.
// ---------------------------------------------------------------------------
//
CHgTeleportEcomPlugin::~CHgTeleportEcomPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::Show
// ---------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::Show()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::Show, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTeleportStateCategory, KTeleportStateKey, value );
    value &= ~KTeleportBackgroundValue;
    value |= KTeleportForegroundValue;
    SetTeleportStateProperty( value );  
    
    HGLOG_OUT();    
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::Dismiss
// ---------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::Dismiss()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::Dismiss, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTeleportStateCategory, KTeleportStateKey, value );
    value &= ~KTeleportForegroundValue;
    value |= KTeleportBackgroundValue;
    SetTeleportStateProperty( value );  
    
    HGLOG_OUT();     
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::HandleLongAppKeyPress
// ---------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::HandleLongAppKeyPress()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::Show, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTeleportStateCategory, KTeleportStateKey, value );
    value &= ~KTeleportShortAppKeyPressed;
    value |= KTeleportLongAppKeyPressed;
    SetTeleportStateProperty( value );    
    
    HGLOG_OUT();    
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::HandleShortAppKeyPress
// ---------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::HandleShortAppKeyPress()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::Dismiss, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTeleportStateCategory, KTeleportStateKey, value );
    value &= ~KTeleportLongAppKeyPressed;
    value |= KTeleportShortAppKeyPressed;
    SetTeleportStateProperty( value ); 
    
    HGLOG_OUT();     
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::IsVisible
// ---------------------------------------------------------------------------
//
TBool CHgTeleportEcomPlugin::IsVisible()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::IsVisible, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt value( 0 );
    RProperty::Get( KTeleportStateCategory, KTeleportStateKey, value );
    if ( value & KTeleportForegroundValue )
        {
        HGLOG_OUT();
        return ETrue;
        }
    HGLOG_OUT();
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CHgTeleportEcomPlugin::IsReady
// ---------------------------------------------------------------------------
//
TBool CHgTeleportEcomPlugin::IsReady()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::IsReady, HGLOG_LOCAL );
    HGLOG_IN();

    TBool ret = EFalse;    
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    
    if ( eikonEnv )
        {
        TApaTaskList taskList( eikonEnv->WsSession() );
        TApaTask task = taskList.FindApp( KTeleportAppUidValue ); 
    
        if ( task.Exists() )
            {
            ret = ETrue;
            }
        }
    
    HGLOG1_OUT( "IsReady returns: %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CHgTeleportEcomPlugin::SetTeleportStateProperty
// -----------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::SetTeleportStateProperty( TInt aValue )
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::SetTeleportShowProperty, HGLOG_LOCAL );
    HGLOG_IN();
    
    if ( RProperty::Set( 
            KTeleportStateCategory, KTeleportStateKey, aValue ) != KErrNone )
        {
        DefineTeleportStateProperty();
        TInt error = 
            RProperty::Set( KTeleportStateCategory, KTeleportStateKey, aValue );
        if ( error != KErrNone )
            {
            HGLOG1( HGLOG_INFO, "RProperty::Set Error: %d", error ); 
            }        
        }
    
    HGLOG_OUT();    
    }

// -----------------------------------------------------------------------------
// CHgTeleportEcomPlugin::DefineTeleportStateProperty
// -----------------------------------------------------------------------------
//
void CHgTeleportEcomPlugin::DefineTeleportStateProperty()
    {
    HGLOG_CONTEXT( CHgTeleportEcomPlugin::DefineTeleportShowProperty, HGLOG_LOCAL );
    HGLOG_IN();
    
    TInt error = RProperty::Define( 
            KTeleportStateCategory, KTeleportStateKey, RProperty::EInt );
    if ( error != KErrNone )
        {
        HGLOG1( HGLOG_INFO, "RProperty::Define Error: %d", error );
        }              
    
    HGLOG_OUT();
    }

// End of file 
