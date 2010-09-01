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
* Description:  Component for tooltips and other popups
*
*/



#ifndef _XNPOPUP_H
#define _XNPOPUP_H

//  INCLUDES
#include "xncomponent.h"
#include "xnnode.h"
#include <e32base.h>
#include "mxncomponentinterface.h"

// CLASS DECLARATION

namespace XnPopupInterface
{
    _LIT8(KType, "tooltip");
   

class MXnPopupInterface : public XnComponentInterface::MXnComponentInterface
   {

public:
    static inline const TDesC8& Type()
        {
        return KType;
        }

public: // New functions

    /**
     * Shows the popup
     * @param aContainerRect Rectangle containing the control that has a popup. 
     * The popup will not overlap with this rect.
     */
		virtual void ShowPopupL(const TRect& aContainerRect) = 0;
		virtual void ShowPopupL(const TRect& aContainerRect, 
								TTimeIntervalMicroSeconds32 aShowDelay, 
								TTimeIntervalMicroSeconds32 aDisplayTime) = 0;
	
		/**
		 * Hides the popup
		 */
		virtual void HidePopupL() = 0;
    

private:

   };
}   

/**
*  @ingroup group_xnpopupfactory
*  @lib NONE
*  @since Series 60 3.1
*/
class CXnPopup : public CXnComponent, public XnPopupInterface::MXnPopupInterface
   {
public:
	/**
	 * 2 phase construction.
	 */
	static CXnPopup* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnPopup();

public: // functions from base classes

  /**
   * From MXnPopupInterface Shows the popup
   * @param aContainerRect Rectangle containing the control that has a popup. 
   * The popup will not overlap with this rect.
   * @param aPopupRect Rectangle of the control that has a popup. 
   * The popup will not overlap with this rect and the popup will be drawn adjacent to this rect.
   */
	virtual void ShowPopupL(const TRect& aContainerRect);
    virtual void ShowPopupL(const TRect& aContainerRect, 
    						TTimeIntervalMicroSeconds32 aShowDelay, 
    						TTimeIntervalMicroSeconds32 aDisplayTime);
	/**
	 * From MXnPopupInterface Hides the popup
	 */
	virtual void HidePopupL();
 	/**
    * Create a component interface according to the given type.
    * @param aType Type of the interface to create
    * @return Created interface or NULL if the provided type is not supported.
    */ 
  virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType);        

private:

	CXnPopup();
	void ConstructL();
	
private:

   };


#endif      // _XNPOPUP_H
            
// End of File
