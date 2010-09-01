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


#ifndef C_POLICYEVALUATOR_H
#define C_POLICYEVALUATOR_H


#include <e32base.h>
#include "aipolicyelement.h"

class CXnNodeAppIf;
   
namespace AiXmlUiController
{
    
    
/**
 *  @ingroup group_xmluicontroller
 * 
 *  Policy evaluator class
 *
 *  @lib AiXmlUiMain
 */
class CPolicyEvaluator : public CBase
    {

public:

    static CPolicyEvaluator* NewL();

    virtual ~CPolicyEvaluator();

    /**
     * Evaluates content policy for ui element. 
     *
     * @param aTarget target ui element
     * @param aPolicyArray array of found policies to fill
     * @leave 
     */
    void EvaluateContentPolicyL( CXnNodeAppIf& aTarget,
                                 RAiPolicyElementArray& aPolicyArray );
    
    /**
     * Evaluates empty content policy for ui element. 
     *
     * @param aTarget target ui element
     * @param aPolicyArray array of found policies to fill
     * @leave 
     */
    void EvaluateEmptyContentPolicyL( CXnNodeAppIf& aTarget,
                                      RAiPolicyElementArray& aPolicyArray );

    /**
     * Evaluates Visiblity policy for ui element. 
     *
     * @param aTarget target ui element
     * @param aResource resource element to publish
     * @param aPolicyArray array of found policies to fill
     * @leave 
     */
    void EvaluateVisibilityPolicyL( CXnNodeAppIf& aTarget,
                                    RAiPolicyElementArray& aPolicyArray );

    /**
     * Evaluates ContentChanged policy for ui element. 
     *
     * @param aTarget target ui element
     * @param aResource resource element to publish
     * @param aPolicyArray array of found policies to fill
     * @leave 
     */
    void EvaluateContentChangedPolicyL( CXnNodeAppIf& aTarget,
                                    RAiPolicyElementArray& aPolicyArray );

    /**
     * Evaluates resource policy for ui element. 
     *
     * @param aTarget target ui element
     * @param aResource resource element to publish
     * @param aPolicyArray array of found policies to fill
     * @leave 
     */
    void EvaluateResourcePolicyL( CXnNodeAppIf& aTarget,
                                  CXnNodeAppIf& aResource,
                                  RAiPolicyElementArray& aPolicyArray );
private:

    CPolicyEvaluator();

    void ConstructL();
    
    void EvaluatePolicyL( CXnNodeAppIf& aTarget,
                          RAiPolicyElementArray& aPolicyArray,
                          const TDesC8& aPolicyClass );
    
    TBool MatchCondition( const TDesC8& aId,
                          const TDesC8& aCondition );
                          
    TBool MatchVisibilityConditionL( const TDesC8& aCondition,
                                    CXnNodeAppIf& aProperty,
                                    RAiPolicyElementArray& aPolicyArray );
                          
    };

} // namespace AiXmlUiController
    
#endif // C_POLICYEVALUATOR_H
