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


#ifndef C_XMLUICONTROLLER_H
#define C_XMLUICONTROLLER_H

#include <aisystemuids.hrh>
#include "aiuicontroller.h"
#include "aiuieventhandler.h"


const TInt KImplementationUidXmlUiController = AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML;
const TUid KUidXmlUiController = { KImplementationUidXmlUiController };


class CApaApplication;
class TXnUiEngineAppIf;
class CXnNodeAppIf;
class CRepository;

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
class CXmlUiController : public CAiUiController,
                         public MAiMainUiController,
                         public MAiUiEventHandler
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
     * Gets UiFw observer
     *
     * @since S60 5.0
     * @return UiFw observer     
     */
    MAiUiFrameworkObserver* UiFrameworkObserver() const;

    /**
     * Gets UiFw event handler 
     *
     * @since S60 5.0
     * @return UiFw event handler     
     */    
    MAiFwEventHandler* FwEventHandler() const;
    
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
     * Gets native ui publishers
     * 
     * @since S60 5.0
     * @param aPlugins Array where plugins are appended.
     */
    void PublisherInfoL( RAiPublisherInfoArray& aPlugins );

    /**
     * Gets xml ui publishers
     * 
     * @since S60 5.0
     * @param aSource Xml node which defineds the publisher
     * @param aInfo Publisher info filled based aSource information
     */    
    void PublisherInfoL( CXnNodeAppIf& aSource,
                         TAiPublisherInfo& aInfo );

public: 
    // from CAiUiController

    void LoadUIDefinitionL();
    
    void GetPluginsL( RAiPublisherInfoArray& aPlugins );
                                             
    void GetSettingsL( const TAiPublisherInfo& aPubInfo, 
        RAiSettingsItemArray& aSettings );
    
    void ActivateUI();
    
    MAiContentObserver& GetContentObserver();
    
    void SetEventHandler( MAiFwEventHandler& aFwEventHandler );
    
    void RemovePluginFromUI( MAiPropertyExtension& aPlugin );
            
    MAiMainUiController* MainInterface();
    
    MAiSecondaryUiController* SecondaryInterface();
        
private: 
    // from MAiMainUiController

    void RunApplicationL();
    
    CCoeEnv& CoeEnv();
    
    void SetUiFrameworkObserver( MAiUiFrameworkObserver& aObserver );
        
    void Exit();
    
    TBool IsMenuOpen();
    
private: 
    // from MAiUiEventHandler

    TBool HandleUiEvent(TAny* aEvent, const TDesC8& aParam);
              
private: 
    // Constructors

    CXmlUiController();
    
    void ConstructL();

private: 
    // New methods    
    static CApaApplication* NewApplication();
    
    void HandleFocusGainedL( const TDesC8& aUiElement1, const TDesC8& aUiElement2, CXnNodeAppIf& aOrigin );
    
    void HandleSetElementSizeL( const TDesC8& aElementName, CXnNodeAppIf& aOrigin );
    
    static TInt ExitTimerCallBack(TAny *aSelf);

    void GetSettingsFromCRL( const TAiPublisherInfo& aPubInfo, RAiSettingsItemArray &aPluginSettings );
    
    void GetContentItemL(  CXnNodeAppIf& aNode, RAiSettingsItemArray& aSettings, HBufC*& aItemName );
    
    void GetConfigurationsL( CXnNodeAppIf& aNode, RAiSettingsItemArray& aSettings, const TDesC& aConfOwner );
    
    void GetContentModelL(const TAiPublisherInfo& aPubInfo, RAiSettingsItemArray& aSettings);
                
private: // data
        
    /**
     * Ai content observer. Not own.
     */
    MAiContentObserver* iObserver;
    
    /**
     * Ui framework observer. Not own.
     */
    MAiUiFrameworkObserver* iUiFrameworkObserver;
    
    /**
     * Event handler. Not own.
     */
    MAiFwEventHandler* iFwEventHandler;
    
    /**
     * Node id generator. Own.
     */
    CXmlNodeIdGenerator* iNodeIdGenerator;
    
    /**
     * App UI. Not own.
     */
    CAppUi* iAppUi;
    
    /**
     * Timer to call AppUi's exit. Own.
    **/
    CPeriodic *iExitTimer; 
    
    /**
     * Cenrep. Own.
     */
    CRepository *iAISettingsRepository;

    /**
     * CPS publisher. Own.
     */
    CContentPublisher* iCPSpublisher;
    
    /**
     * Flag to indicate whether running as main UI controller
     */
    TBool iRunningAsMain;
    };    
}  // namespace AiXmlUiController

#endif  // C_XMLUICONTROLLER_H
