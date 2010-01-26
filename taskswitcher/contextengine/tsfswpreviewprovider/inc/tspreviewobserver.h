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
* Description:  Preview provider API
*
*/


#ifndef TSPREVIEWOBSERVER_H
#define TSPREVIEWOBSERVER_H

// INCLUDES
#include <e32base.h>

/**
 * Observer for preview screenshot changes. Client must implement this and 
 * register itself when calling CTsFastSwapPreviewProvider::NewL. Callback
 * methods are called whenever there is a new preview screenshot taken from
 * the applications or when an application is exited.
 *
 * @see CTsFastSwapPreviewProvider
 */
class MTsFastSwapPreviewObserver
    {
public:

    /**
     * Called when application has changed and screenshot has been taken. 
     * New sceenshots will be taken and provided to the client via this 
     * callback every time the application goes to background.
     *
     * @param aWgId Window group id of the application.
     * @param aFbsHandle Handle to screenshot CFbsBitmap.
     */         
    virtual void HandleFswPpApplicationChange( TInt aWgId, TInt aFbsHandle ) = 0;
    
    /**
     * Called when application has been exited. Client can free the resources
     * related to aWgId (for example a copied bitmap) since the application is
     * no more running.
     *
     * @param aWgId Window group id of the application.
     */         
    virtual void HandleFswPpApplicationUnregistered( TInt aWgId ) = 0;    
    };

#endif // TSPREVIEWOBSERVER_H

// End of file
