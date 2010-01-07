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
class CAknStylusPopUpMenu;
class CEikonEnv;
class CAknButton;
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
     * Loads a button from the widgetmanager resource file
     * using CAknButton::ConstructFromResourceL
     * 
     * @param aResource handle to the empty button instance
     * @param aResourceId ID of the resource to use in loading
     */
    void LoadButtonL(
            CAknButton& aResource,
            TInt aResourceId );

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
    const TDesC& NoDescriptionText();
    
private: // internal methods

    /** constructor */
    CWmResourceLoader(
            CEikonEnv& aEnv );
    
    /** 2nd phase constructor */
    void ConstructL();

    /** load the resource */
    void LoadResourcesL();

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
     * resource file offset
     */
    TInt        iResourceFileOffset;

    /**
     * ICON file
     */
    TFileName   iMifFile;
    
    /**
     * info note dialog
     */
    CAknInformationNote *iNote;

    /**
     * Default description text
     */    
    HBufC*      iDescription;
    };

#endif // WMRESOURCELOADER_H_
