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
* CImageConverter declaration
*
*/

#ifndef WMEFFECTMANAGER_H
#define WMEFFECTMANAGER_H

//  INCLUDES
#include <e32base.h>

class CCoeEnv;
class CAlfEffectObserver;

// CONSTANTS
// Effects
const TInt KAppStartEffectStyle = 1007;
const TInt KAppExitEffectStyle = 1008;

/**
 *  Struct which holds needed info of effect.
 */
NONSHARABLE_STRUCT( TWmEffect )
    {
    TInt iState;
    TInt iType;
    TInt iId;
    };

/**
 *  Effect manager.
 *  Handles starting and ending effects.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CWmEffectManager ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @return new instance of CWmEffectManager.
     */
    static CWmEffectManager* NewL( CCoeEnv& aCoeEnv );

    /**
     * Destructor.
     */
    ~CWmEffectManager();
    
public:
    /**
     * Begin handling of fullscreen effect.
     * @param aId effect id
     */
    void BeginFullscreenEffectL( TInt aId );
    
    /**
     * When UiRendered is called effect is ended and will be drawn
     */
    void UiRendered();
    
    /**
     * Returns true if effect is ongoing.
     */
    TBool IsEffectActive();
    
private:    
    /**
     * C++ default constructor.
     */
    CWmEffectManager( CCoeEnv& aCoeEnv );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
private:
    /**
     * Starts fullscreen effect.
     * @param aEffect effect data
     * @return ETrue if effect is started, EFalse otherwise.
     */
    TBool DoBeginFullscreenEffect( TWmEffect& aEffect );
    
    /**
     * Waits ongoing effect to complete.
     * @param aInterval Time to wait for complete.
     * @return ETrue if effect completed in given time, EFalse otherwise.
     */
    TBool WaitActiveEffect( TInt aInterval );
    
    /**
     * Removes and destroys effect from effect list.
     * @param aEffect effect data
     */
    void RemoveEffect( TWmEffect* aEffect );
    
private: // data
    /**
     * List of started effects.
     */
    RPointerArray<TWmEffect> iEffects;
    
    /**
     * COE env
     */
    CCoeEnv& iCoeEnv;
    
    /**
     * Effect observer
     */
    CAlfEffectObserver* iObserver;
    };

#endif // WMEFFECTMANAGER_H

// End of file