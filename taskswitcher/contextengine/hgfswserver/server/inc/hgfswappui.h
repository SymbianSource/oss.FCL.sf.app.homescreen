/*
 * ===========================================================================
 *  Name        : hgfswappui.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : appui for server
 *  Version     : %version: 5 %
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

#ifndef __HGFSWAPPUI_H
#define __HGFSWAPPUI_H

#include <aknappui.h>

class MHgFswTaskListObserver;
class MHgFswResourceObserver;

/**
 * Appui for the fsw server.
 * Reimplements HandleWsEventL from base class to get notifications about
 * window group changes.
 */
class CHgFswAppUi : public CAknAppUi
    {
public:
    ~CHgFswAppUi();
    void ConstructL();

    /**
     * Setter function.
     *
     * @param aTaskListObserver Observer to be notified if task list
     *                     has changed
     */
    void SetTaskListObserver( MHgFswTaskListObserver& aTaskListObserver );

    /**
     * Setter function.
     *
     * @param aResourceObserver Observer to be notified when resources are changed.
     */
    void SetResourceObserver( MHgFswResourceObserver& aResourceObserver );

private:
    // from CEikAppUi
    void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );
    void HandleResourceChangeL( TInt aType );
            
private:
    MHgFswTaskListObserver* iTaskListObserver; // not own
    MHgFswResourceObserver* iResourceObserver; // not own
    };

#endif
