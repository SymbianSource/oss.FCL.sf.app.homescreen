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
* Xuikon component node implementation
*
*/

#ifndef XNCOMPONENTNODEIMPL_H
#define XNCOMPONENTNODEIMPL_H

// System includes
#include <e32base.h>

// User includes
#include "xnnodeimpl.h"

// Forward declarations
class CXnComponent;
namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Class declaration
/**
 * Xuikon component node implementation
 *
 * @ingroup group_xnlayoutengine
 * @lib ?library
 * @since Series 60 3.1
 */
class CXnComponentNodeImpl : public CXnNodeImpl
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    IMPORT_C static CXnComponentNodeImpl* NewL( CXnType* aType );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnComponentNodeImpl();

public:
    // New functions

    /**
     * Get the component
     *
     * @since Series 60 3.1
     * @return Component
     */
    IMPORT_C CXnComponent* Component();

    /**
     * Set the component
     *
     * @since Series 60 3.1
     * @param aComponent Component
     */
    IMPORT_C void SetComponent( CXnComponent* aComponent );

    /**
     * Returns whether a component should be created for the node
     *
     * @since Series 60 3.1
     * @return Flag indicating whether a component should be created for the node
     */
    IMPORT_C TBool DoesComponentNeedCreation() const;

    /**
     * Set the flag indicating whether a component should be created for the node
     *
     * @since Series 60 3.1
     * @param aFlag Flag indicating whether a component should be created for the
     *        node
     */
    IMPORT_C void SetComponentNeedsCreation( TBool aFlag );

public:
    /**
     * From CXnNodeImpl.
     * Create a component interface according to the given type.
     *
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */
    IMPORT_C virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
        const TDesC8& aType );

protected:
    /**
     * C++ default constructor.
     */
    IMPORT_C CXnComponentNodeImpl();

    /**
     * Two-phased constructor.
     *
     * @param aType Type
     */
    IMPORT_C void ConstructL( CXnType* aType );

private:
    // Data

    /**
     * Component.
     * Own.
     */
    CXnComponent* iComponent;

    /**
     * Flag indicating whether a component should be created for the node
     */
    TBool iComponentNeedsCreation;
    };

#endif
