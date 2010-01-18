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
* WidgetManager configuration class
*
*/

#ifndef WMCONFIGURATION_H
#define WMCONFIGURATION_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;
class CWmResourceLoader;

/**
 * Configuration class for Widget Manager
 * 
 * @class CWmConfiguration
 */
NONSHARABLE_CLASS( CWmConfiguration ) : public CBase
	{
	
public: // constructors and destructor
	
    /** Two-phased constructor. */
	static CWmConfiguration* NewL(
	        CWmResourceLoader& aResourceLoader );

	/** Destructor */
	virtual ~CWmConfiguration();
	
private:
    /** constructor */
    CWmConfiguration(
            CWmResourceLoader& aResourceLoader );
    
    /** 2nd phase constructor */
    void ConstructL();

public: // API

    /**
     * portal button methods
     */
    enum TMethod
        {
        ENone = 0, // no method
        EHttp, // open browser to a certain page
        EWidget // open given widget with params
        };

    /**
     * Number of portal buttons (1 or more)
     */
    TInt PortalButtonCount();

    /**
     * Label text to be displayed on a portal button
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonText( TInt aIndex );
    
    /**
     * Icon to be displayed on a portal button
     * Logo syntax follows the widget icon syntax in MHsContentInfo
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonIcon( TInt aIndex );
    
    /**
     * Bundle ID related to this button
     * parameter is valid if this button launches an application
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonBundleId( TInt aIndex );
    
    /**
     * Portal button action method.
     * Defines what happens when the portal button is pressed.
     * @param aIndex index of the button, starting at 0
     */
    TMethod PortalButtonPrimaryMethod( TInt aIndex );
    
    /**
     * parameters related to the method.
     * For EHttp this is the HTTP address
     * for EWidget the parameters passed to the widget
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonPrimaryParams( TInt aIndex );
    
    /**
     * Secondary method, which is excuted if
     * primary method fails.
     * Works like Primary method.
     * @param aIndex index of the button, starting at 0
     */
    TMethod PortalButtonSecondaryMethod( TInt aIndex );
    
    /**
     * Parameters for Secondary method. Like Primary method
     * Works like Primary params.
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonSecondaryParams( TInt aIndex );
    
    
private: // New functions
    
    TInt FindCorrectLanguageId();
    HBufC* ReadParameterL( TInt aKey );
    HBufC* ReadLocalisedParameterL( TInt aOffset );

private:

    /**
     * the central repository
     */
    CRepository*            iRepository;

    /**
     * the resource loader
     */
    CWmResourceLoader&      iResourceLoader;

    /**
     * The index in cenrep that matches current
     * system language
     */
    TInt                    iLanguageIndex;

    /**
     * OviStore widget bundle ID
     */
    HBufC*                  iOviStoreBundleId;
    
    /**
     * OviStore widget client param
     */
    HBufC*                  iOviStoreClientParam;
    
    /**
     * Localized Url to start browser
     */
    HBufC*                  iOviStoreUrl;
    
    /**
     * Localised OVI store text
     */
    HBufC*                  iOviStoreText;
    
    /**
     * OVI store icon
     */
    HBufC*                  iOviStoreIcon;
    
    };

#endif // WMCONFIGURATION_H

// End of File
