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


#ifndef _XNVOLUMECONTROLADAPTER_H
#define _XNVOLUMECONTROLADAPTER_H

//  INCLUDES
#include <e32base.h>
#include "xncontroladapter.h"

// FORWARD DECLARATIONS
class CXnNodePluginIf;
class MXnVolumeControlObserver;
class CAknVolumeControl;

// CLASS DECLARATION
/**
*  @ingroup group_xnvolumecontrolfactory
*  @lib xnvolumecontrol.lib
*  @since Series 60 3.2
*/
class CXnVolumeControlAdapter : public CXnControlAdapter, public MCoeControlBackground
   {
public:
	/**
	 * 2 phase construction.
	 */
	static CXnVolumeControlAdapter* NewL(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);

	/**
	 * Destructor.
	 */
	virtual ~CXnVolumeControlAdapter();

public: // New functions
    /**
    * Sets the value of the volume control
    * @param aValue new value
    */
	void SetValueL( TInt aValue );
	
    /**
    * Returns the value of the volume control
    */	
	TInt ValueL();
	
    /**
    * Sets the range for the volume control
    * @param aMinimumValue Minimum value
    * @param aMaximumValue Maximum value
    */	
	void SetRangeL( TInt aMimimumValue, TInt aMaximumValue );
	
    /**
    * Returns the minimum and maximum value of the volume control range
    * @param aMinimumValue minimum value reference
    * @param aMaximumValue maximum value reference
    */	
	void GetRangeL( TInt& aMinimumValue, TInt& aMaximumValue );
	
	/**
	* Sets the observer for the volume control
	* @param pointer to the observer
	*/ 
	void SetObserver( MXnVolumeControlObserver* aObserver );
	
public: // from base classes

    /**
    * From CXnControlAdapter Handles the property changes.
    * @since Series 60 3.1
    * @return void.
    */  
    void DoHandlePropertyChangeL( CXnProperty* aProperty = NULL ); 
    
    /**
    * From CCoeControl Handles the resource change.
    * @since Series 60 3.1
    * @param aType A type of the resource change
    * @return void.
    */
    void HandleScreenDeviceChangedL();

    /**
    * See CCoeControl documentation
    */    	
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
    /**
    * See CCoeControl documentation
    */    
    void SizeChanged();
    
    /**
    * See CCoeControl documentation
    */    
    TInt CountComponentControls() const;
    
    /**
    * See CCoeControl documentation
    */    
    CCoeControl* ComponentControl( TInt aIndex ) const;

  	/**
    * From MCoeControlBackground. Draws the background for a given control.
    * @param aGc Graphics context used for drawing.
    * @param aControl The control being drawn.
    * @param aRect The area to be redrawn.
  	*/
  	void Draw( CWindowGc& aGc, const CCoeControl& aControl,
  	           const TRect& aRect ) const;

private:
	CXnVolumeControlAdapter(CXnControlAdapter* aParent, CXnNodePluginIf& aNode);
	
	void ConstructL();
	
    virtual void Draw(const TRect& aRect) const;
    
    /**
    * Returns the current volume value from property
    */
    TInt GetCurrentValueL();
    
    /**
    * Notifies the observer with new volume value
    */
    void NotifyObserverL();

	/**
	 * Sets volume control CSS properties
     * @param aNode UI Node
     * @return void
	 */
	void SetVolumeControlPropertiesL();
	
	/**
	* Constructs the volume control component from resource
	*/
	void ConstructVolumeControlL();
	
private:
    CXnControlAdapter*  		iParent;        // Parent control, not owned
    CXnNodePluginIf&    		iNode;          // UI node, not owned
    CAknVolumeControl*			iVolumeControl; // Avkon Volume control. Owned.
    MXnVolumeControlObserver*   iObserver;      // Observer. Not owned.
    TRect                       iHitRect;       // Rect where pointer event previously hit
    TInt                        iSteps;         // number of steps
    TInt                        iMutedValue;    // previous value when muted
    TBool                       iMuted;         // is muted?    
    TBool                       iButton1Down;   // Button 1 is pressed down
    
   };

#endif      // _XNVOLUMECONTROLADAPTER_H
            
// End of File
