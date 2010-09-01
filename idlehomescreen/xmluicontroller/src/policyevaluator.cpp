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
* Description:  Policy evaluator
*
*/


#include "xnnodeappif.h"
#include "xnproperty.h"
#include "xnuiengineappif.h"
#include "policyevaluator.h"
#include "aipolicyelement.h"
#include "aixmluiutils.h"
#include "aistrcnv.h"
#include "aixmluiconstants.h"

namespace
    {

    void ReplaceCharacters( TDes8& aDes,
                            TChar aOriginal,
                            TChar aNew )
        {
        if ( aDes.Length() == 0 )
            {
            return;
            }

        TPtr8 des = aDes.MidTPtr(0);
        TInt pos = des.Locate( aOriginal );

        while ( pos != KErrNotFound )
            {
            des[ pos++ ] = aNew;

            if ( pos == des.Length() )
                {
                break;
                }

            des.Set( des.MidTPtr( pos ) );
            pos = des.Locate( aOriginal );
            }
        }

    }

using namespace AiXmlUiController;

enum EAI2Policies
    {
    KAI2PolUnset = 0,
    KAI2PolShow,
    KAI2PolHide,
    };

// ======== MEMBER FUNCTIONS ========

CPolicyEvaluator::CPolicyEvaluator()
    {
    }


void CPolicyEvaluator::ConstructL()
    {
    }


CPolicyEvaluator* CPolicyEvaluator::NewL()
    {
    CPolicyEvaluator* self = new( ELeave ) CPolicyEvaluator;
    return self;
    }


CPolicyEvaluator::~CPolicyEvaluator()
    {
    }

void CPolicyEvaluator::EvaluateContentPolicyL( CXnNodeAppIf& aTarget,
                                               RAiPolicyElementArray& aPolicyArray )
    {
    EvaluatePolicyL( aTarget,
                     aPolicyArray,
                     AiUiDef::xml::policy::KContent );
    }

void CPolicyEvaluator::EvaluateEmptyContentPolicyL( CXnNodeAppIf& aTarget,
                                                    RAiPolicyElementArray& aPolicyArray )
    {
    EvaluatePolicyL( aTarget,
                     aPolicyArray,
                     AiUiDef::xml::policy::KEmptyContent );
    }

void CPolicyEvaluator::EvaluateVisibilityPolicyL( CXnNodeAppIf& aTarget,
                                                  RAiPolicyElementArray& aPolicyArray )
    {
    EvaluatePolicyL( aTarget,
                     aPolicyArray,
                     AiUiDef::xml::policy::KVisibilityPolicy );
    }

void CPolicyEvaluator::EvaluateContentChangedPolicyL( CXnNodeAppIf& aTarget,
                                                  RAiPolicyElementArray& aPolicyArray )
    {
    EvaluatePolicyL( aTarget,
                     aPolicyArray,
                     AiUiDef::xml::policy::KContentChanged );
    }

void CPolicyEvaluator::EvaluateResourcePolicyL( CXnNodeAppIf& aTarget,
                                                CXnNodeAppIf& aResource,
                                                RAiPolicyElementArray& aPolicyArray )
    {
    RPointerArray< CXnNodeAppIf > policyArray =
        FindPropertyElementL( aTarget,
                              AiUiDef::xml::policy::KResource );

    CleanupClosePushL( policyArray );

    aPolicyArray.ReserveL( aPolicyArray.Count() + policyArray.Count() );

    for ( TInt i = 0; i < policyArray.Count(); ++i )
        {
        CXnNodeAppIf* property = policyArray[ i ];

        // Get name property to lookup target ui element
        const TDesC8* name = PropertyValue( *property,
                                      AiUiDef::xml::property::KName );
                
        CXnNodeAppIf* target = NULL;
        if( name )
            {
            // Lookup target ui element
            target = property->UiEngineL()->FindNodeByIdL( *name, property->Namespace() );
            }

        if ( !target )
            {
            // No target found. Continue with next property
            continue;
            }

        const TDesC8* value = PropertyValue( *property,
                                             AiUiDef::xml::property::KValue );

        if ( !value )
            {
            // No value found. Continue with next property
            continue;
            }

        // Check if resource matches condition
        const TDesC8* id = PropertyValue( aResource,
                                          XnPropertyNames::common::KId );

        if ( !id )
            {
            continue;
            }

        TInt conditionEnd = value->Locate( KRightParenthesis );

        if ( conditionEnd++ < 0 )
            {
            continue;
            }

        TPtrC8 condition( value->Left( conditionEnd ) );
        TPtrC8 cssValue( value->Mid( conditionEnd ) );

        HBufC8* condBuffer = condition.AllocL();
        TPtr8 condPtr = condBuffer->Des();
        ReplaceCharacters( condPtr, KLeftParenthesis, KWhiteSpace );
        ReplaceCharacters( condPtr, KComma, KWhiteSpace );
        ReplaceCharacters( condPtr, KRightParenthesis, KWhiteSpace );

        if ( MatchCondition( *id, condPtr ) )
            {
            // Append target and value.
            // This cannot fail because space has been reserved.
            aPolicyArray.Append( TAiPolicyElement( *target, cssValue ) );
            }

        delete condBuffer;
        }

    CleanupStack::PopAndDestroy( &policyArray );
    }

