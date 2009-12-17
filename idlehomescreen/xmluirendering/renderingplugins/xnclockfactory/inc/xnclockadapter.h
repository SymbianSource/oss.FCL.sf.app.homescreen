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
* Description:  Wrapper class for CAknSkinnableClock
*
*/



#ifndef _XNCLOCKADAPTER_H
#define _XNCLOCKADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CXnClockControl;
class CAknLayoutFont;
class CFont;
class TRgb;

// CLASS DECLARATION
/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60
*/
class CXnClockAdapter : public CXnControlAdapter, 
                        public MCoeMessageMonitorObserver
    {
public:
    enum TClockFont
        {
        EDigitalFont,
        EAmPmFont,
        EDateFont
        };
    
public: // constructor and destructor
	/**
	 * 2 phase construction.
	 */
	static CXnClockAdapter* NewL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );

	/**
	 * Destructor.
	 */
	~CXnClockAdapter();

public: // New functions

    /** 
    * Gets font based on type
    * 
    * @return Font based on aType, NULL if not found
    */    
    const CAknLayoutFont* FontL( const TInt aType );
        
    /**
    * Gets text color
    * 
    * @return Text color
    */
    const TRgb& TextColorL();

    /**
    * Gets date information
    * 
    * @return A node which holds date information
    */    
    CXnNodePluginIf* Date() const;

    /**
    * Gets day information
    * 
    * @return A node which holds day information
    */    
    CXnNodePluginIf* Day() const;

    /**
    * Calls DrawNow, if node is laidout
    */        
    void UpdateDisplay() const;

public: // from base classes

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

private: // from MCoeMessageMonitorObserver
    /**
    * @see MCoeMessageMonitorObserver documentation
    */
    void MonitorWsMessage( const TWsEvent& aEvent );
    
private: // constructors

    /**
    * C++ default constructor.
    */
	CXnClockAdapter( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );

    /**
    * By default Symbian 2nd phase constructor is private.
    */	
	void ConstructL();

private: // New functions

    /*
     * Create font
     */    
    void CreateFontL( const TInt aType );

    /*
     * Create color
     */        
    void CreateColorL();
        
private:
    // Parent control, not owned
    CXnControlAdapter*  		iParent;        
    // UI node, not owned
    CXnNodePluginIf&    		iNode;
    // Digital clock date information, not owned
    CXnNodePluginIf*            iDate;
    // Analog clock day information, not owned
    CXnNodePluginIf*            iDay;
    // Clock control, owned    
    CXnClockControl*            iClockControl;  
    // Digital clock font, not owned
    CFont*                      iDigitalFont;   
    // AmPm font, not owned
    CFont*                      iAmPmFont;      
    // Date font, not owned
    CFont*                      iDateFont;      
    // Font color
    TRgb                        iColor;
    // Flag for initializing font color
    TBool                       iColorSet;
    // Flag indicating foreground state
    TBool                       iForeground;
    };

#endif      // _XNCLOCKADAPTER_H
            
// End of File
