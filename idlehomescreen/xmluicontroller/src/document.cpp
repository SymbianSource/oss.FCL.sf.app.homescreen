/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Document class
*
*/


#include "document.h"
#include "application.h"
#include "appui.h"
#include "debug.h"

using namespace AiXmlUiController;

// ========== MEMBER FUNCTIONS ================================================

CDocument::CDocument(CEikApplication &aApp, CXmlUiController& aUiCtl)
    : CXnDocument(aApp, KUidXmlUiApp),
      iUiCtl(aUiCtl)
    {
    }
CDocument* CDocument::NewL(CEikApplication &aApp, CXmlUiController& aUiCtl)
    {
    CDocument* self = NULL;
    
    __TIME("XML UI: Create Document",
    	self = new (ELeave) CDocument(aApp, aUiCtl);
	);
	__HEAP("XML UI: Create Document");

    return self;
    }

CDocument::~CDocument()
    {
    }

CEikAppUi* CDocument::CreateAppUiL()
    {
    return CAppUi::NewL(iUiCtl);
    }

// End of File.
