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
* Description:
* ECom Component factory interface
*
*/

#ifndef XNCOMPONENTFACTORY_H
#define XNCOMPONENTFACTORY_H

// System includes
#include <e32base.h>

// Forward declarations
class CXnComponent;
class CXnControlAdapter;
class CXnNodePluginIf;

// Class declaration
/**
 * Component UI factory interface for creating UI representations of the layout
 * areas.
 *
 * @ingroup group_xnlayoutengine
 * @lib xnlayoutengine.lib
 * @since Series 60 3.1
 */
class MXnComponentFactory
    {
public:
    // Data types

    /**
     * Response values from the component factory. Indicate the action
     * taken or desired from the factory on the requested area.
     */
    enum TXnComponentFactoryResponse
        {
        /** Successful construction of the component */
        EXnFactoryResponseComponentConstructed,
        /** Successful disposal of the component */
        EXnFactoryResponseComponentDisposed,
        /** The factory does not know how to create/dispose given component,
         * reponsibility is passed onto the next factory in the chain */
        EXnFactoryResponseComponentNotSupported,
        /** The factory does not know how to create the given component
         * and does not wish the given area to be passed on to any other factory
         * for creation - often the area is already handled by a parent area */
        EXnFactoryResponseDenyComponentConstruction
        };

public:

    IMPORT_C virtual ~MXnComponentFactory();

public:
    // New functions

    /**
     * Create a component
     *
     * @since Series 60 3.1
     * @param aNode UI node
     * @param aTargetComponent Target component
     * @return Factory response
     */
    IMPORT_C TXnComponentFactoryResponse CreateXnComponentL(
        CXnNodePluginIf& aNode,
        CXnComponent*& aTargetComponent );

    /**
     * Check whether a node needs a component node implementation
     *
     * @since Series 60 3.1
     * @param aType Node type
     * @return ETrue if a component node implementation is required, EFalse
     *        otherwise
     */
    IMPORT_C TBool DoesNodeNeedComponentImplL( const TDesC8& aType );

protected:
    // New functions

    /**
     * C++ default constructor. Protected in order to allow for derivation.
     */
    IMPORT_C MXnComponentFactory();

private:
    /**
     * Create a component
     *
     * @since Series 60 3.1
     * @param aNode UI node
     * @param aTargetComponent Target component
     * @return Factory response
     */
    IMPORT_C virtual TXnComponentFactoryResponse MakeXnComponentL(
        CXnNodePluginIf& aNode,
        CXnComponent*& aTargetComponent ) = 0;

    /**
     * Create a control adapter
     *
     * @since Series 60 3.1
     * @param aNode UI node
     * @param aParent Parent control
     * @return Created control
     */
    IMPORT_C virtual CXnControlAdapter* MakeXnControlAdapterL(
        CXnNodePluginIf& aNode,
        CXnControlAdapter* aParent = NULL ) = 0;
    };

#endif
