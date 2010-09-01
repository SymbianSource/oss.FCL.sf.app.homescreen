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
* Description:  Define gesture observer interface
 *
*/


#ifndef TSGESTUREOBSERVER_H
#define TSGESTUREOBSERVER_H

#include <e32base.h>
#include <akntouchgesturefw.h>

/**
 * MTsGestureObserver
 * Define gesture observer interface
 */
NONSHARABLE_CLASS(MTsEventControlerObserver)
    {
public:
    /**
     * Declare drag gesture handling.
     * @param aOffset - drag distance
     */
    virtual void MoveOffset(const TPoint& aOffset, TBool aDrawNow)=0;
    
    /**
     * Declare tap gesture handling.
     * @param aPoint - tap position
     */
    virtual void TapL(const TPoint& aPoint)=0;
    
    /**
     * Declare long tap gesture handling.
     * @param aPoint - tap position
     */
    virtual void LongTapL(const TPoint& aPoint)=0;
    
    /**
     * Declare drag gesture handling.
     * @param aEvent - drag event
     */
    virtual void DragL(const AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent) =0;
    
    /**
     * Declare animation stop handling
     */
    virtual void Stop()=0;
    
    /**
     * Declare getter for view position
     */
    virtual TPoint ViewPos() const =0;
    };

#endif // TSGESTUREOBSERVER_H
