/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  highlight control class
*
*/
#include <eiklbi.h>

#include "mmhighlighttimer.h"
#include "mmwidgetcontainer.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmHighlightTimer::CMmHighlightTimer(CMmWidgetContainer* aContainer) :
	CActive(EPriorityStandard), iState(EInactivityState), 
	iContainer( aContainer )
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CMmHighlightTimer* CMmHighlightTimer::NewLC(
		CMmWidgetContainer* aContainer)
	{
	ASSERT(aContainer);
	CMmHighlightTimer* self = new (ELeave) CMmHighlightTimer( aContainer );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CMmHighlightTimer* CMmHighlightTimer::NewL(
		CMmWidgetContainer* aContainer)
	{
	CMmHighlightTimer* self = CMmHighlightTimer::NewLC( aContainer );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::ConstructL()
	{
	User::LeaveIfError( iTimer.CreateLocal() );
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmHighlightTimer::~CMmHighlightTimer()
	{
	Cancel();
	iTimer.Close();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::DoCancel()
	{
	if (iState == ETimerExpirationCompletionState)
		{
		iTimer.Cancel();
		}
	else if (iState == EExternalCompletionState)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrCancel );
		}
	iState = EInactivityState;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::StartL( TInt aTime, TBool aPressDownState )
	{
	CListBoxView* view = iContainer->Widget()->View();
	if ( IsActive() &&
			iTimerHighlightIndex != iContainer->Widget()->CurrentItemIndex())
		{
		// draw over previous highlight
		// this occurs when e.g. highlight is set by rocker and later another 
		// item is pressed.
		DrawHighlightL( iTimerHighlightIndex );
		}
	Cancel();
	if (aTime > 0)
		{
		iTimer.After(iStatus, aTime);
		iState = ETimerExpirationCompletionState;
		}
	else
		{
		iStatus = KRequestPending;
		iState = EExternalCompletionState;
		}

	SetActive();
	iContainer->SetHighlightVisibilityL( ETrue );
		{
		SetPressDownState( aPressDownState );
		TInt currentItemIndex =
			iContainer->Widget()->View()->CurrentItemIndex();
		DrawHighlightL( currentItemIndex );
		iTimerHighlightIndex = currentItemIndex;
		}
	iContainer->SetHighlightVisibilityL( EFalse );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::RunL()
	{
	DrawHighlightL(iContainer->Widget()->View()->CurrentItemIndex());
	iState = EInactivityState;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmHighlightTimer::RunError(TInt /*aError*/)
	{
	iState = EInactivityState;
	return KErrNone;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::StopL( TBool aDrawOverHighlight )
	{
	if (IsActive())
		{
		Cancel();
		if ( aDrawOverHighlight )
			{
			DrawHighlightL( iContainer->Widget()->View()->CurrentItemIndex() );
			}
		}
	iState = EInactivityState;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmHighlightTimer::TimerHighlightIndex() const
	{
	return iTimerHighlightIndex;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::DrawHighlightL(TInt aItemIndex)
	{
	if ( iContainer->IsVisible() )
		{
		CListBoxView* view = iContainer->Widget()->View();
		iContainer->Widget()->DrawNow( TRect(
				view->ItemPos( aItemIndex ), view->ItemSize(
						aItemIndex) ) );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::SetPressDownState(TBool aPressDownState)
	{
	if ( aPressDownState )
		{
		iContainer->Widget()->View()->ItemDrawer()->SetFlags(
				CListItemDrawer::EPressedDownState );
		}
	else 
		{
		iContainer->Widget()->View()->ItemDrawer()->ClearFlags(
				CListItemDrawer::EPressedDownState );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmHighlightTimer::ContinueL( TInt aTime )
	{
	if ( IsActive() )
		{
		Cancel();
		if ( aTime > 0 )
			{
			iTimer.After( iStatus, aTime );
			iState = ETimerExpirationCompletionState;
			}
		else
			{
			iStatus = KRequestPending;
			iState = EExternalCompletionState;
			}
		SetActive();
		}
	}
// End of file
