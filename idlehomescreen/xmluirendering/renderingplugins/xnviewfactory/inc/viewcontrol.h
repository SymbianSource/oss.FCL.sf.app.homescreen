/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon View plugin.
*
*/

#ifndef _VIEWCONTROL_H
#define _VIEWCONTROL_H

//  System includes
#include <e32base.h>

// User includes
#include "xncontroladapter.h"

// Forward declarations
class CXnNodePluginIf;

// Class declaration
/**
*  @ingroup group_xnviewfactory
*  @lib xn3viewfactory.dll
*  @since S6060 3.1
*/   
class CViewControl : public CXnControlAdapter
   {
public: 
    // Constructors and destructor
    
    /**
     * 2 phase construction.
     */
    static CViewControl* NewL( CXnNodePluginIf& aNode );

    /**
     * Destructor.
     */
    ~CViewControl();

private: 
    // Constructor
    CViewControl();
    
    void ConstructL( CXnNodePluginIf& aNode );
   
private:
    // data
    
   };

#endif      // _VIEWCONTROL_H
            
// End of File
