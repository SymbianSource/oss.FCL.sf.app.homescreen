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
* Description:  Wrapper class for Animation.
*
*/



#ifndef _XNANIMATION_H
#define _XNANIMATION_H

//  INCLUDES
#include "xncomponent.h"
#include "mxncomponentinterface.h"

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

namespace XnAnimationInterface
{
_LIT8(KType, "animation");
class MXnAnimationInterface : public XnComponentInterface::MXnComponentInterface
   {

public:
		
    static inline const TDesC8& Type()
        {
        return KType;
        }
		
// Add New functions if needed 
   };
 }   
    
/**
*  @ingroup group_xnanimationfactory
*  @lib xn3animationfactory.dll
*  @since Series 60 9.2
*/
class CXnAnimation : public CXnComponent, public XnAnimationInterface::MXnAnimationInterface
   {

public:

	/**
	 * 2 phase construction.
	 */
	static CXnAnimation* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnAnimation();

public: // from CCoeControl
   	/**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType); 
  
	        
private:
    /**
     * Default constructor.
     */
    CXnAnimation();
    
    /**
     * ConstructL
     */
	void ConstructL();
   };

#endif      // _XNANIMATION_H
            
// End of File
