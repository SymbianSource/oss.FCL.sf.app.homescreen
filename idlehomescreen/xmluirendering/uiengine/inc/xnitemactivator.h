/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Item activator
*
*/


#ifndef _XNITEMACTIVATOR_H
#define _XNITEMACTIVATOR_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CXnAppUiAdapter;
class CXnNode;

/**
 * Item activator
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CXnItemActivator ) : public CTimer
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnItemActivator* NewL( CXnAppUiAdapter& aAppUiAdapter );        

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnItemActivator* NewLC( CXnAppUiAdapter& aAppUiAdapter );         

    /**
     * Destructor
     */
    ~CXnItemActivator();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnItemActivator( CXnAppUiAdapter& aAppUiAdapter );         
    
private:
    // from CTimer
    
    /**
     * @see CTimer
     */
    void RunL();

    /**
     * @see CTimer
     */    
    TInt RunError( TInt aError );
    
    /**
     * @see CTimer
     */    
    void DoCancel();
    
public: 
    // new functions
    
    /**
     * Schedules item activation. 
     * Outstanding activation is cancelled.
     * 
     * @since S60 5.2
     * @param aItemToActivate Item to activate
     */
    void Activate( CXnNode* aItemToActivate );
            
private:
    // data

    /** AppUiAdapter, not owned */
    CXnAppUiAdapter& iAppUiAdapter;
    /** Activate trigger, owned */
    CXnNode* iTrigger;
    /** Item to activate, not owned */
    CXnNode* iItemToActivate;
    };

#endif // _XNITEMACTIVATOR_H

// End of file
