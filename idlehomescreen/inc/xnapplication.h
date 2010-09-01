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
* Application base class for Xuikon
*
*/

#ifndef XNAPPLICATION_H
#define XNAPPLICATION_H

// System includes
#include <aknapp.h>

// Class declaration
/**
 * Application base class for Xuikon.
 *
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since Series 60 3.1
 */
class CXnApplication : public CAknApplication
    {
public:
    /**
     * C++ default constructor.
     */
    IMPORT_C CXnApplication( TUid aApplicationUid );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnApplication();

protected:
    /**
     * From CApaApplication.
     * Returns the application uid.
     *
     * @since Series 60 3.1
     * @return Application uid
     */
    IMPORT_C virtual TUid AppDllUid() const;

    /**
     * From CApaApplication.
     * Creates a document.
     *
     * @since Series 60 3.1
     * @param aProcess Application framework process object
     * @return Application document
     */
    IMPORT_C virtual CApaDocument* CreateDocumentL();

private:
    // Data

    /**
     * application uid
     */
    TUid iApplicationUid;
    };

#endif
