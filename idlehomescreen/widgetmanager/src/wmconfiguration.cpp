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
#include "wmstore.h"

// some constants regarging the central repository localised section.
// these will ensure CR localisation section compatibility even if
// new strings are added
const TInt KLangOffsetStoreUrl = KUrl_0 - KLangId_0;
const TInt KLangOffsetStoreText = KTitle_0 - KLangId_0;
const TInt KLangGroupSize = KLangId_1 - KLangId_0;
const TUint32 KLastLangId = KLangId_9;

_LIT( KOpAppTypeS60, "s60" );
_LIT( KOpAppTypeCwrt, "cwrt" );
_LIT( KOpAppTypeWrt, "wrt" );
_LIT( KOpAppTypeJava, "java" );
_LIT( KOpAppTypeQt, "qt" );
_LIT( KSeparator, ":" );
_LIT( KStoreIconFormat, "uid(%d)");
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
    
	}

// ---------------------------------------------------------
// CWmConfiguration::~CWmConfiguration()
// ---------------------------------------------------------
//
CWmConfiguration::~CWmConfiguration()
	{
    delete iRepository;    
    iConfItems.ResetAndDestroy();
	}

// ---------------------------------------------------------
// CWmConfiguration::ConstructL
// ---------------------------------------------------------
//
void CWmConfiguration::ConstructL()
	{
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
            TUid::Uid( KCrWidgetManager ) );
    
    TLinearOrder<CWmConfItem> order 
        = CWmConfiguration::CompareStoreOrder;

    // read Ovi parameters
    TInt oviEnabled;
    ReadIntParameter( KOviStoreEnabled, oviEnabled );
    if ( oviEnabled )
        {
        CWmConfItem* ovi = CWmConfItem::NewLC();
        ovi->iId = CWmStore::EOvi;
        
        // localised ovistore text
        _LIT( KFormatStr, "%S\t%S");
        HBufC* title = StringLoader::LoadLC( R_QTN_WM_OVI_STORE_TITLE );
        HBufC* desc = StringLoader::LoadLC( R_QTN_WM_OVI_STORE_DESC );
        
        ovi->iTitle = HBufC::NewL( title->Des().Length() +
                                   desc->Des().Length() + 
                                   KFormatStr().Length() );

        ovi->iTitle->Des().Format( KFormatStr(), title, desc );
        
        CleanupStack::PopAndDestroy( desc );
        CleanupStack::PopAndDestroy( title );
        
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
        ovi->iIcon = buf.AllocL();
        
        ReadIntParameter( KOviOrder, ovi->iOrder );
        ReadInt32Parameter( KOviUid, ovi->iAppUid.iUid );
        ovi->iAppParam = ReadDescParameterL( KOviAppParam );

        // Item to Array
        iConfItems.InsertInOrderL( ovi, order );
        CleanupStack::Pop( ovi );
        }
    
    // read Store parameters if Store enabled
    TInt storeEnabled = 0;
    ReadIntParameter( KStoreEnabled, storeEnabled );
    if ( storeEnabled )
        {
        iLanguageIndex = FindCorrectLanguageId();
        
        CWmConfItem* op = CWmConfItem::NewLC();
        op->iId = CWmStore::EStore1;
        
        op->iTitle = ReadLocalisedParameterL( KLangOffsetStoreText );
        op->iUrl = ReadLocalisedParameterL( KLangOffsetStoreUrl );
        // read non-localised parameters
        op->iIcon = ReadDescParameterL( KIcon );
        ReadApplicationInfoL( KAppTypeAndId, *op );
        op->iAppParam = ReadDescParameterL( KAppParam );        
        ReadIntParameter( KOrder, op->iOrder );
        
        // Item to Array
        iConfItems.InsertInOrderL( op, order );
        CleanupStack::Pop( op );
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
    for( TUint32 i=KLangId_0; i<=KLastLangId && languageIndex<0; i+=KLangGroupSize )
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
        err = iRepository->Get( KLangId_0 + aOffset, buf );
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
// CWmConfiguration::ReadApplicationInfoL
// ---------------------------------------------------------
//
void CWmConfiguration::ReadApplicationInfoL( 
        TInt aKey, CWmConfItem& aConfItem )
    {
    HBufC* applicationInfo = ReadDescParameterL( aKey );
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
                aConfItem.iAppType = ES60;
                aConfItem.iAppUid = StringToUid( appId );
                SetStoreIconL( aConfItem );
                }
            else if ( !type.Compare( KOpAppTypeCwrt ) )
                {
                aConfItem.iAppType = ECwrt;
                aConfItem.iAppUid = FetchWidgetUidFromRegistryL( appId );
                SetStoreIconL( aConfItem );
                }
            else if ( !type.Compare( KOpAppTypeWrt ) )
                {
                aConfItem.iAppType = EWrt;
                aConfItem.iAppUid = FetchWidgetUidFromRegistryL( appId );
                SetStoreIconL( aConfItem );
                }
            else if ( !type.Compare( KOpAppTypeJava ) )
                {
                // java support is not fully implemented
                User::Leave( KErrGeneral );
                }
            else if ( !type.Compare( KOpAppTypeQt ) )
                {
                aConfItem.iAppType = EQt;
                aConfItem.iAppUid = StringToUid( appId );
                SetStoreIconL( aConfItem );
                }
            }
        CleanupStack::PopAndDestroy( applicationInfo );
        }
    }

