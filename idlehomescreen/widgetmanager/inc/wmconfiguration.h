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

    /** Operator Application type */
    enum TOpAppType
        {
        EUnknown,
        ES60,
        ECwrt,
        EWrt,
        EJava,
        EQt,
		EReserved
        };

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
    
    /**
     * Returns bool if buttons are mirrored.
     * @return true when operator button on left on portraid and 
     * on top on landscape.
     */
    TBool PortalButtonsMirrored();
    
    /**
     * Returns application type by button index
     * @param aIndex button index
     * @return application type
     */
    TOpAppType PortalButtonApplicationType( TInt aIndex );
    
    /**
     * Returns application id as string
     * @param aIndex button index
     * @param aOperatorAppId string to get app id
     */
    void PortalButtonApplicationId( TInt aIndex, TDes& aOperatorAppId );
    
    /**
     * Returns application id as uid
     * @param aIndex button index
     * @param aOperatorAppId uid to get app id
     */
    void PortalButtonApplicationId( TInt aIndex, TUid& aOperatorAppId );
    
private: // New functions
    
    void LoadConfigurationL();
    TInt FindCorrectLanguageId();
    HBufC* ReadDescParameterL( TInt aKey );
    void ReadInt32Parameter( TInt aKey, TInt32& aValue );
    void ReadIntParameter( TInt aKey, TInt& aValue );
    HBufC* ReadLocalisedParameterL( TInt aOffset );
    void ReadOperatorApplicationInfoL();
    TUid StringToUid( const TDesC& aStr );
    
    /** Returns uid of the widget with a particular bundle identifier. */
    TUid FetchWidgetUidFromRegistryL( const TDesC& aBundleId );
    
    /** sets operator icon to button if not already defined */
    void SetOperatorIconL( TUid aUid );
	
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
     * OVI store laucher uid
     */
    TUid                    iOviStoreUid;
    
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
    
    /**
     * OPERATOR store button application parameter
     */
    HBufC*                  iOperatorParam;
    
    /**
     * OPERATOR store application id as string. Used for java
     */
    HBufC*                  iOperatorAppIdStr;
    
    /**
     * OPERATOR store application id as uid.
     */
    TUid                    iOperatorAppIdUid;
        
    /**
     * OPERATOR store application type
     */
    TOpAppType              iOperatorAppType;
        
    /**
     * Tells if operator button is enabled. 
     */
    TBool                   iOperatorButtonEnabled;
        
    /**
     * Tells if buttons need to be mirrored for operator. 
     */
    TBool                   iButtonsMirrored;
	};

#endif // WMCONFIGURATION_H

// End of File
