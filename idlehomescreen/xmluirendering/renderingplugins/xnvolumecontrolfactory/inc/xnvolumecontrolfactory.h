/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory for creating CXnVolumeControl -component
*
*/


#ifndef _XNVOLUMECONTROLFACTORY_H
#define _XNVOLUMECONTROLFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "xncomponentfactory.h"

// FORWARD DECLARATIONS
class CXnComponent;

// CLASS DECLARATION

/**
*  @ingroup group_xnvolumecontrolfactory
* 
*  Component factory interface for creating UI representations of the elements.
*  This class is wrapper for containing CEikLabel
*
*  @lib xnvolumecontrol.lib
*  @since Series 60 3.2
*/
// inheritance order must be this, despite codescanner warnings
class CXnVolumeControlFactory : public MXnComponentFactory, public CBase
    {
    public: // Data types

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnVolumeControlFactory* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnVolumeControlFactory();


    public: // Functions from base classes

        /**
        * Creates the Xuikon component for text component.
        * @param aNode Corresponding DOM-node
        * @param aTargetComponent Pointer reference to the component to be created.
        */
        virtual TXnComponentFactoryResponse MakeXnComponentL( 
                                                CXnNodePluginIf& aNode,
                                                CXnComponent*& aTargetComponent
        										);
        
        /**
        * Creates the control adapter for text component.
        * @param aNode Corresponding DOM-node
        */
        virtual CXnControlAdapter* MakeXnControlAdapterL( 
                                                CXnNodePluginIf& aNode,
                                                CXnControlAdapter* aParent
        										);

    private:

        /**
        * 2nd phase constructor. 
        */
        void ConstructL();

        /**
        * C++ default constructor. 
        */
        CXnVolumeControlFactory();

    };
    
// Exported factory function required by ECOM-framework
IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);

#endif      // _XNVOLUMECONTROLFACTORY_H
            
// End of File
