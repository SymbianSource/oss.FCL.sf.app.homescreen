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
* Description:  Client API
 *
*/


#ifndef TSFSWCLIENTOBSERVER_H
#define TSFSWCLIENTOBSERVER_H

#include <e32base.h>

/**
 * Observer interface for getting notifications about fast swap content changes.
 */
class MTsFswObserver
    {
public:
    /**
     * Called when there is a change in the fast swap content.
     * The data can be queried via CTsFswClient::GetContentL
     */
    virtual void HandleFswContentChanged() = 0;
    };

#endif
