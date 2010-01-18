/*
 * ===========================================================================
 *  Name        : hgfswengine.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Task monitor engine
 *  Version     : %version: sa1spcx1#22 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#ifndef __HGFSWENGINE_H
#define __HGFSWENGINE_H

#include <e32base.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <fbs.h>
#include <eikenv.h>
#include <w32std.h>
#include <e32hashtab.h>
#include <apgcli.h>
#include <e32property.h>
#include "hgpreviewobserver.h"
#include "hgfswentry.h"
#include "hgfswobservers.h"

class CHgFsAlwaysShownAppList;
class CHgFsHiddenAppList;
class CHgFsWidgetList;
class MHgFswEngineObserver;
class CHgFswIconCache;
class CHgFastSwapPreviewProvider;
class CApaWindowGroupName;
//class CHgContextUtility;

// descriptor big enough to store hex repr of 32-bit integer plus 0x prefix
typedef TBuf<10> TAppUidHexString;

/**
 * Engine for fast swap server.
 * Performs task monitoring, notifications about wg changes are
 * received from the appui.
 */
NONSHARABLE_CLASS( CHgFswEngine ) : public CActive,
        public MHgFswTaskListObserver,
        public MHgFswResourceObserver,
        public MHgFsHiddenAppListObserver,
        public MHgFastSwapPreviewObserver
    {
public:
    /**
     * Creates a new instance.
     * @param   aObserver   ref to observer
     */
    IMPORT_C static CHgFswEngine* NewL( MHgFswEngineObserver& aObserver );
    
    /**
     * @copydoc NewL
     */
    IMPORT_C static CHgFswEngine* NewLC( MHgFswEngineObserver& aObserver );
    
    /**
     * Destructor.
     */
    ~CHgFswEngine();

    /**
     * Returns a reference to the current content.
     * Also performs sanity checks, e.g. associates application icons
     * when no screenshot has been received.
     * @return  ref to content array
     */
    IMPORT_C const RHgFswArray& FswDataL();
    
    /**
     * Tries to close the given app.
     * @param   aWgId   value given by WgId() for an entry in iData
     */
    IMPORT_C void CloseAppL( TInt aWgId );
    
    /**
     * Brings the given app to foreground.
     * @param   aWgId   value given by WgId() for an entry in iData
     */
    IMPORT_C void SwitchToAppL( TInt aWgId );

    /**
     * Returns the uid of the foreground app or KNullUid.
     * Will never return hidden apps, only those which can
     * also be seen in the array returned by FswDataL.
     *
     * In case of embedded apps the behaviour is controlled
     * by aType: it will return either the uid of the embedded
     * application or the container app.
     *
     * @param   aType   @see CHgFswClient::THgFswFgAppType
     */
    IMPORT_C TUid ForegroundAppUidL( TInt aType );

private:
    // from CActive
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();

    // from MHgFswTaskListObserver
    void UpdateTaskList();
    
    // from MHgFswResourceObserver
    void HandleResourceChange( TInt aType );

    // from MHgFsHiddenAppListObserver
    void HiddenAppListUpdated();

    // from MHgFastSwapPreviewObserver
    void HandleFswPpApplicationChange( TInt aWgId, TInt aFbsHandle );
    void HandleFswPpApplicationUnregistered( TInt aWgId );

private:
    /**
     * Constructor.
     */
    CHgFswEngine( MHgFswEngineObserver& aObserver );
    
    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();
    
    /**
     * Gets the window group list and reconstructs the fsw content.
     * @return   TBool   ETrue if the list has been modified
     */
    TBool CollectTasksL();
    
    /**
     * Called from CollectTasksL for each entry in the task list.
     * @param   aWgId       window group id
     * @param   aAppUid     application uid
     * @param   aWgName     window group name or NULL
     * @param   aNewList    list to add to
     * @param   aIsWidget   true if the entry corresponds to a web widget
     * @return  TBool   ETrue if it was really a new entry in the list
     */
    TBool AddEntryL( TInt aWgId, const TUid& aAppUid,
        CApaWindowGroupName* aWgName, RHgFswArray& aNewList,
        TBool aIsWidget );

    /**
     * Checks if there is an entry for same app in the content list.
     * If yes then it takes some of the data for the entry that
     * will correspond to the same app in the refreshed content list.
     * @param   aEntry      new entry in content list
     * @param   aAppUid     application uid
     * @param   aChanged    ref to change-flag, set to ETrue if it is sure
     * that the new content list will be different from the previous one
     * @param   aNewList    ref to new content list
     * @return  ETrue if app was found
     */
    TBool CheckIfExistsL( CHgFswEntry& aEntry,
        const TUid& aAppUid,
        TBool& aChanged,
        RHgFswArray& aNewList );

    /**
     * Adds running widgets to the list.
     * @param   aNewList    array to add to
     */
    void CheckWidgetsL( RHgFswArray& aNewList );

    /**
     * Finds out the app uid for the given window group id.
     * @param   aWgId   a valid window group id
     * @return  application uid
     */
    TUid AppUidForWgIdL( TInt aWgId );
    
    /**
     * Returns the parent's wg id or KErrNotFound.
     * @param   aWgId   a valid window group id
     * @return parent wg id or KErrNotFound if there is no parent
     */
    TInt FindParentWgId( TInt aWgId );
    
    /**
     * Finds out the application name.
     * @param   aWindowName window group name or NULL
     * @param   aAppUId     application uid
     * @param   aWgId       window group id
     * @return  application name, ownership transferred to caller
     */
    HBufC* FindAppNameLC( CApaWindowGroupName* aWindowName,
        const TUid& aAppUid, TInt aWgId );

    /**
     * Makes a copy of the bitmap with the given handle.
     * @param   aFbsHandle          bitmap handle
     * @param   aKeepAspectRatio    if true then aspect ratio is kept
     * @return  CFbsBitmap*     the copy, ownership transferred to caller
     */
    CFbsBitmap* CopyBitmapL( TInt aFbsHandle, TBool aKeepAspectRatio );
    
    /**
     * Checks if the app to which the screenshot belongs is in the task list
     * and updates the entry when found. Also triggers change notification
     * to observer when data is modified.
     * @param   aWgIdForScreenshot  wgid for the screenshot 
     * @param   aBitmapHandle       handle for screenshot bitmap
     */
    void AssignScreenshotHandle( TInt aWgIdForScreenshot, TInt aBitmapHandle );
    
    /**
     * Returns the bitmap handle for the screenshot belonging to the given app
     * or 0 if not (yet) found.
     * @param   aWgIdForApp     a window group id (from the window group list)
     * @return  bitmap handle or 0
     */
    TInt LookupScreenshotHandle( TInt aWgIdForApp );
    
    /**
     * Callback for the iUpdateStarter timer.
     * Calls CollectTasksL and notifies the observer if the task list
     * has really been modified.
     */
    static TInt UpdateStarterCallback( TAny* aParam );

    /**
     * Brings the given web widget to foreground.
     * @param   aWidgetIndex    index in the list of running widgets
     */
    void SwitchToWidgetL( TInt aWidgetIndex );

    /**
     * Configues the preview provider with current screen size.
     */
    void SetPreviewParams();

    /**
     * Registers and unregisters window groups to iPreviewProvider
     * if the window group list has been changed.
     */
    void UpdatePreviewContent();

    /**
     * Gets and publishes the foreground app uid to CFW.
     */
    void PublishFgAppUidL();

    /**
     * Helper function to publish something to CFW.
     * @param   aType   context type
     * @param   aValue  value to publish
     */
    //void PublishContextL( const TDesC& aType, const TDesC& aValue );

private: // data    
    MHgFswEngineObserver& iObserver;
    RHgFswArray iData; // current fsw content, i.e. the task list
    CEikonEnv* iEnv; // not own

    // always shown app list, own    
    CHgFsAlwaysShownAppList* iAlwaysShownAppList;
    
    // hidden app list, own
    CHgFsHiddenAppList* iHiddenAppList;
    
    // web widget list, own
    CHgFsWidgetList* iWidgetList;

    // window server session
    RWsSession iWsSession;
    
    // apparc session
    RApaLsSession iAppArcSession;

    /**
     * Hash table storing the screenshots.
     * Key: window group id for the screenshot
     * (the one received in ApplicationChange)
     * Value: CFbsBitmap*, pointers owned
     */    
    RHashMap<TInt, CFbsBitmap*> iScreenshots;
    
    // preview provider instance, own
    CHgFastSwapPreviewProvider* iPreviewProvider;
    
    // timer to defer content refresh
    CPeriodic* iUpdateStarter;
    
    // app icon provider/container instance, own
    CHgFswIconCache* iAppIcons;
    
    // true if web widgets are supported by the system
    TBool iWidgetsSupported;
    // wgid of widget appui is saved here
    TInt iWidgetAppUiWgId;

    // PS property to listen for swi status changes
    RProperty iSwiProp;
    // when true CollectTasksL will call GetAllApps etc.
    // which is slow and need not be done normally, except
    // during startup and perhaps when new applications are installed
    TBool iAppDataRefreshNeeded;

    // window group ids returned by last WindowGroupList call
    RArray<TInt> iWgIds;
    
    // Dirty flag, indicates that iData is not up-to-date because
    // there were no subscribed clients during a previous possible
    // change of the task list.
    TBool iTaskListDirty;

    // For publishing the foreground app uid to Context Framework    
    //CHgContextUtility* iContextUtility;
    TAppUidHexString iFgAppUidStr;
    TUid iFgAppUid;

    };

#endif
