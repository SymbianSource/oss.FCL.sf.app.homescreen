/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory for creating CXnBitmap -component
*
*/



#ifndef _XNBITMAPFACTORY_H
#define _XNBITMAPFACTORY_H

//  INCLUDES
#include "xncomponentfactory.h"
#include <e32base.h>
#include <e32std.h>

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

// FORWARD DECLARATIONS
class CXnComponent;
class CXnMap;
class CXnType;

// CLASS DECLARATION

/**
*  @ingroup group_xnbitmapfactory
*  Layout Area UI factory interface for creating UI representations of the layout areas.
*  This class is wrapper for containing CBitmap
*
*  @lib xn3bitmapfactory.dll
*  @since Series 60 3.1
*/
// inheritance order must be this, despite codescanner warnings
class CXnBitmapFactory : public MXnComponentFactory, public CBase
    {
    public: // Data types

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnBitmapFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnBitmapFactory();

    public: // Functions from base classes

		/**
		* @see MXnComponentFactory.h
		*/
        TXnComponentFactoryResponse MakeXnComponentL( 
                                                CXnNodePluginIf& aNode,
                                                CXnComponent*& aTargetComponent
        										);

		/**
		* @see MXnComponentFactory.h
		*/
        CXnControlAdapter* MakeXnControlAdapterL( 
                                                CXnNodePluginIf& aNode,
                                                CXnControlAdapter* aParent = NULL
        										);

    private:

        /**
        * 2nd phase constructor. 
        */
        void ConstructL();

        /**
        * C++ default constructor. 
        */
        CXnBitmapFactory();
    };
    
// Exported factory function required by ECOM-framework
IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);

#endif      // _XNBITMAPFACTORY_H
            
// End of File
