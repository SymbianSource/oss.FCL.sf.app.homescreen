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
class CXnPluginData;
class CXnViewData;
class CXnNode;

// Constants

// Control effect context ids
#define KGfxContextAddWidget            TUid::Uid( 0x102750F4 )
#define KGfxContextRemoveWidget         TUid::Uid( 0x102750F4 )
#define KGfxContextActivateNextView     TUid::Uid( 0x102750F1 )
#define KGfxContextActivatePrevView     TUid::Uid( 0x102750F2 )
#define KGfxContextBgAppear             TUid::Uid( 0x102750F3 )

// Control effect action ids
#define KGfxControlActionAppear         3
#define KGfxControlActionDisappear      4

#define KGfxControlActionAppearPrt         3
#define KGfxControlActionDisappearPrt      5
#define KGfxControlActionAppearLsc         6
#define KGfxControlActionDisappearLsc      7

#define KGfxControlActionBgImgToImgAppear   3
#define KGfxControlActionBgAnimToImgAppear  5

// Full screen effect ids
#define KGfxContextOpenWallpaperView    1007
#define KGfxContextCloseWallpaperView   1008

// Class declaration
/**
 *  Struct which holds needed info of effect.
 */
NONSHARABLE_STRUCT( TXnEffect )
    {
    CXnNode* iNode;
    TInt iState;
    TInt iId;
    };

/**
 *  Effect manager.
 *  Handles starting and ending effects.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CXnEffectManager ) : public CBase
    {
public:
    // constructor and destructor

    /**
     * Two-phased constructor.
     * @return new instance of CXnEffectManager.
     */
    static CXnEffectManager* NewL();

    /**
     * Destructor.
     */
    ~CXnEffectManager();

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
     * Removes and destroys effect from effect list.
     * 
     * @since S60 5.2
     * @param aEffect effect data
     */
    void RemoveEffect( TXnEffect* aEffect );
    
private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CXnEffectManager();

    /**
     * 2nd phase constructor 
     */
    void ConstructL();

private: 
    // data
    
    /** List of started effects, owned */
    RPointerArray<TXnEffect> iEffects;    
    };

#endif      // CXNEFFECTMANAGER_H

// End of File
