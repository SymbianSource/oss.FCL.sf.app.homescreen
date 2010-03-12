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
#include <AknsConstants.h>

#include <widgetmanagerview.rsg>
#include <widgetmanager.mbg>
#include "wmconfiguration.h"
#include "wmresourceloader.h"
#include "wmcrkeys.h"

// some constants regarging the central repository localised section.
// these will ensure CR localisation section compatibility even if
// new strings are added
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
    iOviStoreUid = KNullUid;
    iOviStoreClientParam = NULL;
    iOviButtonTitle = NULL;
    iOviButtonIcon = NULL;
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
    delete iOviStoreClientParam;
    delete iOviButtonTitle;
    delete iOviButtonIcon;
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
    ReadIntParameter( KOviStoreUid, iOviStoreUid.iUid );
    iOviStoreClientParam = ReadDescParameterL( KOviStoreClientParam ); 
    // determine language and read localised parameters
    iLanguageIndex = FindCorrectLanguageId();
    iOperatorButtonTitle = ReadLocalisedParameterL( KLangOffsetOperatorText );
    iOperatorButtonUrl = ReadLocalisedParameterL( KLangOffsetOperatorUrl );

    if ( iOperatorButtonUrl && iOperatorButtonUrl->Length() > 0 )
        {
        // construct the operator button icon.
        iOperatorButtonIcon = ReadDescParameterL( KOperatorButtonIcon );
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
// CWmConfiguration::ReadDescParameterL
// ---------------------------------------------------------
//
HBufC* CWmConfiguration::ReadDescParameterL( TInt aKey )
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
// CWmConfiguration::ReadIntParameterIntL
// ---------------------------------------------------------
//
void CWmConfiguration::ReadIntParameter( TInt aKey, TInt32& aValue )
    {
    TInt read = KErrNone;
    TInt err = iRepository->Get( aKey, read );
    if( err == KErrNone )
        {
        aValue = read;
        }
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
// CWmConfiguration::PortalButtonBrowserUrl
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonBrowserUrl( TInt aIndex )
    {
    if ( aIndex == 1 && iOperatorButtonUrl )
        return *iOperatorButtonUrl;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonClientUid
// ---------------------------------------------------------
//
TUid CWmConfiguration::PortalButtonClientUid( TInt aIndex )
    {
    if ( aIndex == 0 )
        {
        return iOviStoreUid;
        } 
    // Operator client uid not supported. 
    // If operator client is a widget we can use widget registry 
    // to fetch uid of operator widget save it to member and return that here
    
    return KNullUid;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonClientParam
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonClientParam( TInt aIndex )
    {
    if ( aIndex == 0 && iOviStoreClientParam )
        {
        return *iOviStoreClientParam;
        }
    // Operator client param not supported. Can be added if needed.
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

