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
const TInt KEffectTypeFullscreen = 1;
const TInt KEffectTypeControl = 2;

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
// CXnEffectManager::BeginControlEffectL
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginControlEffectL( TInt /*aId*/, CXnPluginData& /*aPlugin*/ )
    {
    /*
    TXnEffect* effect = new (ELeave) TXnEffect;
    CleanupStack::PushL( effect );
    effect->iId = aId;
    effect->iType = KEffectTypeControl;
    effect->iNode = aPlugin.Node()->LayoutNode();
    iEffects.AppendL( effect );
    CleanupStack::Pop( effect );
    
    if ( effect->iNode && !effect->iNode->IsLaidOut() )
        {
        effect->iState = KWaitForLayout;
        }
    else
        {
        DoBeginControlEffect( *effect );
        }
    */
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginControlEffectL
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginControlEffectL( TInt aId,
                RPointerArray<CXnPluginData>& aPlugins )
    {
    TInt count = aPlugins.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        BeginControlEffectL( aId, *aPlugins[i] );
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginFullscreenEffectL
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginFullscreenEffectL( TInt aId, CXnViewData& aView )
    {
    // Only one fullscreen effect at time
    if ( EffectCount( KEffectTypeFullscreen ) != 0 )
        {
        return;
        }
    
    TXnEffect* effect = new (ELeave) TXnEffect;
    CleanupStack::PushL( effect );
    effect->iId = aId;
    effect->iType = KEffectTypeFullscreen;
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
        TInt count = iEffects.Count(); // for debugging
        TXnEffect* effect = iEffects[i];
        if ( effect && effect->iState == KEffectStarted )
            {
            if ( effect->iType == KEffectTypeFullscreen )
                {
                GfxTransEffect::EndFullScreen();
                }
            else if ( effect->iType == KEffectTypeControl )
                {
                GfxTransEffect::End( effect->iNode->Control() );
                }
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
    TInt count = iEffects.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TXnEffect* effect = iEffects[i];
        if ( effect && effect->iNode &&
             effect->iState == KWaitForLayout &&
             effect->iNode->IsLaidOut())
            {
            if ( effect->iType == KEffectTypeFullscreen )
                {
                DoBeginFullscreenEffect( *effect );
                }
            else if ( effect->iType == KEffectTypeControl )
                {
                DoBeginControlEffect( *effect );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::DoBeginFullscreenEffect
// -----------------------------------------------------------------------------
//
void CXnEffectManager::DoBeginFullscreenEffect( TXnEffect& aEffect )
    {
    CCoeEnv* coe( CCoeEnv::Static() );
           
    if ( coe->WsSession().GetFocusWindowGroup() != 
         coe->RootWin().Identifier() )
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
// CXnEffectManager::DoBeginControlEffect
// -----------------------------------------------------------------------------
//
void CXnEffectManager::DoBeginControlEffect( TXnEffect& aEffect )
    {
    CCoeEnv* coe( CCoeEnv::Static() );
           
    if ( coe->WsSession().GetFocusWindowGroup() != 
         coe->RootWin().Identifier() )
        {
        // Window group is not focused
        RemoveEffect( &aEffect );
        return;
        }

    // Set effect begin point
    if ( aEffect.iNode )
        {
        GfxTransEffect::Begin( aEffect.iNode->Control() , aEffect.iId );
        aEffect.iState = KEffectStarted;
        }
    else
        {
        RemoveEffect( &aEffect );
        }
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

// -----------------------------------------------------------------------------
// CXnEffectManager::EffectCount
// -----------------------------------------------------------------------------
//
TInt CXnEffectManager::EffectCount( TInt aType )
    {
    TInt effectCount = 0;
    TInt count = iEffects.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iEffects[i]->iType == aType )
            {
            effectCount++;
            }
        }
    return effectCount;
    }

//  End of File
