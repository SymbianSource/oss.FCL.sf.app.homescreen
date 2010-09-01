/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Views node implementation
*
*/


#ifndef __XNVIEWSNODEIMPL_H__
#define __XNVIEWSNODEIMPL_H__

// System includes
#include    <e32base.h>

// User includes
#include    "xnnodeimpl.h"

// Forward declarations
class CXnType;
class CXnComponent;

// Class declaration

/**
 *  Implementation of views node.
 *  
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since Series 60 3.1
 */
class CXnViewsNodeImpl : public CXnNodeImpl
    {
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    IMPORT_C static CXnViewsNodeImpl* NewL( CXnType* aType );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnViewsNodeImpl();

public:
    // New functions
    
    /**
     * Gets the component associated with the node impl
     * 
     * @since Series 60 3.1
     * @return Component associated with the node impl
     */
    IMPORT_C CXnComponent* Component();

    /**
     * Sets the component associated with the node impl
     * 
     * @since Series 60 3.1
     * @param aComponent Component associated with the node impl
     */
    IMPORT_C void SetComponent( CXnComponent* aComponent );

protected:
    
    /**
     * C++ default constructor.
     */
    IMPORT_C CXnViewsNodeImpl();

    /**
     * Two-phased constructor.
     *
     * @param aType Type.
     */
    IMPORT_C void ConstructL( CXnType* aType );

private:
    // Data    
    
    /** Component associated with the node impl */
    CXnComponent* iComponent;
    };

#endif // __XNVIEWSNODEIMPL_H__
