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
* Description:  Used for handling XUIKON events
*
*/


#ifndef C_EVENTHANDLER_H
#define C_EVENTHANDLER_H

#include <e32base.h>

#include "xnvolumecontrolobserver.h"
#include "xnvolumecontrol.h"

class CXnNodeAppIf;
class CXnProperty;
class MAiFwEventHandler;
class MAiStrParser;
class TXnUiEngineAppIf;
class CXnDomNode;

namespace AiXmlUiController
{
    
class CXmlUiController;
class CContentRenderer;
class MAiUiEventHandler;
class MAiApplicationEventHandler;

/**
 *  @ingroup group_xmluicontroller
 * 
 *  This class is used for handling XUIKON events.
 *
 *  Events described in Content Model are handled in here. This class parses
 *  the events and decides what to do with them. Events are either consumed in
 *  UI or in case of a plug-in event forwarded to XML AI Framework for further
 *  processing.
 *
 *  @lib AiXmlUiMain
 */
class CAIXuikonEventHandler : public CBase,
                              public MXnVolumeControlObserver
    {
public:

    static CAIXuikonEventHandler* NewL(CXmlUiController& aXmlUiController,
                                       CContentRenderer& aRenderer);
    ~CAIXuikonEventHandler();

    /**
    * Handle interaction notification from XUIKON UI engine.
    *
    * @see CXnAppUiAdapter::HandleXuikonEventL
    */
    void HandleXuikonEventL( CXnNodeAppIf& aOrigin,
                             CXnNodeAppIf& aTrigger,
                             CXnDomNode& aTriggerDefinition,
                             CXnDomNode& aEvent );

    /**
    * Set handler for plug-in events
    * @param aPluginEventHandler Pointer to event handler
    */
    void SetFwEventHandler( MAiFwEventHandler* aPluginEventHandler );

    /**
    * Set handler for UI events
    * @param aUiEventHandler Pointer to event handler
    */
    void SetUiEventHandler( MAiUiEventHandler* aUiEventHandler );

    /**
    * Set handler for application events
    * @param aApplicationEventHandler Pointer to event handler
    */
    void SetApplicationEventHandler( MAiApplicationEventHandler* 
                                        aApplicationEventHandler );

// from MXnVolumeControlObserver
    
    /**
    * Called when volume value is changed.
    */
    void VolumeChanged( TInt aValue );

private:
    CAIXuikonEventHandler(CXmlUiController& aUiController,
                          CContentRenderer& aRenderer);
                          
    void ConstructL();
    
    TBool HandleSystemEventL( const TDesC8& aEventText, CXnDomNode& aEvent );
    
    TBool HandleUiEventL( const TDesC8& aEventText, CXnNodeAppIf& aOrigin );
    
    TBool HandleApplicationEventL( const TDesC8& aEventText );
    
    void DispatchEventL( CXnDomNode& aEvent,
                         CXnNodeAppIf& aOrigin,
                         CXnDomNode& aTriggerDefinition );
                         
    void HandlePluginEventL( CXnDomNode& aEvent );
        
    void ParseParameterStringL(const TDesC8& aSourcePtr, TPtrC8& aTargetPtr);    
    
private: // data

    class CNullEventHandler;

    /**
     * String parser utility. Own.
     */
    MAiStrParser* iStrParser;

    /**
     * Reference to XML ui controller.
     */
    CXmlUiController& iUiController;
    
    /**
     * Reference to content renderer.
     */
    CContentRenderer& iContentRenderer;
    
    /**
     * Null event handler.
     */
    CNullEventHandler* iNullEventHandler;

    /**
     * Plug-in event handler.
     * Doesn't own.
     */
     MAiFwEventHandler* iFwEventHandler;

    /**
     * UI event handler.
     * Doesn't own.
     */
     MAiUiEventHandler* iUiEventHandler;

    /**
     * Application event handler.
     * Doesn't own.
     */
     MAiApplicationEventHandler* iApplicationEventHandler;
     
    /**
     * Volume control event.
     * Owned.
     */
    HBufC* iVolumeEvent;

    /**
     * Volume control interface.
     * Not owned.
     */
    XnVolumeControlInterface::MXnVolumeControlInterface* iVolumeControl;
    };

} // namespace AiXmlUiController

#endif // C_EVENTHANDLER_H
