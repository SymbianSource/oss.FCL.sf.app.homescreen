/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  suite presentation models container
*
*/


#include "hnsuitesstack.h"
#include "hneventhandler.h"
#include "hnmdmodel.h"
#include "hnsuitemodel.h"
#include "hnsuitemodelcontainer.h"
#include "hnglobals.h"
#include "hncontrollerinterface.h"

const TInt KParentSuiteDistance( 2 );

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnSuiteModelContainer::ConstructL( 
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController )
    {
    iSuitesStack = CHnSuitesStack::NewL( aController );
    iEventHandler = CHnEventHandler::NewL( aModelObserver , aController );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModelContainer* CHnSuiteModelContainer::NewL(
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController  )
    {
    CHnSuiteModelContainer* self = CHnSuiteModelContainer::NewLC(
            aModelObserver , aController );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModelContainer* CHnSuiteModelContainer::NewLC(
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController )
    {
    CHnSuiteModelContainer* self = new( ELeave ) CHnSuiteModelContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aModelObserver, aController );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnSuiteModelContainer::CHnSuiteModelContainer()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSuiteModelContainer::~CHnSuiteModelContainer()
    {
    delete iSuitesStack;
    delete iEventHandler;
    }
 
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::PopSuiteModelL(
        const TDesC& aSuiteName )
    {
    CHnSuiteModel* suiteModel = iSuitesStack->PopL();
    
    if ( suiteModel &&
            suiteModel->SuiteName().Compare( aSuiteName ) )
        {
        User::LeaveIfError( KErrNotFound );
        }

    delete suiteModel;
    suiteModel = iSuitesStack->GetLast();
    return suiteModel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuiteModelContainer::OfferHnEventL( const TInt aEventId,
        const TInt aRecipientId, CLiwGenericParamList* aEventParameters )
    {
    CHnSuiteModel* lastSuiteModel = GetLastSuiteModel();
    TInt err = KErrNotFound;
    
    if( aEventId == KKeyIdMoveHighlight )
        {
        for( int i( 0 ); i<GetSuiteModelCount(); i++ )
            {
            CHnSuiteModel* model = GetSuiteModel( i );
            TBool isRoot( model->CustomId() == KRootId );
            if( model && !isRoot )
                {
                model->SetSuiteHighlightL( model->GetSuiteHighlight() + aRecipientId );
                }
            } 
        }
    else
        {
        if ( lastSuiteModel ) 
            {
            err = lastSuiteModel->OfferHnEventL( aEventId, 
                    aRecipientId, aEventParameters );
            }
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModelContainer::PushNewSuiteModelL(
        const TDesC& aGenre )
    {
    CHnSuiteModel* retModel = NULL;
    retModel = CHnSuiteModel::NewL( *iEventHandler, aGenre );
    iSuitesStack->PushL( retModel );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::GetSuiteModel(
        const TDesC& aSuiteName )
    {
    ASSERT( aSuiteName.Length() );

    TInt count( iSuitesStack->Count() );
    CHnSuiteModel* retModel = NULL;

    for (TInt i = iSuitesStack->Count() - 1; i >= 0; i-- )
            {
            retModel = iSuitesStack->Get( i );
            if ( retModel && !retModel->SuiteName().Compare( aSuiteName ) )
                {
                break;
                }
            else
                {
                retModel = NULL;
                }
            }

    return retModel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnSuiteModelContainer::GetSuiteModelCount() const
    {
    return iSuitesStack->Count();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::GetLastSuiteModel()
    {
    return iSuitesStack->GetLast();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::GetParentSuiteModel()
    {
    CHnSuiteModel* model = NULL;
    if( iSuitesStack->Count() > 1 )
        {
        model = iSuitesStack->Get(
                iSuitesStack->Count() - KParentSuiteDistance );
        }
    return model;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::GetSuiteModel(TInt aPosition)
    {
    return iSuitesStack->Get( aPosition );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnSuiteModel* CHnSuiteModelContainer::GetMatchingSuiteModel(
		TInt64 aCustomId )
	{
	CHnSuiteModel* suiteModel( NULL );

	if ( aCustomId >= 0 )
		{
	    for( TInt i( 0 ); i < GetSuiteModelCount(); i++ )
	    	{
		    CHnSuiteModel* tmpSuiteModel( GetSuiteModel( i ) );
		    
		    // compare suiteCustomIds
		    if ( tmpSuiteModel && ( tmpSuiteModel->CustomId() == aCustomId ) )
		    	{
		    	suiteModel = tmpSuiteModel;
			    break;
		    	}
	    	}
		}
    
    return suiteModel;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnSuiteModelContainer::RemoveLiwObjects()
    {
    for( TInt i = 0; i < GetSuiteModelCount(); ++i )
        {
        GetSuiteModel( i )->RemoveLiwObjects();
        }
    }

// End of file
