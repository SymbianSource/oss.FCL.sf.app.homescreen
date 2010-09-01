/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens for changes in a Central Repository key.
*
*/

#ifndef CENREPOBSERVER_H
#define CENREPOBSERVER_H

// FORWARD DECLARATIONS
class CRepository;
class MCenRepObserver;

// Stop requesting new notifications after this many consecutive errors
const TInt KCenRepErrorRetryCount = 80;

namespace AiXmlUiController
{

/**
* MCenRepObserver
*
* Client application must implement the CenRepObserver interface in
* order to receive a notification when Central Repository key 
* KCRUidCmManager/ KCurrentCellularDataUsage changes.
*
*/
class MCenRepObserver
    {
public:
    /**
    * This method is called every time when Central Repository key 
    * KCRUidCmManager/ KCurrentCellularDataUsage changes.
    *
    * @param aValue The new value for KCurrentCellularDataUsage.
    * 
    */
    virtual void CurrentCellularDataUsageChangedL( const TInt aValue ) = 0;
    };

/**
* CCenRepObserver
*
*/
NONSHARABLE_CLASS( CCenRepObserver ) : public CActive
    {
public: // Constructors and destructor
    static CCenRepObserver* NewL( MCenRepObserver* aObserver );

    static CCenRepObserver* NewLC( MCenRepObserver* aObserver );

    virtual ~CCenRepObserver();

private:
    CCenRepObserver( MCenRepObserver* aObserver );

    void ConstructL();
    
    /**
    * Request notification for network setting change
    */
    void RequestNotifications();
 
private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    /**
    * Cenrep Observer. Not own
    */
    MCenRepObserver*   iObserver;
    
    /**
    * Cenrep repository. Own
    */
    CRepository*       iRepository;
    
    /**
    * Cenrep read error count.
    */
    TInt               iErrorCounter;
    };
}// namespace AiXmlUiController
#endif // AOCENREPOBSERVER_H

// End-of-file
