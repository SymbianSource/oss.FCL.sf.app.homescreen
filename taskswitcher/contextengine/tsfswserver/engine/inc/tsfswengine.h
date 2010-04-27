/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Task monitor engine
 *
*/


#ifndef TSFSWENGINE_H
#define TSFSWENGINE_H

#include <e32base.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <fbs.h>
#include <eikenv.h>
#include <w32std.h>
#include <e32hashtab.h>
#include <apgcli.h>
#include <e32property.h>
#include "tspreviewobserver.h"
#include "tsfswentry.h"
#include "tsfswobservers.h"

class MTsFswEngineObserver;
class CTsFastSwapPreviewProvider;
class CApaWindowGroupName;
class CBitmapRotator;
class CTsRotationTask;
class CTSCpsNotifier;

class CTsFswDataList;

// descriptor big enough to store hex repr of 32-bit integer plus 0x prefix
typedef TBuf<10> TAppUidHexString;

/**
 * Engine for fast swap server.
 * Performs task monitoring, notifications about wg changes are
 * received from the appui.
 */
NONSHARABLE_CLASS( CTsFswEngine ) : public CActive,
        public MTsFswTaskListObserver,
        public MTsFswResourceObserver,
        public MTsFsHiddenAppListObserver,
        public MTsFastSwapPreviewObserver
    {
public:
    /**
     * Creates a new instance.
     * @param   aObserver   ref to observer
     */
    IMPORT_C static CTsFswEngine* NewL( MTsFswEngineObserver& aObserver );

    /**
     * @copydoc NewL
     */
    IMPORT_C static CTsFswEngine* NewLC( MTsFswEngineObserver& aObserver );

    /**
     * Destructor.
     */
    ~CTsFswEngine();

    /**
     * Returns a reference to the current content.
     * Also performs sanity checks, e.g. associates application icons
     * when no screenshot has been received.
     * @return  ref to content array
     */
    IMPORT_C const RTsFswArray& FswDataL();

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
     * @param   aType   @see CTsFswClient::TTsFswFgAppType
     */
    IMPORT_C TUid ForegroundAppUidL( TInt aType );

public:
    /**
     * Callback for rotation completion. Takes ownership of a given
     * bitmap.
     */
    void RotationComplete( TInt aWgId, 
            CFbsBitmap* aBitmap,
            CTsRotationTask* aCompletedTask,
            TInt aError );

    /**
     * Called by CPS publisher when changes occours on widgets' CPS data.
     * Copies screenshot with use of delivered bitmap handle.
     * Moves last changed widget with entry into start position. 
     */
    void HandleWidgetUpdateL( TInt aWidgetId, TInt aBitmapHandle );

private:
    // from CActive
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();

    // from MTsFswTaskListObserver
    void UpdateTaskList();

    // from MTsFswResourceObserver
    void HandleResourceChange( TInt aType );

    // from MTsFsHiddenAppListObserver
    void HiddenAppListUpdated();

    // from MTsFastSwapPreviewObserver
    void HandleFswPpApplicationChange( TInt aWgId, TInt aFbsHandle );
    void HandleFswPpApplicationUnregistered( TInt aWgId );
    void HandleFswPpApplicationBitmapRotation( TInt aWgId, TBool aClockwise );

    void RotateL( CFbsBitmap& aBitmap, TInt aWgId, TBool aClockwise );

private:
    /**
     * Constructor.
     */
    CTsFswEngine( MTsFswEngineObserver& aObserver );

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
     * Makes a copy of the bitmap with the given handle.
     * @param   aFbsHandle          bitmap handle
     * @param   aKeepAspectRatio    if true then aspect ratio is kept
     * @return  CFbsBitmap*     the copy, ownership transferred to caller
     */
    CFbsBitmap* CopyBitmapL( TInt aFbsHandle, TBool aKeepAspectRatio );

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
     * Close running widget
     * @param aOffset - widget info offset
     */
    void CloseWidgetL( TInt aOffset );

    /**
     * Removes screenshot bitmap from contained iDataList
     */
    void RemoveScreenshot( TInt aWgId );

    /**
     * Add or overwrite screenshot datamap into iDataList
     * @return ETrue if content was changed
     */
    TBool StoreScreenshot( TInt aWgId, CFbsBitmap* aBmp );

private:
    // data    
    MTsFswEngineObserver& iObserver;
    CEikonEnv* iEnv; // not own

    // window server session
    RWsSession iWsSession;

    // apparc session
    RApaLsSession iAppArcSession;

    // preview provider instance, own
    CTsFastSwapPreviewProvider* iPreviewProvider;

    // timer to defer content refresh
    CPeriodic* iUpdateStarter;

    // true if web widgets are supported by the system
    TBool iWidgetsSupported;

    // PS property to listen for swi status changes
    RProperty iSwiProp;

    // window group ids returned by last WindowGroupList call
    RArray<TInt> iWgIds;

    // For publishing the foreground app uid to Context Framework    
    TAppUidHexString iFgAppUidStr;
    TUid iFgAppUid;

    // For rotating bitmaps
    RPointerArray<CTsRotationTask> iRotaTasks;

    CTsFswDataList* iDataList;//own

    //Used to observ wigets' actions on CPS service
    CTSCpsNotifier* iCpsWidgetPublisher;//owned
    };

/**
 * Listener for rotation complete event
 */
NONSHARABLE_CLASS( CTsRotationTask ) : public CActive
    {
public:
    CTsRotationTask( CTsFswEngine& aEngine );
    ~CTsRotationTask();
    void StartLD( TInt aWgId, CFbsBitmap* aBitmapHandle, TBool aClockwise );
private:
    void RunL();
    void DoCancel();
private:
    // Data
    CTsFswEngine& iEngine;
    TInt iWgId;
    CFbsBitmap* iBitmap; // owned for the duration of transformation
    CBitmapRotator* iRotator; // owned
    };

#endif
