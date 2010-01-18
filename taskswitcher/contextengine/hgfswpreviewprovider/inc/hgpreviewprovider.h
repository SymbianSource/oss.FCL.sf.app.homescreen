/*
* ==============================================================================
*  Name        : %name: hgpreviewprovider.h %
*  Part of     : Hg fast swap preview provider plugin
*  Description : Preview provider API
*  Version     : %version: sa1spcx1#2 %
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef HGPREVIEWPROVIDER_H
#define HGPREVIEWPROVIDER_H

// INCLUDES
#include <e32base.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CHgFastSwapPreviewProviderClient;
class MHgFastSwapPreviewObserver;

/**
 * Preview provider API class. Applications (window groups) must be
 * registered before screenshots can be received. Also, closed apps
 * should be unregistered to save resources.
 * TakePreview() can be called to take a screenshot right away.
 * A client needing notifications has to create an instance
 * so that screenshots can be delivered to it via MHgFastSwapPreviewObserver.
 *
 * @see MHgFastSwapPreviewObserver
 * @lib hgpreviewprovider.lib
 */
NONSHARABLE_CLASS( CHgFastSwapPreviewProvider ) : public CBase
    {
public:

    /**
     * Two phased constructor.
     * @see MHgFastSwapPreviewObserver
     *
     * @param aObs Observer.     
     */   
    IMPORT_C static CHgFastSwapPreviewProvider* NewL(
        MHgFastSwapPreviewObserver& aObs );
    
    /**
     * Destructor.
     */   
    IMPORT_C ~CHgFastSwapPreviewProvider();
    
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
    CHgFastSwapPreviewProvider();
    
    /**
     * 2nd phase constructor.
     *
     * @param aObs Observer.
     */       
    void ConstructL( MHgFastSwapPreviewObserver& aObs );
       
private:    

    /**
     * Implementation class.
     */   
    CHgFastSwapPreviewProviderClient* iClient;
    };

#endif // HGPREVIEWPROVIDER_H

// End of file
