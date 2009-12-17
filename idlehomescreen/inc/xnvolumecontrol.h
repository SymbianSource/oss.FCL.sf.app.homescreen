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
* Description:  Wrapper class for AknVolumeControl
*
*/


#ifndef _XNVOLUMECONTROL_H
#define _XNVOLUMECONTROL_H

//  INCLUDES
#include <e32base.h>
#include "xncomponent.h"
#include "mxncomponentinterface.h"

// FORWARD DECLARATIONS
class MXnVolumeControlObserver;

// CLASS DECLARATION
namespace XnVolumeControlInterface
{
_LIT8(KType, "volumecontrol");
class MXnVolumeControlInterface : public XnComponentInterface::MXnComponentInterface
   {
public:
		
    static inline const TDesC8& Type()
        {
        return KType;
        }
		
public: // New functions
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/	
	virtual void SetValue( TInt aValue ) = 0;
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/	
	virtual TInt Value() = 0;
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	virtual void SetRange( TInt aMinimumValue, TInt aMaximumValue ) = 0;
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/	
	virtual void GetRange( TInt& aMiminumValue, TInt& aMaximumValue ) = 0;
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	virtual void SetObserver( MXnVolumeControlObserver* aObserver ) = 0;

private:

   };
 }   

/**
*  @ingroup group_xnvolumecontrolfactory
*  @lib xnvolumecontrol.lib
*  @since Series 60 3.2
*/
class CXnVolumeControl : public CXnComponent, public XnVolumeControlInterface::MXnVolumeControlInterface
   {

public:
	/**
	 * 2 phase construction.
	 */
	static CXnVolumeControl* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnVolumeControl();

public: // New functions
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	void SetValue( TInt aValue );
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	TInt Value();
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	void SetRange( TInt aMinimumValue, TInt aMaximumValue );
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	void GetRange( TInt& aMiminumValue, TInt& aMaximumValue );
	
	/**
	* Commented in xnvolumecontroladapter.h
	*/
	void SetObserver( MXnVolumeControlObserver* aObserver );

public:

   	/**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType);        
        
private:
	CXnVolumeControl();
	void ConstructL();
   };

#endif      // _XNVOLUMECONTROL_H
            
// End of File