void CPolicyEvaluator::EvaluatePolicyL( CXnNodeAppIf& aTarget,
                                        RAiPolicyElementArray& aPolicyArray,
                                        const TDesC8& aPolicyClass )
    {
    RPointerArray< CXnNodeAppIf > policyArray =
        FindPropertyElementL( aTarget,
                              aPolicyClass );

    CleanupClosePushL( policyArray );

    aPolicyArray.ReserveL( aPolicyArray.Count() + policyArray.Count() );

    for ( TInt i = 0; i < policyArray.Count(); ++i )
        {
        CXnNodeAppIf* property = policyArray[ i ];

        // Get name property to lookup target ui element
        const TDesC8* name = PropertyValue( *property,
                                      AiUiDef::xml::property::KName );
                
        CXnNodeAppIf* target = NULL;
        if( name )
            {
            // Lookup target ui element
            target = property->UiEngineL()->FindNodeByIdL( *name, property->Namespace() );
            }

        if ( !target )
            {
            // No target found. Continue with next property
            continue;
            }

        const TDesC8* value = PropertyValue( *property,
                                             AiUiDef::xml::property::KValue );

        if ( !value )
            {
            // No value found. Continue with next property
            continue;
            }

        if( aPolicyClass == AiUiDef::xml::policy::KVisibilityPolicy )
            {
            TInt conditionEnd = value->Locate( KRightParenthesis );

            if ( conditionEnd++ < 0 )
                {
                continue;
                }

            TPtrC8 condition( value->Left( conditionEnd ) );
            TPtrC8 cssValue( value->Mid( conditionEnd ) );

            HBufC8* condBuffer = condition.AllocLC();
            TPtr8 condPtr = condBuffer->Des();
            ReplaceCharacters( condPtr, KLeftParenthesis, KWhiteSpace );
            ReplaceCharacters( condPtr, KComma, KWhiteSpace );
            ReplaceCharacters( condPtr, KRightParenthesis, KWhiteSpace );

            if ( MatchVisibilityConditionL( condPtr, *property, aPolicyArray ) )
                {
                // Append target and value.
                // This cannot fail because space has been reserved.
                aPolicyArray.Append( TAiPolicyElement( *target, cssValue ) );
                }

            CleanupStack::PopAndDestroy( condBuffer );
            }
        else
            {
            // Append target and value.
            // This cannot fail because space has been reserved.
            aPolicyArray.Append( TAiPolicyElement( *target, *value ) );
            }
        }

    CleanupStack::PopAndDestroy( &policyArray );
    }

TBool CPolicyEvaluator::MatchCondition( const TDesC8& aId,
                                        const TDesC8& aCondition )
    {
    TBool negation = EFalse;

    TLex8 parser( aCondition );

    if ( parser.NextToken() == AiUiDef::xml::policy::KCondition )
        {
        // Condition found
        parser.SkipSpace();

        // check negation
        if ( parser.Get() == KNotOperator )
            {
            negation = ETrue;
            }
        else
            {
            parser.UnGet();
            }

        parser.SkipSpace();

        TBool found = EFalse;

        // Find id from the list. Stop when found or in the end of string.
        while ( !parser.Eos() )
            {
            if ( parser.NextToken() == aId )
                {
                found = ETrue;
                break;
                }
            }

        // Test if id matches the given condition
        return ( ( found && !negation ) || ( !found && negation ) ); // found XOR negation
        }
    else
        {
        return EFalse;
        }

    }

