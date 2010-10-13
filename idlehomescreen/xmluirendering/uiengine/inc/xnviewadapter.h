/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Xuikon view adapter header file
*
*/

#ifndef XNVIEWADAPTER_H
#define XNVIEWADAPTER_H

// System includes
#include <aknview.h>
#include <babitflags.h>

// User includes
#include "xnuistatelistener.h"

// Forward declarations
class CXnNode;
class CXnViewData;
class CXnViewManager;
class CXnAppUiAdapter;
class CXnControlAdapter;
class CXnKeyEventDispatcher;
class CXnBackgroundManager;
class CXnFocusControl;

// Class declaration
/**
 * Adapts Xuikon view to Avkon view facilities.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnViewAdapter ) : public CAknView,  
    public MXnUiStateObserver
    {    
public:
    /**
     * Two-phase constructor
     */
    static CXnViewAdapter* NewL( CXnAppUiAdapter& aAdapter );

    /**
     * Destructor.
     */
    ~CXnViewAdapter();
    
public:
    // new functions

    /**
     * Flushes view's internals, called when appplication UI is reloaded.
     * 
     * @since S60 5.0
     */
    void ReloadUiL();

    /**
     * Notifies view it will be destroyed
     * 
     * @since S60 5.0
     */    
    void PrepareToExit();

    /**
     * Gets event dispatcher
     * 
     * @since S60 5.0
     * @return Event dispatcher
     */
    CXnKeyEventDispatcher* EventDispatcher() const;
    
    /**
     * Gets background manager
     * 
     * @since S60 5.0
     * return Background manager
     */        
    CXnBackgroundManager& BgManager() const;

    /**
     * Gets focus control
     * 
     * @since S60 5.0
     * return Focus control
     */        
    CXnFocusControl& FocusControl() const;  
	
	 /**
     * updates rsk by UI state
     * 
     * @since S60 5.0
     */  
    void UpdateRskByUiStateL( const CXnViewData& aViewData );

    /**
     * Closes all popups for this container.
     * 
     * @since S60 5.0
     */
    void CloseAllPopupsL();

public:
    // from CAknView

    /**
     * @see CAknView
     */
    TUid Id() const;

private:
    // from CAknView
    
    /**
     * @see CAknView
     */
    void DoActivateL(
        const TVwsViewId &aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8 &aCustomMessage );

    /**
     * @see CAknView
     */
    void DoDeactivate();

private:
    // from MXnUiStateObserver
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyForegroundChanged( 
        MXnUiStateObserver::TForegroundStatus aStatus ); 
            
    /**
     * @see MXnUiStateObserver
     */
    void NotifyLightStatusChanged( TBool aLightsOn );
        
    /**
     * @see MXnUiStateObserver
     */
    void NotifyInCallStateChaged( TBool aInCall );
        
public:
    // new functions

    /**
     * Changes current container's controls state.
     * 
     * @since S60 5.0
     * @param aAwake ETrue if controls should exit from powersavemode, 
     *               EFalse if controls should enter to powersavemode.
     */
    void ChangeControlsStateL( TBool aAwake );
    
    /**
     * Activates new container ro view. 
     * Deactivates internally previous container if it exists.
     * This is always called from DoActivateL or from CXnViewManager when 
     * active view data is changed.
     * 
     * @since S60 5.0
     * @param aContainer Container to activate
     * @param aEnterEditState ETrue if activated container should be set to edit state.
     * @param aEffect Effect uid, default no effect
     */
    void ActivateContainerL( CXnViewData& aContainer, 
        TBool aEnterEditState = EFalse, TUid aEffect = TUid::Null() );
    
private:
    // new functions
    
    /**
     * Deactivates the current container from view.      
     * 
     * @since S60 5.0               
     * @param aHide ETrue if old container is hidden, EFalse otherwise
     */    
    void DeactivateContainerL( TBool aHide = ETrue );

    /**
     * Activates default container to view. 
     * Deactivates internally previous container if it exists.
     * 
     * @since S60 5.0
     * @param aEnterEditState ETrue if default container should be set to edit state.
     */
    void ActivateDefaultContainerL( TBool aEnterEditState = EFalse );

    /**
     * Sets edit state property of container
     * 
     * @since S60 5.0
     * @param aView container
     * @param aEnter ETrue if container should be set to edit state.
     */
    void EnterEditStateL( CXnViewData& aView, TBool aEnter );
    
    /**
     * Callback function to be used with CPeriodic.
     */
    static TInt TimerCallback( TAny *aPtr );
    
private:

    /**
     * C++ Constructor
     **/
    CXnViewAdapter( CXnAppUiAdapter& aAdapter );

    /**
     * Two-phased constructor.
     */
    void ConstructL();

private:
    // Data

    /** AppUi, Not owned */
    CXnAppUiAdapter& iAppUiAdapter;
    /** Background manager, Owned */
    CXnBackgroundManager* iBgManager;
    /** Focus control, Owned */
    CXnFocusControl* iFocusControl;
    /* Event dispatcher, Owned */
    CXnKeyEventDispatcher* iEventDispatcher;
    /* Activate trigger, Owned */
    CXnNode* iActivate;
    /* Deactivate trigger, Owned */
    CXnNode* iDeactivate;
    /** EditState trigger, Owned */
    CXnNode* iEditState;
    /** Active container, Not owned */
    const CXnViewData* iContainer;
    /** Flags */
    TBitFlags32 iFlags;
    /** Timer to set window group order after default view is activated, owned */
    CPeriodic* iTimer;
    /** Action to handle in timer callback */
    TInt iAction;
    };

#endif // XNVIEWADAPTER_H

// End of file
