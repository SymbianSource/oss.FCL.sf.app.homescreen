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
#include "hnsimpleconditiongreater.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnSimpleConditionGreater::CheckCondition( TLiwVariant& aVarLeft, 
        TLiwVariant& aVarRight )
    {
    TBool ret = EFalse;
    
    LIW::TVariantTypeId varLId = aVarLeft.TypeId();
    LIW::TVariantTypeId varRId = aVarRight.TypeId();
    
    if (varLId == varRId)
        {
        if( varLId == LIW::EVariantTypeTInt32 )
            {
            ret = (aVarLeft.AsTInt32() > aVarRight.AsTInt32());
            }
        else if( varLId == LIW::EVariantTypeTInt64 )
            {
            ret = (aVarLeft.AsTInt64() > aVarRight.AsTInt64());
            }
        else if( varLId == LIW::EVariantTypeTUint )
            {
            ret = (aVarLeft.AsTUint() > aVarRight.AsTUint() );
            }
        else if( varLId == LIW::EVariantTypeDesC )
            {
            ret = ( aVarLeft.AsDes().Compare( aVarRight.AsDes()) > 0 );
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnSimpleConditionGreater::GetPositionL( const TDesC8& aBuffer )
    {
    return aBuffer.Find( HnLogicalRelations::KLogicalGreater8 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//   
TInt CHnSimpleConditionGreater::SignLength()
    {
     return HnLogicalRelations::KLogicalGreater8().Length();
    }


