/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <itemmap.h>
#include <aistrcnv.h>

#include "carouselplugindata.h"
#include "carouselpluginengine.h"

using namespace HSPluginSettingsIf;

_LIT( KMenuAttrParam, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperNameLocked, "locked" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );
_LIT8( KProperValueMailbox, "mailbox" );
_LIT8( KMenuAttrUndefUid, "0x99999991" );

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );


#define KMCSCmailMtmUidValue 0x2001F406

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCarouselData::CCarouselData()
    :iDirty( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCarouselData::~CCarouselData()
    {
    delete iName;
    delete iValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselData::SetMenuItem( TMenuItem& aMenuItem )
    {
    iMenuItem = aMenuItem;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TMenuItem& CCarouselData::MenuItem()
    {
    return iMenuItem;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDesC& CCarouselData::Name()
    {
    return *iName;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselData::SetNameL( const TDesC& aName )
    {
	delete iName;
	iName = NULL;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDesC& CCarouselData::Value()
    {
    return *iValue;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselData::SetValueL( const TDesC& aValue )
    {
	delete iValue;
	iValue = NULL;
    iValue = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselData::SetDirty( TBool aDirty )
    {
    iDirty = aDirty;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCarouselData::IsDirty() const
    {
    return iDirty;
    }
    

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CCarouselPluginData* CCarouselPluginData::NewL( CCarouselPluginEngine& aEngine, const TDesC8& aInstanceUid )
    {
    CCarouselPluginData* self = new (ELeave) CCarouselPluginData( aEngine, aInstanceUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCarouselPluginData::CCarouselPluginData( CCarouselPluginEngine& aEngine, const TDesC8& aInstanceUid )
    : iEngine( aEngine ), iInstanceUid( aInstanceUid )
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::ConstructL()
    {
    iPluginSettings = CHomescreenSettings::Instance();
    if( iPluginSettings == NULL )
        {
        User::Leave( KErrUnknown );
        }
    iPluginSettings->AddObserverL( this );

    iMenu.OpenL( KMyMenuData );

    UpdateDataL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCarouselPluginData::~CCarouselPluginData()
    {
    if( iPluginSettings )
        {
        iPluginSettings->RemoveObserver( this );
        }
    iData.ResetAndDestroy();
    iMenu.Close();
    }

// ---------------------------------------------------------------------------
// Gets the instance specific settings from HSPS and creates data items
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::UpdateDataL()
    {
    RPointerArray<CItemMap> settings;
    CleanupResetAndDestroyPushL( settings );

    User::LeaveIfError( iPluginSettings->GetSettingsL( iInstanceUid, settings ) );
    if ( settings.Count() <= 0 )
        {
        User::Leave( KErrNotFound );
        }

    TBool wasEmpty = !iData.Count();
    
    for ( TInt i = 0; i < settings.Count(); i++ )
       {
        CItemMap* itemMap = settings[ i ];
        
        // get properties
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();
        
        CCarouselData* data = GetMenuDataL( properties );
        CleanupStack::PushL( data );
        if ( wasEmpty )
            {
            // list of shortcut slot was empty
            // we append the shortcut data slots one-by-one to the list            
            data->SetDirty( ETrue );
            iData.AppendL( data );
            CleanupStack::Pop( data );
            }
        else 
            {
            // check for updates in existing shortcut data slot
            // if menuitem id has changed, replace the item and 
            // set as dirty
            TInt id( iData[ i ]->MenuItem().Id() );
            
            // id of all bookmarks is zero so name has to be check
            // in case of bookmark has changed
            if ( id >= 0 && ( data->MenuItem().Id() != id ||
                    ( id == 0 && data->Name().CompareF(
                            iData[ i ]->Name() ) != 0 ) ) )
                {
                data->SetDirty( ETrue );
                CCarouselData* oldData = iData[i];
                iData.Remove( i );
                delete oldData;
                
                iData.InsertL( data, i );
                CleanupStack::Pop( data );
                }
            else
                {
                CleanupStack::PopAndDestroy( data );
                }
            }
        }
    
    // Leave if there isn't any data filled into array as there isn't
    // anything shortcut to publish on homescreen.
    if ( iData.Count() <= 0 ) 
        {
        User::Leave( KErrNotFound );
        }
    
    // Cleanup.
    CleanupStack::PopAndDestroy(); // settings
    }

// ---------------------------------------------------------------------------
// Removes obsolete data and saves 'Undefined' item to HSPS
// Used when active mailbox is deleted from system.
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::RemoveDataL( TInt aId )
    {
    TInt count = iData.Count();
    for( TInt i = 0; i < count; i++ )
        {
        CCarouselData* data = iData[i];
        if( data->MenuItem().Type() == KMenuTypeMailbox &&
            data->MenuItem().Id() == aId )
            {
            iData[i]->MenuItem().SetId( KErrNotFound );
            iData[i]->SetDirty( ETrue );
            SaveUndefinedItemL( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// Call back from Homescreen settings
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::SettingsChangedL( const TDesC8& /*aEvent*/,  const TDesC8& /*aPluginName*/,
                           const TDesC8& /*aPluginUid*/, const TDesC8& aPluginId )
    {
    if( aPluginId.CompareF( iInstanceUid ) == 0 )
        {
        UpdateDataL();
        }   
    }

// ---------------------------------------------------------------------------
// Gets the menu item from engine using the setting properties as filter
// ---------------------------------------------------------------------------
//
CCarouselData* CCarouselPluginData::GetMenuDataL( 
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    if ( aProperties.Count() <= 0 )
		{
		User::Leave( KErrArgument );
		}

    TPtrC8 type;
	TPtrC8 uid;
	TPtrC8 view;
	TPtrC8 param;
    
    // first we need to check the item type
    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {
        if ( aProperties[i]->Name() == KProperNameType )
            {
            type.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameUid)
            {
            uid.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameView)
            {
            view.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameParam )
            {
            param.Set( aProperties[i]->Value());
            }
        }
    CCarouselData* data = new ( ELeave ) CCarouselData();
    CleanupStack::PushL( data );
    if( type == KProperValueBookmark )
        {
        GetBkmDataL( uid, view, param, *data );
        }
    else if( type == KProperValueFolder )
        {
        GetFolderData( param, *data );
        }
    else if( type == KProperValueMailbox )
        {
        GetMailboxDataL( uid, param, *data );
        }
    else
        {
        GetMCSDataL( aProperties, *data );
        }
    CleanupStack::Pop( data );
    return data;
    }

// ---------------------------------------------------------------------------
// Creates bookmark data item.
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::GetBkmDataL( const TDesC8& aUid, const TDesC8& aView, const TDesC8& aParam, CCarouselData& aData )
    {
    TMenuItem item;
        
    if( aUid.Length() > 0 )
        {
        CMenuFilter* filter = CMenuFilter::NewLC();
        HBufC* name( NULL );
        HBufC* value( NULL );
        name = AiUtility::CopyToBufferL( name, KProperNameUid );
        CleanupStack::PushL( name );
        value = AiUtility::CopyToBufferL( value, aUid );
        CleanupStack::PushL( value );
        filter->HaveAttributeL( *name, *value );  
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( name );
        item = iEngine.FindMenuItemL( *filter );
        CleanupStack::PopAndDestroy( filter );    
        }
    
    item.SetType( KMenuTypeUrl );
    aData.SetMenuItem( item );

    HBufC* view( NULL );
    view = AiUtility::CopyToBufferL( view, aView );
    CleanupStack::PushL( view );
    aData.SetValueL( *view );
    CleanupStack::PopAndDestroy( view );
    
    HBufC* param( NULL );
    param = AiUtility::CopyToBufferL( param, aParam );
    CleanupStack::PushL( param );
    aData.SetNameL( *param );
    CleanupStack::PopAndDestroy( param );
    }
    
// ---------------------------------------------------------------------------
// Creates folder data item.
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::GetFolderData( const TDesC8& aParam, CCarouselData& aData )
    {
    // In folder case, we have to extract id from 
    // param attribute and return item with this id
    // convert id to integer
    TInt id;
    TLex8 lextmp( aParam);
    lextmp.Val( id );
    
    TMenuItem item;
    item.SetType( KMenuTypeFolder );
    item.SetId( id );
    aData.SetMenuItem( item );
    }

// ---------------------------------------------------------------------------
// Creates mailbox data item.
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::GetMailboxDataL( const TDesC8& aUid, const TDesC8& aParam, CCarouselData& aData )
    {
    TInt id( KErrNotFound );
    TLex8 lextmp( aUid);
    lextmp.Val( id );
    
    TMenuItem item;
    item.SetType( KMenuTypeMailbox );
    item.SetId( id );
    aData.SetMenuItem( item );
    
    HBufC* param( NULL );
    param = AiUtility::CopyToBufferL( param, aParam );
    CleanupStack::PushL( param );
    
    aData.SetNameL( *param );
    
    CleanupStack::PopAndDestroy( param );
    }
    
// ---------------------------------------------------------------------------
// Gets data item from MCS
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::GetMCSDataL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties,
        CCarouselData& aData)
    {
    CMenuFilter* filter = CMenuFilter::NewLC();

    // Exclude 'view' and 'param' attributes from search criteria by default
    // Criterias will be added to filter if setting defines them
    filter->DoNotHaveAttributeL( KMenuAttrView );
    filter->DoNotHaveAttributeL( KMenuAttrParam );
    
    // then add all property/value pairs to the filter
    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {
        // skip the type property
        if( aProperties[i]->Name() == KProperNameType )
            {
            continue;
            }

        HBufC* name( NULL );
        HBufC* value( NULL );
        name = AiUtility::CopyToBufferL( name, aProperties[i]->Name() );
        CleanupStack::PushL( name );
        value = AiUtility::CopyToBufferL( value, aProperties[i]->Value() );
        CleanupStack::PushL( value );

        if ( value->Length() != 0 )
            {
            filter->HaveAttributeL( *name, *value );  
            }
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( name );
        }
    
    // locked property excluded from search pattern 
    filter->DoNotHaveAttributeL( KMenuAttrLocked );
    
    TMenuItem item = iEngine.FindMenuItemL( *filter );
    CleanupStack::PopAndDestroy( filter );
    aData.SetMenuItem( item );
    }

// ---------------------------------------------------------------------------
// Returns menu item for given index
// ---------------------------------------------------------------------------
//
CCarouselData& CCarouselPluginData::DataItemL( TInt aIndex )
    {
    if( aIndex < 0 || aIndex >= iData.Count())
        {
        User::Leave( KErrArgument ); 
        }
    return *iData[aIndex];
    }

// ---------------------------------------------------------------------------
// Save the undefined item.
// ---------------------------------------------------------------------------
//
void CCarouselPluginData::SaveUndefinedItemL( const TInt& aIndex )
    {
    RPointerArray<CItemMap> settingItems;
    CleanupResetAndDestroyPushL( settingItems );
    
    iPluginSettings->GetSettingsL( iInstanceUid, settingItems );
    if ( aIndex >= 0 && aIndex < settingItems.Count() )
        {
        CItemMap* itemMap = settingItems[ aIndex ];
        
        // get properties
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();

        for ( TInt i = 0; i < properties.Count(); i++ )
            {
            if ( properties[ i ]->Name() == KProperNameType )
                {
                properties[ i ]->SetValueL( KProperValueAppl );
                }
            else if ( properties[ i ]->Name() == KProperNameUid )
                {
                properties[ i ]->SetValueL( KMenuAttrUndefUid );
                }
            else if ( properties[ i ]->Name() == KProperNameView )
                {
                properties[ i ]->SetValueL( KNullDesC8 );
                }
            else if ( properties[ i ]->Name() == KProperNameParam )
                {
                properties[ i ]->SetValueL( KNullDesC8 );
                }
            else if ( properties[ i ]->Name() == KProperNameLocked )
                {
                properties[i]->SetValueL( KNullDesC8 );
                }
            }
        }
    // ETrue tells that modified settings are stored also to plugin reference
    iPluginSettings->SetSettingsL( iInstanceUid, settingItems, ETrue );
    CleanupStack::PopAndDestroy(); // settingItems
    }

// End of file
