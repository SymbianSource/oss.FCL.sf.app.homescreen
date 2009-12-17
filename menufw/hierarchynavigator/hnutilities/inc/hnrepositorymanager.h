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


#ifndef HNREPOSITORYMANAGER_H_
#define HNREPOSITORYMANAGER_H_

#include <e32base.h>
#include <centralrepository.h>

#include "hnglobals.h"

/**
 * Class for managing operation on the central repository.
 * It is used to store widget types of suites in the central repository.
 * 
 * @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnRepositoryManager ) : public CBase
    {

public:
    /**
     * Factory method.
     * 
     * @since S60 v5.0
     * @return Returns fully qualified object.
     */
    IMPORT_C static CHnRepositoryManager * NewL();
    
    /**
     * Factory method.
     * 
     * @since S60 v5.0 
     * @return Returns fully qualified object.
     */    
    IMPORT_C static CHnRepositoryManager * NewLC();
    
    /**
     * Stores widget type in the repository.
     * 
     * @since S60 v5.0
     * @param aSuiteName Name of the suite.
     * @param aWidgetType Widget type to be stored.
     * @return Error code.
     */    
    IMPORT_C TInt StoreSuiteWidgetTypeL( const TDesC & aSuiteName, 
                                         THnSuiteWidgetType aWidgetType );
    
    /**
     * Deletes all suite related information from the repository.
     * 
     * @since S60 v5.0
     * @param aSuiteName Name of the suite.
     * @return Error code.
     */    
    IMPORT_C TInt DeleteSuiteRelatedL( const TDesC & aSuiteName );
    
    /**
     * Read suite widget type from the repository.
     * 
     * @since S60 v5.0
     * @param aSuiteName Name of the suite.
     * @param aSuiteWidget Widget type.
     * @return Error code.
     */    
    IMPORT_C TInt ReadSuiteWidgetTypeL(
            const TDesC & aSuiteName, THnSuiteWidgetType& aSuiteWidget );
    
    /**
     * Standard C++ destructor.
     * 
     * @since S60 v5.0
     */    
    IMPORT_C ~CHnRepositoryManager();
    
private:

    /**
     * Second phase symbian constructor.
     * 
     * @since S60 v5.0
     */
    void ConstructL();
    
    /**
     * Standard C++ constructor.
     * 
     * @since S60 v5.0
     */    
    CHnRepositoryManager();
    
private: // data

    /**
     * Widget type position.
     */
    const TInt KWidgetTypePosition;

    /**
     * Central Repository object.
     * Own.
     */
    CRepository *iRepository;
    
    };

#endif // HNREPOSITORYMANAGER_H_
