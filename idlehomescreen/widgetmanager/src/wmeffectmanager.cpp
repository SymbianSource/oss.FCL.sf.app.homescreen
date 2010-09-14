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
* Description:  Effect manager.
*
*/


// SYSTEM INCLUDE FILES
#include <eikapp.h>
#include <aknappui.h>
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects
#include <akntranseffect.h>                 // For transition effects
#include <alf/alfcompositionutility.h>

// INCLUDE FILES
#include "wmeffectmanager.h"

// CONSTANTS
const TInt KEffectTypeFullscreen = 1;

const TInt KWaitForLayout = 1;
const TInt KEffectStarted = 2;

const TInt KEffectWaitInterval( 250000 ); //250ms
const TInt KWaitInterval( 25000 ); // 25ms

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CWmEffectManager::CWmEffectManager( CCoeEnv& aCoeEnv ): iCoeEnv (aCoeEnv)
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CWmEffectManager::ConstructL()
    {
    iObserver = CAlfEffectObserver::NewL(); 
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmEffectManager* CWmEffectManager::NewL( CCoeEnv& aCoeEnv )
    {
    CWmEffectManager* self = new (ELeave) CWmEffectManager( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CWmEffectManager::~CWmEffectManager()
    {
    delete iObserver;
    GfxTransEffect::AbortFullScreen();
    iEffects.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CWmEffectManager::BeginFullscreenEffectL
// -----------------------------------------------------------------------------
//
void CWmEffectManager::BeginFullscreenEffectL( TInt aId )
    {
    TWmEffect* effect = new (ELeave) TWmEffect;
    CleanupStack::PushL( effect );
    effect->iId = aId;
    effect->iType = KEffectTypeFullscreen;
    effect->iState = KWaitForLayout;
    iEffects.AppendL( effect );
    
    CleanupStack::Pop( effect );

    if ( !DoBeginFullscreenEffect( *effect ) )
        {
        RemoveEffect( effect );
        }
    }

// -----------------------------------------------------------------------------
// CWmEffectManager::UiRendered
// -----------------------------------------------------------------------------
//
void CWmEffectManager::UiRendered()
    {
    for ( TInt i = 0; i < iEffects.Count(); )
        {
        TWmEffect* effect = iEffects[i];
        if ( effect && effect->iState == KEffectStarted )
            {
            GfxTransEffect::EndFullScreen();
            RemoveEffect( effect );
            }
        else
            {
            // RemoveEffect() will remove from iEffects array and
            // thats why we need to have conter in else
            i++;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWmEffectManager::DoBeginFullscreenEffect
// -----------------------------------------------------------------------------
//
TBool CWmEffectManager::DoBeginFullscreenEffect( TWmEffect& aEffect )
    {
    if ( iCoeEnv.WsSession().GetFocusWindowGroup() != 
         iCoeEnv.RootWin().Identifier() )
        {
        // Window group is not focused
        RemoveEffect( &aEffect );
        return EFalse;
        }
    
    if ( !WaitActiveEffect( KEffectWaitInterval ) )
        {
        return EFalse;
        }

    const TInt flags( AknTransEffect::TParameter::EActivateExplicitCancel );
    const TUid targetAppUid( iAvkonAppUi->Application()->AppDllUid() );

    // Set effect begin point
    GfxTransEffect::BeginFullScreen( aEffect.iId , iAvkonAppUi->ClientRect(),
        AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(
        targetAppUid, flags ) );
    
    aEffect.iState = KEffectStarted;
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWmEffectManager::WaitActiveEffect
// -----------------------------------------------------------------------------
//
TBool CWmEffectManager::WaitActiveEffect( TInt aInterval )     
    {
    TBool retval( EFalse );
    TInt loop( aInterval / KWaitInterval );
    
    while ( loop >= 0 )
        {               
        TInt count( iObserver->ActiveEffectsCount() );
        
        if ( count == 0 )
            {
            retval = ETrue;
            break;                        
            }
        
        User::After( KWaitInterval );
        loop--;
        }

    return retval;    
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::RemoveEffect
// -----------------------------------------------------------------------------
//
void CWmEffectManager::RemoveEffect( TWmEffect* aEffect )
    {
    TInt index = iEffects.Find( aEffect );
    if ( index != KErrNotFound )
        {
        TWmEffect* temp = iEffects[index];
        iEffects.Remove( index );
        delete temp;
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::IsEffectActive
// -----------------------------------------------------------------------------
//
TBool CWmEffectManager::IsEffectActive()
    {
    TBool retVal( EFalse );
    if ( iObserver->ActiveEffectsCount() )
        retVal = ETrue;
    return retVal;
    }

// End fo file

