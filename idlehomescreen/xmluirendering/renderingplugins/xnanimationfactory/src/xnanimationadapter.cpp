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
#include <aknsdrawutils.h>
#include <aknsconstants.h>
#include <barsread.h>
#include "xnnodepluginif.h"
#include "xndomproperty.h"
#include "xnproperty.h"
#include "xncontroladapter.h"

#include "xnanimationadapter.h"

const TInt KPeriodicTimerInterval5Sec(100000); // 0.1 sec
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
    iAnimationSarted = EFalse;
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
    if (iPeriodicTimer)
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        iPeriodicTimer = NULL;
        }
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
               iDisplay = ETrue; 
               StartAnimation();
               }    
           else
               {
               iDisplay = EFalse;
               StopAnimation();
               }
           }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::MakeVisible()
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::MakeVisible( TBool aVisible )
    {
    if( aVisible )
        {
        StartAnimation();
        }
    else
        {
        StopAnimation();
        }
    CCoeControl::MakeVisible( aVisible );
    }

// -----------------------------------------------------------------------------
// CXnAnimationAdapter::Update
// -----------------------------------------------------------------------------
//
void CXnAnimationAdapter::Update()
    {
    if (iAnimationSarted )
        {
        switch(iSkinId.iMinor )
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
    CXnAnimationAdapter* self = static_cast<CXnAnimationAdapter*> (aAny);

    // Update widget every 5 seconds
    self->Update();

    return KErrNone; // Return value ignored by CPeriodic
    }

// ----------------------------------------------------------------------------
// CXnAnimationAdapter::StartAnimation()
// ----------------------------------------------------------------------------
//
void CXnAnimationAdapter::StartAnimation()
    {
    if ( !iAnimationSarted && iDisplay )
        {
       TRAPD(err, iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityIdle) );
       if ( err == KErrNone )
           {
           iPeriodicTimer->Start( KPeriodicTimerInterval5Sec,
                   KPeriodicTimerInterval5Sec, TCallBack( TimerCallBack, this ) );
           iSkinId = KAknsIIDQgnHomeRefreshing1;
           iAnimationSarted = ETrue;
           }
        }
    }

// ----------------------------------------------------------------------------
// CXnAnimationAdapter::StopAnimation()
// ----------------------------------------------------------------------------
//
void CXnAnimationAdapter::StopAnimation()
    {
    if ( iAnimationSarted )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        iPeriodicTimer = NULL;
        iSkinId = KAknsIIDNone;
        iAnimationSarted = EFalse;
        }
    }

// End of File

