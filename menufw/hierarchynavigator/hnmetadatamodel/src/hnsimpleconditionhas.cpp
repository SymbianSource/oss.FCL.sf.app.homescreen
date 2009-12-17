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
#include "hnsimpleconditionhas.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnSimpleConditionHas::CheckCondition( TLiwVariant& aVarLeft, 
        TLiwVariant& aVarRight )
    {
    TBool ret = EFalse;
    
    LIW::TVariantTypeId varLId = aVarLeft.TypeId();
    LIW::TVariantTypeId varRId = aVarRight.TypeId();
    
    if (varLId == varRId)
        {
        if( varLId == LIW::EVariantTypeDesC )
            {
            TPtrC left  = aVarLeft.AsDes();
            TPtrC right = aVarRight.AsDes();
            
            if ( KErrNotFound != left.Find( right ) )
                {
                ret = ETrue;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnSimpleConditionHas::GetPositionL( const TDesC8& aBuffer )
    {
    return aBuffer.Find( HnLogicalRelations::KLogicalHas8 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//   
TInt CHnSimpleConditionHas::SignLength()
    {
     return HnLogicalRelations::KLogicalHas8().Length();
    }
