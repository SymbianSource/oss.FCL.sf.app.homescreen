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
* Description:  event handler class
*
*/


#ifndef C_HNEVENTHANDLER_H
#define C_HNEVENTHANDLER_H

#include <liwcommon.h>

class CHnMetaDataModel;
class CHnMdAction;
class MLiwInterface;
class CLiwServiceHandler;
class CLiwGenericParamList;
class CHnMdBaseKey;
class TLiwVariant;
class CHnMdItem;
class CHnActionModel;
class MHnMdModelEventObserver;
class CHnServiceHandler;
class MHnControllerInterface;

/**
 *  Event handler class.
 * 
 *  Event Handler is designed to handle events. 
 *  It reveives an event id from the UI, 
 *  mapps this event into an appropriate action, 
 *  and runs this action.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS(CHnEventHandler) : public CBase, public MLiwNotifyCallback
{
public: // Constructors and destructor

   /**
    * Two-phased constructor.
    * 
    * @since S60 5.0
    * @param aModelObserver Model event observer.
    * @param aController Interface enabling to issues request to Hn engine.    
    * @return Fully constructed object.
    */
    static CHnEventHandler* NewL( MHnMdModelEventObserver& aModelObserver,
            MHnControllerInterface& aController );

   /**
    * Two-phased constructor.
    * 
    * @since S60 5.0
    * @param aModelObserver Model event observer.
    * @param aController Interface enabling to issues request to Hn engine.
    * @return Fully constructed object.
    */
    static CHnEventHandler* NewLC( MHnMdModelEventObserver& aModelObserver,
            MHnControllerInterface& aController);
    
   /**
    * Destructor.
    * 
    * @since S60 5.0
    */
    ~CHnEventHandler();    
    
    /**
     * Executes the action that was found in the Model.
     * 
     * @since S60 v3.2
     * @see CHnActionModel
     * @param aActionModel A pointer to the object of the CHnActionModel class.
     * @return Error code. KErrNone in case of a correct execution (even if
               given action was not found). KErrNotFound in case of incorrect
               action parameters.
     */
    IMPORT_C TInt ExecuteActionL( CHnActionModel* aActionModel );
       
private:

   /**
    * Constructor for performing 1st stage construction
    * 
    * @since S60 5.0
    * @param aModelObserver A model event observer reference.
    * @param aController A HH controller reference.
    */
    CHnEventHandler( MHnMdModelEventObserver& aModelObserver,
                     MHnControllerInterface& aController );

   /**
    * EPOC default constructor for performing 2nd stage construction
    * 
    * @since S60 5.0
    */
    void ConstructL();

    /**
     * Notifies UI about actions that can be passed back to the UI,
     * i.e. openning nested suites, widget change.
     * 
     * @since S60 5.0
     * @param aActionModel Model of the action.
     * @return Error code
     */
    TInt ExecuteInternalActionL( CHnActionModel* aActionModel );

    /**
     * Executes action from Extension Manager.
     * 
     * @since S60 v5.0
     * @param aActionModel Model of the action.
     * @return Error code
     */
    TInt ExecuteExtensionManagerActionL( CHnActionModel* aActionModel );

    /**
     * Uses default mechanism to execute an action.
     * 
     * @since S60 v5.0
     * @param aActionModel Model of the action.
     * @return Error code
     */
    TInt ExecuteStandardActionL( CHnActionModel* aActionModel );
        
    /**
     * Extracts extension uid from the action.
     * 
     * @since S60 v5.0
     * @param aInterface Interface to invoke action on.
     * @param aUid The resulting UID.
     * @return Error code
     */
    TInt ExtractUidFromActionL( const TDesC8& aInterface, TUid& aUid );

    
// from MLiwNotifyCallback
public :    
	
    /**
     * Handles notification.
     * 
     * @param aCmdId The Id of the command triggered.
     * @param aEventId The Id of the event.
     * @param aEventParamList Parameters to event.
     * @param aInParamList Input parameter list.
     * 
     * @return Error code.
     */
    TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& aInParamList );
        
private: // data
    
    /**
     * Not own.
     * Reference to HnEngine - Event observer.
     */
    MHnMdModelEventObserver& iEventObserver;
    
    /**
     * Own.
     */
    CHnServiceHandler* iServiceHandler;
    
    /**
     * Not own.
     * Reference to App UI - Event observer.
     */
    MHnControllerInterface& iControllerInterface;
    
    /**
     * ETrue if a standard asynchroneous action is being executed by
     * this event handler.
     */
    TBool iIsExecutingStandardAsyncAction;

};

#endif // C_HNEVENTHANDLER_H
