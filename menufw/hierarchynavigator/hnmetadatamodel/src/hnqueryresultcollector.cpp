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
* Description:  
*
*/


#include <liwservicehandler.h>

#include "hnqueryresultcollector.h"
#include "hnmdquery.h"
#include "hnmdqueries.h"
#include "hnconvutils.h"
#include "hnservicehandler.h"
#include "hnmdservice.h"
#include "hnmdservicecommand.h"

using namespace LIW;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnQueryResultCollector* CHnQueryResultCollector::NewL(
		const CHnMdQueries& aQueries , const CLiwGenericParamList& aParams )
	{
	CHnQueryResultCollector* self = NewLC( aQueries , aParams );
	CleanupStack::Pop( self );
	return self;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnQueryResultCollector* CHnQueryResultCollector::NewLC(
		const CHnMdQueries& aQueries , const CLiwGenericParamList& aParams )
	{
	CHnQueryResultCollector* self =
	new ( ELeave ) CHnQueryResultCollector( aQueries , aParams );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnQueryResultCollector::CHnQueryResultCollector(
		const CHnMdQueries& aQueries ,
		const CLiwGenericParamList& aParams) : CActive ( EPriorityStandard ),
                                        	   iQueries( aQueries ),
                                        	   iParams( aParams )
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnQueryResultCollector::~CHnQueryResultCollector()
	{
	Cancel();
	delete iResults;
	delete iCurrentQueryResults;
	delete iSh;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::ConstructL()
	{
	iResults = CLiwGenericParamList::NewL();
	iCurrentQueryResults = CLiwGenericParamList::NewL();
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::StartAsynchL(
        MHnQueryResultCollectorNotification* aNotification )
    {
    __ASSERT_DEBUG( aNotification , User::Panic( KMatrixPanic , 0) );
    iNotification = aNotification;
    ResetL();
    if( iQueryIterator < iQueries.Count())
        {
        ExecQueryAsynchL();
        }
    else
        {
        iNotification->ResultsCollectedL( iResults );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::ResetL()
    {
    iQueryIterator = 0;
    iCurrentQueryResults->Reset();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::DoCancel()
    {
    iCurrentQueryResults->Reset();
    delete iSh;
    iSh = NULL;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnQueryResultCollector::NextQuery()
    {
    iQueryIterator++;
    return iQueryIterator < iQueries.Count();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::HandleQueryResultsL()
    {
    HBufC8* name8 = HnConvUtils::StrToStr8FastLC(
            iQueries.Query( iQueryIterator ).NameSpace() );
    CLiwDefaultMap* map = CLiwDefaultMap::NewL();
    map->PushL();

    TLiwGenericParam param;
    param.PushL();
    for (TInt i = 0; i < iCurrentQueryResults->Count(); i++)
        {
        iCurrentQueryResults->AtL(i, param);
        map->InsertL( param.Name(), param.Value() );
        param.Reset();
        }

    param.SetNameAndValueL( *name8, TLiwVariant( map) );
    iResults->AppendL(param);

    CleanupStack::PopAndDestroy( &param);
    CleanupStack::PopAndDestroy( map);
    CleanupStack::PopAndDestroy(name8);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::RunL()
    {
    if ( !iStatus.Int() )
        {
        HandleQueryResultsL();
        }

    if( NextQuery() )
        {
        ExecQueryAsynchL();
        }
    else
        {
        iNotification->ResultsCollectedL( iResults );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::ExecQueryAsynchL()
    {
    __ASSERT_DEBUG( iQueryIterator < iQueries.Count() ,
                    User::Panic( KMatrixPanic , 0) );
    //cancel any running evaulations
    Cancel();
    ExecQueryL( iQueries.Query( iQueryIterator ) );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnQueryResultCollector::ExecQueryL( CHnMdQuery& aQuery )
    {
    CLiwGenericParamList* constructor
        = aQuery.GetService().EvaluateConstructorL( iParams );
    CleanupStack::PushL( constructor );
    CLiwGenericParamList* command
        = aQuery.GetService().EvaluateCommandL( iParams );
    CleanupStack::PushL( command );

    delete iSh;
    iSh = NULL;
    iSh = CHnServiceHandler::NewL(
            aQuery.GetService().GetServiceName(),
            aQuery.GetService().GetInterfaceName(),
            aQuery.GetService().GetCommand().GetCommandName(),
            aQuery.GetService().GetCommand().GetCommandMode(),
            constructor,
            command );

    CleanupStack::Pop( command );
    CleanupStack::Pop( constructor );

    iSh->ExecuteL( *iCurrentQueryResults, iStatus );
    }



// End of file
