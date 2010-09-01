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
* Description:   ?Description
*
*/


#include <xmlengdom.h>
#include <liwservicehandler.h>

#include "hnservicehandler.h"
#include "hnliwutils.h"
#include "hnglobals.h"
#include "hnmdbasekey.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnServiceHandler::ConstructL(
        const TDesC8& aService, const TDesC8& aInterface,
        const TDesC8& aCommand,
        CLiwGenericParamList* aConstructor,
        CLiwGenericParamList* aServiceCommand )
    {
    iServiceName.CreateL( aService );
    iInterfaceName.CreateL( aInterface );
    iCommandName.CreateL( aCommand );

    iServiceHandler = CLiwServiceHandler::NewL();
    iInput = CLiwGenericParamList::NewL();

    iOutputForAS = CLiwGenericParamList::NewL();

    iServiceInterface = NULL;
    SetServiceInterfaceL( aConstructor );

    // Please note that by setting these two member variables
    // we take ownership of aConstructor and aServiceCommand.
    // No leaving functions may be called in ConstructL from
    // now on.
    iConstructor = aConstructor;
    iCommand = aServiceCommand;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnServiceHandler* CHnServiceHandler::NewL(
        const TDesC8& aService, const TDesC8& aInterface,
        const TDesC8& aCommand, TServiceMode aMode,
        CLiwGenericParamList* aConstructor,
        CLiwGenericParamList* aServiceCommand )
    {
    CHnServiceHandler* self = CHnServiceHandler::NewLC( aService, aInterface,
            aCommand, aMode,  aConstructor, aServiceCommand );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnServiceHandler* CHnServiceHandler::NewLC(
        const TDesC8& aService, const TDesC8& aInterface,
        const TDesC8& aCommandName, TServiceMode aMode,
        CLiwGenericParamList* aConstructor,
        CLiwGenericParamList* aServiceCommand )
    {
    CHnServiceHandler* self =  new( ELeave ) CHnServiceHandler( aMode );
    CleanupStack::PushL( self );
    self->ConstructL( aService, aInterface, aCommandName, aConstructor,
            aServiceCommand );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnServiceHandler::CHnServiceHandler( TServiceMode aMode ) :
        iMode( aMode )
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnServiceHandler::~CHnServiceHandler()
    {

    iServiceName.Close();
    iInterfaceName.Close();
    iCommandName.Close();

    if ( iServiceInterface )
        {
        iServiceInterface->Close();
        }

    delete iInput;
    delete iConstructor;
    delete iCommand;
    delete iOutputForAS;

    if ( iServiceHandler )
        {
        iServiceHandler->Reset();
        delete iServiceHandler;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnServiceHandler::ServiceHandlerMatchesModel(
    const TDesC8& aService,
    const TDesC8& aInterface,
    const CLiwGenericParamList* aConstructor ) const
    {
    __ASSERT_DEBUG( aConstructor, User::Invariant() );
    
    TBool ret = ( aInterface == iInterfaceName && aService == iServiceName );

    if ( ret )
        {
        const TInt count = iConstructor->Count();
        ret = ( count == aConstructor->Count() );
        for ( TInt i = 0; ret && i < count; ++i )
            {
            ret = ( ( *iConstructor )[i] == ( *aConstructor )[i] );
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnServiceHandler::PrepareForNextExecutionL(
        const TDesC8& aCommand, TServiceMode aMode,
        CLiwGenericParamList* aServiceCommand )
    {
    __ASSERT_DEBUG( aServiceCommand && aServiceCommand != iCommand,
            User::Invariant() );
    
        { // braces for scope only
        RBuf8 copy;
        CleanupClosePushL( copy );
        copy.CreateL( aCommand );
        iCommandName.Swap( copy );
        CleanupStack::PopAndDestroy( &copy );
        }
    
    iMode = aMode;
    
    delete iCommand;
    iCommand = aServiceCommand;
    // Ownership of aServiceCommand has been taken. Do not add any piece of
    // code that could leave after this line.
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnServiceHandler::ExecuteL( MLiwNotifyCallback* aCallback,
        TInt aCmdOptions)
    {
    TInt err( KErrNotSupported );
    if( iServiceInterface )
        {
        err = KErrNotSupported;
        iOutputForAS->Reset();
        iInput->Reset();
        iInput->AppendL( *iCommand );
        switch( iMode )
            {
            case EServiceModeAsynchronous:
                {
                __ASSERT_DEBUG( aCallback , User::Panic( KMatrixPanic , 0 ) );
                TRAP(err, iServiceInterface->ExecuteCmdL(
                 iCommandName, *iInput, *iOutputForAS, aCmdOptions, aCallback ) );
                }
                break;
            case EServiceModeSynchronous:
                {
                TRAP(err, iServiceInterface->ExecuteCmdL(
                 iCommandName, *iInput, *iOutputForAS, aCmdOptions, NULL ) );
                }
                break;
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnServiceHandler::ExecuteL(
        CLiwGenericParamList& aOutParamList, TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    (*iClientStatus) = KRequestPending;
    iOutputForAO = &aOutParamList;

    TInt err( KErrNotSupported );
    if( iServiceInterface )
        {
        err = KErrNone;
        iInput->Reset();
        iInput->AppendL( *iCommand );

        switch (iMode)
            {
            case EServiceModeAsynchronous:
                {
                TRAP(err, iServiceInterface->ExecuteCmdL(
                     iCommandName, *iInput, *iOutputForAO, 0, this ) );
                if ( err != KErrNone )
                	{
                	User::RequestComplete( iClientStatus, err );
                	}
                }
                break;
            case EServiceModeSynchronous:
                 {
                 TRAP(err, iServiceInterface->ExecuteCmdL(
                     iCommandName, *iInput, *iOutputForAO ) );
                 User::RequestComplete( iClientStatus, KErrNone );
                 }
                 break;
            default:
                break;
            }
        }
    else
    	{
    	// KErrNone ensures that CHnQueryResultCollector::HandleQueryResultsL()
    	// is called, empty results are added to list, and
    	// CHnMdItem::ResultsCollectedL doesn't get confused
    	User::RequestComplete( iClientStatus, KErrNone );
    	}

    return err;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CHnServiceHandler::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    // reset
    if( iOutputForAO )
        {
        iOutputForAO->Reset();
        iOutputForAO->AppendL( aEventParamList );

        TLiwGenericParam param;
        TInt error(KErrNone);
        param.PushL();
        for (TInt i = 0; i < aEventParamList.Count(); i++)
            {
            if ( param.Name().Compare( KErrorCode8 ) == 0
            		&& param.Value().AsTInt32() == KSErrNoMemory )
            	{
            	error = KErrNoMemory;
            	break;
            	}
            }
        CleanupStack::PopAndDestroy(&param);
        User::RequestComplete( iClientStatus, error);
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CHnServiceHandler::SetServiceInterfaceL(
        CLiwGenericParamList* aConstructor )
    {
    CLiwGenericParamList& inParam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParam = iServiceHandler->OutParamListL();

    CLiwCriteriaItem* critItem = CLiwCriteriaItem::NewLC( KLiwCmdAsStr,
        iInterfaceName, iServiceName );

    critItem->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    RCriteriaArray critArr;
    CleanupClosePushL( critArr );
    critArr.AppendL( critItem );
    iServiceHandler->AttachL( critArr );
    inParam.AppendL( *aConstructor );
    iServiceHandler->ExecuteServiceCmdL( *critItem, inParam, outParam );
    CleanupStack::PopAndDestroy( &critArr );
    CleanupStack::PopAndDestroy( critItem );

    TInt pos( KErrNone );
    outParam.FindFirst( pos, iInterfaceName );
    if ( pos != KErrNotFound )
        {
        iServiceInterface = outParam[pos].Value().AsInterface();
        }

    return pos;
    }

// End of file
