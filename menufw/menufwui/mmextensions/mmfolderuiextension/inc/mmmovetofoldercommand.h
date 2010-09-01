/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Add folder command
 *
*/



#ifndef MMMOVETOFOLDERCOMMAND_H
#define MMMOVETOFOLDERCOMMAND_H

#include <e32base.h>
#include "mmfoldercommand.h"

class CMmFolder;

/**
 * CMmMoveToFolderCommand is a class inherits from CMmFolderCommand.
 * It's responsible for handling command adding folder to MCS. Plugin supports
 * MMCreateNewFolderCommand command. It displays query, get name and executes
 * MCS query creating new folder. After successfully folder creation it send
 * request to HN engine to set focus on added folder.
 *
 * @lib mmfolderuiextensionplugin.lib
 * @since S60 5.0
 * @ingroup group_mmfolderuiextensionplugin
 */
NONSHARABLE_CLASS( CMmMoveToFolderCommand ) : public CMmFolderCommand
    {
public:
    /** 
     * Instantiates an object of this type.
     * @return Instance of the CMmMoveToFolderCommand class.
     */
    static CMmMoveToFolderCommand* NewL();

    /**
     * Virtual C++ Destructor.
     */
    ~CMmMoveToFolderCommand();

private:
    
    /**
     * Performs add folder operation.
     * @param aEventParamList List of the event parameters.
     */
    void ExecuteActionL( CLiwGenericParamList* aEventParamList );
    
    /**
     * Handles notifications caused by an asynchronous Execute*CmdL call
     * or an event.
     * @param aCmdId The service command associated to the event.
     * @param aEventId Occured event, see LiwCommon.hrh.
     * @param aEventParamList Event parameters, if any, as defined per
     *        each event.
     * @param aInParamList Input parameters, if any, given in the
     *        related HandleCommmandL.
     * @return Error code for the callback.
     */
    TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
                        CLiwGenericParamList& aEventParamList,
                        const CLiwGenericParamList& aInParamList );
    
    /**
     * Default C++ constructor.
     */
    CMmMoveToFolderCommand();
    
    /**
     * Second phase Symbian constructor.
     */
    void ConstructL();

    /**
     * Second phase Symbian constructor.
     */
    CMmFolder* CurrentFolder();
    
    /**
     * Selects folder.
     */
    void SelectFolderL( const RPointerArray< CMmFolder >& aFolderContent );
    
    /**
     * Moves item with given id to another folder.
     * @param aId Id of an item to be moved.
     * @param aFolderId Id of the target folder. 
     *        If KErrNotFound than item moved in 
     *        hierarchy is the current folder.
     * @param aBeforeItemId Id of the item after which an item should be 
     *        placed. If KErrNotFound than items are placed at the end of the
     *        folder. 
     */
    void MoveToFolderL( TInt aId, TInt aFolderId,
                        TInt aBeforeItemId = KErrNotFound );
    
    
    /**
     * Gets index of the selected item.
     * @param aItems Folder list.
     * @param aCurrentFolderId Id of the current folder.
     * @return Index of the selected item.
     */
    TInt SelectFolderFromListL( CDesCArrayFlat* aItems, TBool aIsRoot );
    
    /**
     * Provides array contains folder icons.
     * @return Array of icons.
     */
    CArrayPtr<CGulIcon>* GetFolderIconsL();
    
    /**
     * Browses folder from list.
     * @param aParentId Parent folder id.
     * @param aMovingItemId Id of the moving item.
     * @param aDestinationFolderId Id of the selected folder.
     * @param aWidgetType Type of the current widget.
     */
    void BrowseFolderL( TInt aParentId, TInt aMovingItemId,
            TInt& aDestinationFolderId, const TDesC& aWidgetType );
    
    /**
     * Displays notification.
     */
    void NotificationMovingCompleteL();
    
    /**
     * Displays notification.
     */
    void NotificationItemAlreadyStoredL();
    
    /**
     * Gets default name of root folder.
     * @param aIdentifier Identifier of resource.
     * @return Descriptor from resource.
     */
    HBufC* GetFromResourceLC( TInt aIdentifier );
    
    /**
     * Removes last folder from opened hierarchy
     */
    void StepBackInHierarchy();
    
    
    /**
     * Handles MoveToFolder operation
     */
    void ExecuteMoveToFolderL();
    
    /**
     * Fill aFolderArray with folders from GetList output
     */
    void GetFoldersL(RPointerArray<CMmFolder >& aFolderArray);
    
private:
    
    /**
     * Id of the parent folder.
     */
    TInt32 iParentId;
    
    TInt iItemId;
    
    TBool iFlagLeave;
    
    RBuf iWidgetType;
    
    RBuf iItemName;
    
    RBuf iFolderName;
    
    RPointerArray< CMmFolder > iOpenedHierarchy;
    
    };



NONSHARABLE_CLASS( CMmFolder ) : public CBase
    {
public:
    /** 
     * Instantiates an object of this type.
     * @return Instance of the CMmMoveToFolderCommand class.
     */
    static CMmFolder* NewL();

    /**
     * Virtual C++ Destructor.
     */
    ~CMmFolder();
    
    /**
     * Setter for the folder name.
     * @param aName Name of the folder.
     */
    void SetNameL( const TDesC& aName );
    
    /**
     * Getter for the folder name.
     * @return Name of the item.
     */
    const TDesC& Name() const;
    
    /**
     * Setter for the folder id.
     * @param aId Id of the folder.
     */
    void SetId( TInt aId );
    
    /**
     * Getter for the folder id.
     * @return Id of the folder.
     */
    TInt Id();
    
private:
    /**
     * Default C++ constructor.
     */
    CMmFolder();
    
    /**
     * Second phase Symbian constructor.
     */
    void ConstructL();
    
private:
    /**
     * Own.
     * Name of the folder.
     */
    RBuf iName;
    
    /**
     * Id of the folder.
     */
    TInt iId;
    
    };

#endif /*MMMOVETOFOLDERCOMMAND_H*/

// End of file
