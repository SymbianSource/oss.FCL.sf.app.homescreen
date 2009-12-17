/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmIconFileProvider implementation.
*
*/

#include "wmcommon.h"
#include "wmiconfileprovider.h"

// ---------------------------------------------------------------------------
// CWmIconFileProvider::CWmIconFileProvider
// ---------------------------------------------------------------------------
//
CWmIconFileProvider::CWmIconFileProvider( RFs& aFs )
    :iFs( aFs )
    {
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::~CWmIconFileProvider
// ---------------------------------------------------------------------------
//
CWmIconFileProvider::~CWmIconFileProvider()
    {
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::SetIconFile
// ---------------------------------------------------------------------------
//
void CWmIconFileProvider::SetIconFile( const TDesC& aFileName )
    { 
    iFileName.Copy( aFileName ); 
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::NewL
// ---------------------------------------------------------------------------
//
CWmIconFileProvider* CWmIconFileProvider::NewL( RFs& aFs )
    {
    CWmIconFileProvider* self = new (ELeave) CWmIconFileProvider( aFs );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::ConstructL
// ---------------------------------------------------------------------------
//
void CWmIconFileProvider::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::RetrieveIconFileHandleL
// ---------------------------------------------------------------------------
//
void CWmIconFileProvider::RetrieveIconFileHandleL( 
    RFile& aFile, 
    const TIconFileType /*aType*/ )
    {
	User::LeaveIfError( aFile.Open( iFs, iFileName, EFileShareAny ) );
    }

// ---------------------------------------------------------------------------
// CWmIconFileProvider::Finished
// ---------------------------------------------------------------------------
//
void CWmIconFileProvider::Finished()
    {
    }

// end of file

