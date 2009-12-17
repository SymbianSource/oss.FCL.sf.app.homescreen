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
* Description:  Control adapter class for tooltips and other popups
*
*/



#ifndef _XNPOPUPADAPTER_H
#define _XNPOPUPADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CXnNodePluginIf;

// CLASS DECLARATION
/**
*  @ingroup group_xnpopupfactory
*  @lib NONE
*  @since Series 60 3.1
*/
class CXnPopupAdapter : public CXnControlAdapter
    {
public:
    enum TPositionHint
        {
        ENone,
        EAboveLeft,
        EAboveRight,
        EBelowLeft,
        EBelowRight,
        ELeft,
        ERight
        };
        
public:
	/**
	 * 2 phase construction.
	 */
	static CXnPopupAdapter* NewL(CXnNodePluginIf& aNode, CXnControlAdapter* aParent);

	/**
	 * Destructor.
	 */
	virtual ~CXnPopupAdapter();

public: // New functions
  /**
   *  Shows the popup
   * @param aContainerRect Rectangle containing the control that has a popup. 
   * The popup will not overlap with this rect.
   */
	void ShowPopupL(const TRect& aContainerRect);
    void ShowPopupL(const TRect& aContainerRect, 
    				TTimeIntervalMicroSeconds32 aShowDelay, 
    				TTimeIntervalMicroSeconds32 aDisplayTime);
	/**
	 * Hides the popup
	 */
	void HidePopupL();
	
	/**
	* @see CXnControlAdapter
	*/
	void DoEnterPowerSaveModeL( TModeEvent aEvent );
	
	/**
	* @see CXnControlAdapter
	*/
    void HandleScreenDeviceChangedL();	

	/**
	* @see CXnControlAdapter
	*/
    void DoHandlePropertyChangeL(CXnProperty* aProperty = NULL); 
   
private:
    void Draw(const TRect& aRect) const;

	CXnPopupAdapter();
	void ConstructL(CXnNodePluginIf& aNode, CXnControlAdapter* aParent);
	
    void CalculatePosition( TRect aPopupRect );

private:
	mutable CXnNodePluginIf* iNode;
	CActive* iActiveObject;
	CCoeAppUi* iAppUi;
	TRect iContainerRect;
	TRect iPopupRect;
    TPositionHint iPositionHint;
    };
#endif      // _XNPOPUPADAPTER_H
            
// End of File
