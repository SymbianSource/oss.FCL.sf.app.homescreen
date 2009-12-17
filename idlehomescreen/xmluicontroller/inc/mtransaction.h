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
* Description:  Transaction base class
*
*/


#ifndef M_TRANSACTION_H
#define M_TRANSACTION_H

#include <e32hashtab.h>


inline TUint32 NodeHash(const TAny* aPtr)
    {
    return (TUint32)aPtr;
    };

inline TBool NodeRelation(const TAny* aPtr1, const TAny* aPtr2)
    {
    return aPtr1 == aPtr2;
    };

// Default functions for xnnode type

TEMPLATE_SPECIALIZATION class RHashTableBase::Defaults<
                                        CXnNodeAppIf,
                                        RHashTableBase::EDefaultSpecifier_Normal>
	{
public:
	inline static TGeneralHashFunction32 Hash();
	inline static TGeneralIdentityRelation Id();
	};

inline TGeneralHashFunction32 RHashTableBase::Defaults<
                                        CXnNodeAppIf,
                                        RHashTableBase::EDefaultSpecifier_Normal>::Hash()
	{return (TGeneralHashFunction32)&NodeHash;}

inline TGeneralIdentityRelation RHashTableBase::Defaults<
                                        CXnNodeAppIf,
                                        RHashTableBase::EDefaultSpecifier_Normal>::Id()
	{return (TGeneralIdentityRelation)&NodeRelation;}

	
namespace AiXmlUiController
{
    
class CTransactionElement;
class MTransactionElement;
class MTransactionFactory;

typedef RPointerArray<CXnProperty> RPropertyArray;

typedef RPtrHashMap<CXnNodeAppIf, RPropertyArray> RPropertyHashMap;

/**
 *  MTransaction represents a transaction of publish operations.
 *
 *  @lib AiXmlUiMain
 */
class MTransaction
    {

public:

    /**
     * Appends a transaction element in this transaction.
     *
     * @param aElement the transaction element
     */
    virtual void Append(MTransactionElement& aElement) = 0;
    
    /**
     * Commits this transaction by committing all transaction elements.
     *
     * @param [out] aLayoutChanged true if UI layout changes
     */
    virtual void CommitL( TBool& aLayoutChanged, RPropertyHashMap& aPropertyHashMap ) = 0;
    
    /**
     * Resets this transaction for reuse.
     *
     * @param aFactory the factory which created this transaction
     */
    virtual void Reset(MTransactionFactory& aFactory) = 0;
    
    virtual TDblQueIter<CTransactionElement> ElementIter() = 0;
protected:

    ~MTransaction() {};

    };

} // namespace AiXmlUiController

#endif // M_UPDATETRANSACTION_H
