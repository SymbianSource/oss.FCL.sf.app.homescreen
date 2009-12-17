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
* Xuikon component base class
*
*/

#ifndef XNCOMPONENT_H
#define XNCOMPONENT_H

// System includes
#include <e32base.h>

// Forward declarations
class CXnControlAdapter;
class CXnNodePluginIf;
namespace XnComponentInterface
    {
    class MXnComponentInterface;
    }

// Class declaration
/**
 *  Xuikon component base class.
 *
 *  @ingroup group_xnlayoutengine
 *  @lib ?library
 *  @since Series 60 3.1
 */
class CXnComponent : public CBase
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     * @param aType Type.
     */
    IMPORT_C static CXnComponent* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnComponent();

public:
    // New functions

    /**
     * Sets a control adapter.
     *
     * @since Series 60 3.1
     * @param aAdapter Adapter.
     */
    IMPORT_C void SetControlAdapter( CXnControlAdapter* aAdapter );

    /**
     * Returns the control adapter.
     *
     * @since Series 60 3.1
     * @return Control adapter.
     */
    IMPORT_C CXnControlAdapter* ControlAdapter();

    /**
     * Returns component's node.
     *
     * @since Series 60 3.1
     * @return Node.
     */
    IMPORT_C CXnNodePluginIf* Node();

    /**
     * Sets the component's node.
     *
     * @since Series 60 3.1
     * @param aNode Node
     */
    IMPORT_C void SetNode( CXnNodePluginIf& aNode );

    /**
     * Enter power save mode. This is done when application goes to background
     * or lights go off.
     * Derived classes should override this function and stop animations,
     * timers etc. when the function is called.
     */
    IMPORT_C virtual void EnterPowerSaveModeL();

    /**
     * Exit power save mode. This is done when application comes to foreground
     * or lights go on.
     * Derived classes should override this function and restart animations,
     * timers etc. when the function is called.
     */
     IMPORT_C virtual void ExitPowerSaveModeL();

    /**
     * Create a component interface according to the given type.
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */
    IMPORT_C virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
        const TDesC8& aType );

protected:
    /**
     * C++ default constructor.
     */
    IMPORT_C CXnComponent();

    /**
     * Two-phased constructor.
     */
    IMPORT_C void ConstructL();

private:
    // Data

    /**
     * Pointer to control adapter.
     * Own.
     */
    CXnControlAdapter* iAdapter;

    /**
     * Component's node.
     * Not own.
     */
    CXnNodePluginIf* iNode;
    };

#endif
