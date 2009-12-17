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

// INCLUDE FILES
#include "wmeffectmanager.h"

// CONSTANTS
const TInt KEffectTypeFullscreen = 1;

const TInt KWaitForLayout = 1;
const TInt KEffectStarted = 2;

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
    iEffects.Append( effect );
    
    CleanupStack::Pop( effect );

    DoBeginFullscreenEffect( *effect );
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
void CWmEffectManager::DoBeginFullscreenEffect( TWmEffect& aEffect )
    {
    if ( iCoeEnv.WsSession().GetFocusWindowGroup() != 
         iCoeEnv.RootWin().Identifier() )
        {
        // Window group is not focused
        RemoveEffect( &aEffect );
        return;
        }

    const TInt flags( AknTransEffect::TParameter::EActivateExplicitCancel );
    const TUid targetAppUid( iAvkonAppUi->Application()->AppDllUid() );

    // Set effect begin point
    GfxTransEffect::BeginFullScreen( aEffect.iId , iAvkonAppUi->ClientRect(),
        AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(
        targetAppUid, flags ) );
    
    aEffect.iState = KEffectStarted;
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

// End fo file

