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
* Avkon adapter for Xuikon
*
*/

#ifndef XNAPPUIADAPTER_H
#define XNAPPUIADAPTER_H

// System includes
#include <aknViewAppUi.h>

// Forward declarations
class TXnUiEngineAppIf;
class CXnNodeAppIf;
class CXnDomNode;
class CXnAppUiAdapterImpl;
class CXnUiEngine;
class CXnUiStateListener;
class CXnViewManager;
class CXnViewAdapter;
class MHsContentControlUi;
class CXnEffectManager;
class MHsContentControl;
// Class declaration
/**
 * Avkon adapter for Xuikon.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
class CXnAppUiAdapter : public CAknViewAppUi
    {
public:
    /**
     * C++ default constructor.
     *
     * @param aApplicationUid Application UID
     */
    IMPORT_C CXnAppUiAdapter( TUid aApplicationUid );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnAppUiAdapter();

public:
    // New functions

    /**
     * Removes and deregisters view from AppUi
     *
     * @since S60 5.2
     * @param aView view to deregister
     */
    IMPORT_C void RemoveViewL( CAknView& aView );

    /**
     * Get the UI engine
     *
     * @since Series 60 3.1
     * @return UI engine
     */
    IMPORT_C TXnUiEngineAppIf* UiEngineL();

    /**
     * Handle interaction notification from the engine
     *
     * @since Series 60 3.1
     * @param aOrigin Node that originated the event
     * @param aTrigger Trigger information of the event, as created by event
     *        originator
     * @param aTriggerDefinition Trigger information as it exists in the action
     *        definition in xml
     * @param aEvent Event information of the event
     */
    IMPORT_C virtual void HandleXuikonEventL(
        CXnNodeAppIf& aOrigin,
        CXnNodeAppIf& aTrigger,
        CXnDomNode& aTriggerDefinition,
        CXnDomNode& aEvent );

    /**
     * Handles data plugin loading.
     * To be overriden by subclass.     
     *
     * @since S60 5.0
     * @param aPublisher Publisher to load
     * @param aReason Load reason
     * @return KErrNone if succesful, system wide error code otherwise     
     */
    IMPORT_C virtual TInt LoadPublisher(
        CXnNodeAppIf& aPublisher, TInt aReason );

    /**
     * Handles data plugin destroying.
     * To be overriden by subclass.     
     *
     * @since S60 5.0
     * @param aPublisher Publisher to destroy     
     * @param aReason Destroy reason
     * @return KErrNone if succesful, system wide error code otherwise
     */
    IMPORT_C virtual TInt DestroyPublisher(
        CXnNodeAppIf& aPublisher, TInt aReason );

    /**
     * Handles dynamic menuitem element initialisation.
     * To be overriden by subclass.     
     *
     * @since S60 5.0
     * @param aItemType Menuitem type 
     * @param aList List of data plugins
     * @return ETrue if menuitem should be shown, EFalse otherwise     
     */    
    IMPORT_C virtual TBool DynInitMenuItemL( 
        const TDesC& aItemType, 
        RPointerArray< CXnNodeAppIf >* aList = NULL );
    
    /*
     * Returns the Xml Ui view as CAknView reference.
     * 
     * @since S60 5.0
     * @return Xml Ui View 
     */
    IMPORT_C CAknView& CXnAppUiAdapter::View() const;
	    
    /**
     * Handles changes when entering or exiting edit mode 
     * To be overriden by subclass.     
     *
     * @since S60 5.0
     * @param aEnter enter or exit edit mode
     */
    IMPORT_C virtual void HandleEnterEditModeL( TBool aEnter );

    /**
     * Routes the events from external rendering plug-ins to content plug-ins.
     *
     * @since S60 5.2
     * @param aEvent Event string
     * @param aDestination Destination node for the event
     */
    IMPORT_C virtual void HandleEventL( const TDesC& aEvent, 
        CXnNodeAppIf& aDestination );
    
public:
    /**
     * From CEikAppUi.
     * Second-phase constructor
     *
     * @since Series 60 3.1
     */
    IMPORT_C void ConstructL();

protected:

    /**
     * From CAknAppUi.
     *
     * @since Series 60 3.1
     */
    IMPORT_C void HandleResourceChangeL( TInt aType );

    /**
     * From CAknAppUi.
     *
     * @since Series 60 5.2
     */
    IMPORT_C void PrepareToExit();

public:
    // new functions
        
    /**
     * Reloads the UI
     */
    void ReloadUiL();

    /**
     * Gets UiEngine
     * 
     * @since S60 5.0
     * @return UiEngine
     */    
    CXnUiEngine& UiEngine() const;
    
    /**
     * Gets ViewManager
     * 
     * @since S60 5.0
     * @return ViewManager
     */    
    CXnViewManager& ViewManager() const;

    /**
     * Gets ViewAdapter
     * 
     * @since S60 5.0
     * @return ViewAdapter
     */        
    CXnViewAdapter& ViewAdapter() const;
    
    /**
     * Gets UiStateListener
     * 
     * @since S60 5.0
     * @return UiStateListener
     */    
    CXnUiStateListener& UiStateListener() const;
    
    /**
     * Gets a Content Control UI
	 *
	 * @since S60 5.0
	 * @param aType Content control UI type
	 * @return Interface to content control UI, NULL if not present
     */
    MHsContentControlUi* HsContentController( const TDesC8& aType ) const;
	
    /**
     * Get effect manager
     *
     * @since S60 5.0
     * @return effect manager
     */
    CXnEffectManager* EffectManager() const;

    /**
     * Gets a Content Control server interface
     *
     * @since S60 5.0
     * @return Interface to content control server, NULL if not present
     */
    MHsContentControl* HsContentControlSrv() const;

    /**
     * Queries whether the focus control is visible
     *
     * @since S60 5.0
     * @return ETrue focus is shown, EFalse otherwise
     */    
    TBool FocusShown() const;

    /**
     * Makes focus control invisible
     *
     * @since S60 5.0     
     */        
    void HideFocus();

    /**
     * Makes focus control visible
     *
     * @since S60 5.0     
     */        
    void ShowFocus();

private:
    // Data

    /**
     * flag for application exit
     */
    TBool iExitingApp;

    /**
     * application uid
     */
    TUid iApplicationUid;

    /**
     * appui adapter impl.
     * Own.
     */
    CXnAppUiAdapterImpl* iImpl;

    /**
     * Proxy to UiEngine.
     * Own.
     */
    TXnUiEngineAppIf* iUiEngineAppIf;
    };

#endif

// End of file
