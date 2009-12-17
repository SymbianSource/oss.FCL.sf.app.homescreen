/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  receives key pressed notifications
*
*/


#ifndef M_MMKEYEVENTOBSERVER_H
#define M_MMKEYEVENTOBSERVER_H

#include <coedef.h>

/**
 *  Interface for handling key events.
 *
 *  @code
 *  @endcode
 *  @lib 
 *  @since S60 v3.0
 */
class MMmKeyEventObserver
    {
public:
	
    /**
     * Called by implementing class to handle incoming key event (e.g. rocker key pressed).
     *
     * @since S60 v3.0
     * @param aKeyEvent The type of key event triggered.
     * @param aType The Code of the event.
     * @return Information if the key event has been handled.
     */
    virtual TKeyResponse HandleKeyPressedL( const TKeyEvent &aKeyEvent, 
        TEventCode aType ) = 0; 
    
    };
    
#endif // M_MMKEYEVENTOBSERVER_H
