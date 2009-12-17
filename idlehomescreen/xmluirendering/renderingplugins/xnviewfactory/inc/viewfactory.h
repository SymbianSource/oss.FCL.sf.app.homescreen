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
* Description:  Factory for creating CXnView -component
*
*/



#ifndef _XNVIEWFACTORY_H
#define _XNVIEWFACTORY_H

//  INCLUDES
#include "xncomponentfactory.h"
#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

// FORWARD DECLARATIONS

/**
*  @ingroup group_xnviewfactory
* 
*  Layout Area UI factory interface for creating UI representations of the layout areas.
*  This class is wrapper for containing a box
*
*  @lib xn3viewfactory.dll
*  @since S6060 3.1
*/
// inheritance order must be this, despite codescanner warnings
class CXnViewFactory : public MXnComponentFactory, public CBase
    {
    public: // Data types

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnViewFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnViewFactory();

    public: // Functions from base classes

        /**
        * From MXnComponentFactory Interface for creating a view component.
        * @since Series 60 3.1
        * @param aNode Component's node.
        * @param aTargetComponent Created component is set here.
        * @return EXnFactoryResponseComponentConstructed if component created.
        */
        TXnComponentFactoryResponse MakeXnComponentL(
                CXnNodePluginIf& aNode, CXnComponent*& aTargetComponent );


        /**
        * From MXnComponentFactory Interface for creating Ui control adapter.
        * @since Series 60 3.1
        * @param aNode Component's node.
        * @param aParent Parent of the adapter, which is created here.
        * @return Created control adapter.
        */  
        CXnControlAdapter* MakeXnControlAdapterL(CXnNodePluginIf& aNode,
                                                 CXnControlAdapter* aParent = NULL);

    private:

        /**
        * 2nd phase constructor. 
        */
        void ConstructL();

        /**
        * C++ default constructor. 
        */
        CXnViewFactory();

    private:    // Data
    };
    
// Exported factory function required by ECOM-framework
IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);

#endif      // _XNVIEWFACTORY_H
            
// End of File
