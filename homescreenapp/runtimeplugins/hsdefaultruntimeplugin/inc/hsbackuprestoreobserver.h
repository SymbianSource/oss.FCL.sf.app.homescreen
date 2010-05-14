/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef HSBACKUPRESTOREOBSERVER_H
#define HSBACKUPRESTOREOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <connect/abclient.h>

class CHsBRActiveCallback;

NONSHARABLE_CLASS(CHsBackupRestoreObserver) : public CActive
    {

public:

    /**
     * Creates an instance of CCPServerBURListener implementation.
     * @param aCallBack Pointer to observer interface.
     */
    static CHsBackupRestoreObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CHsBackupRestoreObserver();    

private:
    /**
     * Constructor.
     * @param aCallBack Pointer to observer interface.
     */
    CHsBackupRestoreObserver();
    
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Handles changes in backup state.
     */
	void HandleBackupStateL( const TInt aValue );
	
	/**
     * Subsribes notifications of backup/restore p&s key.
     */
    void SubscribePSKey();    

private:
    /**
     * From CActive.
     */
    void DoCancel();

    /**
     * From CActive.
     */
    void RunL();

    /**
     * From CActive.
     */
    TInt RunError( TInt aError );

private:
    /**
     * User side interface to Publish & Subscribe.
     */
    RProperty iProperty;

    /**
     * Callback interface for widget's active data owner.
     * Own.
     */
    CHsBRActiveCallback* iCallBack;

    /**
     * Active backup client.
     * Own.
     */
	conn::CActiveBackupClient* iActiveBackupClient;
    
    /**
     * Last state of BUR: either backup or restore or normal or unset
     */
    TInt iLastType;

    /**
     * Interface for notifying changes in Backup and Restore.
     * Not own.
     */
    //MBURListenerCallback* iCallback;

    };

#endif // HSBACKUPRESTOREOBSERVER_H
// End of File