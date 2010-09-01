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


#include "document.h"
#include "application.h"
#include "appui.h"

using namespace AiNativeUiController;

CDocument::CDocument(CEikApplication &aApp, CNativeUiController* aUiCtl)
    : CAknDocument(aApp),
      iUiCtl(aUiCtl)
    {
    }
CDocument* CDocument::NewL(CAknApplication &aApp, CNativeUiController* aUiCtl)
    {
    CDocument* self = NULL;
    
    self = new (ELeave) CDocument(aApp, aUiCtl);

    return self;
    }

CDocument::~CDocument()
    {
    }

CAknAppUi* CDocument::CreateAppUiL()
    {
    return CAppUi::NewL(iUiCtl);
    }

// End of File.
