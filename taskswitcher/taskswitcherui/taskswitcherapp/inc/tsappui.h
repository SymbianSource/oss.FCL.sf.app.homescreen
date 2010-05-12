/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  appui
 *
*/


#ifndef TSAPPUI_H
#define TSAPPUI_H

#include <e32base.h> 
#include <aknappui.h>

#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>

#include "tsproplistener.h"
#include "tscenreplistener.h"

// FORWARD DECLARATIONS
class CTsAppView;
class CTsActionUtility;
class CTsDeviceState;

/**
 * CTsAppUi
 * Appui for tsak swicher app
 */
class CTsAppUi : public CAknAppUi, 
                         public MTsPropertyChangeObserver,
                         public MTsCenrepChangeObserver,
                         public MGfxTransEffectObserver
    {

public:
    enum {
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //TODO make maping to NGA transition id
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ENoneTransition = 0,
    EForegroundTransition = 3,
    EBackgroundTransition = 4,
    EActivationTransition = 5,
    EActivationAppShowTransition = 1507,
    
    };
    /**
     * Perform the second phase construction of a CTsAppUi object
     * this needs to be public due to the way the framework constructs the AppUi
     */
    void ConstructL();

    /**
     * Perform the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the AppUi
     */
    CTsAppUi();

    /**
     * Destroy the object and release all memory objects
     */
    ~CTsAppUi();

public: //effects
    /**
     * @return value of theme effects enabled flag
     */
    TBool EffectsEnabled() const;
    
    /**
     * Function start transition  
     */
    void StartTransion( TUint aTransitionType,
            TUid aNextAppUid = TUid::Null(),
            TInt aWgId = 0 );
    
    /**
     * from MGfxTransEffectObserver
     */
    void TransitionFinished(const CCoeControl* aControl, TUint aAction);
    
    void MoveAppToForeground( TUint aTransitionType );
    void MoveAppToBackground( TUint aTransitionType,
            TUid aAppUid = TUid::Null(),
            TInt aWgId = 0 );
    
    /**
     * Functions for showing/hiding popups.
     */
    void RequestPopUpL();
    void DisablePopUpL();
    
    /**
     * Function for layout change check
     */
    TBool LayoutChangeAllowed();
    
public:
    /**
     * From MTsCenrepChangeObserver
     */
    void CenrepChanged( TUint32 aKey, TInt aNewValue );
    

private:

    // From MTsPropertyChangeObserver
    void PropertyChanged( TUid aCategory, TUint aKey );

    // From CEikAppUi
    void HandleResourceChangeL( TInt aType );
    void HandleForegroundEventL( TBool aForeground );
    void HandleCommandL( TInt aCommand );
    void HandleWsEventL(const TWsEvent& aEvent,
            CCoeControl* aDestination);
    
    void StartTransition( TUint aTranstionId,
                          TBool aVisibility,
                          TBool aLayers, 
                          TUint aSubCom );
    
    void StartAppActivateTransition( TUid aNextAppUid, TInt aWgId );

private: // New functions
    
    /**
     * Called from HandleForegroundEventL if the app came to foreground.
     */
    void HandleSwitchToForegroundEvent();
    
    /**
     * Called from HandleForegroundEventL if the app went to background.
     */
    void HandleSwitchToBackgroundEvent();

    /**
     * Sets P&S property which is used to monitor and set Taskswitcher to 
     * background/foreground and short/long app key press.
     * 
     * @param aValue Property value. 
     */
    void SetTaskswitcherStateProperty( TInt aValue );
    
    /**
     * Sends request for close some 
     * other apps to free memory for 
     * task switcher app
     */
    void FreeMemoryRequest();
    
    /**
     * Returns id of window group lying beneath task
     * switcher window group. 
     * In case of embeded app it returns top paren window group.
     * 
     * @param  aIgnoreParentChild  if set to ETrue, parent/child relationship
     *                             will be ignored when deteriminig window
     *                             group id
     */
    TInt WgIdOfUnderlyingApp( TBool aIgnoreParentChild );

    static TInt GoToBackgroundTimerCallback( TAny* aParam );
    
    /*
     * Returns id of top parent window group for embeded window group.
     * @param aChildWg embeded window group id
     * @return top parent window group id or 0 in case od standlalone 
     */
    TInt GetTopParentWg( TInt aChildWg);

    /*
     * Returns id of parent window group for embeded window group.
     * @param aChildWg embeded window group id
     * @return parent window group id or 0 in case od standlalone 
     */
    TInt GetParentWg( TInt aChildWg );

private:

    // Taskswitcher application view, owned
    CTsAppView* iAppView;
    
    // Listener for app key long press P&S, owned
    CTsPropertyListener* iPropListener;
    
    // Listener for orientation layout changes
    CTsPropertyListener* iLayoutListener;
    
    // Tells if Taskswitcher is in foreground or in background
    TBool iForeground;
    
    // Application arch task which refers to our own app
    TApaTask iApplicationTask;

    // Device state
    CTsDeviceState* iDeviceState;

    CPeriodic* iGoToBackgroundTimer;
    
    TBool iEffectsEnabled;
    
    /**
     * Cenrep listener for the tfxserver enabled/disabled key.
     */    
    CTsCenrepListener* iThemeEffectsEnabledWatcher;
    
    // Owned window for showing controls
    RWindowGroup iWg;
    
    // Monitors popup mode
    TBool iIsPopUpShown;
    
    // For starting in background
    TBool iUiStarted;
    TBool iDisableAppKeyHandling;
    
    TInt iUnderAppWgId;
    
    };

#endif // TSAPPUI_H
