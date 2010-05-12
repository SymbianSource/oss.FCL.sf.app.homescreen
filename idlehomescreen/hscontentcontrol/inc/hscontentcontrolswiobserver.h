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
* Description: Interface for getting notified for Swi events.
*
*/


#ifndef HSCONTENTCONTROLSWIOBSERVER_H
#define HSCONTENTCONTROLSWIOBSERVER_H

/**
* MHsContentControlSwiObserver
* 
* @brief The observer of Swi operations. The derived class needs to implement
*  the functions below and will be notified by CHsContentControlSwiListener
*  
* @see CHsContentControlSwiListener
*/
class MHsContentControlSwiObserver
	{
public:
    /**
     * Notification of Swi event ( install/uninstall/restore)
     * @param aUidList list of package UID which is being processed.
     */
    virtual void HandleSwiEvent( const RArray<TUid>& aUidList ) = 0;
	};

#endif  // HSCONTENTCONTROLSWIOBSERVER_H

// End of file
