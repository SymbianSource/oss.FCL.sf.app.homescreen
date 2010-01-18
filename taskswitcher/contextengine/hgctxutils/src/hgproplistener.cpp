/*
* ===========================================================================
*  Name        : HgPropListener.cpp
*  Part of     : Hg
*  Description : Active class to get notifications about changes to a P&S property
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent   of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ===========================================================================
*/

#include <e32cmn.h>

#include "hgproplistener.h"

EXPORT_C CHgPropertyListener::CHgPropertyListener(
        TUid aCategory, TUint aKey, MHgPropertyChangeObserver& aObserver )
	: CActive( CActive::EPriorityStandard),
	    iObserver( aObserver ), iCategory( aCategory ), iKey( aKey )
	{
	CActiveScheduler::Add( this );
	TInt err = iProperty.Attach( iCategory, iKey );
	if ( err == KErrNone )
		{
		iProperty.Subscribe( iStatus );
		SetActive();
		}
	}

EXPORT_C CHgPropertyListener::~CHgPropertyListener()
	{
	Cancel();
	iProperty.Close();
	}

void CHgPropertyListener::DoCancel()
	{
	iProperty.Cancel();
	}
    
void CHgPropertyListener::RunL()
	{
	if ( iStatus.Int() != KErrCancel ) // when cancelling the subscribe it completes with KErrCancel
		{
		iObserver.PropertyChanged( iCategory, iKey );
		iProperty.Subscribe( iStatus );
		SetActive();
		}
	}    

TInt CHgPropertyListener::RunError( TInt /*aError*/ )
	{
	iProperty.Subscribe( iStatus );
	SetActive();
	return KErrNone;
	}

	
// end of file
