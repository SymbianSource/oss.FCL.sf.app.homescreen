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

// some constants regarging the central repository localised section.
// these will ensure CR localisation section compatibility even if
// new strings are added
const TInt KLangOffsetOviStoreUrl = KOviStoreBrowserUrl0 - KLangId0;
const TInt KLangOffsetOperatorUrl = KOperatorButtonUrl0 - KLangId0;
const TInt KLangOffsetOperatorText = KOperatorButtonText0 - KLangId0;
const TInt KLangGroupSize = KLangId1 - KLangId0;
const TUint32 KLastLangId = KLangId9;


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
    iRepository = NULL;
    iOviStoreBundleId = NULL;
    iOviStoreClientParam = NULL;
    iOviButtonTitle = NULL;
    iOviButtonIcon = NULL;
    iOviButtonUrl = NULL;
    iOperatorButtonTitle = NULL;
    iOperatorButtonIcon = NULL;
    iOperatorButtonUrl = NULL;
	}

// ---------------------------------------------------------
// CWmConfiguration::~CWmConfiguration()
// ---------------------------------------------------------
//
CWmConfiguration::~CWmConfiguration()
	{
    delete iRepository;
    delete iOviStoreBundleId;
    delete iOviStoreClientParam;
    delete iOviButtonTitle;
    delete iOviButtonIcon;
    delete iOviButtonUrl;
    delete iOperatorButtonTitle;
    delete iOperatorButtonIcon;
    delete iOperatorButtonUrl;
	}

// ---------------------------------------------------------
// CWmConfiguration::ConstructL
// ---------------------------------------------------------
//
void CWmConfiguration::ConstructL()
	{
	// localised ovistore button text
    iOviButtonTitle = StringLoader::LoadL( R_QTN_WM_GO_TO_OVI_STORE );

    // ovistore icon descriptor. It will look something like this:
    // skin( 0x101f86e3 0x23f6 ):mif( z:\resource\apps\widgetmanager.mif 16388 16389 )
    _LIT( KSkinMifIconFormat, "skin( 0x%x 0x%x ):mif( %S %d %d )");
    const TInt KMaxIconDescriptorLength = 256;
    TBuf<KMaxIconDescriptorLength> buf;
    buf.Format( KSkinMifIconFormat(),
             EAknsMajorGeneric, EAknsMinorGenericQgnMenuOviStore,
             &iResourceLoader.IconFilePath(),
             EMbmWidgetmanagerQgn_menu_ovistore,
             EMbmWidgetmanagerQgn_menu_ovistore_mask );
    iOviButtonIcon = buf.AllocL();

    // read data from repository
    TRAP_IGNORE( LoadConfigurationL(); );

	}

// ---------------------------------------------------------
// CWmConfiguration::LoadConfigurationL
// ---------------------------------------------------------
//
void CWmConfiguration::LoadConfigurationL()
    {
    iRepository = CRepository::NewL( 
            TUid::Uid( KCrWidgetManagerm ) );

    // read fixed parameters
    iOviStoreBundleId = ReadParameterL( KOviStoreBundleId );
    iOviStoreClientParam = ReadParameterL( KOviStoreClientParam );
    // determine language and read localised parameters
    iLanguageIndex = FindCorrectLanguageId();
    iOviButtonUrl = ReadLocalisedParameterL( KLangOffsetOviStoreUrl );
    iOperatorButtonTitle = ReadLocalisedParameterL( KLangOffsetOperatorText );
    iOperatorButtonUrl = ReadLocalisedParameterL( KLangOffsetOperatorUrl );

    if ( iOperatorButtonUrl && iOperatorButtonUrl->Length() > 0 )
        {
        // construct the operator button icon.
        iOperatorButtonIcon = ReadParameterL( KOperatorButtonIcon );
        }

    delete iRepository;
    iRepository = NULL;
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
    for( TUint32 i=KLangId0; i<=KLastLangId && languageIndex<0; i+=KLangGroupSize )
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
    if ( iOperatorButtonUrl && iOperatorButtonUrl->Length() > 0 )
        return 2;
    return 1;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonText
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonText( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviButtonTitle )
        return *iOviButtonTitle;
    if ( aIndex == 1 && iOperatorButtonTitle )
        return *iOperatorButtonTitle;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonIcon
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonIcon( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviButtonIcon )
        return *iOviButtonIcon;
    if ( aIndex == 1 && iOperatorButtonIcon )
        return *iOperatorButtonIcon;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonPrimaryMethod
// ---------------------------------------------------------
//
CWmConfiguration::TMethod
    CWmConfiguration::PortalButtonPrimaryMethod( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviStoreBundleId && iOviStoreBundleId->Length() > 0 )
        return EWidget;
    if ( aIndex == 1 && iOperatorButtonUrl && iOperatorButtonUrl->Length() > 0 )
        return EHttp;
    return ENone;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonPrimaryService
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonPrimaryService( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviStoreBundleId && iOviStoreBundleId->Length() > 0 )
        return *iOviStoreBundleId;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonPrimaryParams
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonPrimaryParams( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviStoreClientParam )
        return *iOviStoreClientParam;
    if ( aIndex == 1 && iOperatorButtonUrl )
        return *iOperatorButtonUrl;
    return KNullDesC;
    }


// ---------------------------------------------------------
// CWmConfiguration::PortalButtonSecondaryMethod
// ---------------------------------------------------------
//
CWmConfiguration::TMethod
    CWmConfiguration::PortalButtonSecondaryMethod( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviButtonUrl && iOviButtonUrl->Length() > 0 )
        return EHttp;
    return ENone;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonSecondaryService
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonSecondaryService( TInt aIndex )
    {
    IndexConversion( aIndex );
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonSecondaryParams
// ---------------------------------------------------------
//
const TDesC&
    CWmConfiguration::PortalButtonSecondaryParams( TInt aIndex )
    {
    IndexConversion( aIndex );
    if ( aIndex == 0 && iOviButtonUrl )
        return *iOviButtonUrl;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::IndexConversion
// ---------------------------------------------------------
//
void CWmConfiguration::IndexConversion( TInt& /*aIndex*/ )
    {
    // if there is a need to configure switching button places
    // (operator button left, then OVI button) then this method
    // would have something like this:
    
    // if ( iSwitchButtons ) aIndex = 1 - aIndex;
    
    // but for now, this method is empty.
    }




// End of File

