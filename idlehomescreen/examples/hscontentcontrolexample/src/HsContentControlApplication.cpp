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
* Description:  Content Control example
*
*/


// [[[ begin generated region: do not modify [Generated System Includes]
// ]]] end generated region [Generated System Includes]

// [[[ begin generated region: do not modify [Generated Includes]
#include "HsContentControlApplication.h"
#include "HsContentControlDocument.h"
#ifdef EKA2
#include <eikstart.h>
#endif
// ]]] end generated region [Generated Includes]

/**
 * @brief Returns the application's UID (override from CApaApplication::AppDllUid())
 * @return UID for this application (KUidHsContentControlApplication)
 */
TUid CHsContentControlApplication::AppDllUid() const
    {
    return KUidHsContentControlApplication;
    }

/**
 * @brief Creates the application's document (override from CApaApplication::CreateDocumentL())
 * @return Pointer to the created document object (CHsContentControlDocument)
 */
CApaDocument* CHsContentControlApplication::CreateDocumentL()
    {
    return CHsContentControlDocument::NewL( *this );
    }

#ifdef EKA2

/**
 *    @brief Called by the application framework to construct the application object
 *  @return The application (CHsContentControlApplication)
 */    
LOCAL_C CApaApplication* NewApplication()
    {
    return new CHsContentControlApplication;
    }

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
 */    
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
    
#else     // Series 60 2.x main DLL program code

/**
* @brief This standard export constructs the application object.
* @return The application (CHsContentControlApplication)
*/
EXPORT_C CApaApplication* NewApplication()
    {
    return new CHsContentControlApplication;
    }

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
*/
GLDEF_C TInt E32Dll(TDllReason /*reason*/)
    {
    return KErrNone;
    }

#endif // EKA2
