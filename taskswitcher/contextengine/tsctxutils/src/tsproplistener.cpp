/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <e32cmn.h>
#include "tsproplistener.h"

// --------------------------------------------------------------------------
// CTsPropertyListener::CTsPropertyListener
// --------------------------------------------------------------------------
//
EXPORT_C CTsPropertyListener::CTsPropertyListener(
        TUid aCategory, TUint aKey, MTsPropertyChangeObserver& aObserver )
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

// --------------------------------------------------------------------------
// CTsPropertyListener::~CTsPropertyListener
// --------------------------------------------------------------------------
//
EXPORT_C CTsPropertyListener::~CTsPropertyListener()
    {
    Cancel();
    iProperty.Close();
    }

// --------------------------------------------------------------------------
// CTsPropertyListener::DoCancel
// --------------------------------------------------------------------------
//
void CTsPropertyListener::DoCancel()
    {
    iProperty.Cancel();
    }
    
// --------------------------------------------------------------------------
// CTsPropertyListener::RunL
// --------------------------------------------------------------------------
//
void CTsPropertyListener::RunL()
    {
    if ( iStatus.Int() != KErrCancel ) // when cancelling the subscribe it completes with KErrCancel
        {
        iObserver.PropertyChanged( iCategory, iKey );
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }    

// --------------------------------------------------------------------------
// CTsPropertyListener::RunError
// --------------------------------------------------------------------------
//
TInt CTsPropertyListener::RunError( TInt /*aError*/ )
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    return KErrNone;
    }
    
// end of file
