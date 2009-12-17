/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer for changes in Publish & Subscribe keys
*
*/


#include "psobserver.h"
#include "mpscallback.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;

CPSObserver::CPSObserver( MPSCallback& aCallback, TUid aCategory, TInt aKey ) :
    CActive( CActive::EPriorityStandard ),
	iCallback( aCallback ),	
	iCategory( aCategory ), 
	iKey( aKey )
    {
    CActiveScheduler::Add( this );
    }

CPSObserver* CPSObserver::NewL( MPSCallback& aCallback, TUid aCategory, TInt aKey )
    {
    CPSObserver* self = new( ELeave ) CPSObserver( aCallback, aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }
    
CPSObserver::~CPSObserver()
    {
    Cancel();
    }

void CPSObserver::ConstructL()
    {
    // subscribe for changes
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    iProperty.Subscribe( iStatus );
    SetActive();
    }

void CPSObserver::RunL()
	{
    iProperty.Subscribe( iStatus );
    SetActive();

    TInt value;
    if( iProperty.Get( iCategory, iKey, value ) != KErrNotFound )
        {
		iCallback.ValueUpdatedL( iKey, value );
        }
	}

TInt CPSObserver::RunError()
	{
	Cancel();
	return KErrNone;		
	}

void CPSObserver::DoCancel()
	{
    iProperty.Close();
	}

TInt CPSObserver::GetValue( TInt &aValue ) const
    {
    return iProperty.Get( iCategory, iKey, aValue );
    }


//  End of File
