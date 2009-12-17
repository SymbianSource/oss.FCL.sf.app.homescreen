/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for handling application events.
*
*/


#ifndef M_AIAPPLICATIONEVENTHANDLER_H
#define M_AIAPPLICATIONEVENTHANDLER_H

namespace AiXmlUiController
{
    
/**
 *  Extension of the CAiContentPublisher interface, which allows Active Idle
 *  Framework to notify their plug-ins that they must to handle event.
 *  Plug-ins must provide implementation of interface only if they are
 *  supporting event model (e.g. provides possibility to manipulate with
 *  engine properties via UI).
 *
 *  @lib AiXmlUiMain
 */
class MAiApplicationEventHandler
    {
    
protected:

    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiApplicationEventHandler() { }

public:  // New functions
    /**
     * Invoked by the framework when plug-in must handle an event.
     *
     * @param aEvent - unique identifier of event.
     * @param aParam - parameters associated with event. Each UI Definition
     *        declares events in the format: <event name>(<event params>),
     *        where <event name> is mapped by the framework to unique
     *        identifier suppied in aEvent, <event params> are provided to
     *        plug-in as-is in the descriptor.
     * @return ETrue if event was handled by this handler.
     */
    virtual TBool HandleApplicationEvent(TInt aEvent, const TDesC8& aParam) = 0;

    };
    
} // namespace AiXmlUiController

#endif  // M_AIAPPLICATIONEVENTHANDLER_H

// End of File.
