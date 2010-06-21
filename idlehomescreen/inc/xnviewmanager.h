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
* Description: View Manager 
*
*/

#ifndef XNVIEWMANAGER_H
#define XNVIEWMANAGER_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CXnUiEngine;
class CXnComposer;
class CXnODTParser;
class CXnEditor;
class CXnEcomHandler;
class CXnRootData;
class CXnViewData;
class CXnPluginData;
class CXnNode;
class CXnODT;
class CXnResource;
class CXnControlAdapter;
class CXnAppUiAdapter;
class CHsContentInfo;
class CXnViewAdapter;
class CXnOomSysHandler;

namespace hspswrapper
    {
    class CHspsWrapper;
    }

using namespace hspswrapper;

// Classe declaration
/**
 * Notifies view changes
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( MXnViewObserver )
    {
public:
    // new functions
    virtual void NotifyViewActivatedL( const CXnViewData& aViewData ) = 0;

    virtual void NotifyViewDeactivatedL( const CXnViewData& aViewData ) = 0;

    virtual void NotifyConfigureWidgetL(
        const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData ) = 0;

    virtual void NotifyWidgetAdditionL( const CXnPluginData& aPluginData ) = 0;

    virtual void NotifyWidgetRemovalL( const CXnPluginData& aPluginData ) = 0;
    
    virtual void NotifyViewAdditionL( const CXnPluginData& aPluginData ) = 0;

    virtual void NotifyViewRemovalL( const CXnPluginData& aPluginData ) = 0;
    
    virtual void NotifyAllViewsLoadedL() = 0;
    
    virtual void NotifyContainerActivatedL( const CXnViewData& aViewData ) = 0;
    };

/**
 * Manages views in UiEngine.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnViewManager ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
    static CXnViewManager* NewL(
        CXnAppUiAdapter& aAdapter );

    /**
     * Two-phased constructor.
     */    
    static CXnViewManager* NewLC(
        CXnAppUiAdapter& aAdapter );        

    /**
     * Destructor
     */
    ~CXnViewManager();

private:
    // constructors
  
    /** 
     * C++ constructor
     */
    CXnViewManager( CXnAppUiAdapter& aAdapter );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
