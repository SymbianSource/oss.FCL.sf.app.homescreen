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
#include "wmstore.h"

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

    /** Store's Application type */
    enum TStoreAppType
        {
        EUnknown,
        EOviCwrt,
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
     * Number of Stores
     */
    TInt StoreCount();
    
    /**
     * Returns Store configuration array ref
     */
    const RPointerArray<CWmConfItem>& StoreConfArray();
    
    /**
	 * To compare orders
	 */
    static TInt CompareStoreOrder( 
            const CWmConfItem& aItemOne, const CWmConfItem& aItemTwo );
    
private: // New functions
    
    void LoadConfigurationL();
    TInt FindCorrectLanguageId();
    HBufC* ReadDescParameterL( TInt aKey );
    void ReadInt32Parameter( TInt aKey, TInt32& aValue );
    void ReadIntParameter( TInt aKey, TInt& aValue );
    HBufC* ReadLocalisedParameterL( TInt aOffset );
    void ReadApplicationInfoL( TInt aKey, CWmConfItem& aConfItem );
    TUid StringToUid( const TDesC& aStr );
    
    /** Returns uid of the widget with a particular bundle identifier. */
    TUid FetchWidgetUidFromRegistryL( const TDesC& aBundleId );
    
    /** sets Store icon to  if not already defined */
    void SetStoreIconL( CWmConfItem& aConfItem );

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
     * Array of configuration items. Array is in order as they should appear in UI
     */    
    RPointerArray<CWmConfItem> iConfItems;
	};


/**
 * Configuration item class for Widget Manager
 * 
 * @class CWmConfiguration
 */
NONSHARABLE_CLASS( CWmConfItem ) : public CBase
    {
    public: // constructors and destructor

        /** Two-phased constructors. */
        static CWmConfItem* NewL();

        static CWmConfItem* NewLC();
        
        /** Destructor */
        ~CWmConfItem();
        
    private:
        /** constructor */
        CWmConfItem();
        
        /** 2nd phase constructor */
        void ConstructL();
        
public: // API
    /**
     * Returns Store id
     */
    CWmStore::TStoreId Id();
    
    /**
     * Returns Label text to be displayed on a Store 
     */
    const TDesC& Title();
    
    /**
     * Returns Icon to be displayed on a Store 
     * Logo syntax follows the widget icon syntax in CWmImageConverter
     */
    const TDesC& Icon();  

    /**
     * Returns Order of Store
     */
    TInt Order();
    
    /**
     * Returns url to for browser
     */
    const TDesC& BrowserUrl();   
   
    /**
     * Returns application type
     */
    CWmConfiguration::TStoreAppType AppType();
    
    /**
     * Returns application uid 
     */
    TUid AppUid();
    
    /**
     * Returns application parameter by index 
     */
    const TDesC& AppParam();
    
private:    
    /**
     * Store Id
     */
    CWmStore::TStoreId      iId;
    
    /**
     * Store text
     */
    HBufC*                  iTitle;
    
    /**
     * Store icon
     */
    HBufC*                  iIcon;
    
    /**
     * Store order
     */
    TInt                    iOrder;
            
    /**
     * Store browser Url
     */
    HBufC*                  iUrl;              
  
    /**
     * Application type
     */
    CWmConfiguration::TStoreAppType           iAppType;
    
    /**
     * Application uid
     */
    TUid                    iAppUid;
    
    /**
     * Application parameter
     */
    HBufC*                  iAppParam;
    
    friend class CWmConfiguration;
    };
#endif // WMCONFIGURATION_H

// End of File
