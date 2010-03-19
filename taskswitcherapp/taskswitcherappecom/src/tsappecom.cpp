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

// AknCapServer UID, used for P&S category
const TUid KTaskswitcherStateCategory = { 0x10207218 };

// Taskswitcher UI, used as P&S key
const TUint KTaskswitcherStateKey = 0x2002677D;

// Values for Taskswitcher launching P&S
const TInt KTaskswitcherBackgroundValue = 1;
const TInt KTaskswitcherForegroundValue = KTaskswitcherBackgroundValue << 1;
const TInt KTaskswitcherLongAppKeyPressed = KTaskswitcherForegroundValue << 1;

// Taskswitcher application UID, for checking if taskswitcher is running
const TUid KTaskswitcherAppUidValue = { 0x2002677D };

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
    DefineTaskswitcherStateProperty();
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::~CTsEcomPlugin()
// Destructor.
// ---------------------------------------------------------------------------
//
CTsEcomPlugin::~CTsEcomPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::Show
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::Show()
    {
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherBackgroundValue;
    value |= KTaskswitcherForegroundValue;
    value |= KTaskswitcherLongAppKeyPressed;
    SetTaskswitcherStateProperty( value );  
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::Dismiss
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::Dismiss()
    {
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value &= ~KTaskswitcherForegroundValue;
    value |= KTaskswitcherBackgroundValue;
    SetTaskswitcherStateProperty( value );  
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::HandleLongAppKeyPress
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::HandleLongAppKeyPress()
    {
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    value |= KTaskswitcherLongAppKeyPressed;
    SetTaskswitcherStateProperty( value );    
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::HandleShortAppKeyPress
// ---------------------------------------------------------------------------
//
void CTsEcomPlugin::HandleShortAppKeyPress()
    { 
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::IsVisible
// ---------------------------------------------------------------------------
//
TBool CTsEcomPlugin::IsVisible()
    {
    TInt value( 0 );
    RProperty::Get( KTaskswitcherStateCategory, KTaskswitcherStateKey, value );
    if ( value & KTaskswitcherForegroundValue )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CTsEcomPlugin::IsReady
// ---------------------------------------------------------------------------
//
TBool CTsEcomPlugin::IsReady()
    {
    TBool ret = EFalse;    
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
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CTsEcomPlugin::SetTaskswitcherStateProperty
// -----------------------------------------------------------------------------
//
void CTsEcomPlugin::SetTaskswitcherStateProperty( TInt aValue )
    {
    if ( RProperty::Set( 
            KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue ) != KErrNone )
        {
        DefineTaskswitcherStateProperty();
        RProperty::Set( KTaskswitcherStateCategory, KTaskswitcherStateKey, aValue );      
        }
    }

// -----------------------------------------------------------------------------
// CTsEcomPlugin::DefineTaskswitcherStateProperty
// -----------------------------------------------------------------------------
//
void CTsEcomPlugin::DefineTaskswitcherStateProperty()
    {
    RProperty::Define( 
        KTaskswitcherStateCategory, KTaskswitcherStateKey, RProperty::EInt );
            
    }

// End of file 
