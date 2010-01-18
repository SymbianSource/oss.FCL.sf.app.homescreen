/*
 * ============================================================================
 *  Name        : hgteleportappui.h
 *  Part of     : Hg Teleport
 *  Description : appui
 *  Version     : %version: sa1spcx1#26 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 *
 */

#ifndef HGTELEPORTAPPUI_H
#define HGTELEPORTAPPUI_H

#include <e32base.h> 
#include <aknappui.h>

#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>

#include "hgproplistener.h"
#include "hgcenreplistener.h"

// FORWARD DECLARATIONS
class CHgTeleportAppView;
class CHgActionUtility;
class CHgTeleportDeviceState;

/**
 * CHgTeleportAppUi
 * Appui for hg teleport app
 */
class CHgTeleportAppUi : public CAknAppUi, 
                         public MHgPropertyChangeObserver,
                         public MHgCenrepChangeObserver,
                         public MGfxTransEffectObserver
    {

public:
    /**
     * Perform the second phase construction of a CHgTeleportAppUi object
     * this needs to be public due to the way the framework constructs the AppUi
     */
    void ConstructL();

    /**
     * Perform the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the AppUi
     */
    CHgTeleportAppUi();

    /**
     * Destroy the object and release all memory objects
     */
    ~CHgTeleportAppUi();

public: //effects
    /**
     * @return value of theme effects enabled flag
     */
    TBool EffectsEnabled() const;
    
    /**
     * Function start transition  
     */
    void StartTransion( TUint aTransitionType );
    
    /**
     * from MGfxTransEffectObserver
     */
    void TransitionFinished(const CCoeControl* aControl, TUint aAction);
    
    void MoveAppToForeground( TUint aTransitionType );
    void MoveAppToBackground( TUint aTransitionType );
    
public:
    /**
     * From MHgCenrepChangeObserver
     */
    void CenrepChanged( TUint32 aKey, TInt aNewValue );
    

private:

    // From MHgPropertyChangeObserver
    void PropertyChanged( TUid aCategory, TUint aKey );

    // From CEikAppUi
    void HandleResourceChangeL( TInt aType );
    void HandleForegroundEventL( TBool aForeground );
    void HandleCommandL( TInt aCommand );
    
    void StartTransition( TUint aTranstionId,
                          TBool aVisibility,
                          TBool aLayers, 
                          TUint aSubCom );

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
     * Sets P&S property which is used to monitor and set Teleport to 
     * background/foreground and short/long app key press.
     * 
     * @param aValue Property value. 
     */
    void SetTeleportStateProperty( TInt aValue );
    
    /**
     * Sends request for close some 
     * other apps to free memory for 
     * teleport app
     */
    void FreeMemoryRequest();

    static TInt GoToBackgroundTimerCallback( TAny* aParam );

private:

    // Teleport application view, owned
    CHgTeleportAppView* iAppView;
    
    // Listener for app key long press P&S, owned
    CHgPropertyListener* iPropListener;
    
    // Tells if Teleport is in foreground or in background
    TBool iForeground;
    
    // Application arch task which refers to our own app
    TApaTask iApplicationTask;

    // Device state
    CHgTeleportDeviceState* iDeviceState;

    // Buffer to keeping some memory allocation while being in background.    
    TUint8* iMemAllocBuf;

    CPeriodic* iGoToBackgroundTimer;
    
    TBool iEffectsEnabled;
    
    /**
     * Cenrep listener for the tfxserver enabled/disabled key.
     */    
    CHgCenrepListener* iThemeEffectsEnabledWatcher;
    };

#endif // HGTELEPORTAPPUI_H
