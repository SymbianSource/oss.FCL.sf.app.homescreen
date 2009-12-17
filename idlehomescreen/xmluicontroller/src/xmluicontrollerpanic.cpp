/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Module specific panic definitions
*
*/


#include <e32std.h>
#include "xmluicontrollerpanic.h"

using namespace AiXmlUiController;

void AiXmlUiController::Panic( TAiXmlUiControllerPanic aPanic )
    {
    _LIT( KPanicCategory, "XmlUiController" );
    User::Panic( KPanicCategory, aPanic );
    }
