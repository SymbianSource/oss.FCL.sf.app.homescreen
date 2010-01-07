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



#ifndef C_HNMDMODEL_H
#define C_HNMDMODEL_H

#include <e32base.h>
#include <xmlengelement.h>
#include <liwgenericparam.h>
#include <eikenv.h>
#include "hnglobals.h"

class CHnMdLocalization;
class CHnSuiteModelContainer;
class CHnFilter;
class THnIdGenerator;
class CHnMdSuite;
class CHnMdModel;
class CHnItemId;
class MHnSuiteObserver;
class CHnXmlModelProvider;
class CHnMdModel;
class MHnMdModelEventObserver;
class CHnSuiteModel;
class CHnRepositoryObserver;
class MHnEditModeInterface;
class CHnBitmapIdCache;

/**
 * Type of the property.
 *
 * @ingroup group_hnmetadatamodel
 */
enum TPropertyType
    {
    EUndefined = 0,
    EGenre,
    EVisualItemCount
    };

/**
 * Mode type.
 *
 * @ingroup group_hnmetadatamodel
 */
enum TMdMode
    {
    EMdModeNormal = 0,
    EMdModeEdit
    };

/**
 * Common pointers.
 *
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
struct THnMdCommonPointers
    {
    /**
     * Not own - Localization.
     */
    CHnMdLocalization* iLocalization;

    /**
     * Not own - Query generator.
     */
    THnIdGenerator* iIdGenerator;

    /**
     * Not own - Suite model container.
     */
    CHnSuiteModelContainer* iContainer;

    /**
     * Not own - Meta data model.
     */
    CHnMdModel* iModel;

    /**
     * Not own - Xml model provider.
     */
    CHnXmlModelProvider* iXmlModelProvider;

    /**
     * Not own - Cache for bitmap and mask ids.
     */
    CHnBitmapIdCache* iBitmapIdCache;

    /**
     * Not own - Model event observer.
     */
    MHnMdModelEventObserver* iModelEventObserver;

    /**
     * Not own - Edit mode interface.
     */
    MHnEditModeInterface* iEditModeInterface;

    /**
     * Not own - Handler to file server session;
     */
    RFs& iFs;

    /**
     * Gets static pointer from TLS.
     */
    static THnMdCommonPointers* Static() {return (THnMdCommonPointers*)Dll::Tls(); }

    /**
     * Sets pointer to TLS.
     *
     * @param aCmnPtrs Pointer to set
     */
    static void SetStatic( THnMdCommonPointers* aCmnPtrs ) {  Dll::SetTls(aCmnPtrs); }

  private:

    // only model can create it
    friend class CHnMdModel;

    /**
     * Standard C++ consturctor.
     */
    THnMdCommonPointers() : iFs( CEikonEnv::Static()->FsSession() )
        {
        iLocalization = NULL;
        iIdGenerator = NULL;
        iContainer = NULL;
        iModel = NULL;
        iXmlModelProvider = NULL;
        iModelEventObserver = NULL;
        iEditModeInterface = NULL;
        }

    /**
     * Copy constructor declared as private to disallow copying.
     *
     * @param aCmnPtrs Pointer to set.
     */
    THnMdCommonPointers( const THnMdCommonPointers& aCmnPtrs ):iFs( aCmnPtrs.iFs ) { }

    /**
     * Assignment operator
     */
    THnMdCommonPointers& operator=( const THnMdCommonPointers& ){return *this;}
    };

/**
 *  Id generator.
 *
 *  @lib ?library
 *  @since S60 5.0
 *  @ingroup group_hnmetadatamodel
 */
struct THnIdGenerator
    {
    /**
     * Standatrd C++ constructor.
     */
    THnIdGenerator()
        {
        iId = 0;
        }

    /**
     * Gets next id.
     * @return Next id.
     */
    TInt GetNextId()
        {
        return iId++;
        }
    private:

    /**
     * Copy constructor declared as private to disallow copying.
     * @param aGenerator Id generator;
     */
    THnIdGenerator(const THnIdGenerator& aGenerator);

    /**
     * Assignment operator.
     */
    THnIdGenerator& operator=(const THnIdGenerator&);

    /**
     * Id
     */
    TInt iId;
    };

