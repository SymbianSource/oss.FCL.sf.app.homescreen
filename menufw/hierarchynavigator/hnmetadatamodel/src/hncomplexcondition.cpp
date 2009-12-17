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

#include "hncomplexcondition.h"
#include "hnliwutils.h"
#include "hnglobals.h"
#include "hnconditionfactory.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnComplexCondition* CHnComplexCondition::NewL( TDesC8 & aElement )
    {
    CHnComplexCondition* self = new (ELeave) CHnComplexCondition();
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnComplexCondition::CHnComplexCondition()
    {
    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnComplexCondition::~CHnComplexCondition()
    {
    delete iRightCondition;
    delete iLeftCondition;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnComplexCondition::CheckForOrOnTheSameLevelL( TDesC8 & aConditionString )
    {
    TBool ret = EFalse;
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
                ret = EFalse;
                lexer.Inc( lexer.Remainder().Length() );
                break;             
            case ELogicalOr:
                if (lookForClosingBrace == 0)
                    {
                    ret = ETrue;
                    }
                break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnComplexCondition::ConstructL( TDesC8 & aElement )
    {
    TLex8 lexer( aElement );
    TInt lookForClosingBrace( 0 );
    while  ( !lexer.Eos() )
        {     
        TChar character = lexer.Get();
        switch ( TUint( character ) )
            {
            case EOpeningBrace :
                lookForClosingBrace++;
                break;
            case EClosingBrace :
                lookForClosingBrace--;
                User::LeaveIfError( lookForClosingBrace );
                break;
            case ELogicalAnd:
                {
                TPtrC8 remainder = lexer.Remainder();
                if ( CheckForOrOnTheSameLevelL( remainder ) )
                    {
                    break;
                    }
                }
            case ELogicalOr:
                if (lookForClosingBrace == 0)
                    {
                    iOperation = (TOperator) TUint(character);
                    TPtrC8 rightSide = lexer.Remainder();
                    iRightCondition = HnConditionFactory::NewL( rightSide );
                    lexer.UnGet();
                    TPtrC8 leftSide = lexer.MarkedToken();
                    iLeftCondition = HnConditionFactory::NewL( leftSide );
                    lexer.Inc( lexer.Remainder().Length() );
                    }
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnComplexCondition::ResultL( const CLiwGenericParamList& aQueryResults, 
                                                                TInt aPos )
    {
    TBool ret = false;
    
    if ( iOperation == ELogicalAnd )
        {
        ret = iLeftCondition->ResultL(aQueryResults, aPos) && 
            iRightCondition->ResultL(aQueryResults, aPos);
        }
    else if ( iOperation == ELogicalOr )
        {
        ret = iLeftCondition->ResultL(aQueryResults, aPos) || 
            iRightCondition->ResultL(aQueryResults, aPos);
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    
    return ret;
    }

