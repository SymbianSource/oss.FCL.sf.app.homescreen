/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  receives key pressed notifications
*
*/

#ifndef M_MMVISIBILITYOBSERVER_H
#define M_MMVISIBILITYOBSERVER_H

#include <coedef.h>

/**
 *  Interface for handling application changing background/foreground states.
 *
 *  @code
 *  @endcode
 *  @lib 
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
class MMmVisibilityObserver
    {
public:
    
    /**
     * Called when application goes into foreground (e.g options menu
     * goes off or application is opened ).
     * 
     * @since S60 v5.0
     */
    virtual void HandleForegroundGainedL() = 0;
    
    /**
     * Called when application goes into background (e.g the options list
     * is open and application view is dimmed).
     *
     * @since S60 v5.0
     */
    virtual void HandleBackgroundGainedL() = 0;
    
    };
    
#endif // M_MMVISIBILITYOBSERVER_H
