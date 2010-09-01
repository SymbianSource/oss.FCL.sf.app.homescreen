/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

 
#ifndef TSDATACHANGEINTERFACE_H
#define TSDATACHANGEINTERFACE_H

#include <e32base.h>

class CCoeControl;

/**
 * Callback mixin class to move the whole app to background or foreground.
 */
class MTsDataChangeObserver
    {
public:
    virtual void DataChanged( CCoeControl* aWhere, TInt aNewCount ) = 0;
    };

#endif // TSDATACHANGEINTERFACE_H
