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

#include <coeview.h>
#include "xnappuiadapter.h"


class CAiUiIdleIntegration;
class MAiUiFrameworkObserver;

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
    
    void HandleForegroundEventL( TBool aForeground );
    
    void HandleCommandL( TInt aCommand );
    
    void HandleResourceChangeL( TInt aType );

    void HandleXuikonEventL( CXnNodeAppIf& aOrigin, CXnNodeAppIf& aTrigger,  
         CXnDomNode& aTriggerDefinition, CXnDomNode& aEvent ); 
                                                          
    void HandleWsEventL( const TWsEvent& aEvent, 
            CCoeControl* aDestination );

    void LoadDataPluginsL( RPointerArray< CXnNodeAppIf >& aList );
          
    void DestroyDataPluginsL( RPointerArray< CXnNodeAppIf >& aList );
           
    void SetOnlineStateL( RPointerArray< CXnNodeAppIf >& aList );
    
    TBool DynInitMenuItemL( const TDesC& aItemType, 
        RPointerArray< CXnNodeAppIf >* aList = NULL );

    void HandlePageSwitch();
    
    void HandleEnterEditModeL( TBool aEnter );
	
private:
    // new functions
    void LoadNativeDataPluginsL();
    void DestroyNativeDataPluginsL();
    
private:
    // from MCoeViewActivationObserver
    
    /**
     * @see MCoeViewActivationObserver
     */
    void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId, 
        const TVwsViewId& aViewIdToBeDeactivated );
           
public: 
    // new functions
    
    /**
     * Sets plugins online state
     * 
     * @since S60 5.0
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
               
private: 
    // Constructors

    CAppUi( CXmlUiController& aUiCtl, TUid aAppUid );
    
    void ConstructL();
          
private:     
    // data

    /**
     * UI Controller implementation. Not own.
     */
    CXmlUiController& iUiCtl;

    /**
     * Event handler. Own.
     */
    CAIXuikonEventHandler* iEventHandler;
    
    /**
     * Content renderer. Own.
     */
    CContentRenderer* iContentRenderer;
    
    /**
     * Ui framework event observer. Not own.
     */
    MAiUiFrameworkObserver* iUiFwObserver;
    
    /**
     * Idle Integration helper object. Own.
     */
    CAiUiIdleIntegration* iIdleIntegration;

    /**
     * Online/Offline helper object. Own.
     */
    COnlineOfflineHelper* iHelper;    
    
    /**
     * Flag to indicate whether native plugins are loaded.
     */
    TBool iNativePluginsLoaded;
    
    /**
     * Flag to indicate whether edit mode is active.
     */
    TBool iIsEditModeActive;
    };
    
}  // namespace AiXmlUiController
    
#endif  // C_APPUI_H

// End of File.
