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
* Description:  Implements Xuikon animation component.
*
*/



#ifndef _XNANIMATIONAPADAPTER_H
#define _XNANIMATIONAPADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"


// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CAknBitmapAnimation;
class CXnAnimationControl;
class CPeriodic;

// CLASS DECLARATION
/**
*  @ingroup group_xnbitmapfactory
*  @lib xn3bitmapfactory.dll
*  @since Series 60 3.1
*/
class CXnAnimationAdapter : public CXnControlAdapter
   {
    public:
        /**
         * 2 phase construction.
         */
        static CXnAnimationAdapter* NewL(CXnNodePluginIf& aNode);
    
        /**
         * Destructor.
         */
        virtual ~CXnAnimationAdapter();
    
    public: // From Base classes 
        
        // From CXnControlAdapter
        void DoHandlePropertyChangeL(CXnProperty* aProperty = NULL);
        
        // From CXnControlAdapter
        void DoEnterPowerSaveModeL( TModeEvent aEvent );
        
        // From CXnControlAdapter
        void DoExitPowerSaveModeL( TModeEvent aEvent );
        
    protected:
        /**
         * From CCoeControl.
         * Draws the control.
         * 
         * @param aRect TRect to draw
         */
        void Draw(const TRect& aRect) const;
        
        /**
        * From CCoeControl.
        * Control   visual state.
        * 
        * @param aVisible visibility (ETrue/EFalse)
        */
        void MakeVisible( TBool aVisible );
        
    private:
        /**
        * Constructor 
        * 
        * @param aNode plugin node
        */
        CXnAnimationAdapter(CXnNodePluginIf& aNode);
        
        /**
        * 2nd phase constructor 
        * 
        * @param aNode plugin node
        */
        void ConstructL(CXnNodePluginIf& aNode);
    
        /**
        * Periodic timer call back  
        * 
        * @param aAny pointer reference
        */
        static TInt TimerCallBack(TAny* aAny);
        
        /**
        * Update the next image for animation  
        * 
        */
        void Update();
    
        /**
        * Starts the animation   
        * 
        */
        void StartAnimation();
        
        /**
        * Stops the animation   
        * 
        */
        void StopAnimation();
        
    private: // Data
        
        // UI node, not owned
        CXnNodePluginIf& iNode;
       
        // Image Skind Id
        TAknsItemID iSkinId;
       
        // Timer for animation, owned  
        CPeriodic* iPeriodicTimer;
        
        // Whether the renderer is in power save mode or not
        TBool iInPowerSaveMode;
   };

#endif      // _XNANIMATIONAPADAPTER_H
            
// End of File
