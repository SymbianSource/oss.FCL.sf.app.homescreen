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
* Description:  Factory for creating CXnNewsticker -component
*
*/


#ifndef _XNNEWSTICKERFACTORY_H
#define _XNNEWSTICKERFACTORY_H

//  INCLUDES
#include "xncomponentfactory.h"

// FORWARD DECLARATIONS
class CXnComponent;

// CLASS DECLARATION

/**
* @ingroup group_xnnewstickerfactory
* 
*  Component factory interface for creating UI representations of the newsticker element.
*
*  @lib xn3newstickerfactory.dll
*  @since Series 60 3.2
*/
// inheritance order must be this, despite codescanner warnings
class CXnNewstickerFactory : public MXnComponentFactory, public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnNewstickerFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnNewstickerFactory();
        
    public: // Functions from base classes

        /**
        * Creates the Xuikon component for newsticker component.
        * @param aNode Corresponding DOM-node
        * @param aTargetComponent Pointer reference to the component to be created.
        * @return EXnFactoryResponseComponentConstructed if component created. 
        */
        virtual TXnComponentFactoryResponse MakeXnComponentL(CXnNodePluginIf& aNode,
            CXnComponent*& aTargetComponent);

        /**
        * From MXnComponentFactory Interface for creating UI control adapter.
        * @param aNode Component's node.
        * @param aParent Parent of the adapter, which is created here.
        * @return Created control adapter.
        */        
        virtual CXnControlAdapter* MakeXnControlAdapterL(CXnNodePluginIf& aNode,
            CXnControlAdapter* aParent);
        
    private:

        void ConstructL();

        CXnNewstickerFactory();
    };
    
// Exported factory function required by ECOM-framework
IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);

#endif      // _XNNEWSTICKERFACTORY_H
            
// End of File
