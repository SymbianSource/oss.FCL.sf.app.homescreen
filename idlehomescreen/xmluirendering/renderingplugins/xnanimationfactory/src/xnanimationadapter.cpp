/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon Bitmap Plugin.
*
*/
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsConstants.h>
#include <barsread.h>
#include <debug.h>

#include "xnnodepluginif.h"
#include "xndomproperty.h"
#include "xnproperty.h"
#include "xncontroladapter.h"

#include "xnanimationadapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnAnimationAdapter* CXnAnimationAdapter::NewL(CXnNodePluginIf& aNode)
    {
	CXnAnimationAdapter* self = new( ELeave ) CXnAnimationAdapter( aNode );
    
    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::ConstructL(CXnNodePluginIf& aNode)
    {
    CXnControlAdapter::ConstructL( aNode );
    iSkinId = KAknsIIDNone;
    }
    
// -----------------------------------------------------------------------------
// CXnAnimationAdapter::CXnAnimationAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnAnimationAdapter::CXnAnimationAdapter(CXnNodePluginIf& aNode) : iNode( aNode )    
    {
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::~CXnAnimationAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnAnimationAdapter::~CXnAnimationAdapter()
    {
    StopAnimation();
    }


// -----------------------------------------------------------------------------
// CXnAnimationAdapter::Draw
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::Draw(const TRect& aRect) const
    {
    CXnControlAdapter::Draw( aRect );    
    AknsDrawUtils::DrawCachedImage( AknsUtils::SkinInstance(),
               SystemGc(), aRect, iSkinId  );
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::HandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name( ) );
        
        if( name == XnPropertyNames::style::common::KDisplay )
           {
           const TDesC8& display( aProperty->StringValue() );
 
           if( display == XnPropertyNames::style::common::display::KBlock )
               {
               StartAnimation();
               }    
           else
               {
               StopAnimation();
               }
           }
        }
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::DoEnterPowerSaveModeL
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::DoEnterPowerSaveModeL( TModeEvent /*aEvent*/ )
    {
    iInPowerSaveMode = ETrue;
    StopAnimation();
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::DoExitPowerSaveModeL
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::DoExitPowerSaveModeL( TModeEvent /*aEvent*/ )
    {
    iInPowerSaveMode = EFalse;
    StartAnimation();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::MakeVisible()
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::MakeVisible( TBool aVisible )
    {
    TBool visible( IsVisible() ? ETrue : EFalse );
    if ( visible == aVisible ) { return; }
    
    CCoeControl::MakeVisible( aVisible );
    
    if ( aVisible )
        {
        StartAnimation();
        }
    else
        {
        StopAnimation();
        }
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::Update
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::Update()
    {
    if ( iPeriodicTimer && iPeriodicTimer->IsActive() )
        {
        switch( iSkinId.iMinor )
            {
            case EAknsMinorGenericQgnHomeRefreshing1:
                {
                iSkinId.Set(KAknsIIDQgnHomeRefreshing2);
                }
                break;
            case EAknsMinorGenericQgnHomeRefreshing2:
                {
                iSkinId.Set(KAknsIIDQgnHomeRefreshing3);
                }
                break;
            case EAknsMinorGenericQgnHomeRefreshing3:
                {
                iSkinId.Set(KAknsIIDQgnHomeRefreshing4);
                }
                break;                
            case EAknsMinorGenericQgnHomeRefreshing4:
                {
                iSkinId.Set(KAknsIIDQgnHomeRefreshing1);
                }
                break;
            default :
                break;
            };
        DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CXnAnimationAdapter::PeriodicTimerCallBack(TAny* aAny)
// The call back function.
// ----------------------------------------------------------------------------
//
TInt CXnAnimationAdapter::TimerCallBack(TAny* aAny)
    {
    __PRINTS( "CXnAnimationAdapter::TimerCallback, timer runs" );
    CXnAnimationAdapter* self = static_cast<CXnAnimationAdapter*> (aAny);

    // Update widget
    self->Update();

    return KErrNone; // Return value ignored by CPeriodic
    }

// ----------------------------------------------------------------------------
// CXnAnimationAdapter::StartAnimation()
// ----------------------------------------------------------------------------
//
void CXnAnimationAdapter::StartAnimation()
    {
    if ( !iPeriodicTimer && IsVisible() && !iInPowerSaveMode )
        {
       TRAPD(err, iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityIdle) );
       if ( err == KErrNone )
           {
		   const TInt KPeriodicTimerInterval = 100000; // 0.1 sec
           iPeriodicTimer->Start( KPeriodicTimerInterval,
                   KPeriodicTimerInterval, TCallBack( TimerCallBack, this ) );
           iSkinId = KAknsIIDQgnHomeRefreshing1;
           }
        }
    }

// ----------------------------------------------------------------------------
// CXnAnimationAdapter::StopAnimation()
// ----------------------------------------------------------------------------
//
void CXnAnimationAdapter::StopAnimation()
    {
    if ( iPeriodicTimer && iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }
    delete iPeriodicTimer;
    iPeriodicTimer = NULL;
    iSkinId = KAknsIIDNone;
    }

// End of File

