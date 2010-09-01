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
* Description:  appui for server
 *
*/


#ifndef TSFSWAPPUI_H
#define TSFSWAPPUI_H

#include <aknappui.h>

class MTsFswTaskListObserver;
class MTsFswResourceObserver;

/**
 * Appui for the fsw server.
 * Reimplements HandleWsEventL from base class to get notifications about
 * window group changes.
 */
class CTsFswAppUi : public CAknAppUi
    {
public:
    ~CTsFswAppUi();
    void ConstructL();

    /**
     * Setter function.
     *
     * @param aTaskListObserver Observer to be notified if task list
     *                     has changed
     */
    void SetTaskListObserver( MTsFswTaskListObserver& aTaskListObserver );

    /**
     * Setter function.
     *
     * @param aResourceObserver Observer to be notified when resources are changed.
     */
    void SetResourceObserver( MTsFswResourceObserver& aResourceObserver );

private:
    // from CEikAppUi
    void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );
    void HandleResourceChangeL( TInt aType );
            
private:
    MTsFswTaskListObserver* iTaskListObserver; // not own
    MTsFswResourceObserver* iResourceObserver; // not own
    };

#endif
