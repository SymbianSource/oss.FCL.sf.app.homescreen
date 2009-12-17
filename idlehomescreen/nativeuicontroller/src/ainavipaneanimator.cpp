/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Navipane animator.
*
*/


#include "ainavipaneanimator.h"
#include "aistatuspanel.h"
#include "ainavipaneanimatorcallback.h"

using namespace AiNativeUiController;

const TInt KAnimDelay = 2000000;


CAiNaviPaneAnimator::CAiNaviPaneAnimator( CAiStatusPanel& aStatusPane,
                AiNativeUiModel::TRenderingPriorities& aPriorities,
                MAiNaviPaneAnimatorCallback& aCallback )
    : iStatusPane( aStatusPane ),
        iPriorities( aPriorities ),
        iCallback( aCallback )
    {
    }


void CAiNaviPaneAnimator::ConstructL()
    {
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    }


CAiNaviPaneAnimator* CAiNaviPaneAnimator::NewL( CAiStatusPanel& aStatusPane,
                AiNativeUiModel::TRenderingPriorities& aPriorities,
                MAiNaviPaneAnimatorCallback& aCallback )
    {
    CAiNaviPaneAnimator* self = new( ELeave )CAiNaviPaneAnimator( aStatusPane,
                                                            aPriorities,
                                                            aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiNaviPaneAnimator::~CAiNaviPaneAnimator()
    {
    iAnimArray.ResetAndDestroy();
    if( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    }


void CAiNaviPaneAnimator::AddItemL( TInt aContentId, const TDesC16& aText )
    {
    const TInt count = iAnimArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iAnimArray[i]->Name() == aContentId )
            {
            iAnimArray[i]->SetValueL( aText );
            return;
            }
        }

    CAiNameValuePair* newItem = CAiNameValuePair::NewLC(
                                                    aContentId,
                                                    aText );
    User::LeaveIfError( iAnimArray.Append( newItem ) );
    CleanupStack::Pop( newItem );
    }


void CAiNaviPaneAnimator::RemoveItem( TInt aContentId )
    {
    const TInt count = iAnimArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iAnimArray[i]->Name() == aContentId )
            {
            delete iAnimArray[i];
            iAnimArray[i] = NULL;
            iAnimArray.Remove( i );
            return;
            }
        }
    }


void CAiNaviPaneAnimator::CancelAnimationL()
    {
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        iCurrentItem = 0;
        iCallback.AnimationCompleteL();
        }
    }

TBool CAiNaviPaneAnimator::IsActive()
	{
	return iPeriodic->IsActive();
	}

void CAiNaviPaneAnimator::AnimateL()
    {
    if( !iPeriodic->IsActive() )
        {
        SortAnimArrayL();
        //draw first item immediately
        if( DrawTextL() )
            {
            iPeriodic->Start( KAnimDelay, KAnimDelay, TCallBack( AnimCallback, this ) );
            }
        }
    }


TInt CAiNaviPaneAnimator::AnimCallback( TAny* aPtr )
    {
    CAiNaviPaneAnimator* self = static_cast<CAiNaviPaneAnimator*>( aPtr );

    if( self )
        {
        TRAP_IGNORE( self->DrawTextL() );
        }

    return KErrNone;
    }


TBool CAiNaviPaneAnimator::DrawTextL()
    {
    if( iCurrentItem < iAnimArray.Count() )
        {
        iStatusPane.SetNaviPaneTextL( iAnimArray[iCurrentItem]->Value() );
        iStatusPane.RenderNaviPaneL();
        ++iCurrentItem;
        return ETrue;
        }
    else
        {
        iPeriodic->Cancel();
        iCurrentItem = 0;
        iCallback.AnimationCompleteL();
        return EFalse;
        }
    }

void CAiNaviPaneAnimator::SortAnimArrayL()
    {
    const TInt count = iAnimArray.Count();
    
    // loop through the animator array and sort items in priority order
    for( TInt i = 0; i < count - 1; i++ )
        {
        for( TInt k = i + 1; k < count; k++)
            {
            if( iPriorities.RenderingPriority( iAnimArray[i]->Name() ) <
                iPriorities.RenderingPriority( iAnimArray[k]->Name() ) )
                {
                CAiNameValuePair* temp = iAnimArray[k];
                iAnimArray[k] = iAnimArray[i];
                iAnimArray[i] = temp;
                }
            }
        }
    }


CAiNaviPaneAnimator::CAiNameValuePair*
CAiNaviPaneAnimator::CAiNameValuePair::NewLC( const TInt aName,
                                                const TDesC& aValue )
    {
    CAiNameValuePair* self = new( ELeave ) CAiNameValuePair();
    CleanupStack::PushL( self );
    self->ConstructL( aName, aValue );
    return self;
    }


void CAiNaviPaneAnimator::CAiNameValuePair::ConstructL( const TInt aName,
                                                        const TDesC& aValue )
    {
    iName = aName;
    iValue = aValue.AllocL();
    }


CAiNaviPaneAnimator::CAiNameValuePair::~CAiNameValuePair()
    {
    delete iValue;
    }


TInt CAiNaviPaneAnimator::CAiNameValuePair::Name() const
    {
    return iName;
    }


const TDesC16& CAiNaviPaneAnimator::CAiNameValuePair::Value() const
    {
    return *iValue;
    }


void CAiNaviPaneAnimator::CAiNameValuePair::SetValueL( const TDesC& aValue )
    {
    HBufC* temp = aValue.AllocL();
    delete iValue;
    iValue = temp;
    }
