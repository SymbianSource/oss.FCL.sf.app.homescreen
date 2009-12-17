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
* Description:
* Management of views in UiEngine
*
*/

#ifndef XNVIEWMANAGER_H
#define XNVIEWMANAGER_H

// System includes
#include <e32base.h>
#include <babitflags.h>

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
    // Friend classes
    friend class CXnViewAdapter;    
public:
    /**
     * Two-phased constructor.
     */
    static CXnViewManager* NewL(
        CXnAppUiAdapter& aAdapter );
        
    static CXnViewManager* NewLC(
        CXnAppUiAdapter& aAdapter );        

    ~CXnViewManager();

private:
    CXnViewManager( CXnAppUiAdapter& aAdapter );
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

    RPointerArray< CXnNode >& AppearanceNodes() const;

    CArrayPtrSeg< CXnResource >& Resources() const ;
           
    void LoadUiL();

    void ReloadUiL();

    TInt LoadWidgetToPluginL(
        CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData );

    TInt UnloadWidgetFromPluginL(
        CXnPluginData& aPluginData );
        
    TInt ReplaceWidgetToPluginL(
        CHsContentInfo& aContentInfo,
        CXnPluginData& aPluginData,
        TBool aUseHsps = ETrue );

    void ActivateNextViewL();

    void ActivatePreviousViewL();

    TInt AddViewL( CHsContentInfo& aInfo );
    
    void AddViewL();
    
    TInt RemoveViewL( const CHsContentInfo& aInfo );

    void RemoveViewL();
    
    void AddObserver( const MXnViewObserver& aObserver );

    void RemoveObserver( const MXnViewObserver& aObserver );

    CXnRootData& ActiveAppData() const;
    
    CXnViewData& ActiveViewData() const;

    void SetFirstPassDrawCompleteL();

    void SetDataPluginLoadCompleteL( const CXnPluginData& aPluginData );

    void UpdatePluginStateL( CXnPluginData& aPluginData );
       
    TInt ActivateAppL( const TDesC8& aPluginUid );
    TInt ActivateViewL( const TDesC8& aPluginId );    
    
    TInt ViewAmount() const;

    TInt ViewIndex() const;

    CXnOomSysHandler& OomSysHandler() const;
    
    /**
     * Get maximum pages allowed
     * 
     * @return number of pages allowed
     */
    TInt32 MaxPages();    

private:
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

    CXnViewData* ViewData( CXnNode& aNode ) const;

    CXnViewData& PreviousViewData() const;

    CXnViewData& NextViewData() const;

    void UpdateCachesL();

    void ValidateActiveViewL();
    
    void InvalidateActiveView();

    void ReportWidgetAmountL( const CXnViewData& aViewData );
    
    void ShowDiskFullMessageL() const;
       
public:
    void UpdatePageManagementInformationL();

#if 0 // MSK icon change
private:
    TInt ResolveIconIndex( TInt aPageCount, TInt aPageNum ) const;
#endif // MSK icon change
        
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
     * Failed plugins.
     * Own.
     */    
    RPointerArray< CXnPluginData > iFailedPlugins;
    
    /**
     * Flags
     */
    TBitFlags32 iFlags;

    /**
     * OOM system handler. Owned.
     */
    CXnOomSysHandler* iOomSysHandler;    
    };

// Inline functions
#include "xnviewmanager.inl"

#endif
