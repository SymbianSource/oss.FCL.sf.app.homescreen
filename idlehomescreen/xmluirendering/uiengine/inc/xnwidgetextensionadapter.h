/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Widget extension adapter
*
*/

#ifndef __XNWIDGETEXTENSIONADAPTER_H__
#define __XNWIDGETEXTENSIONADAPTER_H__

//  INCLUDES
#include "xncontroladapter.h"
#include <e32base.h>

#include "xncontroladapter.h"

// Forward declarations
class CXnAppUiAdapter;
class CXnNode;
class CXnUiEngine;

// CLASS DECLARATION

/**
*  @ingroup group_xnlayoutengine
*  Popup Control adapter interface class
*  @lib xnlayoutengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( CXnWidgetExtensionAdapter ) : public CXnControlAdapter/*, 
    public MCoeMessageMonitorObserver*/
    {
    public: // Constructors and destructor

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
        /**
        * Two-phased constructor.
        */
		static CXnWidgetExtensionAdapter* NewL( CXnNodePluginIf& aNode );
		
        /**
        * Destructor.
        */
		~CXnWidgetExtensionAdapter();
         
    private: // Functions from base classes    
        /**
        * @see CCoeControl documentation        
        */         
        void MakeVisible( TBool aVisible );

        /**
        * see CCoeControl 
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
        * see CCoeControl 
        */
        void Draw(const TRect& aRect) const;        

    private: // Constructors
		/**
        * C++ default constructor.
        */
	    CXnWidgetExtensionAdapter( CXnNodePluginIf& aNode );

	   	/**
        * Two-phased constructor.        
        */ 
	    void ConstructL();	   
	    
    private: // New functions
        /**
        * Hides this control
        */ 
        void HidePopupL();
            
        void CalculatePosition();
        
    private:
        
        // Data
        // UiEngine, not owned
        CXnUiEngine* iUiEngine; 	
        
        // Node 
        CXnNodePluginIf& iNode;
        
        // position hint
        TPositionHint iPositionHint;
        
        CXnAppUiAdapter* iAppUiAdapter;


	};

#endif // __XNWIDGETEXTENSIONADAPTER_H__

// End of File
