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


#include <aisystemuids.hrh>
#include "aiuicontroller.h"
#include "aiuiframeworkobserver.h"
#include "aicontentobserver.h"

#include <aiutility.h>

class MAiPropertyExtension;
class MAiPSPropertyObserver;

namespace AiNativeUiController
{

class CAiNativeRenderer;
class CAiStatusPanel;
class CAppUi;
class CAiToolbarRenderer;

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
class CNativeUiController : public CAiUiController, 
                            public MAiSecondaryUiController, 
                            public MAiMainUiController, 
                            public MAiUiFrameworkObserver,
                            public MAiContentObserver
	{
public:

    static CNativeUiController* NewL();

	virtual ~CNativeUiController();

// from base class CAiUiController

    void PrepareToExit();

    void LoadUIDefinitionL();

    void GetPluginsL(RAiPublisherInfoArray& aPlugins);

    void GetSettingsL(const TAiPublisherInfo& aPubInfo, 
                        RAiSettingsItemArray& aSettings);

    void ActivateUI();

    MAiContentObserver& GetContentObserver();

    void SetEventHandler(MAiFwEventHandler& aEventHandler);

    MAiFwEventHandler* FwEventHandler();

    void RemovePluginFromUI( MAiPropertyExtension& aPlugin );

    MAiMainUiController* MainInterface();

    MAiSecondaryUiController* SecondaryInterface();
    
    void HandleLoadedPlugins(const RAiPublisherInfoArray& /*aRequiredPlugins*/ ) {};
    
// from base class MAiMainUiController

    void RunApplicationL();

    CCoeEnv& CoeEnv();

    void SetUiFrameworkObserver( MAiUiFrameworkObserver& aObserver );
    
    TBool IsMenuOpen();

// from base class CAiSecordaryUiController

    void SetCoeEnv( CCoeEnv& aCoeEnv );

    MAiUiFrameworkObserver* UiFrameworkObserver(); 

// from base class MAiUiFrameworkObserver

    void HandleResourceChange( TInt aType );

    void HandleForegroundEvent( TBool aForeground );

// from base class MAiContentObserver
       
    TInt StartTransaction(TInt aTxId);

    TInt Commit(TInt aTxId);

    TInt CancelTransaction(TInt aTxId);

    TBool CanPublish(MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex);

    TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex );

    TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );

    TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );

    TInt Clean(MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex );

    TAny* Extension(TUid aUid);

    TBool RequiresSubscription( const TAiPublisherInfo& aPublisherInfo ) const;
    
    TInt SetProperty( MAiPropertyExtension& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue );
    
    TInt SetProperty( MAiPropertyExtension& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue,  
            MAiContentObserver::TValueType aValueType);

// new methods

    /**
     * Set app ui instance to this class.
     *
     * @since S60 3.2
     * @param aAppUi is pointer to app ui.
     */
    void SetAppUi( CAppUi* aAppUi )
        {
        iAppUi = aAppUi;
        }
        
    void VariateToMainUiController();

    void Exit();

private:

    CNativeUiController();

    static CApaApplication* NewApplication();

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
     * Recreates the toolbar renderer
     */
    TBool RecreateToolbarRendererL();

    /**
     * Deletes and removes the toolbar renderer from
     * renderer array.
     */
    void DeleteToolbarRenderer();
    
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
    TInt DoPublish( MAiPropertyExtension& aPlugin, TInt aContent, T& aData, TInt aIndex );
    
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
    void GetSettingsFromCRL( const TAiPublisherInfo& aPubInfo,
                                                  RAiSettingsItemArray &aPluginSettings );

private:     // Data

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
    RAiPublisherInfoArray iPlugins;
    
    /**
      * Plug-in event handler.
      * Doesn't own.
      */
    MAiFwEventHandler* iFwEventHandler;
    
    /**
     * Pointer to this, when role is main ui controller.
     */
    MAiMainUiController* iMain;

    /**
     * Ui framework observer. Not own.
     */
    MAiUiFrameworkObserver* iUiFrameworkObserver;
    
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
    
    /**
     * Pointer to our toolbar renderer. Do not delete
     * renderer through this pointer
     */
    CAiToolbarRenderer *iToolbarRenderer;

    };

#include "nativeuicontroller.inl"

} // namespace AiNativeUiController

#endif

// End of File.
