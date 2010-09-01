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
*
*/


#ifndef M_MMLONGTAPOBSERVER_H
#define M_MMLONGTAPOBSERVER_H

#include <coedef.h>

/**
 *  Interface for handling long tap events.
 *
 *  @code
 *  @endcode
 *  @lib 
 *  @since S60 v3.0
 */
class MMmLongTapObserver
    {
public:
	
    virtual void HandleLongTapEventL( const TPoint& aPenEventLocation ) = 0; 
    
    };
    
#endif // M_MMLONGTAPOBSERVER_H
