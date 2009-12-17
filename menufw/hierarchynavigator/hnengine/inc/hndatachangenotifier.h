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
* Description:  
*
*/
 

#ifndef C_HNDATACHANGENOTIFIER_H
#define C_HNDATACHANGENOTIFIER_H

#include "hndatachangeobserver.h"


/**
 * @ingroup group_hnengine
 * Data change notifier interface.
 *
 * @lib hnengine
 * @since S60 v5.0
 */
class MHnDataChangeNotifier
    {    
    public:
    
    /**
     * Subscribes observer.
     *
     * @since S60 v5.0
     * @param aObserv Observer to HNEngine.
     */
    virtual void SubscribeL( CHnDataChangeObserver* aObserv ) = 0;  
    };


#endif // C_HNDATACHANGENOTIFIER_H
