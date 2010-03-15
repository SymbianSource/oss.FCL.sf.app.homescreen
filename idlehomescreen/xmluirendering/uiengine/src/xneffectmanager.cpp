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


// System includes
#include <eikapp.h>
#include <aknappui.h>
#include <gfxtranseffect/gfxtranseffect.h>  
#include <akntranseffect.h>                 

// User includes
#include "xneffectmanager.h"
#include "xnplugindata.h"
#include "xnviewdata.h"
#include "xnnode.h"
#include "xndomnode.h"
#include "xncontroladapter.h"

// Constants
const TInt KWaitForLayout = 1;
const TInt KEffectStarted = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnEffectManager::CXnEffectManager
//
// -----------------------------------------------------------------------------
//
CXnEffectManager::CXnEffectManager()
    {
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::ConstructL
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::NewL
//
// -----------------------------------------------------------------------------
//
CXnEffectManager* CXnEffectManager::NewL()
    {
    CXnEffectManager* self = new ( ELeave ) CXnEffectManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::~CXnEffectManager
//
// -----------------------------------------------------------------------------
//
CXnEffectManager::~CXnEffectManager()
    {
    GfxTransEffect::AbortFullScreen();
    iEffects.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginFullscreenEffectL
//
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
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::UiRendered()
    {
    for ( TInt i = 0; i < iEffects.Count(); )
        {
        TXnEffect* effect( iEffects[i] );
        
        if ( effect && effect->iState == KEffectStarted )            
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
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::UiLayouted()
    {
    for ( TInt i = 0; i < iEffects.Count(); )
        {
        TBool effectStarted( ETrue );
        
        TXnEffect* effect( iEffects[i] );
        
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
//
// -----------------------------------------------------------------------------
//
TBool CXnEffectManager::DoBeginFullscreenEffect( TXnEffect& aEffect )
    {
    CCoeEnv* env( CCoeEnv::Static() );
           
    RWsSession& session( env->WsSession() );
    
    if ( session.GetFocusWindowGroup() != env->RootWin().Identifier() )          
        {
        // Window group is not focused
        return EFalse;
        }

    const TInt flags( AknTransEffect::TParameter::EActivateExplicitCancel );
    const TUid targetAppUid( iAvkonAppUi->Application()->AppDllUid() );
    
    // Must give some time before starting effect, because otherwise
    // fullscreen effect may contain unwanted parts (dialog, note, etc.)
    // which was shown when fullscreen effect is about to be started
    session.Finish();
    User::After( 1000 );
        
    // Set effect begin point
    GfxTransEffect::BeginFullScreen( aEffect.iId , iAvkonAppUi->ClientRect(),
        AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(
        targetAppUid, flags ) );
    
    aEffect.iState = KEffectStarted;
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::RemoveEffect
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::RemoveEffect( TXnEffect* aEffect )
    {
    TInt index( iEffects.Find( aEffect ) );
    
    if ( index != KErrNotFound )
        {
        TXnEffect* temp( iEffects[index] ); 
        delete temp;
        temp = NULL;
        
        iEffects.Remove( index );        
        }
    }

//  End of File
