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
* Description:  Native UI controller.
*
*/


#ifndef C_NATIVEUICONTROLLER_H
#define C_NATIVEUICONTROLLER_H

// System includes
#include <aisystemuids.hrh>

// User includes
#include <aiuicontroller.h>
#include <aicontentobserver.h>
#include <aiutility.h>

// Forward declarations
class MAiPSPropertyObserver;
class CHsContentPublisher;
class THsPublisherInfo;

namespace AiNativeUiController
{
class CAiNativeRenderer;
class CAiStatusPanel;
class CAppUi;

const TInt KImplementationUidNativeUiController = AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE;

const TUid KUidNativeUiController = { KImplementationUidNativeUiController };

/**
* Native UI controller.
* This class works as a secondary UI controller. It implements 
* the MAiContentObserver interface thus handling and forwarding the data coming
* from the publishers.
*
* @since Series 60 3.2
*/
NONSHARABLE_CLASS( CNativeUiController ) : public CAiUiController, 
    public MAiSecondaryUiController, public MAiMainUiController,                                                         
    public MAiContentObserver
	{
public:
    // Constructor and destructor
    static CNativeUiController* NewL();

	~CNativeUiController();

public:	
    // from CAiUiController
   
    void LoadUIDefinitionL();
    
    void GetSettingsL( const THsPublisherInfo& aPublisherInfo, 
        RAiSettingsItemArray& aSettings );

    void ActivateUI();

    MAiContentObserver& GetContentObserver();

    void SetEventHandler( MAiFwEventHandler& aEventHandler );
    
    void SetStateHandler( MAiFwStateHandler& aStateHandler );

    MAiFwEventHandler* FwEventHandler();

    MAiFwStateHandler* FwStateHandler();
       
    MAiMainUiController* MainInterface();

    MAiSecondaryUiController* SecondaryInterface();
            
public:    
    // from MAiMainUiController

    void RunApplicationL();

    CCoeEnv& CoeEnv();
    
    TBool IsMenuOpen();

public:
    // from CAiSecordaryUiController

    void SetCoeEnv( CCoeEnv& aCoeEnv );

public:     
    // from MAiContentObserver       
    TInt StartTransaction(TInt aTxId);

    TInt Commit(TInt aTxId);

    TInt CancelTransaction(TInt aTxId);

    TBool CanPublish(CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex);

    TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex );

    TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );

    TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );

    TInt Clean(CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );

    TAny* Extension(TUid aUid);

    TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const;
    
    TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue );
    
    TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue,  
            MAiContentObserver::TValueType aValueType);

private:
    // constructors
    
    /**
     * C++ default contructor
     */
    CNativeUiController();

public:    
    // new functions
       
    /**
     * Set app ui instance to this class.
     *
     * @since S60 3.2
     * @param aAppUi is pointer to app ui.
     */
    void SetAppUi( CAppUi* aAppUi );

    /**
     * Promotes this UI controller as main UI controller.
     *
     * @since S60 3.2     
     */    
    void VariateToMainUiController();

    /**
     * Runs exit timer.
     *
     * @since S60 3.2     
     */        
    void Exit();

    /**
     * Preperas to exit
     * 
     * @since S60 3.2  
     */
    void PrepareToExit();
    
private:
    // new functions
    
    /**
     * Add renderer.
     *
     * Ownership transferred.
     *
     * @since S60 3.2
     * @param aRenderer is pointer to renderer.
     */
    void AddRendererL( CAiNativeRenderer* aRenderer );

    /**
     * Remove a renderer. Renderer is only
     * removed from the array and is deleted ONLY
     * when aDelete = ETrue
     *
     * @since S60 5.0
     * @param aRenderer is pointer to the renderer to be removed
     * @param aDelete is ETrue when the renderer will be deleted
     * @return ETrue upon successful removing.
     */
    TBool RemoveRenderer( CAiNativeRenderer *aRenderer, TBool aDelete = EFalse );
    
    /**
     * Template function for publish.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is content id
     * @param aData is published data
     * @param aIndex is index in the control, not used by Native UI Controller.
     * @return KErrNone if publishing was successful.
     */
	template<class T>
    TInt DoPublish( CHsContentPublisher& aPlugin, TInt aContent, T& aData, TInt aIndex );
    
    /**
     * Handles idle state changes.
     *
     * @since S60 v3.2
     */
    static TInt HandleIdleStateEvent( TAny* aPtr );
    
    /**
     * Handles keylock state changes.
     *
     * @since S60 v3.2
     */
    static TInt HandleKeylockStateEvent( TAny* aPtr );

    static TInt HandlePluginConfChange( TAny* aPtr );
    
    static TInt ExitTimerCallBack( TAny* aSelf );
    void GetSettingsFromCRL( const THsPublisherInfo& aPublisherInfo,
           RAiSettingsItemArray &aPluginSettings );
    
    static CApaApplication* NewApplication();
    
private:     
    // data

    /**
     * Array of renderers.
     * Own.
     */
    RPointerArray<CAiNativeRenderer> iRenderers;

    /**
     * Offset of resource file.
     */
    TInt iResourceOffset;

    /**
     * Status panel
     * Own.
     */
    CAiStatusPanel* iStatusPanel;
    
    /**
     * Reference to available CONE environment object. Not own.
     */
    CCoeEnv* iCoeEnv;
    
    /**
     * Idle state PS observer.
     * Own.
     */
    MAiPSPropertyObserver* iIdleStatusObserver;
    
    /**
     * Keylock state PS observer.
     * Own.
     */
    MAiPSPropertyObserver* iKeylockStatusObserver;
    

    /**
     * ExtHS plugin configuration PS observer.
     * Own.
     */
    MAiPSPropertyObserver* iExtHsPluginConfChange;

    /**
     * Native UI controller plug-ins
     */
    RArray< THsPublisherInfo > iPlugins;
    
    /**
      * Plug-in event handler.
      * Doesn't own.
      */
    MAiFwEventHandler* iFwEventHandler;

    /**
      * Plug-in state handler.
      * Doesn't own.
      */    
    MAiFwStateHandler* iFwStateHandler;
    
    /**
     * Pointer to this, when role is main ui controller.
     */
    MAiMainUiController* iMain;

    /**
     * Pointer to app ui.
     * Not own.
     */
    CAppUi* iAppUi;
    
    /**
     * Indicates if Native UI Controller takes responsibilities of 
     * Main UI Controller.
     */
    TBool iRunningAsMain;
    
    /**
    * Timer to call AppUi's exit
    **/
    CPeriodic *iExitTimer;
    
    /**
    * To prevent double loading of the UI
    **/
    TBool iUiLoaded;
    };

#include "nativeuicontroller.inl"

} // namespace AiNativeUiController

#endif

// End of File.
