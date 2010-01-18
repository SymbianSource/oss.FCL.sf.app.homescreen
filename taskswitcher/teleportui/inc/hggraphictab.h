/*
* ==============================================================================
*  Name        : hggraphictab.h
*  Part of     : Hg Graphics Loader
*  Description : Header file with filename and skin id tables
*  Version     : %version: 1 %
*
*  Copyright (c) 2007 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef __HGGRAPHICTAB_H__
#define __HGGRAPHICTAB_H__

#include <e32base.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids


/**
 * Name of image file in case icon is not available from the skin.
 * Path should not be included.
 * Use _S( "" ) if skin id should be used instead.
 * Order and count must match THgGraphicId. (except the last-marker)
 */
static const TText* const KHgGraphicFiles[] =
    {
    _S( "" )
    };

/**
 * Skin related IDs. Not in use yet.
 * Use all zeros if image file should be used instead.
 * Order and count must match THgGraphicId. (except the last-marker)
 */
static const TAknsItemID KHgGraphicSkinIds[] =
    {
    KAknsIIDNone
    };

#endif // __HGGRAPHICTAB_H__
