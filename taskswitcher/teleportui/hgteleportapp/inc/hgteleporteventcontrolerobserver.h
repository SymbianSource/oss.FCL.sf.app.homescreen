/*
 * ============================================================================
 *  Name        : hgteleportgestureobserver.h
 *  Part of     : Hg Teleport
 *  Description : Define gesture observer interface
 *  Version     : %version: 3 %
 *
 *  Copyright © 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 *
 */

#ifndef HGTELEPORTGESTUREOBSERVER_H
#define HGTELEPORTGESTUREOBSERVER_H

#include <e32base.h>
#include <akntouchgesturefw.h>

/**
 * MHgTeleportGestureObserver
 * Define gesture observer interface
 */
NONSHARABLE_CLASS(MHgTeleportEventControlerObserver)
    {
public:
    /**
     * Declare drag gesture handling.
     * @param aOffset - drag distance
     */
    virtual void MoveOffset(const TPoint& aOffset)=0;
    
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
    virtual void Drag(const AknTouchGestureFw::MAknTouchGestureFwDragEvent& aEvent) =0;
    
    /**
     * Declare animation stop handling
     */
    virtual void Stop()=0;
    
    /**
     * Declare getter for view position
     */
    virtual TPoint ViewPos() const =0;
    };

#endif // HGTELEPORTGESTUREOBSERVER_H
