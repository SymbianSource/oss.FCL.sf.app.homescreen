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
#include <alf/alfcompositionutility.h>
#include <layoutmetadata.cdl.h>

// User includes
#include "xnappuiadapter.h"
#include "xnbackgroundmanager.h"
#include "xneffectmanager.h"
#include "xnplugindata.h"
#include "xnviewdata.h"
#include "xnviewadapter.h"
#include "xnnode.h"
#include "xndomnode.h"
#include "xncontroladapter.h"

// Constants
const TInt KWaitForLayout( 1 );
const TInt KEffectStarted( 2 );

const TInt KControlEffectWaitInterval( 100000 ); // 100ms
const TInt KFullScreenEffectWaitInterval( 300000 ); // 300ms
const TInt KWaitInterval( 25000 ); // 25ms
const TInt KInterval( 500000 );

#define IS_VALID( e ) \
    ( e == KGfxContextActivateNextView || \
    e == KGfxContextActivatePrevView )

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnEffectManager::CXnEffectManager
//
// -----------------------------------------------------------------------------
//
CXnEffectManager::CXnEffectManager( CXnAppUiAdapter& aAppUiAdapter )
    : CTimer( CActive::EPriorityIdle ), iAppUiAdapter( aAppUiAdapter ), 
      iGroupId( -1 )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::ConstructL
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::ConstructL()
    {
    CTimer::ConstructL();
    
    OrientationChanged();

#ifndef NO_ALF_OBSERVER    
    iObserver = CAlfEffectObserver::NewL();
#endif
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::NewL
//
// -----------------------------------------------------------------------------
//
CXnEffectManager* CXnEffectManager::NewL( CXnAppUiAdapter& aAppUiAdapter )
    {
    CXnEffectManager* self = new ( ELeave ) CXnEffectManager( aAppUiAdapter );
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
    Cancel();
           
    delete iObserver;
           
    iControls.Reset();
    
    iEffects.Reset();
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::RunL
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::RunL() 
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginFullscreenEffectL
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginFullscreenEffectL( TInt aId, CXnViewData& aView )
    {    
    TXnEffect effect;
    
    effect.iId = aId;    
    effect.iNode = aView.ViewNode();
    
    if ( !aView.ViewNode()->IsLaidOut() )
        {
        effect.iState = KWaitForLayout;
        
        iEffects.AppendL( effect );               
        }
    else
        {
        if ( DoBeginFullscreenEffect( effect ) )
            {
            iEffects.AppendL( effect );                               
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BeginFullscreenEffectL
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BeginFullscreenEffectL( TInt aId )
    {
    TXnEffect effect;
    
    effect.iId = aId;
    effect.iExplicitEnd = ETrue;
    
    if ( DoBeginFullscreenEffect( effect ) )
        {
        iEffects.AppendL( effect );
        }
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::EndFullscreenEffect
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::EndFullscreenEffect( TInt aId )
    {
    for ( TInt i = 0; i < iEffects.Count(); i++ )
        {
        TXnEffect& effect( iEffects[i] );
        
        if ( effect.iId == aId && 
            effect.iExplicitEnd && 
            effect.iState == KEffectStarted )
            {
            GfxTransEffect::EndFullScreen();
            
            iEffects.Remove( i );
            break;
            }
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
        TXnEffect& effect( iEffects[i] );
        
        if ( effect.iState == KEffectStarted && !effect.iExplicitEnd )            
            {
            GfxTransEffect::EndFullScreen();
                        
            iEffects.Remove( i );            
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
        TBool started( ETrue );
        
        TXnEffect& effect( iEffects[i] );
        
        if ( effect.iNode && effect.iState == KWaitForLayout &&             
             effect.iNode->IsLaidOut() )
            {
            started = DoBeginFullscreenEffect( effect );                       
            }
        
        if ( !started )
            {
            iEffects.Remove( i );            
            }
        else
            {
            i++;
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
    if ( !iAppUiAdapter.IsForeground() )          
        {
        // Not in foreground
        return EFalse;
        }
    
    if ( !WaitActiveEffect( KFullScreenEffectWaitInterval ) )
        {
        return EFalse;
        }

    const TInt flags( AknTransEffect::TParameter::EActivateExplicitCancel );
    const TUid targetAppUid( iAppUiAdapter.Application()->AppDllUid() );
    
    // Set effect begin point
    GfxTransEffect::BeginFullScreen( aEffect.iId , iAppUiAdapter.ClientRect(),
        AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(
        targetAppUid, flags ) );
    
    aEffect.iState = KEffectStarted;
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::WaitActiveEffect
//
// -----------------------------------------------------------------------------
//
TBool CXnEffectManager::WaitActiveEffect( TInt aInterval )     
    {
    if (iObserver == NULL)
        {
        return ETrue;
        }

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
// CXnEffectManager::BeginActivateViewEffect
//
// -----------------------------------------------------------------------------
//
TBool CXnEffectManager::BeginActivateViewEffect( const CXnViewData& aThis, 
    const CXnViewData& aOther, TUid aEffect )
    {
    CleanupControlEffect();
    
    if ( !IS_VALID( aEffect ) )
        {
        // Not a valid view switch effect uid
        return EFalse;
        }
            
    CXnControlAdapter* thisView( aThis.ViewNode()->Control() );                             
    CXnControlAdapter* otherView( aOther.ViewNode()->Control() ); 
    
    if ( thisView == otherView )
        {
        // Same controls
        return EFalse;
        }
    
    if ( IsActive() )
        {
        Cancel();
        After( KInterval );
        
        return EFalse;
        }
    
    if ( !WaitActiveEffect( KControlEffectWaitInterval ) )
        {               
        After( KInterval );
        
        return EFalse;
        }
    
    iControls.Append( thisView );
    iControls.Append( otherView );
               
    GfxTransEffect::Register( thisView, aEffect );    
    GfxTransEffect::Register( otherView, aEffect );

    iGroupId = GfxTransEffect::BeginGroup();
    
    CFbsBitmap* currentBg( aThis.WallpaperImage() );
    CFbsBitmap* nextBg( aOther.WallpaperImage() );
           
    if ( currentBg || nextBg )
        {
        CCoeControl* bg( &iAppUiAdapter.ViewAdapter().BgManager() );
             
        iControls.Append( bg );
        
        if ( !currentBg && nextBg )
            {           
            GfxTransEffect::Begin( bg, KGfxControlActionBgAnimToImgAppear );
            }
        else
            {
            GfxTransEffect::Begin( bg, KGfxControlActionBgImgToImgAppear );
            }
        
        iBgEffect = ETrue;
        }
    
    // because of changes in nga end has to call here. Effects below are
    // grouped and those will be triggered when calling GfxTransEffect::EndGroup
    if ( iLandscape )
        {
        GfxTransEffect::Begin( thisView, KGfxControlActionDisappearLsc );
        GfxTransEffect::Begin( otherView, KGfxControlActionAppearLsc );
        
        GfxTransEffect::SetDemarcation( otherView, otherView->Position() );
        GfxTransEffect::End( otherView );
            
        GfxTransEffect::SetDemarcation( thisView, thisView->Position() );
        GfxTransEffect::End( thisView );
        }
    else
        {
        GfxTransEffect::Begin( thisView, KGfxControlActionDisappearPrt );
        GfxTransEffect::Begin( otherView, KGfxControlActionAppearPrt );
        
        GfxTransEffect::SetDemarcation( otherView, otherView->Position() );
        GfxTransEffect::End( otherView );
            
        GfxTransEffect::SetDemarcation( thisView, thisView->Position() );
        GfxTransEffect::End( thisView );
        }    
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::EndActivateViewEffect
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::EndActivateViewEffect( const CXnViewData& aThis, 
    const CXnViewData& aOther, TUid aEffect )
    {
    if ( !IS_VALID( aEffect ) || iGroupId == -1 )
        {
        // Not a valid view switch effect uid or group effect is not started
        return;
        }
    
    CXnControlAdapter* thisView( aThis.ViewNode()->Control() );                             
    CXnControlAdapter* otherView( aOther.ViewNode()->Control() );
    
    if ( iBgEffect )
        {    
        CCoeControl* bg( &iAppUiAdapter.ViewAdapter().BgManager() );
        
        GfxTransEffect::SetDemarcation( bg, bg->Position() );
        GfxTransEffect::End( bg );        
        }
                            
    GfxTransEffect::EndGroup( iGroupId );
    
    GfxTransEffect::Deregister( otherView );
    GfxTransEffect::Deregister( thisView );
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::BgAppearEffect
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::BgAppearEffect( CCoeControl* aBg, TBool aWaitActiveEffect )
    {
    if ( aWaitActiveEffect )
        {
        WaitActiveEffect( KFullScreenEffectWaitInterval );               
        }
    
    GfxTransEffect::Begin( aBg, KGfxControlActionBgImgToImgAppear );
    
    GfxTransEffect::SetDemarcation( aBg, aBg->Position() );
    GfxTransEffect::End( aBg );                         
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::Cleanup
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::CleanupControlEffect()
    {
    iBgEffect = EFalse;
    iGroupId = -1;       
    
    iControls.Reset();        
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::ControlEffectActive
//
// -----------------------------------------------------------------------------
//
TBool CXnEffectManager::ControlEffectActive( const CCoeControl* aControl ) const
    {
    TInt index( iControls.Find( aControl ) );
    
    return index != KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CXnEffectManager::OrientationChanged
//
// -----------------------------------------------------------------------------
//
void CXnEffectManager::OrientationChanged()
    {
    iLandscape = Layout_Meta_Data::IsLandscapeOrientation();
    }

//  End of File
