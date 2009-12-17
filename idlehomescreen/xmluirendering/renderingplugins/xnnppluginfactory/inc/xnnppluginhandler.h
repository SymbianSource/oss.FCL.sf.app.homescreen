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
* Description:  Loads the browser plugin via ECOM.
*
*/



#ifndef XNNPPLUGINHANDLER_H
#define XNNPPLUGINHANDLER_H

//  INCLUDES
#include    <e32base.h>
#include    <npupp.h>

// FORWARD DECLARATIONS
class CEcomBrowserPluginInterface;

// CLASS DECLARATION

/**
*  @ingroup group_xnnppluginfactory
* 
*  Loads the browser plugin via ECOM.
*
*  @lib xn3nppluginfactory.lib
*  @since Series 60 3.1
*/
class CXnNPPluginHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnNPPluginHandler* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnNPPluginHandler();


    public: // New functions
        /**
        * Loads the plugin using ECOM interface. The mime type of the
        * plugin is passed to the function.
        * @since Series 60 3.1
        * @param aResolveparam The mime type of the plugin to be loaded.
        * @param aPluginFuncs Netscape plugin functions
        * @return KErrNotFound if the mime type is not found.
        */
        TInt LoadSpecificPluginL( const TDesC8& aResolveparam,  NPPluginFuncs** aPluginFuncs );

    private:

        /**
        * C++ default constructor.
        */
        CXnNPPluginHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        //Plugin interface, owned
        CEcomBrowserPluginInterface* iPluginInterface;
    };

#endif      // XNNPPLUGINHANDLER_H   
            
// End of File
