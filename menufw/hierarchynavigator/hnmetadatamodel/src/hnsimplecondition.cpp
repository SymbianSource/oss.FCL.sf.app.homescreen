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


#include <liwservicehandler.h>
#include <liwvariant.h>

#include "hnsimplecondition.h"
#include "hnsimpleconditionequal.h"
#include "hnsimpleconditiondifferent.h"
#include "hnsimpleconditiongreater.h"
#include "hnsimpleconditionhas.h"
#include "hnsimpleconditionhasnot.h"
#include "hnsimpleconditionsmaller.h"
#include "hnliwutils.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "hnstringhandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSimpleCondition* CHnSimpleCondition::NewL( TDesC8 & aElement )
    {
    CHnSimpleCondition* self = CHnSimpleCondition::CreateImplementationL( aElement );
    CleanupStack::PushL(self);
    self->ConstructL( aElement );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSimpleCondition* CHnSimpleCondition::CreateImplementationL(
        TDesC8 & aElement )
    {
    TPtrC8 buffer = aElement;
    CHnSimpleCondition* ret = NULL;
    if ( buffer.Find( HnLogicalRelations::KLogicalGreater8 )!= KErrNotFound )
        {
        ret =  new( ELeave ) CHnSimpleConditionGreater();
        }
    else if ( buffer.Find( HnLogicalRelations::KLogicalDifferent8 ) != 
        KErrNotFound )
        {
        ret = new( ELeave ) CHnSimpleConditionDifferent();
        }
    else if ( buffer.Find( HnLogicalRelations::KLogicalEqual8 ) != 
        KErrNotFound )
        {
        ret = new( ELeave ) CHnSimpleConditionEqual();
        }
    else if ( buffer.Find( HnLogicalRelations::KLogicalLess8 )!= 
        KErrNotFound )
        {
        ret = new( ELeave ) CHnSimpleConditionSmaller();
        }
    else if ( buffer.Find( HnLogicalRelations::KLogicalHas8 )!= 
        KErrNotFound )
        {
        ret = new( ELeave ) CHnSimpleConditionHas();
        }
    else if ( buffer.Find( HnLogicalRelations::KLogicalHasNot8 )!= 
        KErrNotFound )
        {
        ret = new( ELeave ) CHnSimpleConditionHasNot();
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSimpleCondition::CHnSimpleCondition()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSimpleCondition::~CHnSimpleCondition()
    {
    iReferenceRight.Close();
    iReferenceLeft.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::RemoveQuotes( RBuf8 & aValue )
    {
    TInt quoPos = aValue.Locate('\'');
    if( quoPos == 0 )
        {
        aValue.Delete( 0 , 1 );
        aValue.Delete( aValue.Length() - 1, 1 );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::ConstructL( TDesC8 & aElement )
    {
    TPtrC8 buffer = aElement;
    TInt pos = GetPositionL( buffer );
    ASSERT( pos );
    
    RBuf8 tempbuf;
    CleanupClosePushL( tempbuf );
    tempbuf.CreateL( buffer.Right( buffer.Length() - pos - SignLength() ) );
    iReferenceRight.Close();
    iReferenceRight.CreateL( tempbuf );
    iReferenceRight.Trim();
    CHnSimpleCondition::RemoveQuotes( iReferenceRight );
    
    tempbuf.Close();
    tempbuf.CreateL( buffer.Left( pos ) );
    
    iReferenceLeft.Close();
    iReferenceLeft.CreateL( tempbuf );
    iReferenceLeft.Trim();
    CHnSimpleCondition::RemoveQuotes( iReferenceLeft );
    
    CleanupStack::PopAndDestroy( &tempbuf );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NormalizeVariantTypeDesL( TLiwVariant* aVar )
    {
    if( aVar->TypeId() == LIW::EVariantTypeDesC8 )
        {
        TPtrC8 left;
        aVar->Get( left );
        HBufC * data = HnConvUtils::Str8ToStrFastLC( left );
        aVar->SetL( TLiwVariant( data ) );
        CleanupStack::PopAndDestroy( data );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TLiwVariant* CHnSimpleCondition::CheckTypes( TLiwVariant* aVarLeft,
        TLiwVariant* aVarRight, LIW::TVariantTypeId aType1, 
        LIW::TVariantTypeId aType2 )
    {
    LIW::TVariantTypeId varLeftId = aVarLeft->TypeId();
    LIW::TVariantTypeId varRightId = aVarRight->TypeId();
     
    if (varLeftId == aType1 && varRightId == aType2)
        {
        return aVarLeft;
        }
    else if (varLeftId == aType2 && varRightId == aType1)
        {
        return aVarRight;
        }
    
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateListL( TLiwVariant & aVar )
    {
    ASSERT( aVar.TypeId() == LIW::EVariantTypeList );
    
    CLiwList* temp = CLiwDefaultList::NewL();
    temp->PushL();
    aVar.Get( *temp );
    aVar.SetL( TLiwVariant( TInt32(temp->Count()) ) );
    CleanupStack::PopAndDestroy( temp );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateMapL( TLiwVariant & aVar  )
    {
    ASSERT( aVar.TypeId() == LIW::EVariantTypeMap );
        
    CLiwDefaultMap* temp = CLiwDefaultMap::NewL();
    temp->PushL();
    aVar.Get( *temp ); 
    aVar.SetL( TLiwVariant( TInt32(temp->Count()) ) );
    CleanupStack::PopAndDestroy( temp );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateTUintL( TLiwVariant & aVar )
    {
    ASSERT ( aVar.TypeId() == LIW::EVariantTypeDesC );
    
    TPtrC temp;
    TUint integer;
    
    aVar.Get(temp);
    TLex val( temp );
    val.Val(integer);
    aVar.Set( integer );
        
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateTBoolL( TLiwVariant & aVar )
    {    
    ASSERT( aVar.TypeId() == LIW::EVariantTypeDesC );
        
    TPtrC ptr;
    aVar.Get( ptr );
     
    if ( !HnStringHandler::CompareIgnoreCaseL( ptr, KStringTrue ) )
        {
        aVar.Set( ETrue );
        }
    else
          {
          aVar.Set( EFalse );
           }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateTInt32L( TLiwVariant & aVar )
    {
    ASSERT( aVar.TypeId() == LIW::EVariantTypeDesC
            || aVar.TypeId() == LIW::EVariantTypeTUint );
   
    if ( aVar.TypeId() == LIW::EVariantTypeDesC )
        {
        TPtrC temp;
        TInt32 integer;
        
        aVar.Get(temp);
        TLex val( temp );
        val.Val(integer);
        aVar.Set( integer );
        }
    else if ( aVar.TypeId() == LIW::EVariantTypeTUint )
        {
        TUint unsignedValue( 0U );
        aVar.Get( unsignedValue );
        if( unsignedValue <= static_cast<TUint>( KMaxTInt32 ) )
            {
            aVar.Set( static_cast<TInt32>( unsignedValue ) );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateTInt64L( TLiwVariant & aVar )
    {
    ASSERT(aVar.TypeId() == LIW::EVariantTypeDesC );
    
    TPtrC temp;
    TInt64 integer;
    
    aVar.Get(temp);
    TLex val( temp );
    val.Val(integer);
    aVar.Set( integer );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnSimpleCondition::NegotiateTypesL( TLiwVariant* aVarLeft, 
        TLiwVariant* aVarRight )
    {
    NormalizeVariantTypeDesL( aVarLeft );
    NormalizeVariantTypeDesL( aVarRight );
    
    if (aVarLeft->TypeId() != aVarRight->TypeId())
        {
        TLiwVariant* chosen( NULL );
        
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeList, LIW::EVariantTypeDesC );
        if ( chosen )
            {
            NegotiateListL( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeMap, LIW::EVariantTypeDesC );
        if ( chosen )
            {
            NegotiateMapL( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeDesC, LIW::EVariantTypeTInt32 );
        if ( chosen )
            {
            NegotiateTInt32L( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeDesC, LIW::EVariantTypeTUint );
        if ( chosen )
            {
            NegotiateTUintL( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeDesC, LIW::EVariantTypeTInt64 );
        if ( chosen )
            {
            NegotiateTInt64L( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeDesC, LIW::EVariantTypeTBool );
        if ( chosen )
            {
            NegotiateTBoolL( *chosen );
            return;
            }
        chosen = CheckTypes( aVarLeft, aVarRight, LIW::EVariantTypeTUint, LIW::EVariantTypeTInt32 );
        if ( chosen )
            {
            NegotiateTInt32L( *chosen );
            return;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnSimpleCondition::ResultL( const CLiwGenericParamList& aQueryResults, 
        TInt aPos )
    {
    TBool ret( EFalse );
    
    TLiwVariant varLeft;
    varLeft.PushL();
    TInt err1 = HnLiwUtils::GetVariantL( aQueryResults, iReferenceLeft, aPos, varLeft );
    
    TLiwVariant varRight;
    varRight.PushL();
    TInt err2 = HnLiwUtils::GetVariantL( aQueryResults, iReferenceRight, aPos , varRight );
    
    if ( err1 == KErrNone && err2 == KErrNone )
    	{
    	NegotiateTypesL( &varLeft, &varRight);
    	ret = CheckCondition( varLeft, varRight );
    	}
    
    CleanupStack::PopAndDestroy( &varRight );
    CleanupStack::PopAndDestroy( &varLeft );
    
    return ret;
    }

