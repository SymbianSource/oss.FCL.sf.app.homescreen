/*
 * ============================================================================
 *  Name        : hgteleportdocument.h
 *  Part of     : Hg Teleport
 *  Description : Document class
 *  Version     : %version: sa1spcx1#6 %
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

#ifndef HGTELEPORTDOCUMENT_H
#define HGTELEPORTDOCUMENT_H

#include <e32base.h> 
#include <AknDoc.h>

class CHgTeleportDocument : public CAknDocument
    {
public:
    /**
     * Construct a CHgTeleportDocument for the AVKON application aApp
     * using two phase construction, and return a pointer to the created object
     * aApp application creating this document
     *
     * @since S60 v5.0
     * @return a pointer to the created instance of CHgTeleportDocument
     */
    static CHgTeleportDocument* NewL( CEikApplication& aApp );

    /**
     * Construct a CHgTeleportDocument for the AVKON application aApp
     * using two phase construction, and return a pointer to the created object
     *
     * @since S60 v5.0
     * @param aApp application creating this document
     * @return a pointer to the created instance of CHgTeleportDocument
     */
    static CHgTeleportDocument* NewLC( CEikApplication& aApp );

    /**
     * Destroy the object and release all memory objects
     *
     * @since S60 v5.0
     */
    ~CHgTeleportDocument( );

    /**
     * Makes Startup-application hidden in menu shell and fastswap window
     *
     * @since S60 v5.0
     * @param aWgName application windows group name
     */
    void UpdateTaskNameL( CApaWindowGroupName* aWgName );

public:
    // from CAknDocument
    /**
     * Create a ChgteleportAppUi object and return a pointer to it
     *
     * @since S60 v5.0
     * @return a pointer to the created instance of the AppUi created
     */
    CEikAppUi* CreateAppUiL( );

private:
    /**
     * Perform the second phase construction of a CHgTeleportDocument object
     *
     * @since S60 v5.0
     */
    void ConstructL( );

    /**
     * Perform the first phase of two phase construction
     *
     * @since S60 v5.0
     * @param aApp application creating this document
     */
    CHgTeleportDocument( CEikApplication& aApp );
    };

#endif // HGTELEPORTDOCUMENT_H
