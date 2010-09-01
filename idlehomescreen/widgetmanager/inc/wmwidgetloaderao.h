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

class MWmWidgetloaderObserver
    {
public:
    /**
     * Notifies client when widget list is succesfully loaded
     *
     * @param aWidgetListChanged true if widget list changed
     */
    virtual void LoadDoneL( TBool aWidgetListChanged ) = 0;
    };
    
/**
 * Active object to load widgets into list
 */
NONSHARABLE_CLASS( CWmWidgetLoaderAo ) : public CActive
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
    
    /**
     * Is loading ongoing.
     */
    TBool IsLoading();

    /**
     * Set MWmWidgetloaderObserver observer
      */
    void SetObserver( MWmWidgetloaderObserver* aObserver );
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
    * Handles an active object's request completion event.
    * 
    * @see CActive::RunL
    */
    void RunL();

    /**
     * RunError
     * 
     * @see CActive::RunError
     */
    TInt RunError( TInt aError );
    
    /**
     * Implements cancellation of an outstanding request.
     * 
     * @see CActive::DoCancel
     */
    void DoCancel();

private:

    /**
     * connects to wrt registry
     */
    void OpenSessionL();

    /**
     * disconnects from wrt registry
     */
    void CloseSession();
    
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
    
    /** switch for loading operation */
    TBool iLoading;
    
    /** Notifies client when widget list is fully loaded */
    MWmWidgetloaderObserver* iObserver;
    
    /** tells if widgetlist has changed */
    TBool iWidgetListChanged;
    };

#endif // WMWIDGETLOADERAO_H_
