/*
 * ===========================================================================
 *  Name        : hgfswclient.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Client API
 *  Version     : %version: 6 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#ifndef __HGFSWCLIENT_H
#define __HGFSWCLIENT_H

#include <e32std.h>
#include "hgfswentry.h"

class MHgFswObserver;
class CHgFswClientImpl;

/**
 * Client API for the Huriganes Fast Swap Server.
 */    
NONSHARABLE_CLASS( CHgFswClient ) : public CBase
    {
public:
    /**
     * Possible values for aType argument of ForegroundAppUid().
     */
    enum THgFswFgAppType
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
    IMPORT_C static CHgFswClient* NewL();
    
    /**
     * Creates a new instance.
     * @capability LocalServices
     */
    IMPORT_C static CHgFswClient* NewLC();
    
    /**
     * Destructor.
     */
    ~CHgFswClient();
    
    /**
     * Queries the fast swap content from the server and
     * copies it into the given array.
     * Previous content of the array is removed (by using ResetAndDestroy).
     * @param   aDst    array to which data is stored
     * @capability LocalServices
     */
    IMPORT_C void GetContentL( RHgFswArray& aDst );
    
    /**
     * Subscribes for continuous notifications about changes in the
     * fast swap content.
     * The notifications are generated until CancelSubscribe is called
     * or the instance is destroyed.
     * @capability LocalServices
     */
    IMPORT_C void Subscribe( MHgFswObserver& aObserver );
    
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
    IMPORT_C TUid ForegroundAppUid( THgFswFgAppType aType );

private:
    CHgFswClient();
    void ConstructL();

    CHgFswClientImpl* iImpl;
    };

#endif
