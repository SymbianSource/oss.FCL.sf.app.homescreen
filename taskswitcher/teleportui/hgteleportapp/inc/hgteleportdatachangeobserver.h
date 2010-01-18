/*
 * ============================================================================
 *  Name        : hgteleportdatachangeinterface.h
 *  Part of     : Hg Teleport engine
 *  Description :
 *  Version     : %version: 1 %
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
 
#ifndef HGTELEPORTDATACHANGEINTERFACE_H
#define HGTELEPORTDATACHANGEINTERFACE_H

#include <e32base.h>

class CCoeControl;

/**
 * Callback mixin class to move the whole app to background or foreground.
 */
class MHgTeleportDataChangeObserver
    {
public:
    virtual void DataChanged( CCoeControl* aWhere, TInt aNewCount ) = 0;
    };

#endif // HGTELEPORTDATACHANGEINTERFACE_H
