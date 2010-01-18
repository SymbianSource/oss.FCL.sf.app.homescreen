/*
 * ============================================================================
 *  Name        : hgteleportapplication.h
 *  Part of     : Hg Teleport
 *  Description : Application class
 *  Version     : %version: 4 %
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
 * Template version: 4.2
 */

#ifndef HGTELEPORTAPPLICATION_H
#define HGTELEPORTAPPLICATION_H

#include <e32base.h> 
#include <aknapp.h>

class CHgTeleportApplication : public CAknApplication
    {
public:
    // from CAknApplication
    /**
     * Returns the application DLL UID value
     *
     * @since S60 v5.0
     * @return the UID of this Application/Dll
     */
    TUid AppDllUid( ) const;

protected:
    // from CAknApplication
    /**
     * Create a CApaDocument object and return a pointer to it
     *
     * @since S60 v5.0
     * a pointer to the created document
     */
    CApaDocument* CreateDocumentL( );
    };

#endif // HGTELEPORTAPPLICATION_H
