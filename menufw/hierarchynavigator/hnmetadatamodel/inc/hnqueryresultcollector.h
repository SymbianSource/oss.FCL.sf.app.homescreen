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



#ifndef C_HNQUERYCOLLECTOR_H
#define C_HNQUERYCOLLECTOR_H

#include <e32base.h>

class CLiwGenericParamList;
class CHnMdQueries;
class CHnServiceHandler;
class CHnMdQuery;

// CLASS DECLARATION
/**
 * Query Results Collector - Callback Interface 
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
class MHnQueryResultCollectorNotification
	{
public:
	
	/**
	 * Callback function called after results are collected
	 * 
	 * 	 
	 * @since S60 5.0
     * @param aResults Results collected for the query.
	 */
	virtual void ResultsCollectedL( CLiwGenericParamList* aResults ) = 0; 
	};


// CLASS DECLARATION
/**
 * Query Results Collector.
 * 
 * This class is responsible for collecting queries results.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnQueryResultCollector ): public CActive
	{
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aQueries Queries to operate on.
     * @param aParams Parameters.
     * @return Fully constructed object.
     */
	static CHnQueryResultCollector* NewL( const CHnMdQueries& aQueries ,
	                               const CLiwGenericParamList& aParams);
	
	/**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aQueries Queries to operate on.
     * @param aParams Parameters.
     * @return Fully constructed object.
     */
	static CHnQueryResultCollector* NewLC( const CHnMdQueries& aQueries,
                                  const CLiwGenericParamList& aParams );
	        
    /**
     * Destructor.
     */
	~CHnQueryResultCollector();

    /**
     * Starts performing queries asynchronously.
     *
     * @since S60 5.0
     * @param aNotification callback interface - ResultCollectedL method calles
     *                      when queries results collected.
     */
	void StartAsynchL( MHnQueryResultCollectorNotification* aNotification );
    
protected:
    // from CActive
    /**
     * Resets the current query results.
     */
    void DoCancel();
    
    /**
     * Handles query result collection.
     */
    void RunL();    

private:
    /**
     * Standard C++ constructor.
     * 
     * @param aQueries Queries for which results are to be executed.
     * @param aParams Parameters for the queries.
     */
	CHnQueryResultCollector( const CHnMdQueries& aQueries,
	                         const CLiwGenericParamList& aParams );
	
    /**
     * Standard symbian 2nd phase constructor.
     *
     */
	void ConstructL();
	
    /**
     * Starts performing next query from a raw.
     * 
     */	
	void ExecQueryAsynchL();
	
	/**
     * Resets iterator and current results.
     */
	void ResetL();
	
	/**
     * Check if there is another query waiting.
     * 
     * @return ETrue if there is another query waiting.
     */
	TBool NextQuery();
		
    /**
     * Removes items.
     * 
     */
	void HandleQueryResultsL();
    
    /**
     * Executes query on Service Handler.
     * 
     * @param aQuery Query to be executed.
     */
    void ExecQueryL( CHnMdQuery& aQuery );
    
private:
	
	/**
     * Queries.
     * Not Own.
     */
	const CHnMdQueries& iQueries;
	
	/**
     * Own.
     */
	CLiwGenericParamList* iResults;
	
	/**
     * Not Own.
     */
	const CLiwGenericParamList& iParams;
	
	/**
     * Own.
     */
	TInt iQueryIterator;
	
	/**
     * Own.
     */
	CLiwGenericParamList* iCurrentQueryResults;
	
	/**
     * Not Own.
     */
	MHnQueryResultCollectorNotification* iNotification;
	
	/**
     * Service handler.
     * Own.
     */
    CHnServiceHandler* iSh;
	
	};

#endif // C_HNQUERYCOLLECTOR_H
