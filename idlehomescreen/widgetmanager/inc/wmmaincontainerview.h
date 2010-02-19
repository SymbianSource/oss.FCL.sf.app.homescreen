/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Declares main container view for Widget Manager application.
*
*/

#ifndef WMMAINCONTAINERVIEW_H
#define WMMAINCONTAINERVIEW_H

#include <aknview.h>


class CWmMainContainer;
class CWmPlugin;

/**
 * Avkon view class for WmMainContainerView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CWmMainContainerView WmMainContainerView.h
 */						
NONSHARABLE_CLASS( CWmMainContainerView ) : public CAknView
	{

public: // constructors and destructor
	/**
	 * Two-phased constructor.
	 * 
	 * @param aWmPlugin wm plugin
	 */
	static CWmMainContainerView* NewL( CWmPlugin& aWmPlugin );
	
    /**
     * Two-phased constructor.
     * 
     * @param aWmPlugin wm plugin
     */
	static CWmMainContainerView* NewLC( CWmPlugin& aWmPlugin );	
	
	/** Destructor */
	virtual ~CWmMainContainerView();

	
public:	// from base class CAknView
    /**
     * Returns views id, intended for overriding by sub classes.
     * 
     * @see CAknView::Id
     */
	TUid Id() const;
	
	/**
	 * Command handling function intended for overriding by sub classes.
	 *
	 * @see CAknView::HandleCommandL
	 */
	void HandleCommandL( TInt aCommand );

public:	
	/**
	 * Creates CWmMainContainer
	 */
	CWmMainContainer* CreateContainerL();
	
   /**
     * View deactivation function intended for overriding by sub classes.
     * 
     * @see CAknView::DoActivateL
     */
    void DoDeactivate();
		
public: // from MeikMenuObserver
	
    /** 
     * Foreground event handling function.  
     * @param aForeground Indicates the required focus state of the control.
     */ 
    void HandleForegroundEventL( TBool aForeground );
	    
private:
    CWmMainContainerView( CWmPlugin& aWmPlugin );    
    void ConstructL();    
    
protected: // from base class CAknView
    /**
     * Avkon view architecture system.
     * 
     * @see CAknView::DoActivateL
     */
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	
   /**
     * Event handler for status pane size changes.
     * 
     * @see CAknView::HandleStatusPaneSizeChange
     */
	void HandleStatusPaneSizeChange();

protected:	// From MEikMenuObserver
    /*
     * Menu observer interface.
     * 
     * @see MEikMenuObserver::DynInitMenuPaneL
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
    
    /** menu commands */
    TBool HandleOpenMenuItemSelectedL();
    TBool HandleAddMenuItemSelectedL();
    TBool HandleLaunchMenuItemSelectedL();
    TBool HandleSearchMenuItemSelectedL();
    TBool HandleSortAlphaMenuItemSelectedL();
    TBool HandleUninstallMenuItemSelectedL();
    TBool HandleHelpMenuItemSelectedL();
	TBool HandleDetailsMenuItemSelectedL();
	TBool HandleDeactivateFindPaneL();
	/** setup title in status pane */
	void SetTitleL();

private:
    /** 
     * the container 
     */
	CWmMainContainer* iWmMainContainer;

	/** 
	 * the plugin root (not owned) 
	 */
	CWmPlugin& iWmPlugin;

	};

#endif // WMMAINCONTAINERVIEW_H
