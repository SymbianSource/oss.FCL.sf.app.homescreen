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
* Description:   suite stack
*
*/


#ifndef C_HNSUITESSTACK_H
#define C_HNSUITESSTACK_H

#include <e32base.h>

class CHnSuiteModel;
class MHnControllerInterface;

/**
 *  Class implements Suites stack.
 *  Suite stack is used to keep visual models of suites
 *  in the order they have been opened.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnSuitesStack ): public CBase
    {

public:

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aControllerInterface A HN controller inteface.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnSuitesStack* NewL( MHnControllerInterface& aControllerInterface );
    
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    ~CHnSuitesStack();
    
    /**
     * Pushes a Suite Model onto the stack.
     * 
     * @since S60 v5.0
     * @param aSuiteModel Model to be pushed to stack.
     * @return Quantity of models in the stack after insert.
     */
    IMPORT_C TInt PushL( CHnSuiteModel* aSuiteModel );

    /**
     * Pops the last Suite Model from the top of the stack.
     * Ownership is transfered to the code that invokes it.
     * 
     * @since S60 v5.0
     * @return Suite Model object, ownership is transfered to 
     *         invoking code.
     */
    IMPORT_C CHnSuiteModel* PopL();
    
    /**
     * Gets the last Suite Model, from the top of the stack.
     * 
     * @since S60 v5.0
     * @return Suite Model at the top of the stack.
     */
    IMPORT_C CHnSuiteModel* GetLast();
    
    /**
    * Gets the stack, from a certain position in the stack.
    * Index 0, refers to the lowest Suite Model.
    * 
    * @since S60 v5.0
    * @param aPositon Position to fetch model from.
    * @return Suite Model form a certain posiotion.
    */
    IMPORT_C CHnSuiteModel* Get( TInt aPositon );
    
    /**
     * Gets the Suite Model from the bottom of the stack.
     * 
     * @since S60 v5.0
     * @return Suite Model from the bottom of the stack.
     */
    IMPORT_C CHnSuiteModel* GetRootSuite();
     
    /**
     * Checks if the stack is empty.
     * 
     * @since S60 v5.0
     * @return Bollean value.
     */
    IMPORT_C TBool IsEmpty();
    
    /**
     * Gets the size of the stack.
     * 
     * @since S60 v5.0
     * @return Number of Suite Models in the stack.
     */
    IMPORT_C TInt Count();

private:
    
    /**
     * Constructor.
     * 
     * @since S60 v5.0
     * @param aControllerInterface Reference to App Ui - event observer
     */
    CHnSuitesStack( MHnControllerInterface& aControllerInterface );

    /**
     * Second pahse constructor.
     * 
     * @since S60 v5.0
     */
    void ConstructL( );

private:

    /**
     * Suites stack
     */
    RPointerArray< CHnSuiteModel > iStack;
    
    /**
     * Reference to App Ui - event observer
     */
    MHnControllerInterface& iControllerInterface; 

    };

#endif // C_HNSUITESSSTACK_H
