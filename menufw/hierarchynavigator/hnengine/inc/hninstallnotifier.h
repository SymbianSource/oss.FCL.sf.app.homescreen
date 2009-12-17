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
* Description:  Sis package installation event listener
*
*/
  


#ifndef HNINSTALLNOTIFIER_H
#define HNINSTALLNOTIFIER_H

#include <e32base.h>
#include <e32property.h>

/**
 * @ingroup group_hnengine
 *  Interface for observing Sis installation events.
 *
 * @lib hnengine
 * @since S60 S60 v3.1
 */
class MHnInstallNotifierCallback
    {
    
public:
    /**
     * @ingroup group_hnengine
     * Enum defining the purpouse of the installation event.
     */
    enum TInstOp
        {
        EInstOpNone = 0x00, 
        EInstOpInstall = 0x01, 
        EInstOpUninstall = 0x02, 
        EInstOpRestore = 0x04  
        };

    /**
     * @ i ngroup group_hnengine
     * Enum defining the exit status of the installation event.
     */
/*    enum TInstOpStatus
        {
        EInstOpStatusNone = 0x0000, 
        EInstOpStatusSuccess = 0x0100, 
        EInstOpStatusAborted = 0x0200 
        };*/
        
    /**
     * InstallChangeL is called when the subscribed key has been changed.
     * 
     * @param aStatus Status of the installation event.
     */
    virtual void InstallChangeL( TInt aStatus ) = 0;
    
    };


/**
 * @ingroup group_hnengine
 *  Central Repository notifier.
 * 
 * @lib hnengine
 * @since S60 S60 v3.1
 */
NONSHARABLE_CLASS( CHnInstallNotifier ) : public CActive
    {
    
public:

    /**
     * Creates an instance of CHnMulModelInstallNotifier implementation.
     * 
     * @since S60 v5.0
     * 
     * @param aCallback Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     * @return Fully constructed object.
     */
    static CHnInstallNotifier* NewL( 
                                   MHnInstallNotifierCallback* aCallback,
                                   TUid aCategory, 
                                   TUint aKey );

    /**
     * Default destructor.
     * 
     * @since S60 v5.0
     */
    virtual ~CHnInstallNotifier();


private:
    /**
     * Constructor.
     * 
     * @since S60 v5.0
     * 
     * @param aCallback Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     */
    CHnInstallNotifier( 
                                MHnInstallNotifierCallback* aCallback, 
                                TUid aCategory, 
                                TUint aKey );

    /**
     * Symbian 2nd phase constructor.
     * 
     * @since S60 v5.0
     */
    void ConstructL();

    /**
     * From CActive. Implements cancellation of an outstanding request.
     * 
     * @since S60 v5.0
     */
    void DoCancel();
    
    /**
     * From CActive. Handles an active object's request completion event.
     * 
     * @since S60 v5.0
     */
    void RunL();
    
    /**
     * From CActive.
     * 
     * @since S60 v5.0
     * @param aError The leave code.
     * @return Status code.
     */
    TInt RunError( TInt aError );
    
        
private:
    /**
     * User side interface to Publish & Subscribe.
     */
    RProperty iProperty;

    /**
     * Interface for notifying changes in SWI.
     * Not own.
     */
    MHnInstallNotifierCallback* iCallback;
    
    /**
     * Category uid.
     */
    TUid    iCategory;
    
    /**
     * Key identifier.
     */
    TUint   iKey;

    };

#endif // HNINSTALLNOTIFIER_H

// End of File
