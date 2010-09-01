/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_APPUI_H
#define C_APPUI_H

#include <aknappui.h> 
#include <cenrepnotifyhandler.h>

class CAiUiIdleIntegration;
class MAiUiFrameworkObserver;
class MAiFwEventHandler;
class CExtrStandbyContainerIF;
class CCenRepNotifyHandler;
class CRepository;
class RConeResourceLoader;
class MAiPSPropertyObserver;

namespace AiNativeUiController
{
        
class CNativeUiController;
class CAiNativeUiView;

/**
 *  @ingroup group_nativeuicontroller
 * 
 *  AppUI class of Native UI Controller application
 *
 *  @lib ainatiui
 *  @since S60 3.2
 */
class CAppUi : public CAknAppUi,
               public MCenRepNotifyHandlerCallback
    {
public:   // Constructors and destructor

    static CAppUi* NewL(CNativeUiController* aUiCtl);

    ~CAppUi();
    
// new functions

    CCoeEnv* CoeEnv() { return iCoeEnv; }
            
    void StartL();

// from CCoeAppUi

    TKeyResponse HandleKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );

// From MCenRepNotifyHandlerCallback

    void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    
    void ExtHSThemeChanged();

private:
    
// Functions from CAknAppUi

    void HandleForegroundEventL(TBool aForeground);
    
    void HandleCommandL(TInt aCommand);
    
    void HandleResourceChangeL(TInt aType);

    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    
    void HandleScreenDeviceChangedL();

    void PrepareToExit();

    void SkipVoiceDial();
    
    static TInt TimerDone(TAny* aSelf);

    /**
     * Handles powerkey events.
     *
     * @since S60 v3.2
     */
    static TInt HandlePowerkeyEvent( TAny* aPtr );

// Construction

    CAppUi(CNativeUiController* aUiCtl);
    
    void ConstructL();
    
    void HandlePointerEvent( TPointerEvent &aEvent );    
    
private:     // Data


    /**
     * Framework event handler.
     */
    MAiFwEventHandler* iFwEventHandler;
    
    /**
     * UI Controller implementation. Not own.
     */
    CNativeUiController* iUiCtl;

    /**
     * Ui framework event observer. Not own.
     */
    MAiUiFrameworkObserver* iUiFwObserver;
    
    /**
     * Idle Integration helper object. Own.
     */
    CAiUiIdleIntegration* iIdleIntegration;
    
    /**
     * Main view.
     * Own.
     */
    CAiNativeUiView* iAppView;

    /**
     * Homescreen IF. Own.
     */    
    CExtrStandbyContainerIF* iStanbyContainerIF;

    /**
     * Notify handler for cenrep.
     * Own.
     */
    CCenRepNotifyHandler* iNotifyHandler;		
    
    /**
     * Repository for notify handler.
     * Own.
     */
    CRepository* iAIRepository;
    
    /**
     * Last set status pane layout.
     */
    TInt iCurrentStatusPaneLayout;
    
    /**
     * Timer to handle long key press.
     */
    CPeriodic* iTimer;
    
    /**
     * Voice dial key down indicator.
     */
    TBool iSINDKeyDown;

    /**
     * Resource file loader for ainativeui resource.
     */
    RConeResourceLoader*                iResourceLoader;

    /**
     * Flag to indicate that device has no power key.
     */
    TBool iFeatureNoPowerKey;
    
    /**
     * Power key PS observer. Only used if no power key exists.
     * Own.
     */
    MAiPSPropertyObserver* iPowerkeyStatusObserver;
    
    };
    
}  // namespace AiNativeUiController
    
#endif  // C_APPUI_H

// End of File.
