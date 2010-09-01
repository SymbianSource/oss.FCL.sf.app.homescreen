/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Proxy to access xmluiml language specific element plugins.
*
*/



#ifndef XN_ECOM_ELEMENT_PROXY_H
#define XN_ECOM_ELEMENT_PROXY_H

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include "mxndomvisitor.h"

// CLASS DECLARATION

/**
*  
*  Proxy class to access xmluiml language specific element 
*  visitor plugins
*
*  @lib xnecomelementproxy
*  @since Series 60 3.1
*/
class TXnEComElementProxy
    {
    public:  // Constructors and destructor
        
        /**
        * Load plugin based on it's namespace.
        * Plugin is released calling Release() function.
        * @since Series 60 3.1
        * @param aNS Namespace
        * @return MXnDomVisitor type object. Ownership is transferred to a caller 
        */
        public:
	        IMPORT_C static MXnDomVisitor * CreateImplementationL( const TDesC8& aNS );
    };

#endif      // XN_ECOM_ELEMENT_PROXY_H   
            
// End of File
