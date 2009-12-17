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



#ifndef C_HNSIMPLECONDITION_H
#define C_HNSIMPLECONDITION_H

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengelement.h> 
#include <liwgenericparam.h>
#include "hnglobals.h"
#include "hnconditioninterface.h"

class CLiwGenericParamList;
class TLiwVariant;

/**
 * Simple Condition.
 * 
 * The simple condition class. This is the base class for different
 * condition classes, e.g. Greater, Has, HasNot conditions.
 *  
 * @lib hierarchynavigatorengine
 * @since S60 v3.2
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnSimpleCondition) : public CHnConditionInterface
    {
public:
   
    /**
     * Standard factory function.
     * 
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnSimpleCondition* NewL( TDesC8 & aElement );

    /**
     * Standard C++ Destructor
     */
    ~CHnSimpleCondition();
    
    /**
     * Removes quotes.
     *
     * @since S60 v3.2
     * @param aValue String to remove quotes from.
     */
    static void RemoveQuotes( RBuf8 & aValue );
    
    /**
     * Returns result.
     *
     * @since S60 v3.2
     * @param aQueryResults Results of query.
     * @param aPos Position.
     * @return True if condition is satisfied.
     */    
    TBool ResultL( const CLiwGenericParamList& aQueryResults, TInt aPos );
        
    /**
     * Returns result.
     *
     * @since S60 v3.2
     * @param aVarLeft Left variant.
     * @param aVarRight Right variant.
     * @return True if condition is satisfied.
     */    
    virtual TBool CheckCondition( TLiwVariant& aVarLeft, TLiwVariant& aVarRight ) = 0;

protected:
    
    /**
     *  Standard C++ Constructor
     */
    CHnSimpleCondition();

    /**
     * Standard symbian second phase constructor.
     * 
     * @param aElement Xml element.
     */
    void ConstructL( TDesC8 & aElement );
    
    /**
     * Gets positions.
     * 
     * @param aBuffer Buffer.
     * @return Position.
     */
    virtual TInt GetPositionL( const TDesC8& aBuffer ) = 0;
    
    /**
     * Gets length.
     * 
     * @return Length of an operator.
     */
    virtual TInt SignLength() = 0;

private:
    
    
    /**
     * Checks if both combinations, if either of the pairs, has this 
     * sort of type configuration.
     *
     * @since S60 v3.2
     * @param aVarLeft Left variant.
     * @param aVarRight Right variant.
     * @param aType1 Variant type 1.
     * @param aType2 Variant type 2.
     */
    static TLiwVariant* CheckTypes( TLiwVariant* aVarLeft, TLiwVariant* aVarRight, 
            LIW::TVariantTypeId aType1, LIW::TVariantTypeId aType2 );
    
    /**
     * Normalizes to one sort of descriptor type.
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */
    static void NormalizeVariantTypeDesL( TLiwVariant* aVar );
    
    /**
     * Tries to match types of content.
     *
     * @since S60 v3.2
     * @param aVarLeft Left variant.
     * @param aVarRight Right variant.
     */    
    static void NegotiateTypesL( TLiwVariant* aVarLeft, TLiwVariant* aVarRight );
    
    /**
     * If list is compared to a string, a decision is made, to substitute list a
     * its Count() scalar.
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */    
    static void NegotiateListL( TLiwVariant & aVar );
    
    /**
     * If map is compared to a string, a decision is made, to substitute list a
     * its Count() scalar.
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */    
    static void NegotiateMapL( TLiwVariant & aVar );
    
    /**
     * If tuint is compared to a string, a decision is made, to substitute 
     * it as tuint
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */    
    static void NegotiateTUintL( TLiwVariant & aVar );
    

    /**
     * 
     * @since S60 v3.2
     * @param aVar Variant.
     */
    static void NegotiateTBoolL( TLiwVariant & aVar );

    
    /**
     * If tuint is compared to a string, a decision is made, to substitute 
     * it as tin32
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */  
    static void NegotiateTInt32L( TLiwVariant & aVar );
    
    /**
     * If tuint is compared to a string, a decision is made, to substitute 
     * it as tin32
     *
     * @since S60 v3.2
     * @param aVar Variant.
     */  
    static void NegotiateTInt64L( TLiwVariant & aVar );
    
    /**
     * Creates implementation of CHnMenuCondition.
     * 
     * @param aElement Xml element.
     * @return Instance of CHnMenuCondition.
     */
    static CHnSimpleCondition* CreateImplementationL( TDesC8 & aElement );

protected: // data
        
    /**
     * Own - Key.
     */
    RBuf8 iReferenceRight;
    
    /**
     * Own - Key.
     */
    RBuf8 iReferenceLeft;
    
    };

#endif // C_HNSIMPLECONDITION_H

//End of file
