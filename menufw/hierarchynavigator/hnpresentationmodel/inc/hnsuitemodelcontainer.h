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
* Description:   suite presentation models container
*
*/


#ifndef C_HNSUITEMODELCONTAINER_H
#define C_HNSUITEMODELCONTAINER_H

#include <e32base.h>

class MHnMdModelEventObserver;
class CHnSuitesStack;
class CHnEventHandler;
class CHnSuiteModel;
class CLiwGenericParamList;
class MHnControllerInterface;
class CLiwGenericParamList;

/**
 * Predefined offsets.
 */
enum THighlightOffset
    {
    EOffsetPrevious = -1,
    EOffsetNone = 0,
    EOffsetNext = 1
    };

/**
 *  Suite Model Container class.
 *  It manages the presentation models, and manages their position on the
 *  presentation stack.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnSuiteModelContainer ) : public CBase
    {
public:

    /**
     * Two-phased constructors.
     * 
     * @since S60 5.0
     * @param aModelObserver Interface of the model event observer.
     * @param aController Interface enabling to issues request to Hn engine.
     */
    IMPORT_C static CHnSuiteModelContainer* NewL( 
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController );
    
    /**
     * Two-phased constructors.
     * 
     * @since S60 5.0
     * @param aModelObserver Interface of the model event observer.
     * @param aController Interface enabling to issues request to Hn engine.
     */
    IMPORT_C static CHnSuiteModelContainer* NewLC( 
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController );

    /**
    * Virtual Destructor.
    * 
    * @since S60 5.0
    */
    ~CHnSuiteModelContainer();
    
    /**
     * Pushes a new swuite model to the stack,
     * by the name of aGenre
     * 
     * @since S60 v5.0
     * @param aGenre Name of the suite.
     * @return the Suite Model
     */
    IMPORT_C void PushNewSuiteModelL( const TDesC& aGenre  );

    /**
     * Fetches a Suite Model for the supplied genre name
     * "root" name reserved for root grid
     * 
     * @since S60 v5.0
     * @param aGenre genre name
     * @return the Suite Model
     */
    IMPORT_C CHnSuiteModel* GetSuiteModel( const TDesC& aGenre );
    
    /**
     * Pops the last Suite Model from the internal Suites Stack
     * and retruns in turn the element which remains as the last
     * after the change.
     * 
     * @since S60 v5.0
     * @param aGenre Name of the suite which is expected to be at the top
     * @return Suite Model which remains as the last.
     */
    IMPORT_C CHnSuiteModel* PopSuiteModelL( const TDesC& aGenre );
   
    /**
     * Fetches the count of suites stored in the container.
     * 
     * @since S60 v5.0
     * @return Suite count
     */
    IMPORT_C TInt GetSuiteModelCount() const;
    
    /**
     * Fetches the last suite model from the container.
     * 
     * @since S60 5.0
     * @return Requested, last suite model.
     */
    CHnSuiteModel* GetLastSuiteModel();
    
    /**
     * Fetches parent suite model from the container.
     * 
     * @since S60 5.0
     * @return Requested, last suite model.
     */
    CHnSuiteModel* GetParentSuiteModel();
    
    /**
     * Fetches the last suite model from the container.
     * 
     * @since S60 5.0
     * @param aPosition Position of the model in the stack.
     * @return Requested suite model.
     */
    CHnSuiteModel* GetSuiteModel( TInt aPosition );
    
    /**
     * Handles event.
     *
     * @since S60 5.0
     * @param aEventId Event id.
     * @param aRecipientId Recipient id.
     * @param aEventParameters Parameters to an event.
     * @return Status code.
     */
    IMPORT_C TInt OfferHnEventL( const TInt aEventId,
            const TInt aRecipientId, CLiwGenericParamList* aEventParameters = NULL );
    
    /**
     * Returns CHnSuiteModel matching given criteria (custom_id).
     * 
     * @param aCustomId Suite's custom identifier. 
     * @return Pointer to the suite model or null if model not found.
     */
    IMPORT_C CHnSuiteModel* GetMatchingSuiteModel( TInt64 aCustomId );
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    IMPORT_C void RemoveLiwObjects();
    
private:

    /**
     * Standard C++ Constructor.
     * 
     * @since S60 5.0
     */
    CHnSuiteModelContainer( );
      
    /**
     * Symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     * @param aModelObserver Interface of the model event observer.
     * @param aController Interface enabling to issues request to Hn engine.
     */
    void ConstructL( MHnMdModelEventObserver& aModelObserver,
            MHnControllerInterface& aController );
    
private: // data
        
    /**
     * Event handler.
     * Not own.
     */
    CHnEventHandler* iEventHandler;
    
    /**
     * Suites stack
     * Own
     */
    CHnSuitesStack* iSuitesStack;
   
    };

#endif //C_HNSUITEMODELCONTAINER_H
