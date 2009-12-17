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
* Description:  Wallpaper view's container.
*
*/



#ifndef XNWALLPAPERCONTAINER_H
#define XNWALLPAPERCONTAINER_H

//  INCLUDES
#include <coecntrl.h>
#include <AknsBasicBackgroundControlContext.h>

// CLASS DECLARATION

/**
*  Wallpaper container.
*  Contains wallpaper setting items.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CXnWallpaperContainer ) : public CCoeControl
    {
public:
    /**
     * Two-phased constructor.
     * @return new instance of CXnWallpaperContainer.
     */
    static CXnWallpaperContainer* NewL();
    
    /**
     * Destructor.
     */
    ~CXnWallpaperContainer();
    
    /**
     * From CCoeControl
     */
    void Draw( const TRect& aRect ) const;
     
private:
    /**
     * From CCoeControl
     */
    virtual void SizeChanged();
    
    /**
     * From CCoeControl
     */
    virtual void HandleResourceChange(TInt aType);
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CXnWallpaperContainer();

private:
    /**
     * Background context.
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBgContext;
    };

#endif      // XNWALLPAPERCONTAINER_H  
            
// End of File
