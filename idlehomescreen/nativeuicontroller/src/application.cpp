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


#include <bautils.h>
#include <aisystemuids.hrh>
#include "application.h"
#include "document.h"
#include <data_caging_path_literals.hrh>

using namespace AiNativeUiController;

_LIT( KResourceDrive, "z:" );
_LIT( KAiNativeUiResourceFile, "AiNativeUi.rsc" );

CApplication::CApplication(CNativeUiController* aUiCtl)
    : iUiCtl(aUiCtl)
    {
    }

CApplication* CApplication::New(CNativeUiController* aUiCtl)
    {
    CApplication* self = NULL;
    
  	self = new CApplication(aUiCtl);

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
    return  TUid::Uid(AI_UID3_AIFW_EXE);
    }

TFileName CApplication::ResourceFileName() const
    {
    TFileName resFile;
    resFile.Append( KResourceDrive );
    resFile.Append( KDC_APP_RESOURCE_DIR );
    resFile.Append( KAiNativeUiResourceFile );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
    
    return resFile;
    }

// End of File.
