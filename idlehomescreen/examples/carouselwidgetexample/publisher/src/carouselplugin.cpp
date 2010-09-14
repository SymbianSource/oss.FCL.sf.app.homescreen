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
* Description:  MCS plugin publisher
*
*/

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <gulicon.h>
#include <mcsmenuitem.h>

// User includes
#include <aicontentobserver.h>
#include <aistrcnv.h>
#include <aiutility.h>
#include <aipluginsettings.h>
#include "carouselpluginuids.hrh"
#include "carouselplugin.h"
#include "carouselplugindata.h"
#include "carouselpluginengine.h"

// Constants
const TUint KPluginNameSeprator =  '/';

const TInt KImplUidMCSPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_CAROUSELPLUGIN;

_LIT( KEventNameLaunchByIndex,  "LaunchByIndex" );
_LIT( KEventNameShowSettings,   "ShowSettings" );
_LIT( KContentItemTypeText, "text" );
_LIT( KContentItemTypeImage, "image" );
_LIT( KContentItemTypeData, "data" );

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidMCSPlugin, CCarouselPlugin::NewL ) 
    };

class CPublishItem : public CBase
    {
public:
    ~CPublishItem();
    CPublishItem();
    
    CGulIcon* icon;
    HBufC* text;
    TBool last;
    };

CPublishItem::CPublishItem()
    {
    
    }

