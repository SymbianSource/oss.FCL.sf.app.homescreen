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
* Description:
* Xuikon UI state controller
*
*/

#ifndef XNUISTATELISTENER_H
#define XNUISTATELISTENER_H

// System includes
#include <e32base.h>
#include <hwrmlight.h>
#include <eikspane.h>

// User includes
#include "xnpropertysubscriber.h"

// Forward declarations
class CXnAppUiAdapter;

// Constants

// Class declaration
/**
 * UI resource change observer interface
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( MXnUiResourceChangeObserver )
    {
public:
    /**
     * Notifies statuspane is changed.
     * 
     * @since S60 5.0
     */
    virtual void NotifyStatusPaneSizeChanged() = 0;

    /**
     * Notifies resource changes
     * 
     * @since S60 5.0
     * @param aType Resource type
     */
    virtual void NotifyResourceChanged( TInt aType ) = 0;    
        
    /**
     * Notifies HW flip/slide status changes
     * 
     * @since S60 5.0
     * @param aStatus flip/slide status     
     */
    /* virtual void NotifyFlipStatusChangedL( TInt aStatus ) = 0;*/
    };

// Class declaration
/**
 * UI state observer interface
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( MXnUiStateObserver )
    {
public:
    // new types
    enum TForegroundStatus
        {
        EUnknown,
        EBackground,
        EForeground,
        EPartialForeground,        
        };
public:
    /**
     * Notifies foreground changes
     *
     * @since S60 5.0    
     * @param aState Foreground state
     */
    virtual void NotifyForegroundChanged( 
            TForegroundStatus aStatus ) = 0;
    
    /**
     * Notifies primary display light status is changed.
     * 
     * @since S60 5.0
     * @param aLightsOn ETrue if lights on, EFalse otherwise 
     */
    virtual void NotifyLightStatusChanged( TBool aLightsOn ) = 0;
        
    /**
     * Notifies in-call state is changed.
     * 
     * @since S60 5.0
     * @param aInCall ETrue if in-call on, EFalse otherwise
     */
    virtual void NotifyInCallStateChaged( TBool aInCall ) = 0;    
    };

/**
 * UI state listener
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnUiStateListener ) : public CBase,    
    public MCoeMessageMonitorObserver,
    public MEikStatusPaneObserver,
    public MHWRMLightObserver,
    public MXnPropertyChangeObserver    
    {    
private:
    // friend classes
    friend class CXnAppUiAdapterImpl;
    
public:
    /**
     * Two-phased constructor.
     */
    static CXnUiStateListener* NewL( CXnAppUiAdapter& aAdapter );
    static CXnUiStateListener* NewLC( CXnAppUiAdapter& aAdapter );

    /**
     * Destructor.
     */
    ~CXnUiStateListener();

private:
    /**
     * 2nd phase construtor
     */    
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CXnUiStateListener( CXnAppUiAdapter& aAdapter );

public:
    // New functions
    
    /**
     * Prepares for application exit
     * 
     * @since S60 5.0
     */
    void PrepareToExit();
    
    /*
     * Adds MXnUiStateObserver
     * 
     * @since S60 5.0
     * @param aObserver Observer to add 
     */
    void AddObserver( const MXnUiStateObserver& aObserver );
    
    /*
     * Removes MXnUiStateObserver
     * 
     * @since S60 5.0
     * @param aObserver Observer to remove 
     */    
    void RemoveObserver( const MXnUiStateObserver& aObserver );

    /*
     * Adds MXnUiResourceChangeObserver
     * 
     * @since S60 5.0
     * @param aObserver Observer to add 
     */
    void AddObserver( const MXnUiResourceChangeObserver& aObserver );
    
    /*
     * Removes MXnUiResourceChangeObserver
     * 
     * @since S60 5.0
     * @param aObserver Observer to remove 
     */    
    void RemoveObserver( const MXnUiResourceChangeObserver& aObserver );
    
public:
    void SetEditModeActive( TBool aActive );
    
private:
    // New functions
    void HandleResourceChangeL( TInt aType );
    
private:
    // from MCoeMessageMonitorObserver
    void MonitorWsMessage( const TWsEvent& aEvent );

private:
    // from MEikStatusPaneObserver
    void HandleStatusPaneSizeChange();

private:
    // from MHWRMLightObserver
    void LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus ); 
                                        
private:    
    // from MXnPropertyChangeObserver    
    void PropertyChangedL( const TUint32 aKey, const TInt aValue );

private:
    // Data    

    // AppUi, not owned */
    CXnAppUiAdapter& iAppUiAdapter;    
    /** Light status observer, Owned */
    CHWRMLight* iLightObserver;    
    /** Publish&Subscribe observer for call state, Owned */
    CXnPropertySubscriber* iCallStateObserver;       
    /** MXnUiStateObservers, Not owned */
    RPointerArray< MXnUiStateObserver > iUiStateObservers;
    /** MXnUiResourceChangeObservers, Not owned */
    RPointerArray< MXnUiResourceChangeObserver > iResourceChangeObservers;
    /** Foreground status */
    MXnUiStateObserver::TForegroundStatus iForegroundStatus;
    /** Light status */
    CHWRMLight::TLightStatus iLightStatus;
    /** In-call state */
    TBool iInCallState;
    /** Last received foreground message */
    TInt iForegroundType;
    };

#endif // XNUISTATELISTENER_H

// End of file

