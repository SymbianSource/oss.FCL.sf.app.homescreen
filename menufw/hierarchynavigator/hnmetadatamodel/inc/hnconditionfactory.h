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



#ifndef C_HNCONDITIONFACTORY_H
#define C_HNCONDITIONFACTORY_H

#include <e32def.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengelement.h> 

class CHnConditionInterface;

/**
 * Condition factory.
 * 
 * The purpose of this class is the creation of different types of the condition objects.
 *  
 * @lib hnmetadatamodel
 * @since S60 v3.2
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(HnConditionFactory)
    {
public:
   
    /**
     * Standard factory function.
     * 
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnConditionInterface* NewL( TXmlEngAttr aElement );

    /**
     * Standard factory function.
     * 
     * @param aElement Xml element
     * @return Fully constructed object.
     */
    static CHnConditionInterface* NewLC( TXmlEngAttr aElement );
    
    /**
     * Standard factory function.
     * 
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnConditionInterface* NewL( TDesC8 & aElement );

    /**
     * Standard factory function.
     * 
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnConditionInterface* NewLC( TDesC8 & aElement );
    
private:
    
    /**
     * Prepares the condition for processng.
     * Removes excessive white characters and 
     * superfluous parentheses
     * 
     * @param aConditionString Condition string.
     */
    static void TrimConditionL( RBuf8 & aConditionString );
 
    /**
     * Check condition's brace integrity.
     * 
     * @param aConditionString Condition string.
     * @return Error code. 
     */
    static TInt CheckBraceIntegrityL( TDesC8 & aConditionString );
    
    /**
     * Constructs condition appropriate to its properties.
     * 
     * @param aConditionString Condition string.
     * @return A pointer to the condition's interface.
     */
    static CHnConditionInterface* ConstructConditionL( TDesC8 & aConditionString );

    };

#endif // C_HNCONDITIONFACTORY_H

