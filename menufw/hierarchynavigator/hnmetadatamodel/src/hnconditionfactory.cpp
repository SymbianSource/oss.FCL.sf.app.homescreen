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


#include "hnconditionfactory.h"
#include "hnsimplecondition.h"
#include "hncomplexcondition.h"
#include "hnglobals.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnConditionInterface* HnConditionFactory::NewL( TXmlEngAttr aElement )
    {
    CHnConditionInterface* self = HnConditionFactory::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnConditionInterface* HnConditionFactory::NewLC( TXmlEngAttr aElement )
    {
    TPtrC8 condition = aElement.Value();
    CHnConditionInterface* self = ConstructConditionL( condition );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnConditionInterface* HnConditionFactory::NewL( TDesC8 & aElement )
    {
    CHnConditionInterface* self = HnConditionFactory::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnConditionInterface* HnConditionFactory::NewLC( TDesC8 & aElement )
    {
    CHnConditionInterface* self = HnConditionFactory::ConstructConditionL( aElement );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void HnConditionFactory::TrimConditionL( RBuf8 & aConditionString )
    {
    TBool ret;
    do
        {
        ret = EFalse;
        aConditionString.TrimAll();
        TInt conditionLastIndex = aConditionString.Length()-1;
        if (aConditionString[0] == EOpeningBrace 
                && aConditionString[conditionLastIndex] == EClosingBrace )
            {
            aConditionString[0] = ESpace;
            aConditionString[conditionLastIndex] = ESpace;
            
            ret = ETrue;
            
            if ( CheckBraceIntegrityL( aConditionString ) )
                {
                aConditionString[0] = EOpeningBrace;
                aConditionString[conditionLastIndex] = EClosingBrace;
                ret = EFalse;
                }
            }
        }
    while (ret);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool HnConditionFactory::CheckBraceIntegrityL( TDesC8 & aConditionString )
    {
    TInt ret(KErrNone);
    TLex8 lexer( aConditionString );
    TInt lookForClosingBrace( 0 );
    while  ( !lexer.Eos() )
        {
        switch ( TUint( lexer.Get() ) )
            {
            case EOpeningBrace :
                lookForClosingBrace++;
                break;
            case EClosingBrace :
                lookForClosingBrace--;
                if (lookForClosingBrace < 0)
                    {
                    ret = KErrGeneral;
                    lexer.Inc( lexer.Remainder().Length() );
                    }
                break;             
            }
        }
    
    if (lookForClosingBrace > 0)
        {
        ret = KErrGeneral;
        }
    
    return ret;  
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnConditionInterface* HnConditionFactory::ConstructConditionL(
        TDesC8 & aConditionString )
    {
    RBuf8 condition;
    CleanupClosePushL( condition );
    condition.CreateL( aConditionString );
    HnConditionFactory::TrimConditionL( condition );
    User::LeaveIfError( HnConditionFactory::CheckBraceIntegrityL(condition) );
    
    CHnConditionInterface* ret = NULL;
    
    if ( aConditionString.Find( HnLogicalRelations::KLogicalAnd8 ) == KErrNotFound
         && aConditionString.Find( HnLogicalRelations::KLogicalOr8 ) == KErrNotFound )
        {
        ret =  CHnSimpleCondition::NewL( condition );
        }
    else
        {
        ret = CHnComplexCondition::NewL( condition );
        }
    
    CleanupStack::PopAndDestroy( &condition );
    return ret;
    }

