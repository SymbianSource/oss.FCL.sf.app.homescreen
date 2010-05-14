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
 * Description: Menu Mode Wrapper
 *
 */

#ifndef HS_MENU_MODE_WRAPPER_H
#define HS_MENU_MODE_WRAPPER_H

#include "hsmenumodewrapper.h"
/*!
    \class HsMenuModeWrapper
    \ingroup group_hsmenustateplugin

    \brief Menu Mode Wrapper.

    Represents objects to keep Hs Menu Mode.

    \sa Hs Menu Mode
*/

/*!
   Constructor.
 */

HsMenuModeWrapper::HsMenuModeWrapper(): mMode(NormalHsMenuMode)
{
}
    
/*!
    \return homescreen mode (add mode on / add mode off) set on the object
 */
HsMenuMode HsMenuModeWrapper::getHsMenuMode() const
{
    return mMode;
}

/*!
    \return sets in the object homescreen mode (add mode on / add mode off)
 */
void HsMenuModeWrapper::setHsMenuMode(HsMenuMode menuMode) {
    mMode = menuMode;
}

#endif
