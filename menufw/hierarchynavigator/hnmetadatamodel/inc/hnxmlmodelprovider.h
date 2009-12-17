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



#ifndef C_HNMULMODELPROVIDER_H
#define C_HNMULMODELPROVIDER_H


#include <xmlengdom.h>
#include <bautils.h>
#include <e32hashtab.h>

class CHnXmlModelCache;

/**
 * Xml Model Provider
 * 
 * Parses suites definitions in xml and makes changes in model. 
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnXmlModelProvider) : public CActive
    {
public:
    /**
     * Standard factory method.
     *
     * @since S60 v5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnXmlModelProvider* NewL();

    /**
     * Standard factory method.
     *
     * @since S60 v5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnXmlModelProvider* NewLC();

    /**
     * Standard virtual destructor.
     */
    IMPORT_C virtual ~CHnXmlModelProvider();
    
    /**
     * @see CAsctive::DoCancel
     */
    void DoCancel();

    /**
     * Services the suite synchronization steps according to class iterator.
     *
     * @see CAsctive::RunL
     */
    void RunL();

private:
    
    /**
     * ReadFileLC reads file.
     *
     * @since S60 v5.0
     * @param aPath Path to a file.
     * @return Content of the file.
     */
     HBufC8* ReadFileLC(const TDesC& aPath);

    /**
     * Parses a document.
     *
     * @since S60 v5.0
     * @param aDoc A document to be parsed.
     * @return Xml document.
     */
     RXmlEngDocument ParseDocL( const TDesC8& aDoc );

     
     /**
     * Builds root of xml model from suite definition.
     *
     * @since S60 v5.0
     * @param aSuiteName Suite name.
     * @param aXmlDoc XLM model of the suite.
     * @return Error code.
     */
     TInt CollectSuiteL( const TDesC& aSuiteName, RXmlEngDocument& aXmlDoc );
     
     /**
     * appends items to suite in model,
     *
     * @since S60 v5.0
     * @param aSuiteName Name of suite to append items to.
     * @param aXmlDoc XLM model of the suite.
     */
     void CollectItemsL(const TDesC& aSuiteName, RXmlEngDocument& aXmlDoc );
     
     /**
     * Copies suites and items definitions to working dir.
     *
     * @since S60 v5.0
     * @return Error code.
     */
     TBool SynchronizeSuitesL();
     
    /**
     * Creates model from suite definition.
     *
     * @since S60 v5.0
     * @param aStr Name of suite.
     * @param aXmlDoc XML model of the suite.
     */
    void CreateModelL( const TDesC& aStr, RXmlEngDocument& aXmlDoc  );   
    
    /**
     * Changes event names to ids.
     *
     * @since S60 v5.0
     * @param aElement Xml element.
     */
    void ChangeEventNamesToIdsL( TXmlEngElement& aElement );

    /**
     * Changes event names to items in all the children.
     *
     * @see ChangeEventNamesToIdsL
     * @since S60 v5.0
     * @param aElement Xml element.
     */
    void ChangeEventsToIdsInChildrenL(
            TXmlEngElement & aElement );
    
#ifdef _DEBUG
    /**
     * Logs event mapping.
     *
     * @since S60 v5.0
     * @param aEventMap Event map.
     */
    void LogEventMapping( const RHashMap<HBufC*, TInt>& aEventMap );
#endif

public:     
          
    /**
     * Parses a document.
     *
     * @param aPath Path to a file.
     * @return RXmlEngDocument.
     */
     RXmlEngDocument ParseFileL( const TDesC& aPath );
     
     /**
      * Resets cached models.
      */
     void ResetCache();
     
    /**
     * Reparses a document.
     */
     IMPORT_C void ReloadModelL();
   
    /**
     * Parses a document - searching proper suite.
     *
     * @param aStr Name of the model.
     * @param aXmlDoc A reference to document of RXmlEngDocument class;
     * @return Error code. KErrNone if model found.
     */
    IMPORT_C TInt GetModelL( const TDesC& aStr, RXmlEngDocument& aXmlDoc );
    
     /**
      * Parses a document.
      *
      * @param aEventName Event name.
      * @return Event id.
      */     
     TInt GetNewEventId( HBufC* aEventName );
     
     /**
      * Check if suite exists.
      *
      * @since S60 5.0
      * @param aSuite Suite name.
      * @return ETrue if suite exists, otherwise EFalse.
      */
     TBool SuiteExistsL( const TDesC& aSuite );
    
private:
     
    /**
     * Invoked after asynchronous synchronization is finished.
     */
     void SynchronizationFinishedL();

    /**
     * Sets up normal path to suites.
     *
     * @param aPath Path to be searched for suites.
     */	
     void SearchPathForSuitesL( const TDesC& aPath );
	
    /**
     * Sets up normal path to suites.
     */   
     void SetupSuitePathL();
	
    /**
     * Sets up fail sage path to suites.
     */   
    void SetupFailSafeSuitePathL();
	
    /**
     * Standard C++ constructor.
     */     
    CHnXmlModelProvider();

    /**
     * Standard symbian 2nd phase constructor.
     */     
    void ConstructL();

    /**
    * Finds drive letter where suite definition is.
    *
    * @since S60 v5.0
    * @return Error code
    */
    void CheckDrivesL(); 
               
    
#ifdef _DEBUG
private:
    void LogSuiteModel( const TDesC& aName, 
                                         RXmlEngDocument& aDoc );
#endif // _DEBUG

private: // data

    /**
     * Xml DOM implementation.
     */
    RXmlEngDOMImplementation iDomImpl;

    /**
     * Xml DOM parser.
     */
    RXmlEngDOMParser iDomParser;
                
    /**
     * File server session
     */
    RFs iFs;
        
    /**
     * File manager
     */
    CFileMan* iFileMan;                
    
    /**
     * List containing names of suites (suite name is same as dir name)
     * that should be installed from all drives
     */    
    RHashSet< HBufC* > iInstSuites;
    
    /**
     * Event map.
     */
    RHashMap< HBufC*, TInt > iEventMap;
    
    /**
     * Main path.
     */
    RBuf iPath;
    
    /**
     * Cached suites.
     */
    CHnXmlModelCache* iCache;
    
    /**
     * Own. Iterator used in synchronisation. 
     * Indicates next suite name to be synchronised.
     */
    THashSetIter<HBufC*>* iSuiteSetIterator;

    /**
     * ETrue if suite files on the c: drive were updated during the
     * last synchronization. New suite files also count as updated.
     * EFalse if the last synchronization did not alter any files
     * on the c: drive.
     */
    TBool iSuiteFilesUpdated;

    };
#endif // C_HNMULMODELPROVIDER_H
