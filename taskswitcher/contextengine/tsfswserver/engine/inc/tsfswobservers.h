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
* Description:  Observer interfaces to get notifications from appui and from engine
 *
*/


#ifndef TSFSWOBSERVERS_H
#define TSFSWOBSERVERS_H

/**
 * Interface to get notifications from appui when window groups
 * have changed.
 */
class MTsFswTaskListObserver
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
class MTsFswResourceObserver
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
class MTsFswEngineObserver
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
 * This class is interface for CTsFsHiddenAppList
 * to notify about list change
 */
class MTsFsHiddenAppListObserver
    {
public:
    /**
     * This function is called by CTsFsHiddenAppList when list is changed.
     */
    virtual void HiddenAppListUpdated() = 0;
    };

#endif
