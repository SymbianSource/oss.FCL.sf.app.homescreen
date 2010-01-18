/*
* ==============================================================================
*  Name        : proxy.cpp
*  Part of     : Teleport
*  Description : Teleport application ecom plugin
*  Version     : %version: 2 %
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or stranslating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "hgteleportappecom.h"
#include "hgteleportappecomconst.hrh"

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        KHgTeleportAppPluginImplementationUid,
        CHgTeleportEcomPlugin::NewL )
    };

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount 
        = sizeof ( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of file
