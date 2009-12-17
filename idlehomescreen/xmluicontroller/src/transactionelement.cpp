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
* Description:  ?Description
*
*/


#include    "xnnodeappif.h"
#include    "xnuiengineappif.h"
#include    "xnproperty.h"

#include    "transactionelement.h"
#include    "xmluicontrollerpanic.h"
#include    "aixmluiutils.h"
#include    "aixmluiconstants.h"

#include    "contentprioritymap.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;
using namespace AiUiDef::xml;

/**
 * Template method implementation
 */
void MTransactionElement::CommitL( TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap )
    {
    // Update UI element data
    UpdateDataL();
    
    // Update CSS properties
    ApplyPublishingPolicy( aLayoutChanged, aPropertyHashMap );
    }

CTransactionElement::CTransactionElement( AiUtility::CContentPriorityMap& aContentPriorityMap )
    : iContentPriorityMap( aContentPriorityMap ),
      iContentPriority( KErrNotFound )
    {
    }

CTransactionElement::~CTransactionElement()
    {
    }
    
void CTransactionElement::Reset()
    {
    iTarget = NULL;
    
    iContentPriority = KErrNotFound;
    
    iElementLink.Deque();
    
    iPolicyArray.Reset();
    }
    
RAiPolicyElementArray& CTransactionElement::PolicyArray()
    {
    return iPolicyArray;
    }

void CTransactionElement::SetContentPriority( TInt aPriority )
    {
    iContentPriority = aPriority;
    }

void CTransactionElement::SetCssPropertyMap( CCssPropertyMap* aPropertyMap )
    {
    iPropertyMap = aPropertyMap;
    }

void CTransactionElement::SetTarget(CXnNodeAppIf& aTarget)
    {
    __ASSERT_DEBUG( !iTarget, Panic( ETargetNodeNotNull ) );
    iTarget = &aTarget;
    }

void CTransactionElement::ApplyPublishingPolicy( TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap )
    {
    // Ignore errors if CSS property could not be modified
    TRAP_IGNORE( DoApplyPublishingPolicyL( aLayoutChanged, aPropertyHashMap ) );
    }
            
void CTransactionElement::DoApplyPublishingPolicyL( TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap )
    {
    RArray<CXnNodeAppIf*> targetArray;
    CleanupClosePushL( targetArray );
    
    for ( TInt i = 0; i < iPolicyArray.Count() && iPropertyMap; ++i )
        {
        TAiPolicyElement& element = iPolicyArray[ i ];
        
        RArray<TAiPolicyElement> elementArray;
        CleanupClosePushL( elementArray );
        
        TBool found = EFalse;
        for( TInt i2 = 0; i2 < targetArray.Count(); ++i2 )
            {
            // find from target array
            if( targetArray[i2] == &(element.Target()) )
                {
                found = ETrue;
                break;
                }
            }
            
        if( !found )
            {
            // Mark this target to be processed
            targetArray.Append( &(element.Target()) );
            
            for( TInt i3 = 0; i3 < iPolicyArray.Count(); ++i3 )
                {
                TAiPolicyElement& element2 = iPolicyArray[ i3 ];
                // find form target array
                if( &(element.Target()) == &(element2.Target()) )
                    {
                    elementArray.Append( element2 );
                    }
                }
                
            SetPropertiesToHashMapL( elementArray, *iPropertyMap, aPropertyHashMap );
            
            }
        
        CleanupStack::PopAndDestroy(); // elementArray    
            
        /*SetPropertyL( element.Target(),
                      element.Name(),
                      element.Value(),
                      *iPropertyMap );*/
        }
    
    CleanupStack::PopAndDestroy(); // targetArray
    
    if ( iPolicyArray.Count() > 0 )
        {
        aLayoutChanged |= ETrue;
        }
    }
    
CXnNodeAppIf& CTransactionElement::Target() const
    {
    __ASSERT_DEBUG( iTarget, Panic( ETargetNodeNull ) );
    return *iTarget;
    }

void CTransactionElement::UpdateContentPriorityL()
    {
    if ( iTarget && iContentPriority > KErrNotFound ) // Clear must be done with ClearContentPriority
        {
        const TDesC8* uiElementId = LeaveIfNull( PropertyValue( 
                                                        *iTarget,
                                                        XnPropertyNames::common::KId ),
                                           KErrGeneral );
    
        User::LeaveIfError( iContentPriorityMap.SetCurrentPriority(
                                                        *uiElementId,
                                                        iContentPriority ) );
        }
    }
            
void CTransactionElement::ClearContentPriorityL()
    {
    if( iTarget )
        {
        const TDesC8* uiElementId = LeaveIfNull( PropertyValue(
                                                            *iTarget,
                                                            XnPropertyNames::common::KId ),
                                           KErrGeneral );
        
        iContentPriorityMap.ClearPriority( *uiElementId );
        }
    }
    
//  End of File
