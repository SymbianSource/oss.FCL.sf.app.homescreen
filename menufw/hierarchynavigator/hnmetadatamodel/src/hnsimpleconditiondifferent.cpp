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


#include "hnsimplecondition.h"
#include "hnsimpleconditiondifferent.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnSimpleConditionDifferent::CheckCondition(TLiwVariant& aVarLeft, 
        TLiwVariant& aVarRight )
    {
    return (aVarLeft != aVarRight);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnSimpleConditionDifferent::GetPositionL( const TDesC8& aBuffer )
    {
    return aBuffer.Find( HnLogicalRelations::KLogicalDifferent8 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//   
TInt CHnSimpleConditionDifferent::SignLength()
    {
     return HnLogicalRelations::KLogicalDifferent8().Length();
    }
