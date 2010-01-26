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
* loads widget manager resources and icons
*
*/

// INCLUDE FILES
#include <eikenv.h>
#include <eikappui.h>
#include <aknapp.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <widgetmanagerview.rsg>

#include "wmresourceloader.h"

// CONSTANTS
_LIT( KZdrive, "Z:" );
_LIT( KRelativeMifPath, "\\resource\\apps\\widgetmanager.mif" );
_LIT( KRelativeResourcePathWithWildcard, "\\resource\\apps\\widgetmanagerview.r*" );
_LIT( KRelativeResourcePath, "\\resource\\apps\\widgetmanagerview.rsc" );

// ---------------------------------------------------------
// CWmResourceLoader::NewL
// ---------------------------------------------------------
//
CWmResourceLoader* CWmResourceLoader::NewL(
        CEikonEnv& aEnv )
    {
    CWmResourceLoader* self = new (ELeave) CWmResourceLoader( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmResourceLoader::CWmResourceLoader
// ---------------------------------------------------------
//
CWmResourceLoader::CWmResourceLoader( CEikonEnv& aEnv )
    : iEnv( aEnv )
    {
    }

// ---------------------------------------------------------
// CWmResourceLoader::~CWmResourceLoader
// ---------------------------------------------------------
//
CWmResourceLoader::~CWmResourceLoader()
    {
    UnloadResources();
    delete iNote;
    delete iNoDescription;
    delete iWrtDescription;
    }

// ---------------------------------------------------------
// CWmResourceLoader::ConstructL
// ---------------------------------------------------------
//
void CWmResourceLoader::ConstructL()
    {
    Dll::FileName( iDllName );

    LoadResourcesL();
    DetermineIconFilePath();
    
    iNoDescription = StringLoader::LoadL( 
            R_QTN_WM_DETAILS_NO_DESCRIPTION, &iEnv  );
    
    iWrtDescription = StringLoader::LoadL( 
            R_QTN_WM_WIDGET_DETAILS_WRT, &iEnv  );
    }

// ---------------------------------------------------------
// CWmResourceLoader::LoadResourcesL
// ---------------------------------------------------------
//
void CWmResourceLoader::LoadResourcesL()
    {    
    TFileName resourceFile;
    RFs& fs = iEnv.FsSession();
    TPtrC dllDrive = iDllName.Left(2); 
    resourceFile.Copy( dllDrive );
    resourceFile.Append( KRelativeResourcePathWithWildcard );
    BaflUtils::NearestLanguageFile( fs, resourceFile );
    if ( !BaflUtils::FileExists( fs, resourceFile ) )
        {
        resourceFile.Copy( dllDrive );
        resourceFile.Append( KRelativeResourcePath );
        BaflUtils::NearestLanguageFile( fs, resourceFile );
        if ( !BaflUtils::FileExists( fs, resourceFile ) &&
            dllDrive != KZdrive() )
            {
            // try to load resource from ROM ( partial upgrade case )
            resourceFile.Copy( KZdrive );
            resourceFile.Append( KRelativeResourcePathWithWildcard );
            BaflUtils::NearestLanguageFile( fs, resourceFile );
            if ( !BaflUtils::FileExists( fs, resourceFile ) )
                {
                resourceFile.Copy( KZdrive );
                resourceFile.Append( KRelativeResourcePath );
                BaflUtils::NearestLanguageFile( fs, resourceFile );
                }
            }
        }
    iResourceFileOffset = iEnv.AddResourceFileL( resourceFile );
    }

// ---------------------------------------------------------
// CWmResourceLoader::UnloadResources
// ---------------------------------------------------------
//
void CWmResourceLoader::UnloadResources()
    {
    if ( iResourceFileOffset )
        {
        iEnv.DeleteResourceFile( iResourceFileOffset );        
        }
    }

// ---------------------------------------------------------
// CWmResourceLoader::DetermineIconFilePath
// ---------------------------------------------------------
//
void CWmResourceLoader::DetermineIconFilePath()
    {
    RFs& fs = iEnv.FsSession();
    iMifFile.Copy( iDllName.Left(2) );
    iMifFile.Append( KRelativeMifPath );
    if ( !BaflUtils::FileExists( fs, iMifFile ) )
        {
        // Load from ROM ( partial upgrade case )
        iMifFile.Copy( KZdrive );
        iMifFile.Append( KRelativeMifPath );        
        }
    }

// ---------------------------------------------------------
// CWmResourceLoader::IconFilePath
// ---------------------------------------------------------
//
const TDesC& CWmResourceLoader::IconFilePath()
    {
    return iMifFile;
    }

// ---------------------------------------------------------
// LoadStringLC
// loads a string from resource. If an additional string is
// given (the length is greater than zero) uses a different
// StringLoader method to load.
// ---------------------------------------------------------
//
HBufC* LoadStringLC( TInt aResourceId, const TDesC& aString, CEikonEnv* aEnv )
    {
    if ( aString.Length() > 0 )
        return StringLoader::LoadLC( aResourceId, aString, aEnv );
    else
        return StringLoader::LoadLC( aResourceId, aEnv );
    }

// ---------------------------------------------------------
// CWmResourceLoader::InfoPopupL
// ---------------------------------------------------------
//
void CWmResourceLoader::InfoPopupL( TInt aResourceId, const TDesC& aString )
    {
    HBufC* infoMsg = LoadStringLC( aResourceId, aString, &iEnv );
    iNote = new (ELeave) CAknInformationNote( &iNote );
    iNote->SetTimeout( CAknNoteDialog::ELongTimeout );
    iNote->ExecuteLD( *infoMsg );
    CleanupStack::PopAndDestroy( infoMsg );
    }

// ---------------------------------------------------------
// CWmResourceLoader::ErrorPopup
// ---------------------------------------------------------
//
void CWmResourceLoader::ErrorPopup( TInt aErrorCode )
    {
    iEnv.HandleError( aErrorCode );
    }

// ---------------------------------------------------------
// CWmResourceLoader::NoDescription
// ---------------------------------------------------------
//
const TDesC& CWmResourceLoader::NoDescription()
    {
    return *iNoDescription;
    }

// ---------------------------------------------------------
// CWmResourceLoader::WrtDescription
// ---------------------------------------------------------
//
const TDesC& CWmResourceLoader::WrtDescription()
    {
    return *iWrtDescription;
    }

// end of file

