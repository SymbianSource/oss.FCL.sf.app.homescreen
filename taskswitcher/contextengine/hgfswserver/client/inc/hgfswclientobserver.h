/*
 * ===========================================================================
 *  Name        : hgfswclientobserver.h
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : Client API
 *  Version     : %version: sa1spcx1#2 %
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

#ifndef __HGFSWCLIENTOBSERVER_H
#define __HGFSWCLIENTOBSERVER_H

#include <e32base.h>

/**
 * Observer interface for getting notifications about fast swap content changes.
 */
class MHgFswObserver
    {
public:
    /**
     * Called when there is a change in the fast swap content.
     * The data can be queried via CHgFswClient::GetContentL
     */
    virtual void HandleFswContentChanged() = 0;
    };

#endif