/**
 * Model event observer.
 *
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
class MHnMdModelEventObserver
    {
    public:

     /**
     * Handles modlde event.
     *
     * @since S60 5.0
     * @param aEventName Event name.
     * @param aParams Parameters.
     */
     virtual TInt HandleModelEventL( const TDesC& aEventName,
             CLiwGenericParamList& aParams ) = 0;
    };

/**
 * Meta data model
 *
 * This is the main class of the Meta Data Model component.
 *
 * This class is the entry point for building the meta data model
 * and for evaluation.
 *
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnMdModel ) : public CBase
    {
public:
    enum TForegroundGainTriggeringEvent
        {
        EWhenAnySuiteIsEvaluated,
        EWhenCurrentTopSuiteIsEvaluated,
        ENever
        };
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aModelObserver Model observer.
     * @param aSuiteModelContainer Suite model container.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnMdModel* NewL( MHnMdModelEventObserver* aModelObserver,
            CHnSuiteModelContainer * aSuiteModelContainer );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aModelObserver Model observer.
     * @param aSuiteModelContainer Suite model container.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnMdModel* NewLC( MHnMdModelEventObserver* aModelObserver,
            CHnSuiteModelContainer * aSuiteModelContainer );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdModel();

    /**
     * Evaluates the model. Takes filter as a parameter.
     *
     * @since S60 5.0
     * @param aFilter Filter.
     */
    IMPORT_C void EvaluateL( CHnFilter& aFilter );

    /**
     * Manages evaluation lock.
     *
     * @param aLock Locks evaluation if set to ETrue.
     * @param aEvaluate Runs evaluation if aEvaluate == ETrue.
     */
    IMPORT_C void SetEvaluationLockL( TBool aLock, TBool aEvaluate = ETrue );

    /**
     * Evaluates the model.
     *
     * @since S60 5.0
     */
    IMPORT_C void EvaluateL();

    /**
     * Switches between edit mode and normal mode
     *
     * @since S60 5.0
     * @param aMode Suite model container.
     */
    IMPORT_C void SetModeL( TMdMode aMode );


    /**
     * Gets suite's meta data model.
     *
     * @since S60 5.0
     * @return Suite meta data model.
     */
    IMPORT_C CHnMdSuite* GetLastSuite();

    /**
     * Gets suite's meta data model.
     *
     * @since S60 5.0
     * @param aPosition Position of the suite in the stack.
     * @return Suite meta data model.
     */
    CHnMdSuite* GetSuite( TInt aPosition );

    /**
     * Gets suite's parameters..
     *
     * @since S60 5.0
     * @param aPosition Position of the suite in the stack,
     *        from which gets parameters.
     * @return Suite meta data model.
     */
    IMPORT_C CLiwGenericParamList& GetSuiteParameters( TInt aPosition );


    /**
     * Handles back event.
     *
     * @since S60 v3.2
     * @param aMulContainer Suite model container.
     * @param aGenre Name of the suite.
     * @param aIterations Number of iterations.
     * @return Error code.
     */
    IMPORT_C TInt HandleBackEventL( CHnSuiteModelContainer* aMulContainer,
            const TDesC& aGenre, TInt aIterations = KModelCountForRoot );

    /**
     * Handles sis installation event.
     *
     * @since S60 v3.2
     * @param aMulContainer Suite model container.
     */
    IMPORT_C void HandleSisInstallationEventL(
            CHnSuiteModelContainer* aMulContainer );

    /**
     * Loads a suite from xml data.
     *
     * @since S60 v5.0
     * @param aGenre Name of the suite.
     * @param aSuiteParams Suite parameters.
     * @return Error code.
     */
    IMPORT_C TInt LoadSuiteL( const TDesC& aGenre, CLiwGenericParamList* aSuiteParams = NULL );

    /**
     * Check if suite model exists.
     *
     * @since S60 5.0
     * @param aSuiteModel Suite model name.
     * @return ETrue if suite model exists, otherwise EFalse.
     */
    IMPORT_C TBool SuiteModelExistsL( const TDesC8& aSuiteModel );

    /**
     * Gets suite's meta data model.
     *
     * @since S60 5.0
     * @param aUri Current ui.
     * @return Suite meta data model.
     */
    IMPORT_C void GetCurrentUriL( TDes& aUri );

    /**
     * Deletes last loaded suite model from data model.
     *
     * @since S60 5.0
     */
    IMPORT_C void DeleteLastSuite();


    /**
     * Realeses localization files.
     *
     * @since S60 5.0
     */
    IMPORT_C void ReleaseLocalization();

    /**
     * Reloads localization.
     *
     * @since S60 5.0
     */
    IMPORT_C void ReloadLocalizationL();

    /**
     * Informs metadatamodel to ignore evaluations or
     * to performing evaluations, according to the
     * given parameter.
     *
     * @since S60 5.0
     * @param aIgnore Metadatamodel ignores evaluations when set to ETrue.
     */
    IMPORT_C void IgnoreEvaluations( TBool aIgnore );

    /**
     * Request that menu goes to foreground when evaluation completes.
     *
     * Default behavior is not to go to foreground when evaluation
     * completes. A change of this behavior requested with this method is
     * only effective until the menu is sent to foreground (once). Then the
     * default behavior is assumed again.
     * Subsequent calls override previous changes.
     *
     * @since S60 5.0
     * @param aWhen Determines when the foreground should be gained.
     *              Possible values are defined in the
     *              TForegroundGainTriggeringEvent enum. The value ENever
     *              simply means not to bring menu to foreground.
     */
    IMPORT_C void QueueForeground( TForegroundGainTriggeringEvent aWhen );

    /**
     * Is there a queue for foreground gain.
     *
     * @since S60 5.0
     * @return should foreground be gained after evaluation.
     */
    IMPORT_C TBool IsForegroundQueued() const;

    /**
     * Notifies this metadata model that suite evaluation has completed.
     *
     * @param aJustEvaluatedSuite pointer to the suite whose evaluation
     *                            has completed
     */
    void SuiteModelReadyToShowL( CHnSuiteModel* aJustEvaluatedSuite );

    /**
     * Causes all of the loades suites except for the suite at the bottom
     * of the stack to be reloaded from their corresponding xml file
     * definitions.
     *
     * @since S60 5.0
     * @param aIgnore aModelContainer Model container object.
     */
    void ReloadStackSuitesL( CHnSuiteModelContainer* aModelContainer );

    /**
     * Counts number of loaded suites.
     *
     * @since S60 5.0
     * @return Number of loaded suites
     */
    TInt LoadedSuitesCount();

    /**
     * Evaluates the suite model on top of the stack.
     *
     * @since S60 5.0
     */
    void EvaluateTopSuiteL();

