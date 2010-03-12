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
     * Logo syntax follows the widget icon syntax in CWmImageConverter
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonIcon( TInt aIndex );
    
    /**
     * Returns url to for browser by button index
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonBrowserUrl( TInt aIndex );
    
    /**
     * Returns client uid by button index 
     * @param aIndex index of the button, starting at 0
     */
    TUid PortalButtonClientUid( TInt aIndex );
    
    /**
     * Returns client parameter by button index 
     * @param aIndex index of the button, starting at 0
     */
    const TDesC& PortalButtonClientParam( TInt aIndex );
        
private: // New functions
    
    void LoadConfigurationL();
    TInt FindCorrectLanguageId();
    HBufC* ReadDescParameterL( TInt aKey );
    void ReadIntParameter( TInt aKey, TInt32& aValue );
    HBufC* ReadLocalisedParameterL( TInt aOffset );
    void IndexConversion( TInt& aIndex );

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
     * OVI store button text
     */
    HBufC*                  iOviButtonTitle;
    
    /**
     * OVI store button icon
     */
    HBufC*                  iOviButtonIcon;
    
    /**
     * OVI store button widget client param
     */
    HBufC*                  iOviStoreClientParam;
    
    /**
     * OVI store laucher ui
     */
    TUid                      iOviStoreUid;
    
    /**
     * OPERATOR button text
     */
    HBufC*                  iOperatorButtonTitle;
    
    /**
     * OPERATOR store icon
     */
    HBufC*                  iOperatorButtonIcon;
    
    /**
     * OPERATOR store button browser Url
     */
    HBufC*                  iOperatorButtonUrl;
    
    };

#endif // WMCONFIGURATION_H

// End of File
