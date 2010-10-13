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
* Description:  Non-closeable app list class
 *
*/


#ifndef TSFSWDATALIST_H
#define TSFSWDATALIST_H

#include <e32base.h>
#include <e32hashtab.h>
#include <fbs.h>
#include <apgcli.h>
#include <w32std.h>
#include "tsfswentry.h"
#include "tsfswobservers.h"

class CTsFswEngine;
class CTsFsAlwaysShownAppList;
class CTsFsHiddenAppList;
class CTsFsWidgetList;
class CTsFswIconCache;
class CApaWindowGroupName;

/**
 * Engine's data list.
 */
NONSHARABLE_CLASS( CTsFswDataList ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aParent
     */
    static CTsFswDataList* NewL( CTsFswEngine& aEngine );

    /*
     * Destructor
     */
    ~CTsFswDataList();

private:
    /**
     * Constructor.
     */
    CTsFswDataList( CTsFswEngine& aEngine );

    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();

public:
    /**
     * Returns a reference to the current content.
     * Also performs sanity checks, e.g. associates application icons
     * when no screenshot has been received.
     * @return  ref to content array
     */
    const RTsFswArray& FswDataL();

    /**
     * Gets the window group list and reconstructs the fsw content.
     * @return   TBool   ETrue if the content iData has been modified
     */
    TBool CollectTasksL();

    /**
     * Return contained iAlwaysShownAppList member.
     */
    CTsFsAlwaysShownAppList* AlwaysShownApps();

    /**
     * Return contained iHiddenAppList member.
     */
    CTsFsHiddenAppList* HiddenApps();

    /**
     * Return containd iWidgetList member
     */
    CTsFsWidgetList* Widgets();

    /**
     * Set flag iAppDataRefreshNeeded
     */
    void SetAppDataRefreshNeeded();

    /**
     * Store or updarte screenshot bitmap for specified window group.
     */
    TBool StoreScreenshot( TInt aWgId, CFbsBitmap* aBmp );

    /**
     * remove screenshot bitmap.
     */
    TBool RemoveScreenshot( TInt aWgId );

    /**
     * Obtain screenshot if exists
     */
    CFbsBitmap** FindScreenshot( TInt aWgId );

    /**
     * Moves entry on given window app/widget id to 
     * the first position on conten data list
     */
    TBool MoveEntryAtStart( TInt aAppId, TBool aWidget );
    
    /**
     * If application uid exists in the contained CTsFsAlwaysShownAppList 
     * @return   ETrue  application uid exists in the list
     *           EFalse application uid doesn't exist in the list
     * @param    TInt aWgId the window group application
     */
    TBool IsAlwaysShownAppL( TInt aWgId );
    
    /**
     * Finds out the app uid for the given window group id.
     * @param   aWgId   a valid window group id
     * @param   aUid   	requested uid
     * @return  error code
     */
    TInt AppUidForWgId( TInt aWgId, TUid& aUid );
    
    /**
     * Finds out the app uid for the given window group id.
     * @param   aWgId   a valid window group id
     * @return  application uid
     */
    TUid AppUidForWgIdL( TInt aWgId );

    /**
     * Returns the most top parent's wg id or KErrNotFound.
     * @param   aWgId   a valid window group id
     * @return parent wg id or KErrNotFound if there is no parent
     */
    TInt FindMostTopParentWgId( TInt aWgId );

    /**
     * Checks whether window group id was used by contained data.
     */
    TBool CheckForWgIdUsage( TInt aWgId );
    
private:
    /**
     * Adds running apps to the list.
     * @param   aAppsList    array to add to
     */
    void CollectAppsL( RTsFswArray& aAppsList );

    /**
     * Called from CollectTasksL for each entry in the task list.
     * @param   aWgId       window group id
     * @param   aAppUid     application uid
     * @param   aWgName     window group name or NULL
     * @param   aNewList    list to add to
     * @param   aIsWidget   true if the entry corresponds to a web widget
     * @return  TBool   ETrue if it was really a new entry in the list
     */
    TBool AddEntryL( TInt aWgId, 
			const TUid& aAppUid,
            CApaWindowGroupName* aWgName,
			RTsFswArray& aNewList,
            TBool aIsWidget );

    /**
     * Checks if there is an entry for same app in the content list.
     * If yes then it takes some of the data for the entry that
     * will correspond to the same app in the refreshed content list.
     * In case of widget, update window group field in content list.
     * @param   aEntry      new entry in content list
     * @param   aAppUid     application uid
     * @param   aChanged    ref to change-flag, set to ETrue if it is sure
     * that the new content list will be different from the previous one
     * @param   aNewList    ref to new content list
     * @return  ETrue if app was found
     */
    TBool ConsiderOldDataL( CTsFswEntry& aEntry,
        const TUid& aAppUid,
        TBool& aChanged,
        RTsFswArray& aNewList );

    /**
     * Adds running widgets to the list.
     * @param   aWidgetsList    array to add to
     */
    void CollectWidgetsL( RTsFswArray& aWidgetsList );

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
            	const TUid& aAppUid,
				TInt aWgId );

    /**
     * Checks if the app to which the screenshot belongs is in the task list
     * and updates the entry when found. Also triggers change notification
     * to observer when data is modified.
     * @param   aWgIdForScreenshot  wgid for the screenshot 
     * @param   aBitmapHandle       handle for screenshot bitmap
     * @return   TBool   ETrue if the list has been modified
     */
    TBool AssignScreenshotHandle( TInt aWgIdForScreenshot, TInt aBitmapHandle );

    /**
     * Checks if the app to which the screenshot belongs is in the task list
     * and updates the entry when found. 
     * @param   aWgIdForScreenshot  wgid for the screenshot, 
     *                              in case of widget it stands for widget id 
     * @param   aBitmapHandle       handle for screenshot bitmap
     * @param   aAsigned       		result of operation
     */
    void AssignScreenshotHandle( TInt aWgIdForScreenshot,
			TInt aBitmapHandle,
        	TBool& aAsigned );

    /**
     * Returns the bitmap handle for the screenshot belonging to the given app
     * or 0 if not (yet) found.
     * @param   aWgIdForApp     a window group id (from the window group list)
     * @return  bitmap handle or 0
     */
    TInt LookupScreenshotHandle( TInt aWgIdForApp );

    /**
     * Fit existing class contained data list into give one.
     * Data is being changed with application type consideration that is based 
     * on aConsiderWidgets param. 
     * Function removes or add entries into data depend on given list.
     * @param   aListToFit          list with actual data 
     * @param   aConsiderWidgets    application type 
     * @return  ETrue if change occours on data list, EFalse otherwise   
     */
    TBool FitDataToListL( RTsFswArray& aListToFit, TBool aConsiderWidgets );

    /**
     * Checks if there is an entry for same app in the given list.
     * @param   aEntry      entry
     * @param   aNewList    ref to list
     * @return  ETrue if app was found
     */
    TBool CheckIfExists( const CTsFswEntry& aEntry,
            const RTsFswArray& aNewList ) const;
    
    /**
     * Set flag iTaskListDirty;
     * @param aDirty new flag value. 
     */
    void SetDirty( TBool aDirty );
    
    /**
     * Removes the screenshot handle from content entry.
     * Reverts to parental screenshot, if previously was set to embeded.
     * @param aBmpHandle handle of removed screenshot. 
     */
    TBool RemoveScreenshotFromParent( TInt aBmpHandle );

private:
    CTsFswEngine& iEngine;

    RTsFswArray iData; // current fsw content, i.e. the task list

    // always shown app list, own    
    CTsFsAlwaysShownAppList* iAlwaysShownAppList;

    // hidden app list, own
    CTsFsHiddenAppList* iHiddenAppList;

    // web widget list, own
    CTsFsWidgetList* iWidgetList;

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

    // app icon provider/container instance, own
    CTsFswIconCache* iAppIcons;

    // true if web widgets are supported by the system
    TBool iWidgetsSupported;

    // when true CollectTasksL will call GetAllApps etc.
    // which is slow and need not be done normally, except
    // during startup and perhaps when new applications are installed
    TBool iAppDataRefreshNeeded;

    // Dirty flag, indicates that iData is not up-to-date because
    // there were no subscribed clients during a previous possible
    // change of the task list.
    TBool iTaskListDirty;
    };

#endif //TSFSWDATALIST_H
