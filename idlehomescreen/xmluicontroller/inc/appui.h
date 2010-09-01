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
* Description:  App UI
*
*/


#ifndef C_APPUI_H
#define C_APPUI_H

// System includes
#include <coeview.h>

// User includes
#include <aifwpublisherinfo.h>
#include "xnappuiadapter.h"

// Forward declarations
class CAiUiIdleIntegration;

namespace AiXmlUiController
{        
class CContentRenderer;
class CXmlUiController;
class CAIXuikonEventHandler;
class COnlineOfflineHelper;

/**
 *  @ingroup group_xmluicontroller
 * 
 *  AppUI class of XML UI Controller application
 *
 *  @lib AiXmlUiMain
 */
NONSHARABLE_CLASS( CAppUi ) : public CXnAppUiAdapter,
    public MCoeViewActivationObserver
    {    
public:   // Constructors and destructor

    static CAppUi* NewL( CXmlUiController& aUiCtl );

    ~CAppUi();
    
public:  
    // new functions
    CCoeEnv* CoeEnv() { return iCoeEnv; }
            
private:  
    // from CXnAppUiAdapter
    
    void PrepareToExit();
           
    void HandleCommandL( TInt aCommand );

    void HandleXuikonEventL( CXnNodeAppIf& aOrigin, CXnNodeAppIf& aTrigger,  
         CXnDomNode& aTriggerDefinition, CXnDomNode& aEvent );
    
    void HandleEventL( const TDesC& aEvent, CXnNodeAppIf& aDestination );
                                                          
    void HandleWsEventL( const TWsEvent& aEvent, 
            CCoeControl* aDestination );

    void LoadPublisher( const TAiFwPublisherInfo& aPublisher ); 
                             
    void DestroyPublisher( const TAiFwPublisherInfo& aPublisher );        
                           
    TBool DynInitMenuItemL( const TDesC& aItemType, 
        RPointerArray< CXnNodeAppIf >* aList = NULL );
    
    void HandleEnterEditModeL( TBool aEnter );
       
    void HandleUiReadyEventL();
    
    TInt PublisherInfo( CXnNodeAppIf& aNode, 
        THsPublisherInfo& aInfo ); 

private:
    // new functions
    
    /**
     * Starts long press recognizer of Send key.
     * 
     * @since S60 5.2
     */
    void StartKeyTimerL();
    
    /**
     * Stops Send key long press recognizer.
     * 
     * @since S60 5.2
     */
    void StopKeyTimer();
    
    /**
     * Launches Voice command application after long Send key is pressed.
     * 
     * @since S60 5.2
     */
    void LaunchVoiceCommandL();
    
    /**
     * Callback function for long press timer of the Send key.
     * 
     * @since S60 5.2
     */
    static TInt ReportLongPressL( TAny* aThis );
    
    /**
     * Sets title pane text
     * 
     * @since S60 5.2
     * @param aText Text to set
     */
    void SetTitlePaneTextL( const TDesC& aText );
       
private:
    // from MCoeViewActivationObserver
    
    /**
     * @see MCoeViewActivationObserver
     */
    void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
        const TVwsViewId& aViewIdToBeDeactivated );
    
private:
    // from CCoeAppUi    
    
    /**
     * Gets a list of help contexts for a particular
     * application user interface.
     */
    CArrayFix<TCoeHelpContext>* HelpContextL() const;

public: 
    // new functions
    
    /**
     * Activates UI. Called by UI controller during bootup sequence.
     * 
     * @since S60 5.2
     */
    void ActivateUi();
    
    /**
     * Sets plugins online state
     * 
     * @since S60 5.2
     * @param aOnline ETrue to enter online, EFalse to offline
     * 
     */
    void SetOnlineStateL( TBool aOnline );
    
    /**
     * Gets Ui controller
     *
     * @since S60 5.0
     * @return Ui Controller
     */
    CXmlUiController& UiController() const;
    
    /**
     * Gets online/offline helper object
     *
     * @since S60 5.2
     * @return online/offline helper
     */
    COnlineOfflineHelper* Helper() const;
               
private: 
    // Constructors

    CAppUi( CXmlUiController& aUiCtl, TUid aAppUid );
    
    void ConstructL();
    
    void DoConstructL();
          
private:     
    // data

    /** UI Controller implementation, not owned */
    CXmlUiController& iUiCtl;
    /** Event handler, owned */
    CAIXuikonEventHandler* iEventHandler;    
    /** Content renderer, owned */
    CContentRenderer* iContentRenderer;              
    /** Idle Integration helper object, owed */
    CAiUiIdleIntegration* iIdleIntegration;
    /** Online/Offline helper object, owned */
    COnlineOfflineHelper* iHelper;
    /** Timer for Send key long press recognition, owned */
    CPeriodic* iKeyTimer;
    /** Edit mode title pane text, owned */
    HBufC* iEditModeTitle;
    /** DeviceStatus publisher info */
    THsPublisherInfo iDeviceStatusInfo;
    /** Flag to indicate whether edit mode is active */
    TBool iInEditMode;    
    };
    
}  // namespace AiXmlUiController
    
#endif  // C_APPUI_H

// End of File.
