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
* Description:  Application class
*
*/


#include <bautils.h>
#include <aisystemuids.hrh>
#include "application.h"
#include "document.h"
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include "debug.h"

namespace AiXmlUiController {


_LIT( KAiXmlUiResourceDrive, "z:" );
_LIT( KAiXmlUiResourceFile, "Ai3XmlUi.rsc" );


// ========== MEMBER FUNCTIONS ================================================

CApplication::CApplication(TUid aAppUid, CXmlUiController& aUiCtl)
    : CXnApplication(aAppUid),
      iUiCtl(aUiCtl)
    {
    }

CApplication* CApplication::New(CXmlUiController& aUiCtl)
    {
    CApplication* self = NULL;
    
    __TIME("XML UI: Create Application",
    	self = new CApplication(KUidXmlUiApp, aUiCtl);
    )
    __HEAP("XML UI: Create Application");

    return self;
    }

CApplication::~CApplication()
    {
    }

CApaDocument* CApplication::CreateDocumentL()
    {
    return CDocument::NewL(*this, iUiCtl);
    }

TUid CApplication::AppDllUid() const
    {
    return KUidXmlUiApp;
    }

TFileName CApplication::ResourceFileName() const
    {
    
    TFileName resFile;
    resFile.Append( KAiXmlUiResourceDrive );
    resFile.Append( KDC_APP_RESOURCE_DIR );
    resFile.Append( KAiXmlUiResourceFile );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
    
    return resFile;
    }

} // namespace AiXmlUiController
