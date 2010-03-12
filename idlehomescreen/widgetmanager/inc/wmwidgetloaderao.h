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
* Active object to load widgets into list
*
*/

#ifndef WMWIDGETLOADERAO_H_
#define WMWIDGETLOADERAO_H_

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CWmPlugin;
class CWmListBox;
class CHsContentInfo;
class CWmWidgetData;
class CWmPersistentWidgetOrder;
class RWidgetRegistryClientSession;

// CLASS DECLARATIONS

/**
 * Active object to load widgets into list
 */
NONSHARABLE_CLASS( CWmWidgetLoaderAo ) : public CAsyncOneShot
    {
    
public:
	/**
     * Static constructor
     */
	static CWmWidgetLoaderAo* NewL(
	        CWmPlugin& aWmPlugin,
	        CWmListBox& aTargetList );
	
    /**
     * Destructor.
     */
	~CWmWidgetLoaderAo();

    /**
     * Starts the load process by activating the AO
     */
    void StartLoading();
    
private:
    
    /**
     * Constructor
     */
    CWmWidgetLoaderAo(
            CWmPlugin& aWmPlugin,
            CWmListBox& aTargetList );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();

protected: // from CActive
    
    /**
     * AO body
     */
    void RunL();

    /**
     * AO error handler
     */
    TInt RunError( TInt aError );

private:

    /**
     * loads widgets into the listbox
     */
    void DoLoadWidgetsL();

    /**
     * finds a widget data entry from iWidgetsList that matches
     * given content info. returns a pointer to the data found,
     * or NULL if not found
     */
    CWmWidgetData* FindWidgetData( CHsContentInfo& aContentInfo );
    
    /**
     * adds a widget data entry to iWidgetsList
     * takes ownership of aContentInfo
     */
    void AddWidgetDataL( CHsContentInfo* aContentInfo, TInt& aCount );
    
    /**
     * cleanup resources allocated runing one single run
     */
    void Cleanup();
    
    /** Converts uid to TUid from TDesC8 */
    TUid UidFromString( const TDesC8& aUidString ) const;
    
private: // data

    /** reference to the widget manager root */
    CWmPlugin& iWmPlugin;

    /** target where widgets are to be loaded */
    CWmListBox& iWidgetsList;
    
    /** widget registry */
    RWidgetRegistryClientSession* iWidgetRegistry;
    
    /** persistent widget order */
    CWmPersistentWidgetOrder* iWidgetOrder;
    
	/** uid of currently unistalled widget */
    TUid iUninstallUid;
    };

#endif // WMWIDGETLOADERAO_H_
