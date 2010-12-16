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
* loads widget manager resources and icons
*
*/


#ifndef WMRESOURCELOADER_H_
#define WMRESOURCELOADER_H_

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikonEnv;
class CAknInformationNote;

/**
 * CWmResourceLoader
 */
NONSHARABLE_CLASS( CWmResourceLoader ) : public CBase
    {

public: // construction/destruction
	/** 
	 * static constructor 
	 * 
	 * @param aCoeEnv Control environment.
	 */
	static CWmResourceLoader* NewL(
	        CEikonEnv& aEnv );
	
	/** destructor */
	~CWmResourceLoader();

public: // services

    /**
     * shows a simple info popup with OK button
     * @param aResourceId the resource id of string to be shown
     * @param aString additional string to be replaced into %U
     */
    void InfoPopupL( TInt aResourceId, const TDesC& aString );

    /**
     * shows a standard error popup
     * @param aErrorCode a system-wide standard error code 
     */
    void ErrorPopup( TInt aErrorCode );

    /** icon file path */
    const TDesC& IconFilePath();

    /** default no description text */
    const TDesC& NoDescription();

    /** wrt specific description text */
    const TDesC& WrtDescription();
    
    /** uninstalling text to replace widget name */
    const TDesC& Uninstalling();
    
    /** query popup*/
    TBool QueryPopupL( TInt aResourceId, const TDesC& aString );
	
private: // internal methods

    /** constructor */
    CWmResourceLoader(
            CEikonEnv& aEnv );
    
    /** 2nd phase constructor */
    void ConstructL();

    /** load widgetmanager resource */
    void LoadWmResourcesL();
    
	/** load installer resource */
    void LoadInstallerResourceFileL();

    /** free resources */
    void UnloadResources();

    /** find icon file path */
    void DetermineIconFilePath();

private: // members
    
    /**
     * the environment
     */
    CEikonEnv&    iEnv;

    /**
     * name of this DLL
     */
    TFileName   iDllName;

    /**
     * ICON file
     */
    TFileName   iMifFile;
    
    /**
     * info note dialog
     */
    CAknInformationNote *iNote;

    /**
     * Default no description text
     */    
    HBufC*      iNoDescription;

    /**
     * Wrt specific description text
     */    
    HBufC*      iWrtDescription;
    
    /**
     * text to replace title
     */    
    HBufC*      iUninstalling;
    
    /**
     * resource file offsets
     */
    RArray<TInt> iResourceFileOffsets;
    };

#endif // WMRESOURCELOADER_H_
