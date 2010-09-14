/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __MCSEXAMPLEAPPLICATION_H__
#define __MCSEXAMPLEAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "MCSExample.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidMCSExampleApp =
    {
    _UID3
    };

// CLASS DECLARATION

/**
 * CMCSExampleApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CMCSExampleApplication is the application part of the
 * AVKON application framework for the MCSExample example application.
 */
class CMCSExampleApplication : public CAknApplication
    {
public:
    // Functions from base classes

    /**
     * From CApaApplication, AppDllUid.
     * @return Application's UID (KUidMCSExampleApp).
     */
    TUid AppDllUid() const;

protected:
    // Functions from base classes

    /**
     * From CApaApplication, CreateDocumentL.
     * Creates CMCSExampleDocument document object. The returned
     * pointer in not owned by the CMCSExampleApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    };

#endif // __MCSEXAMPLEAPPLICATION_H__
// End of File
