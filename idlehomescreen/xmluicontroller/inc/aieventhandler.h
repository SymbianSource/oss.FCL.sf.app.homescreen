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
* Description:  Used for handling system events (events with EventHandler-prefix).
*
*/


#ifndef C_AIEVENTHANDLER_H
#define C_AIEVENTHANDLER_H

#include <e32base.h>

class CXnDomNode;
class CLiwServiceHandler;
class MLiwInterface;
class CLiwDefaultMap;

namespace AiXmlUiController
{
class CXmlUiController;

/**
 * 
 *  This class is used for handling and passing events.
 *
 *  Class handles some of the events itself and uses Action Handler Service for the rest.
 *
 *  @lib AiXmlUiMain
 */
class CAIEventHandler : public CBase
    {
public:
    // Constructor
    static CAIEventHandler* NewL( CXmlUiController& aUiController );
    
    // Destructor
    ~CAIEventHandler();

    /**
     * Handles the restricted ( can be triggered only from view) events and forwards the rest
     * into appropriate functions.
     * 
     * @param aEventText Full event name (prefix/event)
     * @param aEvent Includes rest of the parameters. 
     */
    void HandleEventL( const TDesC8& aEventText, CXnDomNode& aEvent );

private:
    
    CAIEventHandler( CXmlUiController& aUiController );
                          
    void ConstructL();
    
    void LaunchAppL( CXnDomNode& aEventNode );
    
    void ActivateViewL( CXnDomNode& aEventNode );
    
    void ExecuteAppLauncherL( CLiwDefaultMap& map );
    
    MLiwInterface* ActionHandlerInterfaceL( );
    
private: // data
    /**
     * Reference to XML ui controller.
     */
    CXmlUiController& iUiController;
    
    /**
     * Action Handler service handler.
     * Own.
     */
    CLiwServiceHandler* iServiceHandler;
    
    /**
     * Service interface.
     * Own
     */
    MLiwInterface* iMsgInterface;
    };
}

#endif // C_AIEVENTHANDLER_H