TBool CPolicyEvaluator::MatchVisibilityConditionL( const TDesC8& aCondition,
                                                  CXnNodeAppIf& aProperty,
                                                  RAiPolicyElementArray& aPolicyArray )
    {
    TBool negation = EFalse;
    TBool andOperation = EFalse;
    TInt nodesVisible = 0; // for managing the end of 'or' op.

    TLex8 parser( aCondition );

    if ( parser.NextToken() == AiUiDef::xml::policy::KCondition )
        {
        // Condition found
        parser.SkipSpace();

        // check negation
        if ( parser.Get() == KNotOperator )
            {
            negation = ETrue;
            }
        else
            {
            parser.UnGet();
            }

        if ( parser.Get() == KAndOperator )
            {
            andOperation = ETrue;
            }
        else
            {
            parser.UnGet();
            }

        parser.SkipSpace();

        // Determine aCondition lists visibilities. Stop when:
        //     - Eos reached
        //     - First 'false' in 'and' op
        //     - First 'true' in 'or' op
        while ( !parser.Eos() )
            {
            TPtrC8 id( parser.NextToken() );
            CXnNodeAppIf* target = NULL;
            TBool nodeVisible = EFalse;

            TBool displayBlockSet = KAI2PolUnset;
            TBool visibilityVisibleSet = KAI2PolUnset;

            // Lookup target ui element
            target = aProperty.UiEngineL()->FindNodeByIdL( id, aProperty.Namespace() );

            if( target )
                {
                // first try to first the display or visibility property updates from policy array
                for( TInt i = 0; i < aPolicyArray.Count(); ++i )
                    {
                    if( &(aPolicyArray[i].Target()) == target )
                        {
                        if( aPolicyArray[i].Name() == XnPropertyNames::style::common::KDisplay )
                            {
                            if( aPolicyArray[i].Value() ==
                                    XnPropertyNames::style::common::display::KBlock )
                                {
                                displayBlockSet = KAI2PolShow;
                                }
                            else if( aPolicyArray[i].Value() ==
                                    XnPropertyNames::style::common::display::KNone )
                                {
                                displayBlockSet = KAI2PolHide;
                                }
                            }
                        if( aPolicyArray[i].Name() == XnPropertyNames::style::common::KVisibility )
                            {
                            if( aPolicyArray[i].Value() ==
                                    XnPropertyNames::style::common::visibility::KVisible )
                                {
                                visibilityVisibleSet = KAI2PolShow;
                                }
                            else if( aPolicyArray[i].Value() ==
                                    XnPropertyNames::style::common::visibility::KHidden )
                                {
                                visibilityVisibleSet = KAI2PolHide;
                                }
                            }
                        }
                    }

                HBufC* displayPropertyValue = NULL;
                if( !displayBlockSet )
                    {
                    // Only check CSS if policy array did not set the value
                    displayPropertyValue = PropertyValueL(
                                            *target,
                                            XnPropertyNames::style::common::KDisplay );
                    CleanupStack::PushL( displayPropertyValue );

                    if( displayPropertyValue )
                        {
                        HBufC8* buf8 = NULL;
                        buf8 = AiUtility::CopyToBufferL( buf8, *displayPropertyValue );
                        if( buf8 )
                            {
                            if( *buf8 == XnPropertyNames::style::common::display::KBlock )
                                {
                                displayBlockSet = KAI2PolShow;
                                }
                            else
                                {
                                displayBlockSet = KAI2PolHide;
                                }
                            delete buf8;
                            }
                        }
                    CleanupStack::PopAndDestroy( displayPropertyValue );
                    }

                if( !visibilityVisibleSet )
                    {
                    displayPropertyValue = PropertyValueL(
                                            *target,
                                            XnPropertyNames::style::common::KVisibility );
                    CleanupStack::PushL( displayPropertyValue );

                    if( displayPropertyValue )
                        {
                        // only if visiblity property is set we affect nodeVisible variable
                        HBufC8* buf8 = NULL;
                        buf8 = AiUtility::CopyToBufferL( buf8, *displayPropertyValue );
                        if( buf8 )
                            {
                            if( *buf8 == XnPropertyNames::style::common::visibility::KVisible )
                                {
                                visibilityVisibleSet = KAI2PolShow;
                                }
                            else
                                {
                                visibilityVisibleSet = KAI2PolHide;
                                }
                            delete buf8;
                            }
                        }
                    CleanupStack::PopAndDestroy( displayPropertyValue );
                    }
                }

            // At least one value has to be 'not unset'
            // and neither can be 'hide'
            if( ( displayBlockSet || visibilityVisibleSet ) &&
                ( ( displayBlockSet != KAI2PolHide ) &&
                  ( visibilityVisibleSet != KAI2PolHide ) ) )
                {
                nodeVisible = ETrue;
                }

            if( nodeVisible )
                {
                ++nodesVisible;
                if( !andOperation && !negation )
                    {
                    // 'Or' ends to first true
                    // 'Not and' ends to first true
                    return ETrue;
                    }
                }
            else if( !nodeVisible && andOperation && !negation )
                {
                // 'And' ends to first false
                return EFalse;
                }

            parser.SkipSpace();
            }
        if( nodesVisible > 0 && !negation )
            {
            return ETrue;
            }
        if( nodesVisible == 0 && negation )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// End of file.
