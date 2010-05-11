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
#include <e32base.h>
#include <aknview.h>

// User includes

// Forward declarations
class CXnWallpaperContainer;
class CXnAppUiAdapter;
class CXnUiEngine;
class CXnSpBgCleaner;

// Constants
const TUid KWallpaperViewUid = { 0x200286DB };

// CLASS DECLARATION

/**
*  Wallpaper view.
*  Possible to select background image.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CXnWallpaperView ) : public CAknView 
    {
public:
    // constructor and destructor

    /**
     * Two-phased constructor.
     * @return new instance of CXnWallpaperView.
     */
    static CXnWallpaperView* NewL( CXnUiEngine& aEngine );

    /**
     * Destructor.
     */
    ~CXnWallpaperView();

public:
    // from CAknView
    
    /**
     * @see CAknView
     */      
    TUid Id() const;

private:
    // from CAknView
    
    /**
     * @see CAknView
     */      
    void DoActivateL( const TVwsViewId& aPrevViewId,        
        TUid aCustomMessageId, const TDesC8& aCustomMessage );
            
    /**
     * @see CAknView
     */      
    void DoDeactivate();

private:
    // private constructors
    
    /**
     * C++ default constructor.
     */
    CXnWallpaperView( CXnUiEngine& aEngine );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:
    // new functions
    
    /**
     * Returns selected wallpaper filename as parameter
     * 
     * @since S60 5.2
     * @param aFileName Selected wallpaper filename
     */
    void SelectedWallpaper( TFileName& aFileName ) const;
    
private:
    // new functions
    
    static TInt HandleCallback( TAny* aAny );
    
    void DoHandleCallbackL();
         
private: 
    // data

    /**
     * Wallpaper container.
     * Own.
     */
    CXnWallpaperContainer* iContainer;
    
    /**
     * Image files
     * Own.
     */
    CDesCArrayFlat* iFiles;
    
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
     * Switch for multiple image selection.
     */
    TBool iMultiple;
        
    /**
     * Async callback.
     * Own.
     */
    CAsyncCallBack* iAsyncCb;
    
    /**
     * Resource file offset
     */
    TInt iResourceOffset;    
    
    /** 
     * Statuspane background cleaner. 
     * Own.
     */
	CXnSpBgCleaner* iSpBgCleaner;
	
	/**
	 * Selected image filename
	 */
	TFileName iFileName;
    };

#endif      // CXNWALLPAPERVIEW_H

// End of File
