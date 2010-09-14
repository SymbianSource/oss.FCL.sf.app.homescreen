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
* Description:  Clock rendering plugin
*
*/



#ifndef _XNCLOCKADAPTER_H
#define _XNCLOCKADAPTER_H

// System includes
#include <e32base.h>
#include <hwrmlight.h>

// User includes
#include "xncontroladapter.h"

// Forward declarations
class CXnNodePluginIf;
class CXnClockControl;
class CXnDateControl;
class CAknLayoutFont;
class CFont;
class TRgb;

// Class declaration
/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CXnClockAdapter ) : public CXnControlAdapter, 
    public MCoeMessageMonitorObserver, public MHWRMLightObserver
    {    
public: 
    // constructor and destructor
    
	/**
	 * 2 phase construction.
	 * 
	 * @param aParent Parent control
	 * @param aNode clock node
	 * 
	 * @return Newly constructed object
	 */
	static CXnClockAdapter* NewL( CXnControlAdapter* aParent,
                                  CXnNodePluginIf& aNode );

	/**
	 * Destructor.
	 */
	~CXnClockAdapter();

public: 
	// New functions
    
    /*
     * Create font
     * 
     * @param aNode Node which contains info of font
     * @param aFontName Font name
     * @param aFont On return points to a created font
     */    
    void CreateFontL( CXnNodePluginIf& aNode,
                      const TDesC& aFontName,
                      CFont*& aFont );
    
    /*
     * Create color
     * 
     * @param aNode Node which contains color info
     * @param aColor On return points to a created color
     */        
    void CreateColorL( CXnNodePluginIf& aNode, TRgb& aColor );

    /**
    * Calls DrawNow, if node is laidout
    */        
    void UpdateDisplay() const;
    
    /**
    * Sets clock format
    * 
    * @param aFormat format of clock to set
    */
    void SetClockFormatL( TClockFormat aFormat );
    
    /**
    * Gets text align from text-align property
    * 
    * @param aNode Node which contains property
    * 
    * @return Text align or CGraphicsContext::ECenter if property is not set
    */
    CGraphicsContext::TTextAlign GetTextAlignL( CXnNodePluginIf& aNode );

public: 
    // from base classes

    /**
    * @see CXnControlAdapter documentation
    */    	
    void DoHandlePropertyChangeL( CXnProperty* aProperty = NULL ); 

    /*
     * @see CXnControlAdapter documentation
     */  
    void SkinChanged();

    /*     
     * @see CXnControlAdapter documentation
     */  
    void HandleScreenDeviceChangedL();

    /*     
     * @see CCoeControl documentation
     */      
    void SizeChanged();
    
    /**
    * @see CCoeControl documentation
    */    	            
    void Draw( const TRect& aRect ) const;

    /**
    * @see CCoeControl documentation
    */    	            
    void MakeVisible( TBool aVisible );

private: 
    // from MCoeMessageMonitorObserver
    
    /**
    * @see MCoeMessageMonitorObserver documentation
    */
    void MonitorWsMessage( const TWsEvent& aEvent );
    
private:
    // from MHWRMLightObserver
    
    /**
     * @see MHWRMLightObserver     
     */    
    void LightStatusChanged( TInt aTarget, 
        CHWRMLight::TLightStatus aStatus );
    
private: 
    // constructors

    /**
    * C++ default constructor.
    */
	CXnClockAdapter( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );

    /**
    * By default Symbian 2nd phase constructor is private.
    */	
	void ConstructL();

private: // New functions
        
    /**
     * Starts clock
     */
    void StartClock();
    
    /**
     * Stops clock
     */
    void StopClock();
    
    /**
    * Sets display property of node to block or none
    * 
    * @param aNode Node which display property is set
    * @param aBlock Value of display property to set
    */
    void SetDisplayToBlockL( CXnNodePluginIf& aNode, TBool aBlock );
    
private:
    // Parent control, not owned
    CXnControlAdapter*  		iParent;        
    // UI node, not owned
    CXnNodePluginIf&    		iNode;
    // Digital clock element, not owned    
    CXnNodePluginIf*            iDigital;
    // Analog clock element, not owned
    CXnNodePluginIf*            iAnalog;
    // Day number element, not owned
    CXnNodePluginIf*            iDay;
    // Am\Pm element, not owned
    CXnNodePluginIf*            iAmpm;
    // Digital face 12h element, not owned
    CXnNodePluginIf*            iDigitalFace12;
    // Digital face 24h element, not owned
    CXnNodePluginIf*            iDigitalFace24;
    // Analog face element, not owned
    CXnNodePluginIf*            iAnalogFace;
    // Date element, not owned
    CXnNodePluginIf*            iDate;
    // Digital clock date element, not owned
    CXnNodePluginIf*            iDigitalDate;
    // Analog clock date element, not owned
    CXnNodePluginIf*            iAnalogDate;    
    // Light observer, owned
    CHWRMLight*                 iLightObserver;
    // Clock control, owned
    CXnClockControl*            iClockControl;
    // Date control, owned
    CXnDateControl*             iDateControl;
    // Day number control, owned
    CXnDateControl*             iDayControl;
    // Flag indicating foreground state
    TBool                       iForeground;
    // Flag indicating lights status
    TBool                       iLightsOn;
    };

#endif      // _XNCLOCKADAPTER_H
            
// End of File
