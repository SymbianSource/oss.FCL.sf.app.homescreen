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
* Description:  Effect manager.
*
*/



#ifndef CXNEFFECTMANAGER_H
#define CXNEFFECTMANAGER_H

// System incldues
#include <e32base.h>

// User includes

// Forward declarations
class CAlfEffectObserver;
class CXnAppUiAdapter;
class CXnPluginData;
class CXnViewData;
class CXnNode;

// Constants

// Control effect context ids
#define KGfxContextAddWidget                TUid::Uid( 0x102750F4 )
#define KGfxContextRemoveWidget             TUid::Uid( 0x102750F4 )
#define KGfxContextActivateNextView         TUid::Uid( 0x102750F1 )
#define KGfxContextActivatePrevView         TUid::Uid( 0x102750F2 )
#define KGfxContextBgAppear                 TUid::Uid( 0x102750F3 )

// Control effect action ids
#define KGfxControlActionAppear             3
#define KGfxControlActionDisappear          4

#define KGfxControlActionAppearPrt          3
#define KGfxControlActionDisappearPrt       5
#define KGfxControlActionAppearLsc          6
#define KGfxControlActionDisappearLsc       7

#define KGfxControlActionBgImgToImgAppear   3
#define KGfxControlActionBgAnimToImgAppear  5

// Full screen effect ids
#define KGfxContextOpenWallpaperView        1007
#define KGfxContextCloseWallpaperView       1008

// Class declaration
/**
 *  Struct which holds needed info of effect.
 */
NONSHARABLE_STRUCT( TXnEffect )
    {
    TXnEffect() 
        : iNode( NULL ), iState( 0 ), iId( 0 ), iExplicitEnd( EFalse )
        {        
        }
        
    CXnNode* iNode;
    TBool iExplicitEnd;
    TInt iState;
    TInt iId;
    };

/**
 *  Effect manager.
 *  Handles starting and ending effects.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CXnEffectManager ) : public CTimer    
    {
public:
    // constructor and destructor

    /**
     * Two-phased constructor.
     * @return new instance of CXnEffectManager.
     */
    static CXnEffectManager* NewL( CXnAppUiAdapter& aAppUiAdapter );

    /**
     * Destructor.
     */
    ~CXnEffectManager();

private:
    // from CTimer
    
    /**
     * @see CTimer
     */
    void RunL();
        
public:
    // new functions
    
    /**
     * Begin handling of fullscreen effect.
     * 
     * @since S60 5.2 
     * @param aId effect id
     * @aView view data
     */
    void BeginFullscreenEffectL( TInt aId, CXnViewData& aView );

    /**
     * Begin handling of fullscreen effect.
     * 
     * @since S60 5.2 
     * @param aId effect id
     */
    void BeginFullscreenEffectL( TInt aId );

    /**
     * Ends fullscreen effect.
     * 
     * @since S60 5.2 
     * @param aId effect id
     */
    void EndFullscreenEffect( TInt aId );
    
    /**
     * Begin view change effect
     * 
     * @since S60 5.2
     * @param aThis Active view
     * @param aOther View to activate
     * @param aEffect Effect uid 
     * @param aBgEffectOnly Show only background effect without view animation 
     * @return ETrue of effect is started, EFalse otherwise
     */    
    TBool BeginActivateViewEffect( const CXnViewData& aThis, 
        const CXnViewData& aOther, TUid aEffect );

    /**
     * End view change effect
     * 
     * @since S60 5.2
     * @param aThis Active view
     * @param aOther View to activate
     * @param aEffect Effect uid 
     */        
    void EndActivateViewEffect( const CXnViewData& aThis, 
        const CXnViewData& aOther, TUid aEffect );
       
    /**
     * Begins background appear effect
     * 
     * @since S60 5.2
     * @param aBg Background control
     * @param aWaitActiveEffect ETrue if active effect is checked, EFalse otherwise
     */
    void BgAppearEffect( CCoeControl* aBg, TBool aWaitActiveEffect = EFalse );
    
    /**
     * When UiRendered is called effect is ended and will be drawn
     * 
     * @since S60 5.2 
     */
    void UiRendered();

    /**
     * When UiLayouted is called effect is started if it was not started earlier.
     * 
     * @since S60 5.2
     */
    void UiLayouted();

    /**
     * Checks if effect is active for control
     * 
     * @since S60 5.2
     * @param aControl Control to check
     * @return ETrue if active, EFalse otherwise
     */
    TBool ControlEffectActive( const CCoeControl* aControl ) const;
    
    /** 
     * Updates iLandscape flag
     * 
     * @since S60 5.2
     */
    void OrientationChanged();

    /** 
     * Cleanup control effect
     * 
     * @since S60 5.2
     */    
    void CleanupControlEffect();
    
private:
    // new functions
    
    /**
     * Starts fullscreen effect.
     * 
     * @since S60 5.2
     * @param aEffect effect data
     * @return ETrue if effect started, otherwise EFalse
     */
    TBool DoBeginFullscreenEffect( TXnEffect& aEffect );
    
    /**
     * Waits active effects to complete
     * 
     * @since S60 5.2
     * @param aInterval Interval in micro seconds to wait active effects to complete     
     * @return ETrue if no active effects ongoing when function returns, EFalse otherwise 
     */
    TBool WaitActiveEffect( TInt aInterval );
    
private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CXnEffectManager( CXnAppUiAdapter& aAppUiAdapter );

    /**
     * 2nd phase constructor 
     */
    void ConstructL();

private: 
    // data
    
    /** Effect observer, owned */
    CAlfEffectObserver* iObserver;
    /** AppUi, not owned */
    CXnAppUiAdapter& iAppUiAdapter;
    /** List of started effects, owned */
    RArray< TXnEffect > iEffects;
    /** List of controls in current effect, not owned */
    RPointerArray< CCoeControl > iControls;
    /** Effect group id */
    TInt iGroupId;    
    /** Flag to indicate whether background effect is started */
    TBool iBgEffect;
    /** Orientation */
    TBool iLandscape;
    };

#endif      // CXNEFFECTMANAGER_H

// End of File
