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
* Declares Store for WidgetManager
*
*/

#ifndef ___WMSTORE_H__
#define ___WMSTORE_H__

#include <AknServerApp.h> //MAknServerAppExitObserver

#include "wmcommon.h"

// FORWARD DECLARATIONS
class CWmMainContainer;
class CFbsBitmap;
class CWmConfiguration;
class CWmConfItem;
class CWmProcessMonitor;
class CBrowserLauncher;
class CWmImageConverter;
class CWmResourceLoader;

/**
 * Store ( ovi, operator ) class for Wm
 * 
 * @class CWmStore wmStore.h
 */
NONSHARABLE_CLASS( CWmStore ) : public CBase,
                                 public MAknServerAppExitObserver
    {
public:
    /** Store Id */
    enum TStoreId
        {
        EUnknown = 0,
        EOvi,
        EStore1,
        EStore2, // for future use. Can be extented
        ELastItem // do not use as store id
        };

    /*
     * Two-phased constructor.
     *
     * @param aStoreIndex index of this Store (0 or 1)
     * @param aWmConfiguration Store config 
     * @param aResourceLoader wm resource loader
     */
    static CWmStore* NewL( CWmConfItem& aWmConfItem,
            CWmResourceLoader& aResourceLoader );
    
    /** Destructor */
    virtual ~CWmStore();
                                      
public: // New Functions

    /**
     * Executes action for store
     */
    void ExecuteL();
    
    /**
     * Icon of store
     * 
     * @return icon of Store ( ownership not transfered ) 
     */
    CFbsBitmap* StoreIcon();
    
    /**
     * Mask for icon of store.
     * 
     * @return mask of Store icon ( ownership not transfered ) 
     */
    CFbsBitmap* StoreMask();

    /**
     * Heading of store
     * 
     * @return heading of store. 
     */
    const TDesC& Heading();
    
    /**
     * Order of store
     * 
     * @return order of store. 
     */
    TInt Order() const;
    
protected: // from MAknServerAppExitObserver

    /** Observes when browser is exited  */
    void HandleServerAppExit( TInt aReason );
   
protected: // Constructors
    
    /** Constructor for performing 1st stage construction */
    CWmStore( CWmConfItem& aWmConfItem );
    
    /** 2nd phase constructor */
    void ConstructL( CWmResourceLoader& aResourceLoader );
    
private:
    
    /** Runs ovi store launcher */
    void RunOviL();
    
    /**  Starts browser */ 
    void StartBrowserL( const TDesC& aUrl );
    
	/** Starts application */
    void RunApplicationL();
    
	/** Starts rprocess by given values */
    void StartProcessL( TUid aUid, const TDesC& aParam );
    	
    /**
     * Opens cwrt/wrt widget to fullscreen. 
     * @param aAppUid Uid of the widget
     * @param aParams Optional parameters
     * */
    void StartWidgetL( TUid aAppUid, const TDesC& aParams );
    
private: //data members

    /**
     * the image converter utility
     */    
    CWmImageConverter*  iImageConverter;

    /**
     * Configuration item for store data
     */
    CWmConfItem& iWmConfItem;
    
    /** icon */
    CFbsBitmap* iIcon;
    
    /** icon mask */
    CFbsBitmap* iMask;
    
    /**
     * Monitors process 
     */
    CWmProcessMonitor* iProcessMonitor;

    /** 
     * Broswer launcher. 
     */
    CBrowserLauncher* iBrowserLauncher;

 	};

#endif //___WMSTORE_H__

// End of File
