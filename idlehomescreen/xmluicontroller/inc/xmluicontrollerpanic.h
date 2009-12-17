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


#ifndef XMLUICONTROLLERPANIC_H
#define XMLUICONTROLLERPANIC_H

namespace AiXmlUiController
{
    
    enum TAiXmlUiControllerPanic
        {
        ETransactionStackEmpty,
        ETransactionStackNotEmpty,
        ETransactionElementListNotEmpty,
        ETargetNodeNull,
        ETargetNodeNotNull,
        EBitmapNull,
        ECriticalPointerNull
        };
        
    void Panic( TAiXmlUiControllerPanic aPanic );
    
} // namespace AiXmlUiController

#endif // XMLUICONTROLLERPANIC_H