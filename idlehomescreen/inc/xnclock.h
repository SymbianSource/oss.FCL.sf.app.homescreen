/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for CEikLabel.
*
*/


#ifndef _XNCLOCK_H
#define _XNCLOCK_H

//  INCLUDES
#include "xncomponent.h"
#include <e32base.h>
#include "xnnode.h"
#include "mxncomponentinterface.h"


// CLASS DECLARATION

namespace XnClockInterface
    {
    _LIT8( KType, "clock" );

    class MXnClockInterface : public XnComponentInterface::MXnComponentInterface
        {
        public:
        		
            static inline const TDesC8& Type()
                {
                return KType;
                }
		
        public: // New functions

        private:
        };
    }   

/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60
*/
class CXnClock : public CXnComponent, public XnClockInterface::MXnClockInterface
    {
public:

	/**
	 * 2 phase construction.
	 */
	static CXnClock* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnClock();

public: // New functions

public: // from CCoeControl

   	/**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL( const TDesC8& aType );        
            
private:

	CXnClock();
	void ConstructL();
    };

#endif      // _XNCLOCK_H
            
// End of File
