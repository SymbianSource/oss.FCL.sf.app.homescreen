/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <xmlengdom.h>
#include <liwservicehandler.h>

#include "hnmdvaluebase.h"
#include "hnmdvalueimage.h"
#include "hnmdvaluetext.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueBase* CHnMdValueBase::CreateL(
        const TXmlEngElement& aElement, THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdValueBase* ret( NULL );
            
    // check the child element name
    if ( !aElement.Name().Compare( KImageElementItem8 ) )
        {
        ret = CHnMdValueImage::NewL( aElement, aCmnPtrs);
        }
    else if ( !aElement.Name().Compare( KTextElementItem8 ) )
        {
        ret = CHnMdValueText::NewL(aElement, aCmnPtrs);
        }
    else if ( !aElement.Name().Compare( KOutputElementItem8 ) )
        {
        // now we have two options, element value, or attribute
        // text will hande it
        ret = CHnMdValueText::NewL(aElement, aCmnPtrs);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }    
    return ret;
    }


