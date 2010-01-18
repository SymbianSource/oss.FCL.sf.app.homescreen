/*
 * ===========================================================================
 *  Name        : hgfswobservers.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Observer interfaces to get notifications from appui and from engine
 *  Version     : %version: sa1spcx1#4 %
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

#ifndef __HGFSWOBSERVERS_H
#define __HGFSWOBSERVERS_H

/**
 * Interface to get notifications from appui when window groups
 * have changed.
 */
class MHgFswTaskListObserver
    {
public:
    /**
     * Function called by AppUi to notify about possible
     * changes in task list
     */
    virtual void UpdateTaskList() = 0;
    };

/**
 * Interface to get notifications from appui's HandleResourceChangeL.
 */
class MHgFswResourceObserver
    {
public:
    /**
     * Function called by AppUi to notify about resource changes.
     */
    virtual void HandleResourceChange( TInt aType ) = 0;
    };

/**
 * Observer interface to get notifications from engine
 * when fsw content has changed.
 */
class MHgFswEngineObserver
    {
public:
    /**
     * Called when there is a change in the task list.
     */
    virtual void FswDataChanged() = 0;

    /**
     * Called to find out how many clients are subscribed
     * for fsw content change notifications.
     */
    virtual TInt FswDataListenerCount() = 0;
    };

/**
 * This class is interface for CHgFsHiddenAppList
 * to notify about list change
 */
class MHgFsHiddenAppListObserver
    {
public:
    /**
     * This function is called by CHgFsHiddenAppList when list is changed.
     */
    virtual void HiddenAppListUpdated() = 0;
    };

#endif