private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdModel( );

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     * @param aModelObserver Model observer.
     * @param aSuiteModelContainer Model container.
     */
    void ConstructL( MHnMdModelEventObserver* aModelObserver,
            CHnSuiteModelContainer * aSuiteModelContainer );

private: // data

    /**
     * Id generator.
     */
    THnIdGenerator iIdGenerator;

    /**
     * Own - Localization.
     */
    CHnMdLocalization* iLocalization;

    /**
     * Own - Xml model provider.
     */
    CHnXmlModelProvider* iXmlModelProvider;

    /**
     * Own - bitmap and mask ids cache.
     */
    CHnBitmapIdCache* iBitmapIdCache;

    /**
     * Not Own - Event observer.
     */
    MHnMdModelEventObserver* iModelEventObserver;

    /**
     * Stores pointers to common objects in the system
     * This object is shared throut the whole system.
     */
    THnMdCommonPointers iCmnPtrs;

    /**
     * Stores information about mode.
     */
    TMdMode iMode;

    /**
     * Loaded Suites stack.
     * Object and content owned.
     */
    RPointerArray<CHnMdSuite> iLoadedSuites;

    /**
     * Repository widget type observer.
     */
    CHnRepositoryObserver* iRepositoryWidgetTypeObserver;

    /**
     * Repository folder change observer.
     */
    CHnRepositoryObserver* iRepositoryShowFolderObserver;

    /**
     * When set, forces metadatamodel to skip evaluations.
     */
    TBool iIgnoreEvaluations;

    /**
     * Queues foreground gain action.
     */
    TBool iForegroundQueued;

    /**
     * Not own - pointer to the suite model that must be evaluated
     * before matrix menu can be brought to foreground.
     *
     * When NULL, then any suite evaluation brings menu to foreground.
     * It is only meaningful when iForegroundQueued is ETrue.
     */
    CHnSuiteModel* iForegroundTriggeringSuite;

    };

#endif // C_HNMDMODEL_H
