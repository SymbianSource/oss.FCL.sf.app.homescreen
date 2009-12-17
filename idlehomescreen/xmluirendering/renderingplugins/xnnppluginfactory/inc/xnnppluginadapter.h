/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for Netscape plugin implementation.
*
*/



#ifndef _XNNPPLUGINADAPTER_H
#define _XNNPPLUGINADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include "xnnpplugincontroladapter.h"
#include <npupp.h>


// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CXnNPPluginHandler;

// CLASS DECLARATION

/**
*  @ingroup group_xnnppluginfactory
* 
*  Wrapper class for Netscape plugin implementation.
*
*  @lib xn3nppluginfactory.lib
*  @since Series 60 3.1
*/
class CXnNPPluginAdapter : public CXnControlAdapter
   {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aParent Parent of the adapter.
        * @param aNode Corresponding DOM-node
        */
        static CXnNPPluginAdapter* NewL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );

        /**
        * Destructor.
        */
        virtual ~CXnNPPluginAdapter();

    public: // Functions from base classes

        /**
        * From CCoeControl Responds to changes to the size and position
        * of the contents of this control.
        * @since Series 60 3.1
        * @return void
        */
        void SizeChanged();
        
        /**
        * Enter power save mode. This is done when application goes to background or lights go off. 
        * Derived classes should override this function and stop animations, timers etc. when the function is called.
        */ 
        void DoEnterPowerSaveModeL(TModeEvent aEvent);

	   	/**
        * Exit power save mode. This is done when application comes to foreground or lights go on. 
        * Derived classes should override this function and restart animations, timers etc. when the function is called.
        */ 
        void DoExitPowerSaveModeL(TModeEvent aEvent);
        
    private:
        /**
        * C++ default constructor.
        */
        CXnNPPluginAdapter();
 
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
    
        TInt ConstructPluginL( CXnControlAdapter* aParent );
    
    private: // Data
        //Node interface, not owned
        CXnNodePluginIf*            iNode;
        //File server handle, owned
        RFs                         iFs;
        //CFileman instance, owned
        CFileMan*                   iFileman;
        //Stores the content file name, owned
        HBufC*                      iContentFile;
        //Pointer to plugin controller, owned
        CXnNPPluginControlAdapter*   iPluginController;
        //Pointer to plugin handler, owned
        CXnNPPluginHandler*			iPluginHandler;
        //Pointer to NPP instance, owned
        NPP                         iInstance;
        //Pointer to NPN functions, owned
        NPPluginFuncs*              iPluginFuncs;
   };

#endif      // _XNNPPLUGINADAPTER_H
            
// End of File
