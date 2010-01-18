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
* WidgetManager configuration class
*
*/

// INCLUDE FILES
#include <centralrepository.h>
#include <StringLoader.h>
#include <aknsconstants.h>

#include <widgetmanagerview.rsg>
#include <widgetmanager.mbg>
#include "wmconfiguration.h"
#include "wmresourceloader.h"
#include "wmcrkeys.h"

// CONSTANTS
const TInt KMaxIconDescriptorLength = 256;

// ---------------------------------------------------------
// CWmConfiguration::NewL
// ---------------------------------------------------------
//
CWmConfiguration* CWmConfiguration::NewL(
        CWmResourceLoader& aResourceLoader )
    {
    CWmConfiguration* self = new (ELeave) CWmConfiguration( aResourceLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmConfiguration::CWmConfiguration()
// ---------------------------------------------------------
//
CWmConfiguration::CWmConfiguration(
        CWmResourceLoader& aResourceLoader )
    : iResourceLoader( aResourceLoader )
	{
    iOviStoreText = NULL;
    iOviStoreIcon = NULL;
	iRepository = NULL;
	iOviStoreBundleId = NULL;
	iOviStoreClientParam = NULL;
    iOviStoreUrl = NULL;
	}

// ---------------------------------------------------------
// CWmConfiguration::~CWmConfiguration()
// ---------------------------------------------------------
//
CWmConfiguration::~CWmConfiguration()
	{
    delete iOviStoreText;
    delete iOviStoreIcon;
	delete iRepository;
    delete iOviStoreBundleId;
    delete iOviStoreClientParam;
    delete iOviStoreUrl;
	}

// ---------------------------------------------------------
// CWmConfiguration::ConstructL
// ---------------------------------------------------------
//
void CWmConfiguration::ConstructL()
	{
	// localised ovistore button text
    iOviStoreText = StringLoader::LoadL( R_QTN_WM_GO_TO_OVI_STORE );

    // ovistore icon descriptor. It will look something like this:
    // skin( 0x101f86e3 0x23f6 ):mif( z:\resource\apps\widgetmanager.mif 16388 16389 )
    TBuf<KMaxIconDescriptorLength> buf;
    _LIT( KSkinMifIconFormat, "skin( 0x%x 0x%x ):mif( %S %d %d )");
    buf.Format( KSkinMifIconFormat(),
             EAknsMajorGeneric, EAknsMinorGenericQgnMenuOviStore,
             &iResourceLoader.IconFilePath(),
             EMbmWidgetmanagerQgn_menu_ovistore,
             EMbmWidgetmanagerQgn_menu_ovistore_mask );
    iOviStoreIcon = buf.AllocL();

    // read data from repository
    TRAP_IGNORE(
        iRepository = CRepository::NewL( 
                TUid::Uid( KCrWidgetManagerm ) );
    
        iLanguageIndex = FindCorrectLanguageId();
        iOviStoreBundleId = ReadParameterL( KOviStoreBunbleId );
        iOviStoreClientParam = ReadParameterL( KOviStoreClientParam );
        iOviStoreUrl = ReadLocalisedParameterL( KOviStoreBrowserUrlOffset );
        );
	}

// ---------------------------------------------------------
// CWmConfiguration::FindCorrectLanguageId
// ---------------------------------------------------------
//
TInt CWmConfiguration::FindCorrectLanguageId()
    {
    TInt languageIndex = KErrNotFound; // the correct language
    TInt englishIndex = KErrNotFound; // english
    TInt anyIndex = KErrNotFound; // backup - any existing
    TLanguage sysLang = User::Language();
    
    //read language id's from cenrep, find a match
    for( TUint32 i=KLangId0; i<=KLangId9 && languageIndex<0; i+=KLangGroupSize )
        {
        TInt crLang = 0;
        if ( iRepository->Get( i, crLang ) == KErrNone )
            {
            if ( crLang == sysLang && languageIndex < 0 )
                { languageIndex = i; }
            if ( crLang == ELangEnglish && englishIndex < 0 )
                { englishIndex = i; }
            if ( crLang > 0 && anyIndex < 0 )
                { anyIndex = i; }
            }
        }
    
    // if correct language was not found, use english
    if ( languageIndex < 0 ) languageIndex = englishIndex;
    // if english was not found, use any configured language
    if ( languageIndex < 0 ) languageIndex = anyIndex;
    // if there are no languages configured, we're in trouble...
    if ( languageIndex < 0 ) languageIndex = 0;
    
    return languageIndex;
    }

// ---------------------------------------------------------
// CWmConfiguration::ReadParameterL
// ---------------------------------------------------------
//
HBufC* CWmConfiguration::ReadParameterL( TInt aKey )
    {
    TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> buf;
    TInt err = iRepository->Get( aKey, buf );

    HBufC* heapBuffer = NULL;
    if ( err == KErrNone )
        {
        heapBuffer = HBufC::NewL( buf.Length() );
        heapBuffer->Des().Copy( buf );
        }
    return heapBuffer;
    }

// ---------------------------------------------------------
// CWmConfiguration::ReadLocalisedParameterL
// ---------------------------------------------------------
//
HBufC* CWmConfiguration::ReadLocalisedParameterL(
        TInt aOffset )
    {
    TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> buf;
    TInt err = KErrNone;
    
    err = iRepository->Get( iLanguageIndex + aOffset, buf );
    if ( err != KErrNone || buf.Length() == 0 )
        {
        // This language is empty. Try default language (index 0)
        err = iRepository->Get( KLangId0 + aOffset, buf );
        }

    // construct string in heap
    HBufC* heapBuffer = NULL;
    if ( err == KErrNone )
        {
        heapBuffer = buf.AllocL();
        }
    return heapBuffer;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonCount
// ---------------------------------------------------------
//
TInt CWmConfiguration::PortalButtonCount()
	{
	return 1;
	}

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonText
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonText( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreText ) return *iOviStoreText;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonIcon
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonIcon( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreIcon ) return *iOviStoreIcon;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonBundleId
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonBundleId( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreBundleId ) return *iOviStoreBundleId;
    return KNullDesC;
    }


// ---------------------------------------------------------
// CWmConfiguration::PortalButtonPrimaryMethod
// ---------------------------------------------------------
//
CWmConfiguration::TMethod
    CWmConfiguration::PortalButtonPrimaryMethod( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreClientParam ) return EWidget;
    return ENone;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonPrimaryParams
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonPrimaryParams( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreClientParam ) return *iOviStoreClientParam;
    return KNullDesC;
    }


// ---------------------------------------------------------
// CWmConfiguration::PortalButtonSecondaryMethod
// ---------------------------------------------------------
//
CWmConfiguration::TMethod
    CWmConfiguration::PortalButtonSecondaryMethod( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreUrl ) return EHttp;
    return ENone;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonSecondaryParams
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonSecondaryParams( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreUrl ) return *iOviStoreUrl;
    return KNullDesC;
    }




// End of File

