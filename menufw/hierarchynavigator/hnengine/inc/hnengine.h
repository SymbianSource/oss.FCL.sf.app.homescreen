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



#ifndef C_HNENGINE_H
#define C_HNENGINE_H

#include <e32base.h>
#include <eikenv.h>
#include <hwrmlight.h>

#include "hnglobals.h"
#include "hninterface.h"
#include "hnmdmodel.h"
#include "hninstallnotifier.h"

class CHnMdModel;
class CHnSuiteModelContainer;
class MHnControllerInterface;
class MHnSuiteObserver;

/**
 * @ingroup group_hnengine
 *  Hierarchy Navigator engine - used by Multimedia Menu
 *  to fetch data models. The most commonly used exported method is TriggerHnEventL, 
 *  which offers event execution for the framework. This functionality is implemented
 *  through the MHnEventGenerator interface. The class owns the meta data model ( CHnMdModel ) 
 *  and suite container ( CHnSuiteModelContainer ) to manage them. The engine manages 
 *  the model by processing model events ( HandleModelEventL ) or reseting the models
 *  ( ResetModelsL ).
 *
 * @lib hnengine
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CHnEngine ) : public CHnInterface,
                  public MHnMdModelEventObserver,
                  public MHnInstallNotifierCallback,
                  public MHWRMLightObserver
    {
public:
    /**
     * Standard factory method.
     *
     * @since S60 v5.0
     * @param aController Controller handler.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnEngine* NewL( MHnControllerInterface& aController );

    /**
     * Standard factory method.
     *
     * @since S60 v5.0
     * @param aController Controller handler.
     * @return Fully constructed object.
     */    
    IMPORT_C static CHnEngine* NewLC( MHnControllerInterface& aController );

    /**
     * Standard C++ destructor.
     * 
     * @since S60 v5.0
     */    
    ~CHnEngine();

    // from CHnInterface
    
    /**
     * Fetches and constructs data structres, needed by Multimedia
     * Menu to display suites.
     *
     * @since S60 v5.0
     * @return Filled suite model object from the stack.
     */
    CHnSuiteModel* GetLastSuiteModelL();
    
    /**
     * Fetches and constructs data structres, needed by Multimedia
     * Menu to display suites.
     *
     * @since S60 v5.0
     * @return Filled suite model object from the stack.
     */
    CHnSuiteModel* GetParentSuiteModelL();
   
    /**
     * Get count of suite models on the stack.
     * 
     * @since S60 v5.0
     * @return Count of suite models on the stack.
     */
    virtual TInt GetSuiteModelsCountL();
    
    /**
     * Checks if suite model is loaded.
     * 
     * @since S60 v5.0
     * @return ETrue if suite model is loaded, otherwise EFalse.
     */
    TBool SuiteModelLoadedL( TInt aId );
    
    /**
     * Loads suite hierarchy specified in URI.
     *
     * @since S60 v5.0
     * @param aUri The URI contained suite(s) to load.
     */
    IMPORT_C void LoadSuitesFromUriL( const TDesC8& aUri );

