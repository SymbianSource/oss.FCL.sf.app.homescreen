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
* Description: Listens for change in ecom registry of content controller
* interface.  
*
*/

#ifndef HSCONTENTCONTROLECOMLISTENER_H 
#define HSCONTENTCONTROLECOMLISTENER_H 

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class MHsContentControlEComObserver;
class REComSession;

// Class declaration
/** 
* @class CHsContentControlEComListener
* 
* @brief An instance of class CHsContentControlEComListener which listens for 
*  changes in the ECOM registry.
*  
* @lib hscontentcontrol.lib
*/
NONSHARABLE_CLASS( CHsContentControlEComListener ) : public CActive
    {
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */ 
    static CHsContentControlEComListener* NewL( 
            MHsContentControlEComObserver& aContentControlEComObserver );                                         
    
    /**
     * Destructor.
     */
    ~CHsContentControlEComListener();

protected: // Functions from base classes
   /**
    * From CActive
    */
   void RunL();

   /**
    * From CActive
    */
   void DoCancel();

   /**
    * From CActive
    */
   TInt RunError( TInt aError );

private: // Constructors
    
   /**
    * Default C++ constructor.
    */ 
    CHsContentControlEComListener( 
            MHsContentControlEComObserver& aHsContentControlEComObserver );

    /**
     * Second phase constructor.
     */ 
    void ConstructL();

private: // Data

    // Reference of MHsContentControlEComObserver
    MHsContentControlEComObserver& iHsContentControlEComObserver;
    
    // An object of type REComSession.
    REComSession* iEComSession;
    };

#endif  // HSCONTENTCONTROLECOMLISTENER_H

// End of file
 
