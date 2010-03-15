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
* Description:  Wallpaper view. View is used to get skin background
*               to image selection grid.
*
*/



#ifndef CXNWALLPAPERVIEW_H
#define CXNWALLPAPERVIEW_H

// System includes
#include <aknview.h>

// User includes

// Forward declarations
class CXnWallpaperContainer;
class CXnAppUiAdapter;
class CXnUiEngine;
class TVwsViewId;
class CPeriodic;

// Constants
const TUid KWallpaperViewUid = { 0x200286DB };

/**
*  Data structure to be used with timer callback.
*
*  @since S60 v5.0
*/
NONSHARABLE_STRUCT( TXnWallpaperViewData )
    {
    TBool iMultiple;
    TUid iAppUid;
    TUid iViewUid;    
    };

// CLASS DECLARATION

/**
*  Wallpaper view.
*  Possible to define current Idle background image.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CXnWallpaperView ) : public CAknView 
    {
public:

    /**
     * Two-phased constructor.
     * @return new instance of CXnWallpaperView.
     */
    static CXnWallpaperView* NewL( CXnUiEngine& aEngine );

    /**
     * Destructor.
     */
    ~CXnWallpaperView();

    /**
     * From CAknView.
     * Returns view id.
     * @return An unsigned integer (view id).
     */
    TUid Id() const;

private:

    /* From CAknView. */
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );
    
    /* From CAknView. */
    void DoDeactivate();
    
    /**
     * C++ default constructor.
     */
    CXnWallpaperView( CXnUiEngine& aEngine );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Callback function to be used with CPeriodic.
     */
    static TInt TimerCallbackL( TAny *aPtr );

private: // data

    /**
     * Wallpaper container.
     * Own.
     */
    CXnWallpaperContainer* iContainer;
        
    /**
     * UiEngine
     * Not owned.
     */
    CXnUiEngine& iEngine;
    
    /**
     * AppUi
     * Not owned.
     */
    CXnAppUiAdapter& iAppUi;
    
    /**
     * Data structure, that is transferred to TimerCallback.
     */
    TXnWallpaperViewData iData;
    
    /**
     * Periodic timer.
     * Own.
     */
    CPeriodic* iTimer;
    
    /**
     * Resource file offset
     */
    TInt iResourceOffset;    
    };

#endif      // CXNWALLPAPERVIEW_H

// End of File
