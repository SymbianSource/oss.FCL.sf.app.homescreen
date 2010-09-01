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



#ifndef _XNTEXT_H
#define _XNTEXT_H

//  INCLUDES
#include "xncomponent.h"
#include <e32base.h>
#include "xnnode.h"
#include "mxncomponentinterface.h"


// CLASS DECLARATION

namespace XnTextInterface
{
_LIT8(KType, "text");
class MXnTextInterface : public XnComponentInterface::MXnComponentInterface
   {

public:
		
    static inline const TDesC8& Type()
        {
        return KType;
        }
		
public: // New functions

    /**
     * Sets the new content to the underlying CEikLabel
     * @param aText The new content
     */
	virtual void SetTextL(const TDesC& aText) = 0;
	
	/**
	 * Returns the text contained in the underlying CEikLabel
	 * @return TDesC* The text
	 */
	virtual const TDesC* Text() = 0;
    

private:

   };
 }   

/**
*  @ingroup group_xntextfactory
*  @lib xn3textfactory.lib
*  @since Series 60 3.1
*/
class CXnText : public CXnComponent, public XnTextInterface::MXnTextInterface
   {

public:


	/**
	 * 2 phase construction.
	 */
	static CXnText* NewL();


	/**
	 * Destructor.
	 */
	virtual ~CXnText();

public: // New functions

    /**
     * Sets the new content to the underlying CEikLabel
     * @param aText The new content
     */
	void SetTextL(const TDesC& aText);
	
	/**
	 * Returns the text contained in the underlying CEikLabel
	 * @return TDesC* The text
	 */
	const TDesC* Text();
    

public: // from CCoeControl

   	/**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType);        
    

        
private:

	CXnText();
	void ConstructL();

   };


#endif      // _XNTEXT_H
            
// End of File
