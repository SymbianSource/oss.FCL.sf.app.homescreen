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
* Description:  Manages Ai3 personalization
*
*/


#ifndef C_XNEDITOR_H
#define C_XNEDITOR_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <hscontentcontroller.h>
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>
#include <AknsSrvClient.h>

// User includes
#include "hspswrapper.h"
#include "cpswrapper.h"
#include "xnviewmanager.h"

// Forward declarations
class CXnNode;
class CAknListQueryDialog;
class CXnViewManager;
class CXnViewData;
class CRepository;
class CXnBackgroundManager;
class CXnOomSysHandler;

namespace hspswrapper
    {
    class CPropertyMap;
    class CAddPluginResult;
    class MHspsWrapperObserver;    
    }

namespace cpswrapper
    {
    class CCpsWrapper;
    class CPublisherMap;
    class CPublisherInfo; 
    class MCpsWrapperObserver;
    }

using namespace hspswrapper;
using namespace cpswrapper;

// Class declaration

/**
 *  Editor handles ai3 personalization
 *
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CXnEditor ) : public CBase, 
    public MXnViewObserver,
    public MCpsWrapperObserver,
    public MHspsWrapperObserver,
    public MHsContentController
    {
public:
    //constructors and destructor
    
    /**
     * Two-phased constructor.
     * 
     * @param aViewManager View manager
     * @param aWrapper HSPS wrapper
     */
    static CXnEditor* NewL( CXnViewManager& aViewManager,
        const TDesC8& aUid );
    
    /**
     * Two-phased constructor.
     * 
     * @param aViewManager View manager
     * @param aWrapper HSPS wrapper
     */
    static CXnEditor* NewLC( CXnViewManager& aViewManager,
        const TDesC8& aUid );

    /**
     * Destructor.
     */
    ~CXnEditor();

public:
    // new functions    
    
    /**
     * Remove uregistered widget from all the views.
     *
     * @since S60 5.0
     * @param aPublisher publisher
     */
    void RemoveUnRegisteredWidgetL( const TDesC& aPublisher );

    /**
     * Removes uninstalled widget from all views
     * 
     * @since S60 5.0
     * @param aContentInfo content info
     */
    void RemoveUnInstalledWidgetL( const CHsContentInfo& aContentInfo );

    /**
     * Runs add widget list query
     * 
     * @since S60 5.0     
     */
    void AddWidgetL();

    /**
     * Remove widget from current view.
     *
     * @since S60 5.0
     * @param aNode Plugin (or its child) where to remove widget.     
     */
    void RemoveWidgetL( CXnNode* aNode );

    /**
     * Replaces widget configuration to HSPS
     * 
     * @since S60 5.0
     * @param aContentInfo content info 
     */
    void ReplaceWidgetL( CHsContentInfo& aContentInfo );
    /**
     * Commit a new order of plugins to the HSPS.
     * 
     * @since S60 5.0
     * @param aPluginArray Array of plugins
     */
    void ReorderWidgetsL( RPointerArray< CXnNode >* aPluginArray );

    /**
     * Toggles widgets' visibility
     * 
     * @since S60 5.0
     * @return Current widgets' visibility state
     */
    TBool ToggleWidgetsVisibiltyL();

    /**
     * Queries wheter the widgets are visible or not
     * 
     * @since S60 5.0
     * @return ETrue if widgets are now visible, EFalse otherwise
     */
    TBool WidgetsVisible() const;

    /**
     * Sets target plugin for add/remove
     * 
     * @since S60 5.0
     * @param aNode a plugin node to be set as target for add/remove action
     */
    void SetTargetPlugin( CXnNode* aNode );

    /**
     * Handles changing of wallpaper
     * 
     * @since S60 5.0
     */
    void SetWallpaperL();
    
    /**
     * Returns the HSPS Wrapper object owned by this.
     * 
     * @since S60 5.0
	 * @return HSPSWrapper
     */
    CHspsWrapper& HspsWrapper() const;

    /**
     * Retrives Id read from central repository.
     * 
     * @since S60 5.0
	 * @return sucess or failure standard error code
     */    
    TInt IdFromCrep ( TDes8& aUid ) const;

private:
    // from MHsContentController
    
    TInt WidgetListL( CHsContentInfoArray& aArray );

    TInt WidgetListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );

    TInt ViewListL( CHsContentInfoArray& aArray );

    TInt ViewListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );

    TInt AppListL( CHsContentInfoArray& aArray );
    
    TInt AddWidgetL( CHsContentInfo& aInfo );
    
    TInt RemoveWidgetL( CHsContentInfo& aInfo );
    
    TInt AddViewL( CHsContentInfo& aInfo );
    
    TInt RemoveViewL( CHsContentInfo& aInfo );
    
    TInt ActivateViewL( CHsContentInfo& aInfo );
    
    TInt ActivateAppL( CHsContentInfo& aInfo );

    TInt ActiveViewL( CHsContentInfo& aInfo );
    
    TInt ActiveAppL( CHsContentInfo& aInfo );

private:
    // constructors
    
    /**
     * C++ constructor
     */
    CXnEditor( CXnViewManager& aViewManager );

    /**
     * Leaving constructor
     */
    void ConstructL( const TDesC8& aUid );

