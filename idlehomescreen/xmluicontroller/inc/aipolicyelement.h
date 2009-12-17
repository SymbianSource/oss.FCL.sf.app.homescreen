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
* Description:  Policy element
*
*/


#ifndef T_AIPOLICYELEMENT_H
#define T_AIPOLICYELEMENT_H


#include <e32std.h>

class CXnNodeAppIf;

namespace AiXmlUiController
{
    

/**
 *  @ingroup group_xmluicontroller
 * 
 *  Active Idle publishing policy element
 *
 *  @lib AiXmlUiMain
 */
class TAiPolicyElement
    {

public:

    TAiPolicyElement();

    TAiPolicyElement( CXnNodeAppIf& aTarget, const TDesC8& aPolicy );

    CXnNodeAppIf& Target() const;

    TPtrC8 Name() const;
    
    TPtrC8 Value() const;
    
    TPtrC8 Policy() const;

private: // data

    /**
     * Target element for policy. Not own.
     */
    CXnNodeAppIf* iTarget;
    
    /**
     * Policy string
     */
    TPtrC8 iPolicy;

    TInt iSeparator;
    };
    
typedef RArray< TAiPolicyElement > RAiPolicyElementArray;
    
} // namespace AiXmlUiController

#endif // T_AIPOLICYELEMENT_H