// ---------------------------------------------------------
// CWmConfiguration::SetStoreIcon
// ---------------------------------------------------------
//
void CWmConfiguration::SetStoreIconL( CWmConfItem& aConfItem )
    {
    if ( aConfItem.iIcon &&
        !aConfItem.iIcon->Des().Length() )
        {
        delete aConfItem.iIcon;
        aConfItem.iIcon = NULL;
        
        aConfItem.iIcon = HBufC::NewL(
                KMaxUidName + KStoreIconFormat().Length() );
        aConfItem.iIcon->Des().Format(
                KStoreIconFormat(), aConfItem.iAppUid );
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

// ---------------------------------------------------------
// CWmConfiguration::CompareStoreOrder
// ---------------------------------------------------------
//
TInt CWmConfiguration::CompareStoreOrder( 
        const CWmConfItem& aItemOne, const CWmConfItem& aItemTwo )
    {
    // 1. zero, if the two objects are equal.
    // 2. a negative value, if the first object is less than the second.
    // 3. a positive value, if the first object is greater than the second.
    TInt ret = 0;
    if ( aItemOne.iOrder < aItemTwo.iOrder )
        {
        ret = -1;
        }
    if ( aItemOne.iOrder > aItemTwo.iOrder )
        {
        ret = 1;
        }
    return ret;
    }
	
// ---------------------------------------------------------
// CWmConfiguration::StoreConfArray
// ---------------------------------------------------------
//
const RPointerArray<CWmConfItem>& CWmConfiguration::StoreConfArray()
    {
    return iConfItems;
    }

// ---------------------------------------------------------
// CWmConfiguration::StoreCount
// ---------------------------------------------------------
//
TInt CWmConfiguration::StoreCount()
    {
    return iConfItems.Count();
    }

// ---------------------------------------------------------
// CWmConfItem::NewL
// ---------------------------------------------------------
//
CWmConfItem* CWmConfItem::NewL()
    {
    CWmConfItem* self = CWmConfItem::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CWmConfItem::NewLC
// ---------------------------------------------------------
//
CWmConfItem* CWmConfItem::NewLC()
    {
    CWmConfItem* self = new ( ELeave ) CWmConfItem();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CWmConfItem::~CWmConfItem
// ---------------------------------------------------------
//
CWmConfItem::~CWmConfItem()
    {
    delete iTitle;
    delete iIcon;
    delete iUrl;              
    delete iAppParam;
    }

// ---------------------------------------------------------
// CWmConfItem::CWmConfItem
// ---------------------------------------------------------
//
CWmConfItem::CWmConfItem()
    {
    }

// ---------------------------------------------------------
// CWmConfItem::ConstructL
// ---------------------------------------------------------
//
void CWmConfItem::ConstructL()
    {
    }

// ---------------------------------------------------------
// CWmConfItem::Id
// ---------------------------------------------------------
//
CWmStore::TStoreId CWmConfItem::Id()
    {
    return iId;
    }

// ---------------------------------------------------------
// CWmConfItem::Title
// ---------------------------------------------------------
//
const TDesC& CWmConfItem::Title()
    {
    return ( iTitle ? *iTitle : KNullDesC() );
    }

// ---------------------------------------------------------
// CWmConfItem::Icon
// ---------------------------------------------------------
//
const TDesC& CWmConfItem::Icon()
    {
    return ( iIcon ? *iIcon : KNullDesC() );
    }

// ---------------------------------------------------------
// CWmConfItem::Order
// ---------------------------------------------------------
//
TInt CWmConfItem::Order()
    {
    return iOrder;
    }

// ---------------------------------------------------------
// CWmConfItem::BrowserUrl
// ---------------------------------------------------------
//
const TDesC& CWmConfItem::BrowserUrl() 
    {
    return ( iUrl ? *iUrl : KNullDesC() );
    }

// ---------------------------------------------------------
// CWmConfItem::AppType
// ---------------------------------------------------------
//
CWmConfiguration::TStoreAppType CWmConfItem::AppType()
    {
    return iAppType;
    }

// ---------------------------------------------------------
// CWmConfItem::AppUid
// ---------------------------------------------------------
//
TUid CWmConfItem::AppUid()
    {
    return iAppUid;
    }

// ---------------------------------------------------------
// CWmConfItem::AppParam
// ---------------------------------------------------------
//
const TDesC& CWmConfItem::AppParam()
    {
    return ( iAppParam ? *iAppParam : KNullDesC() );
    }

// End of File

