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



#ifndef C_HNMDACTION_H
#define C_HNMDACTION_H

#include <e32base.h>
#include <xmlengdom.h>

class CLiwGenericParamList;
class CLiwDefaultMap;
class TLiwVariant;
class CHnMdBaseKey;
class CHnMdServiceCommand;
class CHnActionModel;
class CHnMdService;
class CHnConditionInterface;

/**
 * Action.
 * 
 * This is the class that represents the action. Actions are used
 * to invoke different functions or methods from services, plugins
 * or internally defined in the Matrix Menu.
 *
 * @lib hnmetadatamodel
 * @since S60 v5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdAction) : public CBase
    {
public:

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdAction* NewL( TXmlEngElement aElement );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdAction* NewLC( TXmlEngElement aElement );

    /**
     * standard destructor
     */
    virtual ~CHnMdAction();
      
    /**
     * Evaluates action model based on the instance of meta data action.
     *
     * @since S60 5.0
     * @param aActionModel Visual model af an action.
     * @param aQueriesResultsList Query results.
     * @param aPos Position in the query.
     * @return Error code.
     */    
    TInt EvaluateL( CHnActionModel* aActionModel,
            const CLiwGenericParamList& aQueriesResultsList, 
            TInt aPos = 0);
    /**
     * Checks if the menu item is valid.
     *
     * @since S60 5.0
     * @param aQueryResults Query results in the form of the CLiwGenericParamList object.
     * @param aPos Position in the query.
     * @return True if valid, otherwise false.
     */
    TBool IsValidL( const CLiwGenericParamList& aQueryResults, 
            TInt aPos = 0 );
    
private:

    /**
     * Default constructor.
     */
    CHnMdAction();

    /**
     * EPOC default constructor for performing 2nd stage construction.
     * 
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );
  
    /**
     * Sets interface.
     * @param aAttr Xml attribute.
     */  
    void SetConditionL( TXmlEngAttr aAttr );
  
    
private: // data

    /**
     * Own - Condition.
     */
    CHnConditionInterface* iCondition;
     
    /**
     * Own.
     */
    CHnMdService* iService;

    };

#endif // C_HNMMACTION_H
