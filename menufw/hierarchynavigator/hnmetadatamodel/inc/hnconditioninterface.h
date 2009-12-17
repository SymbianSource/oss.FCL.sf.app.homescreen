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



#ifndef C_HNCONDITIONINTERFACE_H
#define C_HNCONDITIONINTERFACE_H

#include <e32base.h>

class CLiwGenericParamList;

enum TOperator
    {
        EOpeningBrace = '(',
        EClosingBrace = ')', 
        ELogicalAnd = '&',  
        ELogicalOr = '|',  
        ESpace = ' '
    };

/**
 * Condition Interface
 * 
 * This class is an interface for conditions.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnConditionInterface ) : public CBase
    {
    
public:
    /**
     * Returns result.
     *
     * @since S60 v3.2
     * @param aQueryResults Results of query.
     * @param aPos position the list.
     * @return True if condition is true, otherwise false.
     */    
    virtual TBool ResultL( const CLiwGenericParamList& aQueryResults, TInt aPos  = 0 ) = 0;
    
    };

#endif // C_HNCONDITIONINTERFACE_H

