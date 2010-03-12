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


// INCLUDE FILES
#include "xneffectmanager.h"
#include "xnplugindata.h"
#include "xnviewdata.h"
#include "xnnode.h"
#include "xndomnode.h"
#include "xncontroladapter.h"

// SYSTEM INCLUDE FILES
#include <eikapp.h>
#include <aknappui.h>
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects
#include <akntranseffect.h>                 // For transition effects

// CONSTANTS
const TInt KWaitForLayout = 1;
const TInt KEffectStarted = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnEffectManager::CXnEffectManager()
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnEffectManager::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnEffectManager* CXnEffectManager::NewL()
    {
    CXnEffectManager* self = new (ELeave) CXnEffectManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CXnEffectManager::~CXnEffectManager()
    {
    GfxTransEffect::AbortFullScreen();
    iEffects.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginFullscreenEffectL
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginFullscreenEffectL( TInt aId, CXnViewData& aView )
    {
    // Only one fullscreen effect at time
    if ( iEffects.Count() != 0 )
        {
        return;
        }
    
    TXnEffect* effect = new (ELeave) TXnEffect;
    CleanupStack::PushL( effect );
    effect->iId = aId;
    effect->iNode = aView.ViewNode();
    iEffects.AppendL( effect );
    CleanupStack::Pop( effect );
    
    if ( !aView.ViewNode()->IsLaidOut() )
        {
        effect->iState = KWaitForLayout;
        }
    else
        {
        DoBeginFullscreenEffect( *effect );
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::UiRendered
// -----------------------------------------------------------------------------
//
void CXnEffectManager::UiRendered()
    {
    for ( TInt i = 0; i < iEffects.Count(); )
        {
        TXnEffect* effect = iEffects[i];
        if ( effect &&
             effect->iState == KEffectStarted )
            {
            GfxTransEffect::EndFullScreen();
            RemoveEffect( effect );
            }
        else
            {
            i++;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::UiLayouted
// -----------------------------------------------------------------------------
//
void CXnEffectManager::UiLayouted()
    {
    for ( TInt i = 0; i < iEffects.Count(); )
        {
        TBool effectStarted = ETrue;
        TXnEffect* effect = iEffects[i];
        if ( effect && effect->iNode &&
             effect->iState == KWaitForLayout &&
             effect->iNode->IsLaidOut() )
            {
            effectStarted = DoBeginFullscreenEffect( *effect );
            }
        
        if ( effectStarted )
            {
            i++;
            }
        else
            {
            // effect cannot be started, remove it
            RemoveEffect( effect );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::DoBeginFullscreenEffect
// -----------------------------------------------------------------------------
//
TBool CXnEffectManager::DoBeginFullscreenEffect( TXnEffect& aEffect )
    {
    CCoeEnv* coe( CCoeEnv::Static() );
           
    if ( coe->WsSession().GetFocusWindowGroup() != 
         coe->RootWin().Identifier() )
        {
        // Window group is not focused
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
// CXnEffectManager::RemoveEffect
// -----------------------------------------------------------------------------
//
void CXnEffectManager::RemoveEffect( TXnEffect* aEffect )
    {
    TInt index = iEffects.Find( aEffect );
    if ( index != KErrNotFound )
        {
        TXnEffect* temp = iEffects[index];
        iEffects.Remove( index );
        delete temp;
        }
    }

//  End of File
