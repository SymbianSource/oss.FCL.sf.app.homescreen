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
* Description:   action presentation model
*
*/


#ifndef HNACTIONMODEL_H_
#define HNACTIONMODEL_H_

#include <e32base.h>
#include <e32hashtab.h>
#include <liwservicehandler.h>
#include "hnglobals.h"


class MLiwNotifyCallback;
class CLiwGenericParamList;
class CHnMdServiceCommand;
class CHnEventHandler;
class CHnMdBaseKey;
struct THnMdCommonPointers;

/**
 *  Action model class.
 * 
 *  The class represents an evaluated action, with concrete values.
 *  It is constructed from the data contained in the CHnMdAction, 
 *  during evaluation. The afore mentioned process, replaces any 
 *  configurable data with specific information fetched form services
 *  or other sources. Such setup action, may be executed by means
 *  of its methods. However, a reference to an instance of CHnEventHandler
 *  has to be provided. 
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @see CHnMdAction
 *  @see CHnEventHandler
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnActionModel ) : public CBase
    {
    
public:
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C static CHnActionModel* NewL();
    
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    ~CHnActionModel();
    
    /**
     * Concretizes the key values.
     *
     * @since S60 5.0
     * @param aActionModel The action model to be concretized.
     * @param aGenericParamList The list of parameters.
     * @return Concretized instance of action model.
     * 
     */
    CHnActionModel* ConcretizeL( const CHnActionModel & aActionModel,
            CLiwGenericParamList* aGenericParamList );
    
    /**
     * Returns service constructor parameters
     * as a CLiwGenericParamList. The list contains 
     * keys evaluated from the XML configuration.
     *
     * @since S60 5.0
     * @return Constructor parameters.
     * 
     */
    IMPORT_C CLiwGenericParamList*  ConstructorLC();
    
    /**
     * Returns the service name.
     *
     * @since S60 5.0
     * @return Service name.
     */
    IMPORT_C const TDesC8& Service() const;
        
    /**
     * Returns service command content as
     * a CLiwGenericParamList. The list contains
     * keys evaluated from the XML configuration.
     *
     * @since S60 5.0
     * @return Command element.
     */
    IMPORT_C CLiwGenericParamList* CommandLC() const;
        
    /**
     * Returns interface name.
     *
     * @since S60 5.0
     * @return Interface name.
     */
    IMPORT_C const TDesC8& Interface() const;
    
    /**
     * Sets constructor parameters.
     * Method takes copy of items from the array.
     *
     * @since S60 5.0
     * @param aData Constructor parameters.
     */
    IMPORT_C void SetConstructorL( CHnMdBaseKey* aData );
    
    /**
     * Sets the service name.
     *
     * @since S60 5.0
     * @param aService Service name.
     */
    IMPORT_C void SetServiceL( const TDesC8& aService );
        
    /**
     * Sets the service command parameters.
     *
     * @since S60 5.0
     * @param aCommand Command parameters.
     */
    IMPORT_C void SetCommand( CHnMdBaseKey* aCommand );
    
    /**
     * Sets interface name.
     *
     * @since S60 5.0
     * @param aInterface Interface name.
     */
    IMPORT_C void SetInterfaceL( const TDesC8& aInterface );
    
    /**
     * Sets service command name.
     *
     * @since S60 5.0
     * @param aCommandName Name of the service command.
     */
    IMPORT_C void SetCommandNameL( const TDesC8& aCommandName );
    
    
    /**
     * Sets the mode in which the service command
     * should operate.
     *
     * @since S60 5.0
     * @param aMode Mode in which command operates.
     */
    IMPORT_C void SetServiceModeL( TServiceMode aMode );
    
    /**
     * Returns command name.
     *
     * @since S60 5.0
     * @return Command name.
     */
    IMPORT_C const TDesC8& CommandName() const;
    
    
    /**
     * Returns service mode in which the action operates.
     *
     * @since S60 5.0
     * @return Service mode.
     */
    IMPORT_C TServiceMode ServiceMode() const;
    
    /**
     * Executes the action.
     *
     * @since S60 5.0
     * @param aEventHandler A reference to an instance of event handler
     * @param aExecutionParams A pointer to the parameters to the event.
     * @return Error code of the event execution.
     */
    TInt ExecuteL( CHnEventHandler & aEventHandler,
            CLiwGenericParamList *aExecutionParams );
    
private:

    /**
     * Second phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();

    /**
     * Default constructor.
     * 
     * @since S60 5.0
     */
    CHnActionModel();
    
private: // data

    /**
     * Service name.
     */
    RBuf8 iService;

    /**
     * Interface name.
     */
    RBuf8 iInterface;
    
    /**
     * Command name.
     */
    RBuf8 iCommandName;
    
    /**
     * Service mode.
     */
    TServiceMode iMode;

    /**
     * Command parameters.
     * Own.
     */
    CHnMdBaseKey* iCommand;
    
    /**
     * Constructor parameters.
     * Own.
     */
    CHnMdBaseKey* iConstructor;
    
    };

#endif /*HNACTIONMODEL_H_*/
