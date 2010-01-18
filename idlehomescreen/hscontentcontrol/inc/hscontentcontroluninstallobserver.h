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
* Description: Interface for getting notified for SWI uninstall event.
*
*/


#ifndef HSCONTENTCONTROLUNINSTALLOBSERVER_H
#define HSCONTENTCONTROLUNINSTALLOBSERVER_H

/**
* MHsContentControlUninstallObserver
* 
* @brief The observer of uninstall operations. The derived class needs to implement
*  the functions below and will be notified by CHsContentControlUninstallMonitor.
*  
* @see CHsContentControlUninstallMonitor
*/
class MHsContentControlUninstallObserver
	{
public:
    /**
     * Notification of Uninstall event from SWI.
     * @param aPkgUid The package UID which is being uninstalled. 
     */
    virtual void HandleUninstallEvent( const TUid& aPkgUid ) = 0;
	};

#endif  // HSCONTENTCONTROLUNINSTALLOBSERVER_H

// End of file
