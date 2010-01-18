/*
* ==============================================================================
*  Name        : %name: hgpreviewobserver.h %
*  Part of     : Hg fast swap preview provider plugin
*  Description : Preview provider API
*  Version     : %version: 1 %
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef HGPREVIEWOBSERVER_H
#define HGPREVIEWOBSERVER_H

// INCLUDES
#include <e32base.h>

/**
 * Observer for preview screenshot changes. Client must implement this and 
 * register itself when calling CHgFastSwapPreviewProvider::NewL. Callback
 * methods are called whenever there is a new preview screenshot taken from
 * the applications or when an application is exited.
 *
 * @see CHgFastSwapPreviewProvider
 */
class MHgFastSwapPreviewObserver
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

#endif // HGPREVIEWOBSERVER_H

// End of file