public:
    // from MHnMdModelEventObserver
    
     /**
     * Handle model event.
     *
     * @since S60 v5.0
     * @param aEventName Event's name to perform.
     * @param aParams Event specific parameters used to perform action.
     */
     TInt HandleModelEventL( const TDesC& aEventName, CLiwGenericParamList& aParams );
     
    // from CHnInterface
     
    /**
     * Handle back event.
     *
     * @since S60 v5.0
     * @param aGenre Suite given by the name genre.
     * @param aIterations Number of iterations.
     * @return Status code.
     */
     TInt HandleBackEventL( const TDesC& aGenre,
             TInt aIterations = KModelCountForRoot );

    // from MHNMulModelInstallNotifierCallback

     /**
      * InstallChangeL is called when the subscribed key has been changed.
      * 
      * @since S60 v5.0
      * @param aStatus Status of the installation event.
      */
    virtual void InstallChangeL( TInt aStatus );
    
    
    /**
     * Loads default root suite
     * 
     * @since S60 v5.0
     * @param aSuiteParams Suite parameters.
     * @return Status error code.
     */
    IMPORT_C TInt InitializeL( CLiwGenericParamList& aSuiteParams );
    
    // from MHnEventGenerator
    
    /**
     * Triggers an event inside hierarchy navigator.
     * 
     * Used by UI to notify hierarchy navigator of its events,
     * as well as for hierarchy navigator components to notify
     * the module of certain events (such as OnSuiteLoaded etc).
     *
     * @since S60 v5.0
     * @param aHnEventId Internal id of an event (Event names are 
     *                    mapped to ids).
     * @param aRecipientId The id of the recipient item.
     * @param aEventParameters Event specific parameter.
     * @return Status code.
     */
     IMPORT_C TInt TriggerHnEventL( const TInt aHnEventId,
             const TInt aRecipientId, CLiwGenericParamList* aEventParameters = NULL );
     
     /**
      * Splits the source descriptor by a separatos
      * 
      * @since S60 v5.0
      * @param aSource Source descriptor intended to split.
      * @param aSeparator Separator used to split.
      * @return Array of descriptors. 
      */
     RPointerArray< HBufC8 > SplitL( const TDesC8& aSource,
             const TDesC8& aSeparator );
     
     /**
      * Generate param list according to query param.
      * 
      * @param aUriQuery Descriptor consist of param(s) to split
      * @param aUriFragment Descriptor containing an uri fragment.
      * @param aSuiteName The suite's name.  
      * @return Filled generic param list.
      */
     CLiwGenericParamList* UriQueryToLiwListLC( const TDesC8& aUriQuery,
             const TDesC8& aUriFragment, const TDesC8& aSuiteName );
     
     /**
      * Resets MD Model and Suite Container.
      *
      * @since S60 v5.0
      */
     IMPORT_C void ResetModelsL();
     
     // from MHnEditModeInterface 
     
     /**
      * Sets edit mode.
      * 
      * @since S60 v5.0
      * @param aEditMode Set edit mode member to false/true.
      */
     void SetEditModeL( TBool aEditMode );

    /**
     * Enables to issue a request to Hn engine.
     * 
     * @since S60 v5.0
     * @param aParam Input Parameters.
     * @param aOutput Target list for output.  
     */
     IMPORT_C void HandleRequestL( const CLiwGenericParamList& aParam, 
                         CLiwGenericParamList* aOutput = NULL);

    //    from MHWRMLightObserver
    /**
     * Called when the device light status changes.
     * Note that if the light status for certain target changes
     * very rapidly, some state transitions might be missed.
     * It is however guaranteed that latest state is always obtained.
     *
     * @param aTarget Indicates target(s) the new status applies to.
     * @param aStatus Indicates light request status.
     */
    virtual void LightStatusChanged(TInt aTarget,
            CHWRMLight::TLightStatus aStatus);

