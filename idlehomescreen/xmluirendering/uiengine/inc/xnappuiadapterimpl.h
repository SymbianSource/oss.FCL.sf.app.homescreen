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
* Description:  Adapter for Avkon for Xuikon
*
*/


#ifndef XNAPPUIADAPTERIMPL_H
#define XNAPPUIADAPTERIMPL_H

// System includes
#include <e32base.h>
#include <AknProgressDialog.h> // for MProgressDialogCallback
#include "xnpropertysubscriber.h" // for CXnPropertySubscriber

// Forward declarations
class CXnUiEngine;
class CXnAppUiAdapter;
class CXnViewAdapter;
class CXnViewManager;
class CXnUiStateListener;
class CHsContentControlFactory;
class MHsContentControlUi;
class CHsCcProviderClient;
class CXnEffectManager;
class MHsContentControl;
class CXnWaitDialog;
class CXnItemActivator;

// Constants

// Class declaration

/**
*  Avkon Adapter for Xuikon
*  
*  @ingroup group_xnlayoutengine
*  @lib xn3layoutengine.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CXnAppUiAdapterImpl ) : public CBase,
    public MXnPropertyChangeObserver, public MProgressDialogCallback                                               
    {
public:
    // Constructors and destructor

    /**
     * Second-phase constructor
     * 
     * @since Series 60 3.1
     * @param aApplicationUid App Id 
     * @param aAdapter An AppUi adapter
     */
    static CXnAppUiAdapterImpl* NewL( TUid aApplicationUid,
        CXnAppUiAdapter& aAdapter );

    /**
     * Second-phase constructor
     * 
     * @since S60 5.0
     */
    void ConstructL();
    
    /**
     * Destructor.
     */
    ~CXnAppUiAdapterImpl();

public:
    // New functions
        
    /**
     * Get the UI engine
     * 
     * @since Series 60 3.1
     * @return UI engine
     */
    CXnUiEngine& UiEngine() const;

    /**
     * Get the View manager
     * 
     * @since S60 5.0
     * @return ViewManager
     */
    CXnViewManager& ViewManager() const;

    /**
     * Get the View adapter
     * 
     * @since S60 5.0
     * @return ViewAdapter
     */
    CXnViewAdapter& ViewAdapter() const;
    
    /**
     * Get the UI state listener
     * 
     * @since S60 5.0
     * @return UiStateListener
     */
    CXnUiStateListener& UiStateListener() const;
    
    /**
     * Gets the Content control UI interface
	 *
	 * @since S60 5.0
	 * @param aType Content control UI type
     * @return Content control UI interface, NULL if not present
     */
    MHsContentControlUi* HsContentController( const TDesC8& aType ) const;

    /**
     * Gets the Content control factory
     *
     * @since S60 5.0
     * @return Content control factory
     */
    CHsContentControlFactory* HsContentControlFactory();
    
    /**
     * Get the effect manager
     * 
     * @since S60 5.0
     * @return Effect manager
     */
    CXnEffectManager* EffectManager() const;

    /**
     * Gets the Content control server interface
     *
     * @since S60 5.0
     * @return Content control server interface, NULL if not present
     */
    MHsContentControl* HsContentControlSrv() const;

    /**
     * Gets the Item activator
     *
     * @since S60 5.0
     * @return Item activator
     */    
    CXnItemActivator& ItemActivator() const;
    
    /**
     * Reload the UI after application root configuration is changed
     * 
     * @since S60 5.0     
     */
    void ReloadUiL();

    /**
     * Handles a change to the control's resources of type aType
     * 
     * @since Series 60 3.1
     * @param aType Changed resource type
     */
    void HandleResourceChangeL( TInt aType );
    
public: // From MProgressDialogCallback

    /**
     * Callback method from MProgressDialogCallback interface.
     * Gets called when a dialog is dismissed.
     * @param aButtonId Id of the pushed button.
     */
    void DialogDismissedL( TInt aButtonId );

private:
    
    /**
     * C++ default constructor.
     */
    CXnAppUiAdapterImpl( TUid aApplicationUid,    
            CXnAppUiAdapter& aAdapter );

private:    
    // from MXnPropertyChangeObserver    
    void PropertyChangedL( const TUint32 aKey, const TInt aValue );

private:    
    // new functions
    
    /**
     * Launches wait dialog during backup/restore.
     */
    void DisplayWaitDialogL();
    
private:
    // Data
    
    /** AppUi, Not owned. */
    CXnAppUiAdapter& iAdapter;    
    /** Application uid */
    TUid iApplicationUid;
    /** UiEngine, Owned */
    CXnUiEngine* iUiEngine;
    /** View Adapter, Owned */
    CXnViewAdapter* iViewAdapter;
    /** View Manager, Owned */
    CXnViewManager* iViewManager;
    /** UI State listener, Owned */
    CXnUiStateListener* iUiStateListener;
    /** Content control factory, Owned */
    CHsContentControlFactory* iContentControlFactory;
    /** Content control provider client, Owned */
    CHsCcProviderClient* iCcProviderClient;
    /** Effect manager, Owned */
    CXnEffectManager* iEffectManager;
    /** Backup restore wait dialog, Owned */
    CXnWaitDialog* iBURWaitDialog;
    /** Publish&Subscribe observer for Backup & Restore, Owned */
    CXnPropertySubscriber* iBackupRestoreObserver;  
    /** Item activator, Owned */
    CXnItemActivator* iActivator;
    /** Resource offset */
    TInt iResourceOffset;
    };

#endif // XNAPPUIADAPTERIMPL_H

// End of file