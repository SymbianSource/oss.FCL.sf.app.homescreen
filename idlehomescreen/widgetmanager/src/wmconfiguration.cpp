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
#include <widgetregistryclient.h> // widget reqistry

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

_LIT( KOpAppTypeS60, "s60" );
_LIT( KOpAppTypeCwrt, "cwrt" );
_LIT( KOpAppTypeWrt, "wrt" );
_LIT( KOpAppTypeJava, "java" );
_LIT( KOpAppTypeQt, "qt" );
_LIT( KSeparator, ":" );
_LIT( KOperatorIcon, "uid(%d)");
_LIT( KHexPrefix, "0x" );

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

    // ovi
    iOviStoreUid = KNullUid;

    // operator
    iOperatorAppIdUid = KNullUid;
    iOperatorAppType = EUnknown;
    iOperatorButtonEnabled = EFalse;
    iButtonsMirrored = EFalse;
	}

// ---------------------------------------------------------
// CWmConfiguration::~CWmConfiguration()
// ---------------------------------------------------------
//
CWmConfiguration::~CWmConfiguration()
	{
    delete iRepository;
    // ovi
    delete iOviStoreClientParam;
    delete iOviButtonTitle;
    delete iOviButtonIcon;
    // operator
    delete iOperatorButtonTitle;
    delete iOperatorButtonIcon;
    delete iOperatorButtonUrl;
    delete iOperatorParam;
    delete iOperatorAppIdStr;
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

    // read Ovi parameters
    ReadInt32Parameter( KOviStoreUid, iOviStoreUid.iUid );
    iOviStoreClientParam = ReadDescParameterL( KOviStoreClientParam ); 
    
    // read operator parameters if operator button enabled
    ReadIntParameter( KOperatorButtonEnabled, iOperatorButtonEnabled );
    if ( iOperatorButtonEnabled )
        {
        // determine language and read localised parameters
        iLanguageIndex = FindCorrectLanguageId();
        iOperatorButtonTitle = ReadLocalisedParameterL( KLangOffsetOperatorText );
        iOperatorButtonUrl = ReadLocalisedParameterL( KLangOffsetOperatorUrl );
        // read non-localised parameters
        iOperatorButtonIcon = ReadDescParameterL( KOperatorButtonIcon );
        ReadOperatorApplicationInfoL();
        iOperatorParam = ReadDescParameterL( KOperatorParam );
        ReadIntParameter( KOperatorButtonHigherPriority, iButtonsMirrored );
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
// CWmConfiguration::ReadInt32ParameterIntL
// ---------------------------------------------------------
//
void CWmConfiguration::ReadInt32Parameter( TInt aKey, TInt32& aValue )
    {
    TInt value = 0;
	if ( iRepository->Get( aKey, value ) == KErrNone )
	    {
        aValue = value;
	    }
    }

// ---------------------------------------------------------
// CWmConfiguration::ReadIntParameterIntL
// ---------------------------------------------------------
//
void CWmConfiguration::ReadIntParameter( TInt aKey, TInt& aValue )
    {
    aValue = 0;
	iRepository->Get( aKey, aValue );
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
// CWmConfiguration::ReadOperatorApplicationInfoL
// ---------------------------------------------------------
//
void CWmConfiguration::ReadOperatorApplicationInfoL()
    {
    HBufC* applicationInfo = ReadDescParameterL( KOperatorAppTypeAndId );
    if ( applicationInfo && applicationInfo->Des().Length() > 0 )
        {
        CleanupStack::PushL( applicationInfo );
        applicationInfo->Des().TrimAll();

        // parse
        TInt separator = applicationInfo->Des().Find( KSeparator );
        if ( separator != KErrNotFound )
            {
            TPtrC type = applicationInfo->Des().Left( separator );
            TPtrC appId = applicationInfo->Des().Mid(
                    separator + KSeparator().Length() );
            
            if ( !type.Compare( KOpAppTypeS60 ) )
                {
                iOperatorAppType = ES60;
                iOperatorAppIdUid = StringToUid( appId );
                SetOperatorIcon( iOperatorAppIdUid );
                }
            else if ( !type.Compare( KOpAppTypeCwrt ) )
                {
                iOperatorAppType = ECwrt;
                iOperatorAppIdUid = FetchWidgetUidFromRegistryL( appId );
                SetOperatorIcon( iOperatorAppIdUid );
                }
            else if ( !type.Compare( KOpAppTypeWrt ) )
                {
                iOperatorAppType = EWrt;
                iOperatorAppIdUid = FetchWidgetUidFromRegistryL( appId );
                SetOperatorIcon( iOperatorAppIdUid );
                }
            else if ( !type.Compare( KOpAppTypeJava ) )
                {
                //TODO: java support is not fully implemented
                User::Leave( KErrGeneral );
                iOperatorAppType = EJava;
                iOperatorAppIdStr = appId.AllocL();
                }
            else if ( !type.Compare( KOpAppTypeQt ) )
                {
                iOperatorAppType = EQt;
                iOperatorAppIdUid = StringToUid( appId );
                SetOperatorIcon( iOperatorAppIdUid );
                }
            }
        CleanupStack::PopAndDestroy( applicationInfo );
        }
    }

// ---------------------------------------------------------
// CWmConfiguration::SetOperatorIcon
// ---------------------------------------------------------
//
void CWmConfiguration::SetOperatorIcon( TUid aUid )
    {
    if ( iOperatorButtonIcon &&
        !iOperatorButtonIcon->Des().Length() )
        {
        delete iOperatorButtonIcon;
        iOperatorButtonIcon = NULL;
        TRAP_IGNORE( iOperatorButtonIcon = HBufC::NewL(
                KMaxUidName + KOperatorIcon().Length() ) );
        iOperatorButtonIcon->Des().Format(
                KOperatorIcon(), aUid );
        }
    }

// ---------------------------------------------------------
// CWmConfiguration::FetchWidgetUidFromRegistryL
// ---------------------------------------------------------
//
TUid CWmConfiguration::FetchWidgetUidFromRegistryL(
    const TDesC& aBundleId )
    {
    RWidgetRegistryClientSession clientSession;
    User::LeaveIfError( clientSession.Connect() );
    CleanupClosePushL( clientSession );
    TInt ret = clientSession.GetWidgetUidL( aBundleId );
    clientSession.Disconnect();
    CleanupStack::PopAndDestroy( &clientSession );
    return TUid::Uid( ret );
    }
     
// ---------------------------------------------------------
// CWmConfiguration::PortalButtonCount
// ---------------------------------------------------------
//
TInt CWmConfiguration::PortalButtonCount()
    {
    return ( iOperatorButtonEnabled ? 2 : 1 );
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonText
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonText( TInt aIndex )
    {
    if ( aIndex == 0 && iOviButtonTitle )
        return *iOviButtonTitle;
    if ( aIndex == 1 && iOperatorButtonTitle && iOperatorButtonEnabled )
        return *iOperatorButtonTitle;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonIcon
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonIcon( TInt aIndex )
    {
    if ( aIndex == 0 && iOviButtonIcon )
        return *iOviButtonIcon;
    if ( aIndex == 1 && iOperatorButtonIcon && iOperatorButtonEnabled )
        return *iOperatorButtonIcon;
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonBrowserUrl
// ---------------------------------------------------------
//
const TDesC& CWmConfiguration::PortalButtonBrowserUrl( TInt aIndex )
    {
    // No support for ovi here
    if ( aIndex == 1 && iOperatorButtonUrl && iOperatorButtonEnabled )
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
    // operator not supported. Get operator data using 
    // PortalButtonApplicationInfoL
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
    if ( aIndex == 1 && iOperatorParam && iOperatorButtonEnabled )
        {
        return *iOperatorParam;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonsMirrored
// ---------------------------------------------------------
//
TBool CWmConfiguration::PortalButtonsMirrored()
    {
    return iButtonsMirrored;
    } 
   
// ---------------------------------------------------------
// CWmConfiguration::PortalButtonApplicationType
// ---------------------------------------------------------
//
CWmConfiguration::TOpAppType CWmConfiguration::PortalButtonApplicationType( 
        TInt aIndex )
    {
    if ( aIndex == 1 )
        {
        return iOperatorAppType;
        }
    // no support for ovi
    return EUnknown;
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonApplicationId
// ---------------------------------------------------------
//
void CWmConfiguration::PortalButtonApplicationId( 
        TInt aIndex, TDes& aOperatorAppId )
    {
    if ( aIndex == 1 )
        {
        aOperatorAppId.Copy( *iOperatorAppIdStr );
        }
    // no support for ovi
    }

// ---------------------------------------------------------
// CWmConfiguration::PortalButtonApplicationId
// ---------------------------------------------------------
//
void CWmConfiguration::PortalButtonApplicationId( 
        TInt aIndex, TUid& aOperatorAppId )
    {
    if ( aIndex == 1 )
        {
        aOperatorAppId = iOperatorAppIdUid; 
        }
    // no support for ovi
    }

// ---------------------------------------------------------
// CWmConfiguration::StringToUid
// ---------------------------------------------------------
//
TUid CWmConfiguration::StringToUid( const TDesC& aStr )
    {
    if ( aStr.Length() > 0 && aStr.Find( KHexPrefix ) == 0 )
        {
        // hex value 0x12345678
        TLex lex( aStr.Mid( KHexPrefix().Length() ) );
        TUint id = 0;
        if ( lex.Val( id, EHex ) == KErrNone )
            {
            return TUid::Uid( (TInt)id );
            }
        }
    else if (  aStr.Length() > 0 )
        {
        TLex lex( aStr );
        TUint id( 0 );
        if (  KErrNone == lex.Val( id ) )
            {
            return TUid::Uid( (TInt)id );
            }
        }
    return KNullUid;
    }


// End of File

