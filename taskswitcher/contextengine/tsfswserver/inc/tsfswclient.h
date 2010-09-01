/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client API
 *
*/


#ifndef TSFSWCLIENT_H
#define TSFSWCLIENT_H

#include <e32std.h>
#include "tsfswentry.h"

class MTsFswObserver;
class CTsFswClientImpl;

/**
 * Client API for the Huriganes Fast Swap Server.
 */    
NONSHARABLE_CLASS( CTsFswClient ) : public CBase
    {
public:
    /**
     * Possible values for aType argument of ForegroundAppUid().
     */
    enum TTsFswFgAppType
        {
        /**
         * Means that even when the foreground app has another app embedded
         * into it the uid of the container application will be returned.
         */
        EUseStandaloneUid,
        /**
         * Means that if the foreground app has another app embedded
         * into it then the returned uid will be the uid of the embedded
         * app.
         */
        EUseEmbeddedUid
        };

    /**
     * Creates a new instance.
     * @capability LocalServices
     */
    IMPORT_C static CTsFswClient* NewL();
    
    /**
     * Creates a new instance.
     * @capability LocalServices
     */
    IMPORT_C static CTsFswClient* NewLC();
    
    /**
     * Destructor.
     */
    ~CTsFswClient();
    
    /**
     * Queries the fast swap content from the server and
     * copies it into the given array.
     * Previous content of the array is removed (by using ResetAndDestroy).
     * @param   aDst    array to which data is stored
     * @capability LocalServices
     */
    IMPORT_C void GetContentL( RTsFswArray& aDst );
    
    /**
     * Subscribes for continuous notifications about changes in the
     * fast swap content.
     * The notifications are generated until CancelSubscribe is called
     * or the instance is destroyed.
     * @capability LocalServices
     */
    IMPORT_C void Subscribe( MTsFswObserver& aObserver );
    
    /**
     * Cancels the subscription for change notifications.
     * @capability LocalServices
     */
    IMPORT_C void CancelSubscribe();
    
    /**
     * Tries to close the application with the given id.
     * @capability LocalServices
     */
    IMPORT_C void CloseApp( TInt aWgId );
    
    /**
     * Brings the application with the given id into foreground.
     * @capability LocalServices
     */
    IMPORT_C void SwitchToApp( TInt aWgId );

    /**
     * Returns the uid of the foreground app or KNullUid if something goes wrong.
     *
     * Will never return hidden apps, only those which can also be seen in the
     * array returned by GetContentL.
     *
     * However the handling of embedded applications might be different:
     * (as opposed to GetContentL)
     * If aType == EUseEmbeddedUid:
     *   This function will always return the embedded application's uid, unlike
     *   GetContentL which will always refer to the container app (except for the
     *   screenshot of course). This means that the returned uid is simply the
     *   uid belonging to the first (non-hidden, etc.) window group from the window group list.
     * If aType == EUseStandaloneUid:
     *   Embedded applications are ignored and the returned uid is the app uid belonging
     *   to the first parentless window group.
     */
    IMPORT_C TUid ForegroundAppUid( TTsFswFgAppType aType );

private:
    CTsFswClient();
    void ConstructL();

    CTsFswClientImpl* iImpl;
    };

#endif