private:
    /**
     * Default constructor.
     * 
     * @since S60 v5.0
     * @param aController Controller handler.
     */
    CHnEngine( MHnControllerInterface& aController );
    
    /**
     * Standard 2nd phase constructor.
     * 
     * @since S60 v5.0
     */
    void ConstructL();
         
    /**
     * Handle new suite loaded event.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters.
     * @return Status code.
     */
    TInt HandleNewSuiteLoadedEventL( CLiwGenericParamList& aParams );

    /**
     * Handle open suit event.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters that hold liw packed filter.
     * @return Status code.
     */
    TInt HandleReevaluateEventL( CLiwGenericParamList& aParams );

    /**
     * Handles widget change.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters consist of target widget type.
     * @return Status code.
      */
    TInt HandleWidgetChangeL( CLiwGenericParamList& aParams );

    /**
     * Handles SetFocus event.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters consist of target widget type.
     * @return Status code.
     */
    TInt HandleSetFocusEventL( const CLiwGenericParamList& aParams );
    
    /**
     * Handles MoveFocusBeforeDelete event.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters consist of target widget type.
     * @return Status code.
     */
    TInt HandleMoveFocusBeforeDeleteEventL(
            const CLiwGenericParamList& aParams );
    
    /**
     * Handles RefreshIcons event.
     *
     * @since S60 v5.0
     * @param aParams Event specific parameters (ignored)
     * @return Status code.
     */
    TInt HandleRefreshIconsEventL( const CLiwGenericParamList& aParams );

    /**
     * Handles DisableActionsForItem event.
     * 
     * @since S60 v5.0
     * @param aParams Event specific parameters - contains custom item id
     * @return Status code.
     */
    TInt HandleDisableActionsForItemL( const CLiwGenericParamList& aParams );

     /**
      *  Store widget type to repository.
      *
      * @since S60 v5.0
      * @param aSuiteName Suite name to that set widget type. 
      * @param aType Widget type to store in repository.
      * @return Status code.
      */    
    TInt HandleWidgetChangeL( const TDesC& aSuiteName,
            THnSuiteWidgetType aType );
     
         
     /**
      * Parse URI path component containing Suites to be loaded.
      *
      * @since S60 v5.0
      * @param aUriPath Descriptor URI path which consist of suites' names.
      * @param aOutput Array on output containes parsed suite names.
      */
     void ParseSuiteUriPathL( const TDesC8& aUriPath, 
         CDesC8ArrayFlat& aOutput );
   
     
     /**
      * Checks if only root configuration is loaded.
      * Used to decide if reloading models is needed.
      *
      * @since S60 v5.0
      * @return Return ETrue if only root configuration is loaded
      *         otherwise EFalse.
      */
     TBool IsOnlyRootConfLoaded();
     
     /**
      * Resets loaded resources.
      *
      * @since S60 5.0
      */
     void ResetLocalization();
     
     /**
      * Loads suite with given suite name with parameters
      * from uri query. 
      *
      * @since S60 5.0
      * @param aSuiteName Name of the suite to load. 
      * @param aUriQuery Descriptor containig parsed part of the uri.
      * @param aUriFragment Descriptor containig parsed part (fragment) of the uri.
      * @return Error code when loading suite.
      */
     TInt LoadSuiteFromUriL( const TDesC8& aSuiteName, 
         const TDesC8& aUriQuery, const TDesC8& aUriFragment );
     
     /**
      * Reads highlight position from uri fragment 
      * and sets it on the suite model.
      *
      * @since S60 5.0
      * @param aUriFragment Descriptor containig parsed part of the uri.
      */
     void HighlightPlacementFromUriL( const TDesC8& aUriFragment );
    
     
     /**
      * Loads folder and sets focus through CR.
      * 
      * @since S60 v5.0
      */
     void LoadFromCrL( const TDesC8& aUri );
     
     /**
      * Handles loading suite from uri if suites names' are the same.
      * 
      * @param aNextExists ETrue if the next suite's name exists in uri.
      * @param aUri The URI's parameters passed to the suite. 
      */
     void HandleTheSameSuitesL( TBool aNextExists, const TDesC8& aParams );
     
     /**
      * Checks if suites have the same names.
      * 
      * @param aLastSuiteName The last suite's name on the stack.
      * @param aFirstSuiteName The first suite's name in uri.
      * @return ETrue if names are equal
      */
     TBool SuitesAreTheSameL( const TDesC& aLastSuiteName, const TDesC8& aFirstSuiteName );

     /**
      * Decides whether to hide menu depending on current state and uri parameters
      * 
      * @param aUri The Uri to parse for exit procedure.
      * @return ETrue If exit was consumed.
      */
     TBool HandleActionL( const TDesC8& aUri );
     
     /**
      * Loads suites from uri.
      * 
      * @param aUri The uri to parse
      */
     void LoadSuitesL( const TDesC8& aUri );
     
private: // data
        
    /**
     * Own - Meta data model.
     */
    CHnMdModel* iMetaDataModel;
    
    /**
     * Own - Suite container.
     */
    CHnSuiteModelContainer* iSuiteContainer;
       
    /**
     * Not own.
     * EikonEnv for displaying popup messages. 
     */
    CEikonEnv* iEEnv;
    
    /**
     * Not own - Suite observer.
     */
    MHnControllerInterface& iControllerInterface;
    
    /**
     * Installation observer.
     * Own.
     */
    CHnInstallNotifier* iInstObserver;
    
    /**
     * Edit mode
     */
    TBool iEditMode;
    
    /**
     * Light status observer
     */
    CHWRMLight* iLight;
    };

#endif //C_HNENGINE_H
