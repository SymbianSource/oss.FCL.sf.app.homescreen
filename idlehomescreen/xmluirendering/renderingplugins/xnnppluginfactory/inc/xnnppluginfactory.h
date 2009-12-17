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
* Description:  Factory for creating CXnNPPlugin -component
*
*/


#ifndef _XNNPPLUGINFACTORY_H
#define _XNNPPLUGINFACTORY_H

//  INCLUDES
#include <ecom/implementationproxy.h>
#include "xncomponentfactory.h"

// FORWARD DECLARATIONS
class CXnComponent;

// CLASS DECLARATION

/**
*  @ingroup group_xnnppluginfactory
* 
*  Layout Area UI factory interface for creating UI representations of the layout areas.
*  This class is wrapper for containing CBitmap
*
*  @lib xn3nppluginfactory.lib
*  @since Series 60 3.1
*/
// inheritance order must be this, despite codescanner warnings
class CXnNPPluginFactory : public MXnComponentFactory, public CBase
    {
    public: // Data types

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnNPPluginFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnNPPluginFactory();

    public: // Functions from base classes
        /**
        * Creates the Xuikon component for text component.
        * @param aNode Corresponding DOM-node
        * @param aTargetComponent Pointer reference to the component to be created.
        */
        virtual TXnComponentFactoryResponse MakeXnComponentL( 
                                                CXnNodePluginIf& aNode,
                                                CXnComponent*& aTargetComponent );
        /**
        * Creates the control adapter for text component.
        * @param aNode Corresponding DOM-node
        * @param aParent Parent of the adapter, which is created here.
        * @return Created control adapter.
        */
        virtual CXnControlAdapter* MakeXnControlAdapterL( 
                                                CXnNodePluginIf& aNode,
                                                CXnControlAdapter* aParent = NULL );
    private:
        /**
        * C++ default constructor. 
        */
        CXnNPPluginFactory();
        
         /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };
    
// Exported factory function required by ECOM-framework
IMPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

#endif      // _XNNPPLUGINFACTORY_H
            
// End of File
