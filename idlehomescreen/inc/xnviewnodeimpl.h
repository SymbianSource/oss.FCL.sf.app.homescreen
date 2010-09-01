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
* Implementation of view node
*
*/

#ifndef XNVIEWNODEIMPL_H
#define XNVIEWNODEIMPL_H

// System includes
#include <e32base.h>

// User includes
#include "xncomponentnodeimpl.h"

// Forward declarations
class CXnType;

// Class declaration
/**
 * Implementation of view node.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
NONSHARABLE_CLASS( CXnViewNodeImpl ) : public CXnComponentNodeImpl
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    static CXnViewNodeImpl* NewL( CXnType* aType );

    /**
     * Destructor.
     */
    virtual ~CXnViewNodeImpl();

protected:
    /**
     * C++ default constructor.
     */
    CXnViewNodeImpl();

    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    void ConstructL( CXnType* aType );

private:
    // Data
    };

#endif

// End of file