CPublishItem::~CPublishItem()
    {
    delete text;
    }

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CMCSPlugin::NewL
//
// ----------------------------------------------------------------------------
//
CCarouselPlugin* CCarouselPlugin::NewL()
    {
    CCarouselPlugin* self = new ( ELeave ) CCarouselPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ----------------------------------------------------------------------------
// CMCSPlugin::CMCSPlugin
//
// ----------------------------------------------------------------------------
//
CCarouselPlugin::CCarouselPlugin()
    {
    }
    
// ----------------------------------------------------------------------------
// CMCSPlugin::ConstructL
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::ConstructL()
    {
    }
    
// ----------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ----------------------------------------------------------------------------
//
CCarouselPlugin::~CCarouselPlugin()
    {
    Release( iContent );

    delete iEngine;
    iObservers.Close();
    
    DeleteContentModel();
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishL
// Publishes the all the items
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::PublishL()
    {    
    TInt err( KErrNone );
    TInt observers( iObservers.Count() );
    TInt transactionId( reinterpret_cast<TInt>( this ) );
    TInt menuItems ( iEngine->MenuItemCount() );

    for ( TInt i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer( iObservers[ i ] );
        err = observer->StartTransaction( transactionId );
        
        if ( err == KErrNotSupported )
            {
            return;
            }
        
        // Publish content to all items
        for ( TInt j = 0; j < menuItems; j++ )
            {
            // Index has to start from 1 ( j + 1 )
            PublishLItemL( *observer, iEngine->MenuDataL( j ), ( j + 1 ) );
            }// shortcut count
        
        if ( err == KErrNone )
            {
            err = observer->Commit( transactionId );
            
            if ( err == KErrNotSupported )
                {
                return;
                }
            }
        }//observers

    // Set all items not dirty.
    for ( TInt j = 0; j < menuItems; j++ )
        {
        iEngine->MenuDataL( j ).SetDirty( EFalse );
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishLItemL
// Publishes one item to given index
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::PublishLItemL( MAiContentObserver& aObserver, 
    CCarouselData& aData, TInt aIndex )
    {
    if( !aData.IsDirty() )
        {
        return;
        }

    CMenuItem* item = iEngine->FetchMenuItemL( aData );
    CleanupStack::PushL( item );
    
    if ( iContentModel[0].type == KAiContentTypeData )
        {
        if(TPtrC16( ( const TText16* ) iContentModel[ 0 ].cid ) == _L("widget") )
            {
            CGulIcon* icon( iEngine->ItemIconL( item, _L("icon")));
            icon->SetBitmapsOwnedExternally( ETrue );
            aObserver.Publish( *this, 0, TPckgC<TAny*>( icon ), aIndex - 1 );
            }
        }
    CleanupStack::PopAndDestroy( item );
    }

// ----------------------------------------------------------------------------
// PublishEditorItemsL
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::PublishEditorItemsL()
    {/*
    RArray<TMenuItem> items = iEngine->MenuItemsL();
    CleanupClosePushL( items );
    
    TInt count( items.Count());
    for( TInt i = 0; i < count; i++ )
        {
        CMenuItem* item( iEngine->FetchMenuItemL( items[i]));
        CleanupStack::PushL( item );
        CGulIcon* icon( iEngine->ItemIconL( *item, _L("icon")));
        TPtrC name( iEngine->ItemTextL( *item, _L("long_name") ));
        CPublishItem* pItem = new CPublishItem;
        pItem->icon = icon;
        pItem->text = name.AllocL();
        if( i < count - 1 )
            {
            pItem->last = EFalse;
            }
        else
            {
            pItem->last = ETrue;
            }
        iObservers[0]->Publish( *this, 0, TPckgC<TAny*>( pItem), i );
        delete pItem;
        CleanupStack::PopAndDestroy( item );
        }
    CleanupStack::PopAndDestroy( &items );
    */
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Start
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::Start( TStartReason /*aReason*/ )
    {
 
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Stop
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::Stop( TStopReason /*aReason*/ )
    {

    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Resume
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::Resume( TResumeReason aReason )
    {       
    if ( aReason == EForeground )
        {
        iEngine->SetBackupRestore( EFalse );
        
        if ( PublishRequired() )
            {
            TRAP_IGNORE( PublishL() );
            }        
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Suspend
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::Suspend( TSuspendReason aReason )
    {    
    if ( aReason == EGeneralThemeChange )
        {                   
        TInt dataCount( iEngine->MenuItemCount() );
        
        for ( TInt i = 0; i < dataCount; i++ )
            {
            TRAP_IGNORE( iEngine->MenuDataL( i ).SetDirty( ETrue ) );
            }                
        }
    else if ( aReason == EBackupRestore )
        {
        // Prevent item launching during backup / restore 
        iEngine->SetBackupRestore( ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::SubscribeL
//
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
   
// ----------------------------------------------------------------------------
// CMCSPlugin::ConfigureL
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {    
    iEngine = CCarouselPluginEngine::NewL( *this, PublisherInfo().Namespace() );
    
    TLinearOrder<MAiPluginSettings> sortMethod( CCarouselPlugin::CompareItems );
    RAiSettingsItemArray contentItemsArr;

    TInt count( aSettings.Count() );
    
    for ( TInt i = 0; i < count; i++ )
       {
       MAiPluginSettings* setting( aSettings[ i ] );
       
       if( setting->AiPluginItemType() == EAiPluginContentItem )
           {
           MAiPluginContentItem& contItem( setting->AiPluginContentItem() );
           TPtrC name( contItem.Name() );
           TPtrC type( contItem.Type() );
           
           contentItemsArr.InsertInOrder( setting, sortMethod );
           }
       }
    
    iDataCount = contentItemsArr.Count();
    
    if ( iDataCount > 0 )
        {
        // Create the dynamic content Model
        DeleteContentModel();
        
        iContentModel = new ( ELeave ) TAiContentItem[ iDataCount ];
        
        for ( TInt i = 0; i < iDataCount; i++ )
            {
            iContentModel[i].id = i;
        
            MAiPluginContentItem& contentItem( 
                contentItemsArr[ i ]->AiPluginContentItem() );

            if( contentItem.Type() == KContentItemTypeText )
                {
                // text
                iContentModel[i].type = KAiContentTypeText;
                }
            if( contentItem.Type() == KContentItemTypeImage )
                {
                // image
                iContentModel[i].type = KAiContentTypeBitmap;
                }
            else if(contentItem.Type() == KContentItemTypeData )
                {
                // data
                iContentModel[i].type = KAiContentTypeData;
                }
            
            TInt pos( contentItem.Name().Locate( KPluginNameSeprator ) );
            
            HBufC* contentId = HBufC::NewL( contentItem.Name().Length() );
            CleanupStack::PushL( contentId );
            
            TPtr ptr( contentId->Des() ); 
            ptr = contentItem.Name().Mid( pos + 1 );
            
            TInt sizeOfContentId( ptr.Size() + sizeof( wchar_t ) );
            
            iContentModel[i].cid = 
                static_cast<const wchar_t*>( User::AllocL( sizeOfContentId ) );
                
            Mem::Copy( ( TAny* )iContentModel[i].cid, 
                ptr.PtrZ(), sizeOfContentId );
            
            CleanupStack::PopAndDestroy( contentId );
            }
        
        iContent = AiUtility::CreateContentItemArrayIteratorL( 
                iContentModel, iDataCount );
        }
    
    contentItemsArr.Reset();
    // We own the array so destroy it
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::GetProperty
// 
// ----------------------------------------------------------------------------
//
TAny* CCarouselPlugin::GetProperty( TProperty aProperty )
    {
    if( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );        
        }
        
    return NULL;
    }
 
// ----------------------------------------------------------------------------
// CMCSPlugin::HandleEvent
// 
// ----------------------------------------------------------------------------
//
void CCarouselPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {       
    if( aEventName == KEventNameLaunchByIndex )
        {        
        TInt32 index;
        AiUtility::ParseInt( index, aParam );
        
        TRAP_IGNORE( iEngine->LaunchItemL( index - 1 ) );
        }
    else if( aEventName == KEventNameShowSettings )
        {
        TRAP_IGNORE( iEngine->ShowSettingsL() );
        }
    else if( aEventName == _L("ShowEditor"))
        {
        TRAP_IGNORE( PublishEditorItemsL() );
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::CompareItems
//
// ----------------------------------------------------------------------------
//
TInt CCarouselPlugin::CompareItems( const MAiPluginSettings& aFirst,
    const MAiPluginSettings& aSecond )
    {
    MAiPluginSettings& first = const_cast<MAiPluginSettings&>(aFirst);
    MAiPluginSettings& second = const_cast<MAiPluginSettings&>(aSecond);
    return first.AiPluginContentItem().Name().CompareC(second.AiPluginContentItem().Name());    
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::DeleteContentModel
//
// ----------------------------------------------------------------------------
//    
void CCarouselPlugin::DeleteContentModel()
    {
    if( iContentModel )
        {
        for ( TInt i = 0; i < iDataCount; i++ )
            {             
            if( iContentModel[i].cid )
                {
                TAny* cell = static_cast<TAny*>( const_cast<wchar_t*>( iContentModel[i].cid ) );
                User::Free( cell ); // Originally allocated with User::Alloc, so delete
                                    // with correlating method.
                iContentModel[i].cid = NULL;
                }
            }
        
        delete iContentModel;
        iContentModel = NULL;        
        }    
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishRequired
//
// ----------------------------------------------------------------------------
//    
TBool CCarouselPlugin::PublishRequired() const
    {
    TInt count( iEngine->MenuItemCount() );
    
    TBool retval( EFalse );
    
    for ( TInt i = 0; !retval && i < count; i++ )
        {               
        TRAP_IGNORE( retval = iEngine->MenuDataL( i ).IsDirty() );       
        }
        
    
    return retval;
    }

// ======== GLOBAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// Constructs and returns an application object.
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / 
        sizeof( TImplementationProxy );
    return KImplementationTable;
    }

// End of file
