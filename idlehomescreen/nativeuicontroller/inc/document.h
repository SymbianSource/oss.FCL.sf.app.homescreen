/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_DOCUMENT_H
#define C_DOCUMENT_H

#include <AknDoc.h> 
#include <aknapp.h>
#include <aknappui.h>

namespace AiNativeUiController
{
    
class CNativeUiController;

/**
 * Document class of Native UI Controller application. 
 *
 * @lib ainatiui.lib
 * @since S60 3.2
 */
class CDocument : public CAknDocument
    {
public:
    
// Constructors and destructor

    static CDocument* NewL(CAknApplication &aApp, CNativeUiController* aUiCtl);

    virtual ~CDocument();

// Functions from CAknDocument

    CAknAppUi* CreateAppUiL();

private:

    CDocument(CEikApplication &aApp, CNativeUiController* aUiCtl);

private:     // Data

    CNativeUiController* iUiCtl;

    };

} // namespace AiNativeUiController
    
#endif  // C_DOCUMENT_H

// End of File.
