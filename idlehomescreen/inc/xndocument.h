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
* Document base class for Xuikon based applications
*
*/

#ifndef XNDOCUMENT_H
#define XNDOCUMENT_H

// System includes
#include <AknDoc.h>

// Class declaration
/**
 * Document base class for Xuikon.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
class CXnDocument : public CAknDocument
    {
public:
    /**
     * C++ default constructor.
     */
    IMPORT_C CXnDocument( CEikApplication& aApp, TUid aApplicationUid );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnDocument();

public:
    // New functions

    /**
     * Get the application uid
     *
     * @since Series 60 3.1
     * @return Application uid
     */
    IMPORT_C TUid Uid();

public:
    /**
     * From CEikDocument.
     * Creates an AppUi
     *
     * @since Series 60 3.1
     * @return AppUi
     */
    virtual CEikAppUi* CreateAppUiL();

private:
    // Data

    /**
     * application uid
     */
    TUid iApplicationUid;
    };

#endif
