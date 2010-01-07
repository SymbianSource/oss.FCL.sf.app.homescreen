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

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CXnPluginData;
class CXnViewData;
class CXnNode;

/**
 *  Struct which holds needed info of effect.
 */
NONSHARABLE_STRUCT( TXnEffect )
    {
    CXnNode* iNode;
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
NONSHARABLE_CLASS( CXnEffectManager ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @return new instance of CXnEffectManager.
     */
    static CXnEffectManager* NewL();

    /**
     * Destructor.
     */
    ~CXnEffectManager();
    
    /**
     * Begin handling of control effect.
     * @param aId effect id
     * @aPlugin Plugin data
     */
    void BeginControlEffectL( TInt aId, CXnPluginData& aPlugin );
    
    /**
     * Begin handling of control effect.
     * @param aId effect id
     * @aPlugins Array of plugin datas
     */
    void BeginControlEffectL( TInt aId, RPointerArray<CXnPluginData>& aPlugins );
    
    /**
     * Begin handling of fullscreen effect.
     * @param aId effect id
     * @aView view data
     */
    void BeginFullscreenEffectL( TInt aId, CXnViewData& aView );
    
    /**
     * When UiRendered is called effect is ended and will be drawn
     */
    void UiRendered();

    /**
     * When UiLayouted is called effect is started if it was not started earlier.
     */
    void UiLayouted();

private:
    /**
     * Starts fullscreen effect.
     * @param aEffect effect data
     * @return ETrue if effect started, otherwise EFalse
     */
    TBool DoBeginFullscreenEffect( TXnEffect& aEffect );

    /**
     * Starts control effect.
     * @param aEffect effect data
     * @return ETrue if effect started, otherwise EFalse
     */
    TBool DoBeginControlEffect( TXnEffect& aEffect );
    
    /**
     * Removes and destroys effect from effect list.
     * @param aEffect effect data
     */
    void RemoveEffect( TXnEffect* aEffect );

    /**
     * Return number of ongoing effects of given type.
     * @param aType effect type, fullscreen or control
     * 
     * @return Number of ongoing effects of given type
     */
    TInt EffectCount( TInt aType );
    
    /**
     * C++ default constructor.
     */
    CXnEffectManager();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data
    /**
     * List of started effects.
     */
    RPointerArray<TXnEffect> iEffects;
    };

#endif      // CXNEFFECTMANAGER_H

// End of File