private:
    // new functions    
    
      CPublisherInfo* PublisherInfoL( const CHsContentInfo& aContentInfo );

    TInt TemplateWidgetCanBeAddedRemovedL( CHsContentInfo& aContentInfo );
    TInt NonTemplateWidgetCanBeAddedRemovedL( CHsContentInfo& aContentInfo );
    
    TBool IsCurrentViewFull();
    
    /**
     * Filters plugins from the list and checks whether the plugins can be added or removed.
     */
    void FilterPluginsL( CHsContentInfoArray& aContentInfoArray,
        TBool aIgnoreViewFull );
    
    void FilterViewListL( CHsContentInfoArray& aContentInfoArray ); 
    CXnOomSysHandler& OomSysHandler() const;
    
    /**
     * Notifies the MHsContentControl and the MHsContentControlUi if widget list was changed.
     */
    void WidgetListChanged();

    /**
     * Notifies the MHsContentControl and the MHsContentControlUi if view list was changed.
     */
    void ViewListChanged();
    
    /**
     * Creates content info for defined plugin
     */
    CHsContentInfo* CreateContentInfoLC( CXnPluginData& aPlugin, 
        RPointerArray< CHsContentInfo >& aInfos );
    
    /**
     * Appends plugin configurations to the content info array.
     */
    void AppendPluginsL(
            RPointerArray< hspswrapper::CPluginInfo > aPlugins,
            RPointerArray< CHsContentInfo >& aWidgets );
    
    /**
     * Resets pluging configurations and templated publishers.
     */
    void ResetPluginsAndPublishers();
    
    /**
     * Retrieves native application configuration plugins from HSPS
     */
    void HspsApplicationPluginsL( RPointerArray< CHsContentInfo >& aWidgets );
    
    /**
     * Retrieves native view configuration plugins from HSPS
     */
    void HspsViewPluginsL( RPointerArray< CHsContentInfo >& aWidgets );
           
    /**
     * Retrieves widget and template configuration plugins from HSPS
     */
    void HspsWidgetPluginsL( RPointerArray< CHsContentInfo >& aWidgets );        
    void DoHspsWidgetPluginsL();
    
    /**
     * Retrieves publishers from CPS and them with the HSPS's template plugins
     */
    void CpsWidgetPluginsL( RPointerArray< CHsContentInfo >& aWidgets );

    /**
     * Checks whether the view is full
     */
    TBool IsViewFull( CXnViewData& aViewData );
    
    /*
     * Notifies observers that widget list is changed
     */
    void NotifyWidgetListChanged();
    
    /** callback for notify widget launcher */
    static TInt WidgetListChangedCallBack( TAny* aSelf );

    /*
     * Notifies observers that view list is changed
     */
    void NotifyViewListChanged();
    
    /** callback for notify view launcher */
    static TInt ViewListChangedCallBack( TAny* aSelf );

private:
    // from MXnViewObserver    
    
    void NotifyViewActivatedL( const CXnViewData& aViewData );

    void NotifyViewDeactivatedL( const CXnViewData& /*aViewData*/ );

    void NotifyConfigureWidgetL( const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    void NotifyWidgetAdditionL( const CXnPluginData& aPluginData );
    
    void NotifyWidgetRemovalL( const CXnPluginData& aPluginData );

    void NotifyViewAdditionL( const CXnPluginData& /*aPluginData*/ );
    
    void NotifyViewRemovalL( const CXnPluginData& /*aPluginData*/ );
    
    void NotifyAllViewsLoadedL();

    void NotifyContainerActivatedL( const CXnViewData& aViewData );

private:
    // from MCpsWrapperObserver    
 
    void NotifyWidgetUnregisteredL( const TDesC& aPublisher );
    
    void NotifyWidgetRegisteredL();
    
    void NotifyWidgetUpdatedL();
    
private:
    // from MHspsWrapperObserver

    TInt HandleNotifyL( const TDesC8& aEvent, const TDesC8& aAppConfUid,
        const TDesC8& aPluginName, const TDesC8& aOrigUid, 
        const TDesC8& aPluginUid, const TDesC8& aPluginId );    

private:
    // data

    /** View manager, not owned */
    CXnViewManager& iViewManager;
    /** HSPS wrapper, owned */
    CHspsWrapper* iHspsWrapper;  
    /** CPS wrapper, owned */
    CCpsWrapper* iCpsWrapper;
    /** Publisher map, owned */
    CPublisherMap* iPublisherMap;
    /** Add widget listquery, not owned */
    CAknListQueryDialog* iQuery;
    /** Target plugin for add/remove, not owned */
    CXnNode* iTargetPlugin;
    /** Widgets visibility state */
    TBool iWidgetsVisibilityState;    
    /** Central repository , to choose plugin, owned*/
    CRepository* iRepository;
	/** Widget/template plugin configurations, owned */
    RPointerArray< hspswrapper::CPluginInfo > iPluginConfigurations;
	/** Oom handler, owned. */
    CXnOomSysHandler* iOomSysHandler;
	/** Launcher for notifying widget list changes */    
    CPeriodic* iNotifyWidgetListChanged;
    /** Launcher for notifying view list changes */    
    CPeriodic* iNotifyViewListChanged;
    };

#endif // C_XNEDITOR_H
