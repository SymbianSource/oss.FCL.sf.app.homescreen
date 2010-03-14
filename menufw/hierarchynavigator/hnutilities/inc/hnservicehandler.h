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


#ifndef C_HNMDSERVICEHANDLER_H
#define C_HNMDSERVICEHANDLER_H

//#include <mnaiwservices.h>
#include <liwcommon.h>
#include <e32base.h>
#include <xmlengelement.h>

#include "hnglobals.h"

class CLiwServiceHandler;
class CHnMdQuery;
class CLiwGenericParamList;
class MLiwInterface;
class CLiwDefaultMap;
class CHnMdBaseKey;
class CHnMdServiceCommand;
class CHnServiceHandler;

/**
 * Class represeting request to LIW service.
 * The request can be synchronous or asynchronous.
 *
 * @since S60 5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS(CHnServiceHandler) : public CBase, public MLiwNotifyCallback
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aService Service name.
     * @param aInterface Interface name.
     * @param aCommand Request to the Service.
     * @param aMode Asynchronous or Synchronous Request.
     * @param aConstructor Parameters used during Service construction.
     * @param aServiceCommand Additional Parameters for the Request.
     * @return Fully cosntructed object.
     */
    IMPORT_C static CHnServiceHandler* NewL(
            const TDesC8& aService, const TDesC8& aInterface,
            const TDesC8& aCommand, TServiceMode aMode,
            CLiwGenericParamList* aConstructor,
            CLiwGenericParamList* aServiceCommand );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aService Service name.
     * @param aInterface Interface name.
     * @param aCommand Request to the Service.
     * @param aMode Asynchronous or Synchronous Request.
     * @param aConstructor Parameters used during Service construction.
     * @param aServiceCommand Additional Parameters for the Request.
     * @return Fully cosntructed object.
     */
    IMPORT_C static CHnServiceHandler* NewLC(
            const TDesC8& aService, const TDesC8& aInterface,
            const TDesC8& aCommand, TServiceMode aMode,
            CLiwGenericParamList* aConstructor,
            CLiwGenericParamList* aServiceCommand );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    IMPORT_C virtual ~CHnServiceHandler();

    /**
     * Executes the Request on the service.
     *
     * since S60 v5.0
     * @param aOutParamList Request outcome.
     * @param aStatus Status for signalling when request completes.
     * @return Error code.
     */
    IMPORT_C TInt ExecuteL( CLiwGenericParamList& aOutParamList, TRequestStatus& aStatus );

    /**
     * Executes the query on the service, with external
     * notification callback.
     *
     * since S60 v5.0
     * @param aCallback Notification callback.
     * @param aCmdOptions Additional options for the Request.
     * @return Error code.
     */
    IMPORT_C TInt ExecuteL( MLiwNotifyCallback* aCallback,
            TInt aCmdOptions = 0 );


    /**
     * Executes the query on the service.
     * Deprecated - Do not Use this method!!!! Will be removed asap.
     *
     * since S60 v5.0
     * @return Error code.
     */
    IMPORT_C TInt ExecuteL();

    /**
     * Checks if an action model matches an existing service.
     *
     * since S60 v5.0
     * @param aService service.
     * @param aInterface interface.
     * @param aConstructor constructor.
     * @return true if an action model matches the service.
     */
    IMPORT_C TBool ServiceHandlerMatchesModel(
        const TDesC8& aService,
        const TDesC8& aInterface,
        const CLiwGenericParamList* aConstructor ) const;
    
    /**
     * Allows for reusing the object when a request needs to be invoked on
     * the same interface of the same service.
     * 
     * @param aCommand Request to the Service.
     * @param aMode Asynchronous or Synchronous Request.
     * @param aServiceCommand Additional Parameters for the Request.
     */
    IMPORT_C void PrepareForNextExecutionL( const TDesC8& aCommand,
            TServiceMode aMode, CLiwGenericParamList* aServiceCommand );

private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     * @param aMode Synchronous or asynchronous.
     */
    CHnServiceHandler( TServiceMode aMode );


    /**
     * Standard symbian 2nd phase contructor.
     *
     * @since S60 5.0
     * @param aService Service name.
     * @param aInterface Interface name.
     * @param aCommandName Request to the Service.
     * @param aConstructor Parameters used during Service construction.
     * @param aServiceCommand Additional Parameters for the Request.
     */
    void ConstructL(
            const TDesC8& aService, const TDesC8& aInterface,
            const TDesC8& aCommandName,
            CLiwGenericParamList* aConstructor,
            CLiwGenericParamList* aServiceCommand );


    /**
     * Handles notifications caused by an asynchronous Request.
     *
     * @param aCmdId The service command associated to the event.
     * @param aEventId Occured event, see LiwCommon.hrh.
     * @param aEventParamList Event parameters, if any, as defined per
     *        each event.
     * @param aInParamList Input parameters, if any, given in the
     *        related HandleCommmandL.
     * @return Error code for the callback.
     */
    TInt HandleNotifyL(
            TInt aCmdId,
            TInt aEventId,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& aInParamList);

    /**
     * Creates interface.
     *
     * @param aConstructor constructor
     * @return Error code.
     */
    TInt SetServiceInterfaceL( CLiwGenericParamList* aConstructor );

private: // data

    /**
     * Service name.
     * Own.
     */
    RBuf8 iServiceName;

    /**
     * Interface name.
     * Own.
     */
    RBuf8 iInterfaceName;

    /**
     * Command (Request) name.
     * Own.
     */
    RBuf8 iCommandName;

    /**
     * Service mode.
     */
    TServiceMode iMode;

    /**
     * Stores input parameters (not data) of the service search method\
     * (ExecuteServiceCmdL).
     * Own.
     */
    CLiwGenericParamList* iConstructor;

    /**
     *
     * Own.
     */
    CLiwGenericParamList* iCommand;

    /**
     * Parameters for the Request.
     * Own.
     */
    CLiwGenericParamList* iInput;

    /**
     * Request results from ExecuteL();
     * Own.
     */
    CLiwGenericParamList* iOutputForAS;

    /**
     * Request results from
     * ExecuteL( CLiwGenericParamList& aOutParamList, TRequestStatus& aStatus ).
     * Own.
     */
    CLiwGenericParamList* iOutputForAO;

    /**
     * Service interface.
     */
    MLiwInterface* iServiceInterface;

    /**
     * Service Handler.
     * Own.
     */
    CLiwServiceHandler* iServiceHandler;

    /**
     * Client status.
     * Not Own.
     */
    TRequestStatus* iClientStatus;

    /**
     * Waits for asynchronous queries.
     * Own.
     */
    CActiveSchedulerWait* iWait;

    };

#endif // C_HNMDSERVICEHANDLER_H
