/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic and error codes
*
*/



#ifndef XNPANIC_H
#define XNPANIC_H

//  INCLUDES
#include <e32std.h>


// DATA TYPES
/**
 * Definitions for panic values used in Layout Engine
 */
enum TXnPanic
	{
    EXnInvalidConfiguration = 1,
    EXnInvalidDeclaration,
    EXnInvalidPublisherInfo,
    EXnInvalidState,
    EXnCritalPointerNull,
    EXnFailedToComposeInitialView
	};


/**
 * Error code definitions
 */
const TInt KXnErrControlAdapterFailed = -32223; 
const TInt KXnErrAdapterNotWindowOwner = -32224;
const TInt KXnErrAppendingChildToControlAdapterFailed = -32225;
const TInt KXnErrAddChildToAreaFailed = -32226;
const TInt KXnErrODTNotFoundFromRepository = -32227;
const TInt KXnErrAddingProperyToListFailed = -32228;
const TInt KXnErrAddingProperyToListFailed_2 = -32229;
const TInt KXnErrAddingProperyToListFailed_3 = -32230;
const TInt KXnErrAddingProperyToListFailed_4 = -32231;
const TInt KXnErrAddingProperyToListFailed_5 = -32232;
const TInt KXnErrDomPropertyValueNULL = -32233;
const TInt KXnErrHorizontalValueNULL = -32234;
const TInt KXnErrWidthNULL = -32235;
const TInt KXnErrWidthNULL_2 = -32236;
const TInt KXnErrOrientationNULL = -32237;
const TInt KXnErrDirectionNULL = -32238;
const TInt KXnErrOrientationPropertyNULL = -32239;
const TInt KXnErrDirectionPropertyNULL = -32240;
const TInt KXnErrPositioningPropertyNULL = -32241;
const TInt KXnErrOrientationPropertyNULL_2 = -32242;
const TInt KXnErrReverseTreeFailed = -32243;
const TInt KXnErrCXnBreadthFirstTreeIteratorNextLAppendToStackFailed_1 = -32244;
const TInt KXnErrCXnBreadthFirstTreeIteratorNextLAppendToStackFailed_2 = -32245;
const TInt KXnErrCXnBreadthFirstTreeIteratorNextLAppendToStackFailed_3 = -32246;
const TInt KXnErrCXnDepthFirstTreeIteratorNextLAppendToStackFailed = -32247;
const TInt KXnErrAppendingPluginFailed = -32248;
const TInt KXnErrAppendingPluginFailedAgain = -32249;
const TInt KXnErrAppendingPluginImplFailed = -32250;
const TInt KXnErrAppendingMapItemFailed_1 = -32251;
const TInt KXnErrAppendingMapItemFailed_2 = -32252;
const TInt KXnErrBlockProgressionPropertyNULL = -32253; 
const TInt KXnErrBlockProgressionPropertyNULL_2 = -32254;
const TInt KXnErrPluginFailure = -32255;

// FUNCTION PROTOTYPES
GLREF_C void Panic(TXnPanic aPanic);

#endif      // XNPANIC_H   
            
// End of File