public:
    // New functions

    inline void SetUiEngine( CXnUiEngine& aUiEngine );

    inline CXnUiEngine& UiEngine() const;

    inline CXnComposer& Composer() const;

    inline CXnEditor& Editor() const;

    inline CXnAppUiAdapter& AppUiAdapter() const;
           
    CXnODT* ODT() const;

    CXnNode* RootNode() const;

    CXnNode* ViewNode() const;
    
    CXnODTParser& Parser() const;

    CXnEcomHandler& EcomHandler() const;
    
    const RPointerArray< CXnControlAdapter >& Controls() const;

    RPointerArray< CXnNode >& PluginNodes() const;
    
    void PluginDataL( RPointerArray< CXnPluginData >& aList, 
        TBool aGlobal = EFalse ) const;

    TInt PluginDataL( const TDesC8& aParentId, 
        RPointerArray< CXnPluginData >& aList ) const;

    RPointerArray< CXnNode >& AppearanceNodes() const;

    CArrayPtrSeg< CXnResource >& Resources() const ;
           
    void LoadUiL();

    void ReloadUiL();

    TInt LoadWidgetToPluginL(
        const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    TInt UnloadWidgetFromPluginL(
        CXnPluginData& aPluginData, TBool aForce = EFalse );
        
    TInt ReplaceWidgetToPluginL(
        const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    void ActivateNextViewL( TInt aEffectId = 0 );

    void ActivatePreviousViewL( TInt aEffectId = 0 );

    TInt AddViewL( const CHsContentInfo& aInfo );
    
    void AddViewL( TInt aEffectId = 0 );
    
    TInt RemoveViewL( const CHsContentInfo& aInfo );

    void RemoveViewL( TInt aEffectId = 0 );
    
    void AddObserver( const MXnViewObserver& aObserver );

    void RemoveObserver( const MXnViewObserver& aObserver );

    CXnRootData& ActiveAppData() const;
    
    CXnViewData& ActiveViewData() const;
             
    TInt ActivateAppL( const TDesC8& aPluginUid );
    TInt ActivateViewL( const TDesC8& aPluginId );    
    
    TInt ViewAmount() const;

    TInt ViewIndex() const;

    TInt MaxPages() const;
    
    CXnOomSysHandler& OomSysHandler() const;
    
    void UpdatePageManagementInformationL();
                  
    void ShowErrorL( TInt aResource ) const;
    
    void PublishersReadyL( CXnViewData& aViewData, TInt aResult );
        
    void NotifyAllViewsLoadedL();
        
private:
    // new functions
    
    void NotifyContainerChangedL( CXnViewData& aViewToActivate );       

    void NotifyViewActivatedL( const CXnViewData& aViewData );

    void NotifyViewDeactivatedL( const CXnViewData& aViewData );

    void NotifyViewAdditionL( const CXnViewData& aViewData );
   
    void NotifyViewRemovalL( const CXnViewData& aViewData );

    void NotifyConfigureWidgetL(
        const CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    void NotifyWidgetAdditionL( const CXnPluginData& aPluginData );

    void NotifyWidgetRemovalL( const CXnPluginData& aPluginData );

    void NotifyContainerActivatedL( const CXnViewData& aViewData );

    CXnViewData* ViewData( CXnNode& aNode ) const;

    CXnViewData& PreviousViewData() const;

    CXnViewData& NextViewData() const;

    void UpdateCachesL();
          
    void ReportWidgetAmountL( const CXnViewData& aViewData );
                
    TInt ResolveIconIndex( TInt aPageCount, TInt aPageNum ) const;
    
    /**
     * Callback for stability timer
     * 
     * @param   aAny    Pointer to CViewManager object
     * @return  TInt    System wide error code.
     */
    static TInt SystemStabileTimerCallback( TAny* aAny );

    /**
     * Reset crash count
     */
    void ResetCrashCount();    
    
    void DoRobustnessCheckL();
    
    static TInt ContainerActivated( TAny* aAny );
              
private:
    // data

    /**
     * AppUi adapter.
     * Not own.
     */
    CXnAppUiAdapter& iAppUiAdapter;

    /**
     * Application uid
     */
    TUid iApplicationUid;
    
    /**
     * HSPS wrapper. 
     * Not own.
     */
    CHspsWrapper* iHspsWrapper;

    /**
     * UiEngine.
     * Not own.
     */
    CXnUiEngine* iUiEngine;
    
    /**
     * Comopser.
     * Own.
     */
    CXnComposer* iComposer;

    /**
     * Editor.
     * Own.
     */
    CXnEditor* iEditor;
    
    /**
     * Application configuration root data.
     * Own.
     */
    CXnRootData* iRootData;
    
    /**
     * Widget amount trigger.
     * Own.
     */
    CXnNode* iWidgetAmountTrigger;
    
    /**
     * Resources for current view.
     * Own.
     */
    mutable CArrayPtrSeg< CXnResource >* iResources;

    /**
     * Controls for current view.
     * Own.
     */
    mutable RPointerArray< CXnControlAdapter > iControls;

    /**
     * Appearance nodes. This array needs to be updated when UI furniture
     * is changed.
     * Own.
     */
    mutable RPointerArray< CXnNode > iAppearanceNodes;

    /**
     * View observers.
     * Own.
     */
    RPointerArray< MXnViewObserver > iObservers;
    
    /**
     * OOM system handler. 
     * Own.
     */
    CXnOomSysHandler* iOomSysHandler;
    
    /**
     * Timer for system stability.
     * Own.
     */
    CPeriodic* iStabilityTimer;

    /**
     * Flag to indicate first view activation
     */
    TBool iUiReady;
    	
    /**
     * Async callback
     * Own. 
     */
    CPeriodic* iAsyncCb;
    
private:   
    // Friend classes
    
    friend class CXnViewAdapter;           
    };

// Inline functions
#include "xnviewmanager.inl"

#endif
