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

// System include files
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>

// User include files
#include "hscontentcontrolui.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------
// CHsContentControlUi::NewL()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentControlUi* CHsContentControlUi::NewL(
    TUid aImplUid )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplUid,
        _FOFF( CHsContentControlUi, iDestructKey ) );

    CHsContentControlUi* ccUi = 
        reinterpret_cast< CHsContentControlUi* > ( ptr );
    
    if ( ccUi )
        {
        ccUi->iImplUid = aImplUid;        
        }
    
    return ccUi;
    }

// -----------------------------------------------------------------------
// CHsContentControlUi::ImplUid()
// -----------------------------------------------------------------------
//
TUid CHsContentControlUi::ImplUid() const
    {
    return iImplUid;
    }

// -----------------------------------------------------------------------
// CHsContentControlUi::SetContentControlTypeL()
// -----------------------------------------------------------------------
//
void CHsContentControlUi::SetContentControlTypeL(
    const TDesC8& aContentControlType )
    {
    delete iContentControlType;
    iContentControlType = NULL;

    iContentControlType = aContentControlType.AllocL();
    }

// -----------------------------------------------------------------------
// CHsContentControlUi::ContentControlType()
// -----------------------------------------------------------------------
//
const TDesC8& CHsContentControlUi::ContentControlType() const
    {
    return iContentControlType ? *iContentControlType : KNullDesC8();
    }

// End of file
