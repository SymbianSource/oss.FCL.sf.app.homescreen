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
* Description:  Preview provider API
*
*/


#ifndef TSFASTSWAPPREVIEWPROVIDER_H
#define TSFASTSWAPPREVIEWPROVIDER_H

// INCLUDES
#include <e32base.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CTsFastSwapPreviewProviderClient;
class MTsFastSwapPreviewObserver;

/**
 * Preview provider API class. Applications (window groups) must be
 * registered before screenshots can be received. Also, closed apps
 * should be unregistered to save resources.
 * TakePreview() can be called to take a screenshot right away.
 * A client needing notifications has to create an instance
 * so that screenshots can be delivered to it via MTsFastSwapPreviewObserver.
 *
 * @see MTsFastSwapPreviewObserver
 * @lib tspreviewprovider.lib
 */
NONSHARABLE_CLASS( CTsFastSwapPreviewProvider ) : public CBase
    {
public:

    /**
     * Two phased constructor.
     * @see MTsFastSwapPreviewObserver
     *
     * @param aObs Observer.     
     */   
    IMPORT_C static CTsFastSwapPreviewProvider* NewL(
        MTsFastSwapPreviewObserver& aObs );
    
    /**
     * Destructor.
     */   
    IMPORT_C ~CTsFastSwapPreviewProvider();
    
    /**
     * Registers window group for screenshots.
     *
     * @param aWgId Window group id.
     * @return Error code.     
     */       
    IMPORT_C TInt Register( TInt aWgId );
    
    /**
     * Unregisters window group.
     *
     * @param aWgId Window group id.
     * @return Error code.     
     */       
    IMPORT_C TInt Unregister( TInt aWgId );
    
    /**
     * Change screen.
     *
     * @param aScreenId Screen id.
     * @return Error code.     
     */       
    IMPORT_C TInt ChangeScreen( TInt aScreenId );
    
    /**
     * Defines the dimensions and format of the screenshots.
     * @see TDisplayMode.
     *
     * @param aSize Size to be used for the preview screenshots.
     * @param aMode Color depth to be used for the preview screenshots.
     * @return Error code.
     */       
    IMPORT_C TInt SetPreviewParam( const TSize& aSize, TDisplayMode aMode );
    
    /**
     * Takes a screenshot.
     * 
     * @return Error code.     
     */       
    IMPORT_C TInt TakePreview();
    
    /**
     * Ack preview ready message
     */
    IMPORT_C void AckPreview( TInt aPreviewId );
    
private:

    /**
     * C++ constructor.
     */   
    CTsFastSwapPreviewProvider();
    
    /**
     * 2nd phase constructor.
     *
     * @param aObs Observer.
     */       
    void ConstructL( MTsFastSwapPreviewObserver& aObs );
       
private:    

    /**
     * Implementation class.
     */   
    CTsFastSwapPreviewProviderClient* iClient;
    };

#endif // TSFASTSWAPPREVIEWPROVIDER_H

// End of file
