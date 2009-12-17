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
* Description:  Wrapper class for Netscape Plugin API.
*
*/



#ifndef _XNNPPLUGIN_H
#define _XNNPPLUGIN_H

//  INCLUDES
#include "xncomponent.h"

// CLASS DECLARATION

/**
*  @ingroup group_xnnppluginfactory
* 
*  Wrapper class for Netscape Plugin API.
*
*  @lib xn3nppluginfactory.lib
*  @since Series 60 3.1
*/
class CXnNPPlugin : public CXnComponent
   {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CXnNPPlugin* NewL();

        /**
        * Destructor.
        */
        virtual ~CXnNPPlugin();

    private:
        /**
        * C++ default constructor.
        */
        CXnNPPlugin();
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
   };

#endif    // _XNNPPLUGIN_H

// End of File
