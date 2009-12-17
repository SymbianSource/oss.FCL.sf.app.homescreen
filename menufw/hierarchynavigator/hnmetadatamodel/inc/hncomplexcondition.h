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



#ifndef C_HNCOMPLEXCONDITION_H
#define C_HNCOMPLEXCONDITION_H

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengelement.h>

#include "hnconditioninterface.h"

class CLiwGenericParamList;

/**
 * Complex condition.
 * 
 * This class is designed to support complex conditions in the xml configuration.
 *  
 * @lib hnmetadatamodel
 * @since S60 v3.2
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnComplexCondition) : public CHnConditionInterface
    {
public:

     
    /**
     * Standard factory function.
     * 
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnComplexCondition* NewL( TDesC8 & aElement );

    /**
     * Standard C++ Destructor
     */
    ~CHnComplexCondition();
    
    /**
     * Returns result.
     *
     * @since S60 v3.2
     * @param aQueryResults Results of query.
     * @param aPos Position in the list.
     * @return True if condition of true, otherwise false.
     */    
    TBool ResultL( const CLiwGenericParamList& aQueryResults, TInt aPos );  
    
protected:
    
    /**
     *  Standard C++ Constructor
     */
    CHnComplexCondition();

    /**
     * Standard symbian second phase constructor.
     * 
     * @param aElement Xml element.
     */
    void ConstructL( TDesC8 & aElement );
       
    /**
     * Prepares the condition for processng.
     * Removes excessive white characters and 
     * superfluous parentheses
     * 
     * @param aConditionString Condition string.
     */
    TBool CheckForOrOnTheSameLevelL( TDesC8 & aConditionString );
    
protected: // data
        
    /**
     * Logical operation between conditions.
     */
    TOperator iOperation;
    
    /**
     * Own - left side of the condition.
     */
    CHnConditionInterface* iLeftCondition;
    
    /**
     * Own - right side of the condition.
     */
    CHnConditionInterface* iRightCondition;
    
    };

#endif // C_HNCOMPLEXCONDITION_H

