/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  receives key pressed notifications
*  Version     : %version: 8 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef M_MMWIDGETOBSERVER_H
#define M_MMWIDGETOBSERVER_H

#include <coedef.h>
#include <eiklbo.h>
#include "mmkeyeventobserver.h"
#include "mmdraganddropobserver.h"
#include "mmlongtapobserver.h"

/**
 *  MKeyEventObserver used to bundle different interfaces.
 *  A complete set of the bundled interfaces allows to fully
 *  control widgets behaviour.
 *  Class implementing this interface will be able to receive all 
 *  notifications from a widget.
 *
 *  @code
 *  @endcode
 *  @lib 
 *  @since S60 v5.0
 */
class MMmWidgetObserver : public MMmKeyEventObserver,
                          public MMmDragAndDropObserver,
                          public MEikListBoxObserver,
                          public MMmLongTapObserver
    {
    
    };
    
#endif // M_MMWIDGETOBSERVER_H
