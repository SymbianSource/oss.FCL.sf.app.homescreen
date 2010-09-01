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
* Description:  XML UI Controller main class
*
*/

#ifndef _XMLUICONTROLLER_H
#define _XMLUICONTROLLER_H

// System includes

// User includes
#include <aisystemuids.hrh>
#include "aiuicontroller.h"
#include "aiuieventhandler.h"

// Constants
const TInt KImplementationUidXmlUiController = AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML;
const TUid KUidXmlUiController = { KImplementationUidXmlUiController };

// Forward declarations
class CApaApplication;
class TXnUiEngineAppIf;
class CXnNodeAppIf;
class CRepository;
class THsPublisherInfo;

namespace AiXmlUiController
{
class CAppUi;
class CAIXuikonEventHandler;
class CXmlNodeIdGenerator;
class CContentRenderer;
class CContentPublisher;

/**
 *  @ingroup group_xmluicontroller
 * 
 *  Active Idle XML UI Controller.
 */
NONSHARABLE_CLASS( CXmlUiController ) : public CAiUiController,
    public MAiMainUiController, public MAiUiEventHandler                         
    {
public:   
    // Constructors and destructor

    static CXmlUiController* NewL();
    
    ~CXmlUiController();

public: 
    // New functions
    
    /**
     * Sets content observer.
     * 
     * @since S60 5.0
     * @param aObserver Conten observer 
     */
    void SetObserver( MAiContentObserver& aObserver );

    /**
     * Set the app ui when created.
     * 
     * @since S60 5.0
     * @param aAppUi AppUi
     */
    void SetAppUi( CAppUi& aAppUi );
    
    /**
     * Get app ui.
     * 
     * @since S60 5.0
     * @return AppUi
     */
    CAppUi* AppUi() const;

    /**
     * Creates and returns Xuikon event handler.
     * 
     * @since S60 5.0
     * @param aContentRender Content renderer
     * @return Xuikon event handler
     */
    CAIXuikonEventHandler* CreateXuikonEventHandlerL(
        CContentRenderer& aRenderer );
    
    /**
     * Call from App Ui ConstructL to notify AI FW that the App env is ready.
     * 
     * @since S60 5.0
     */
    void NotifyAppEnvReadyL();

    /**
     * Gets UiFw event handler 
     *
     * @since S60 5.0
     * @return UiFw event handler     
     */    
    MAiFwEventHandler* FwEventHandler() const;

    /**
     * Gets UiFw state handler 
     *
     * @since S60 5.2
     * @return UiFw state handler     
     */    
    MAiFwStateHandler* FwStateHandler() const;
    
    /**
     * Gets UiEngine.
     *
     * @since S60 5.0
     * @return UiEngine     
     */        
    TXnUiEngineAppIf* UiEngineL() const;
    
	/**
	 * Get settings repository.
	 * 
	 * @since S60 5.0
	 * @return Settings repository
	 */
    CRepository& SettingsRepository() const;

    /**
     * Gets xml ui publishers
     * 
     * @since S60 5.2
     * @param aSource Xml node which defineds the publisher
     * @param aPublisherInfo Publisher info filled based aSource information
     */    
    void PublisherInfoL( CXnNodeAppIf& aSource,
        THsPublisherInfo& aPublisherInfo );

public: 
    // from CAiUiController

    /**
     * @see CAiUiController
     */
    void LoadUIDefinitionL();
       
    /**
     * @see CAiUiController
     */    
    void GetSettingsL( const THsPublisherInfo& aPublisherInfo, 
        RAiSettingsItemArray& aSettings );
    
    /**
     * @see CAiUiController
     */    
    void ActivateUI();
    
    /**
     * @see CAiUiController
     */    
    MAiContentObserver& GetContentObserver();
    
    /**
     * @see CAiUiController
     */    
    void SetEventHandler( MAiFwEventHandler& aFwEventHandler );
    
    /**
     * @see CAiUiController
     */    
    void SetStateHandler( MAiFwStateHandler& aFwStateHandler );
       
    /**
     * @see CAiUiController
     */    
    MAiMainUiController* MainInterface();
    
    /**
     * @see CAiUiController
     */    
    MAiSecondaryUiController* SecondaryInterface();
        
private: 
    // from MAiMainUiController

    /**
     * @see MAiMainUiController
     */
    void RunApplicationL();
    
    /**
     * @see MAiMainUiController
     */    
    CCoeEnv& CoeEnv();
            
    /**
     * @see MAiMainUiController
     */    
    void Exit();

    /**
     * @see MAiMainUiController
     */        
    TBool IsMenuOpen();
    
private: 
    // from MAiUiEventHandler

    /**
     * @see MAiUiEventHandler
     */        
    TBool HandleUiEvent(TAny* aEvent, const TDesC8& aParam);
              
private: 
    // Constructors

    /**
     * C++ default constructor
     */
    CXmlUiController();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: 
    // New functions
    
    static CApaApplication* NewApplication();
    
    void HandleFocusGainedL( const TDesC8& aUiElement1, 
        const TDesC8& aUiElement2, CXnNodeAppIf& aOrigin );
    
    void HandleSetElementSizeL( const TDesC8& aElementName, 
        CXnNodeAppIf& aOrigin );
    
    static TInt ExitTimerCallBack( TAny *aSelf );
       
    void GetConfigurationsL( CXnNodeAppIf& aNode, 
        RAiSettingsItemArray& aSettings, const TDesC& aConfOwner );
    
    void GetContentModelL( const THsPublisherInfo& aPubInfo, 
        RAiSettingsItemArray& aSettings );
                
private: 
    // data
        
    /** Ai content observer, Not owned */
    MAiContentObserver* iObserver;        
    /** AiFw Event handler, Not owned */
    MAiFwEventHandler* iFwEventHandler;
    /** AiFw State handler, Not owned */
    MAiFwStateHandler* iFwStateHandler;    
    /** * Node id generator, Owned */
    CXmlNodeIdGenerator* iNodeIdGenerator;    
    /** AppUi, Not owned */
    CAppUi* iAppUi;    
    /** Timer to call AppUi's exit, Owned */
    CPeriodic *iExitTimer; 
    /** Cenrep, Owned */
    CRepository *iAISettingsRepository;
    /** CPS publisher, Owned */
    CContentPublisher* iCPSpublisher;    
    /** Flag to indicate whether running as main UI controller */
    TBool iRunningAsMain;
    };    
}  // namespace AiXmlUiController

#endif  // _XMLUICONTROLLER_H

// End of file
