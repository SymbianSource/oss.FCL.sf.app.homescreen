/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
#ifndef TSGRAPHICTAB_H
#define TSGRAPHICTAB_H

#include <e32base.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids


/**
 * Name of image file in case icon is not available from the skin.
 * Path should not be included.
 * Use _S( "" ) if skin id should be used instead.
 * Order and count must match TTsGraphicId. (except the last-marker)
 */
static const TText* const KTsGraphicFiles[] =
    {
    _S( "" )
    };

/**
 * Skin related IDs. Not in use yet.
 * Use all zeros if image file should be used instead.
 * Order and count must match TTsGraphicId. (except the last-marker)
 */
static const TAknsItemID KTsGraphicSkinIds[] =
    {
    KAknsIIDNone
    };

#endif // TSGRAPHICTAB_H
