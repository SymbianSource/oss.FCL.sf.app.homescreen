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
* Description:   Interface for Hierarchy Navigator
*
*/



#ifndef C_HNINTERFACE_H
#define C_HNINTERFACE_H

#include "hneventgenerator.h"

class CHnSuiteModel;
class CLiwGenericParamList;

/**
 * @ingroup group_hnengine
 * An interface for edit mode. Used by hierarchy navigator to track edit mode state status change.
 *
 * @lib hnengine
 * @since S60 v5.0
 */
class MHnEditModeInterface
    {
public:
    /**
     * Sets edit mode.
     * 
     * @since S60 v5.0
     * @param aEditMode Parametr sets edit mode to EFlase/Etrue.
     */
    virtual void SetEditModeL( TBool aEditMode ) = 0;
    };

/**
 * @ingroup group_hnengine
 * Interface for Multimedia Menu to use Hierarchy Navigator engine. The functionality of this interface are
 * suite model event issues like loading a model, counting the models currently loaded or loading the root
 * suite.
 *
 * @lib hnengine
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CHnInterface ) : public CBase, 
                                    public MHnEditModeInterface,
                                    public MHnEventGenerator
    {
public:

    /**
     * Fetches and constructs data structres, needed by Multimedia
     * Menu to display suites.
     *
     * @since S60 v5.0
     * @return Return last loaded suite model on the stack.
     */
    virtual CHnSuiteModel* GetLastSuiteModelL() = 0;
    
    /**
     * Fetches and constructs data structres, needed by Multimedia
     * Menu to display suites.
     *
     * @since S60 v5.0
     * @return Return last loaded suite model on the stack.
     */
    virtual CHnSuiteModel* GetParentSuiteModelL() = 0;

    /**
     * Get count of suite models on the stack.
     * 
     * @since S60 v5.0
     * @return Count of suite models on the stack.
     */
    virtual TInt GetSuiteModelsCountL() = 0;
    
    /**
     * Checks if suite model is loaded.
     * 
     * @since S60 v5.0
     * @return ETrue if suite model is loaded, otherwise EFalse.
     */
    virtual TBool SuiteModelLoadedL( TInt aId ) = 0;
      
    /**
     * Handles back event coming from the UI.
     *
     * @since S60 v5.0
     * @param aGenre Suite's name given by genre name on
     *               which perform back event.
     * @param aIterations Number of iterations.
     * @return Status code.
     */
    virtual TInt HandleBackEventL( const TDesC& aGenre,
            TInt aIterations = KModelCountForRoot ) = 0;
    
    /**
     * Loads suite hierarchy specified in URI.
     *
     * @since S60 v5.0
     * @param aUri The URI path consist of suite's name(s) to load to the stack.
     */
    virtual void LoadSuitesFromUriL( const TDesC8& aUri ) = 0;
    
    /**
     * Resets MD Model and Suite Container.
     *
     * @since S60 v5.0
     */
    virtual void ResetModelsL() = 0;
    
    /**
     * Loads default root suite
     * 
     * @since S60 v5.0
     * @param aSuiteParams Suite parameters.
     * @return Status error code.
     */
    virtual TInt InitializeL( CLiwGenericParamList& aSuiteParams ) = 0;
    
    /**
     * Enables to issue a request to Hn engine.
     * 
     * @since S60 v5.0
     * @param aParam Input Parameters.
     * @param aOutput Target list for output.  
     */
    virtual void HandleRequestL( const CLiwGenericParamList& aParam, 
                         CLiwGenericParamList* aOutput = NULL ) = 0; 
    
    };

#endif  // C_HNINTERFACE_H


