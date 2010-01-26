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
* Description:
*
*/

#include "mmpostevaluationprocessor.h"
#include "mmlistboxitemdrawer.h"
#include "mmlistboxmodel.h"
#include <layoutmetadata.cdl.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmPostEvaluationProcessor::CMmPostEvaluationProcessor(CMmListBoxItemDrawer& aDrawer) :
    CActive(EPriorityLess), iDrawer(aDrawer), iIndexToProcess(0)
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmPostEvaluationProcessor* CMmPostEvaluationProcessor::NewLC(CMmListBoxItemDrawer& aDrawer)
    {
    CMmPostEvaluationProcessor* self = new ( ELeave ) CMmPostEvaluationProcessor(aDrawer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmPostEvaluationProcessor* CMmPostEvaluationProcessor::NewL(CMmListBoxItemDrawer& aDrawer)
    {
    CMmPostEvaluationProcessor* self = CMmPostEvaluationProcessor::NewLC(aDrawer);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmPostEvaluationProcessor::ConstructL()
    {
    CActiveScheduler::Add( this); // Add to scheduler
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmPostEvaluationProcessor::~CMmPostEvaluationProcessor()
    {
    Cancel(); // Cancel any request, if outstanding
    // Delete instance variables if any
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmPostEvaluationProcessor::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmPostEvaluationProcessor::RunL()
    {      
    const TDesC8& mm_template = iDrawer.iMmModel->GetAttributeAsText( 
    		iIndexToProcess, KMmTemplate8 );      
    
    if ( mm_template.Compare( KNullDesC8 ) && mm_template.Compare( KEmpty8 ) )
    	{
        iDrawer.SetupSubCellsL( EFalse, iIndexToProcess );
    	}
    
    if ( ++iIndexToProcess < iDrawer.iMmModel->NumberOfItems() )
    	{
    	SetActive();
    	TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrNone );
    	}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TInt CMmPostEvaluationProcessor::RunError(TInt aError)
    {
    return aError;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmPostEvaluationProcessor::StartAt( TInt aIndex )
	{
	Cancel();
	iIndexToProcess = aIndex;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

//End of file
