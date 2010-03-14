/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef XNEXTEVENTHANDLER_H
#define XNEXTEVENTHANDLER_H

// System includes
#include <e32base.h>

/**
 *  External event hadler interface for routing events from external
 *  rendering plug-ins to content plug-ins
 *
 *  @since S60 v5.2
 */
class MXnExtEventHandler
    {
public:
    /**
     * Routes the events from the external rendering plug-ins to content plug-ins
     * @since Series 60 5.2
     * @param aEvent Event string
     * @param aDestination Destination content plug-in.
     */
    virtual void HandleEventL( const TDesC& aEvent, const TDesC8& aDestination  ) = 0;
    };

#endif //XNEXTEVENTHANDLER_H
