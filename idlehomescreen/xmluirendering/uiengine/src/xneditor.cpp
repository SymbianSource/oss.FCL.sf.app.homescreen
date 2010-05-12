/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages Ai3 personalization
*
*/

// System includes
#include <StringLoader.h>
#include <aknlistquerydialog.h> 
#include <aknnotewrappers.h>
#include <utf.h>
#include <AknsWallpaperUtils.h>
#include <centralrepository.h>
#include <AknSkinsInternalCRKeys.h>
#include "../../../inc/ai3.hrh"

#include <xnuiengine.rsg>
#include <hscontentcontrolui.h>
#include <hscontentcontrol.h>
#include <activeidle2domaincrkeys.h>

// User includes
#include "xnuiengine.h"
#include "xnproperty.h"
#include "xnnode.h"
#include "xntype.h"
#include "xntext.h"
#include "xnappuiadapter.h"

#include "xndomnode.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xndomattribute.h"
#include "hspssapi.h"
#include "cpssapi.h"
#include "xneditmode.h"
#include "xndomdocument.h"
#include "xnnodeimpl.h"
#include "xnodt.h"
#include "xnplugindefs.h"

#include "xnviewmanager.h"
#include "xnplugindata.h"
#include "xnrootdata.h"
#include "xnviewdata.h"
#include "xnwallpaperview.h"
#include "xnbackgroundmanager.h"

#include "xneditor.h"
#include "xnpanic.h"

#include "xnoomsyshandler.h"

using namespace hspswrapper;
using namespace cpswrapper;

// LOCAL CONSTANTS AND MACROS
_LIT8( KEventPluginUnInstalled, "PluginUninstalled" );
_LIT8( KEventRootConfActivated, "AppConfActivated" );
_LIT8( KEventPluginUpdated, "PluginUpdated" );
_LIT8( KEventPluginInstalled, "PluginInstalled" );

_LIT8( KEmptyWidgetUid, "0x2001F47F" );
_LIT8( KDownload, "Editor/DownloadCaption" );

_LIT8( KSingle, "single" );
_LIT8( KMulti, "multi" );


const TInt32 KMultiInstanceUnlimitedValue = -1;

const TUid KDummyUid = { 0x0000000 };

const TUint32 KAICCPluginUIDKey = 0x00003010;
const TInt32 KOpaQDataLen = 10;

enum
    {
    ECanBeAdded = 0x01,
    ECanBeRemoved
    };

// ====================== LOCAL FUNTION PROTOTYPES ============================
static void DeletePluginInfos( TAny* aObject );
static void DeleteItemMaps( TAny* aObject );
static void DeleteContentInfo( TAny* aObject );

static TPtrC ParseWidgetName( const CHsContentInfo& aContentInfo );
static void SetPropertyL( CXnNode& aNode, const TDesC8& aAttribute, 
                          const TDesC8& aValue );    
static void ShowErrorNoteL( const TDesC& aMsg );      
static CXnPluginData* DeterminePlugin( CXnViewManager& aViewManager, 
                                         CXnNode* aPredicate = NULL );
static CXnPluginData* DeterminePlugin( 
                            RPointerArray< CXnPluginData >& aPlugins,                            
                            const CHsContentInfo& aContentInfo );

// ============================ LOCAL FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// DeletePluginInfos
// ---------------------------------------------------------------------------
// 
static void DeletePluginInfos( TAny* aObject )
    {
    reinterpret_cast<
        RPointerArray< hspswrapper::CPluginInfo >* >(
            aObject )->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// DeleteItemMaps
// ---------------------------------------------------------------------------
// 
static void DeleteItemMaps( TAny* aObject )
    {
    reinterpret_cast<
        RPointerArray< hspswrapper::CItemMap >* >(
            aObject )->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// DeleteContentInfo
// ---------------------------------------------------------------------------
// 
static void DeleteContentInfo( TAny* aObject )    
    {
    reinterpret_cast<RPointerArray< CHsContentInfo >* >( 
        aObject )->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// ParseWidgetName
// ---------------------------------------------------------------------------
// 
static TPtrC ParseWidgetName( const CHsContentInfo& aContentInfo )
    {
    TPtrC retval( aContentInfo.Name() );

    TInt pos( retval.Locate( ':' ) );

    if ( pos != KErrNotFound )
        {
        retval.Set( retval.Right( retval.Length() - pos - 1 ) );
        }

    return retval;
    }

// ---------------------------------------------------------------------------
// SetPropertyL
// ---------------------------------------------------------------------------
// 
static void SetPropertyL( CXnNode& aNode, 
    const TDesC8& aAttribute, const TDesC8& aValue )
    {
    CXnDomNode* node( aNode.DomNode() );
    
    if ( node )
        {
        CXnDomStringPool* sp( node->StringPool() );

        // create new property
        CXnDomPropertyValue* value = CXnDomPropertyValue::NewL( sp );
        CleanupStack::PushL( value );

        value->SetStringValueL( CXnDomPropertyValue::EString, aValue );

        CXnProperty* prop = CXnProperty::NewL( aAttribute, value, *sp );
                    
        CleanupStack::Pop( value );
        CleanupStack::PushL( prop );

        aNode.SetPropertyL( prop );

        CleanupStack::Pop( prop );
        }
    }

// ---------------------------------------------------------------------------
// ShowErrorNoteL
// ---------------------------------------------------------------------------
// 
static void ShowErrorNoteL( const TDesC& aMsg )
    {
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
    CleanupStack::PushL( note );

    note->ExecuteLD( aMsg );

    CleanupStack::Pop( note );    
    }

// ---------------------------------------------------------------------------
// DeterminePlugin
// Determinates target plugin where widget should be added
// ---------------------------------------------------------------------------
// 
CXnPluginData* DeterminePlugin( CXnViewManager& aViewManager, 
    CXnNode* aPredicate )
    {
    CXnViewData& viewData( aViewManager.ActiveViewData() );

    if ( aPredicate )
        {
        CXnPluginData* plugin( viewData.Plugin( aPredicate ) );

        if ( plugin && !plugin->Occupied() )
            {
            return plugin;
            }
        }

    RPointerArray< CXnNode >& nodes( aViewManager.PluginNodes() );

    for ( TInt i = 0; i < nodes.Count(); i++ )
        {
        CXnNode* node( nodes[i] );

        CXnPluginData* plugin( viewData.Plugin( node ) );

        if ( plugin && !plugin->Occupied() )
            {
            return plugin;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// DeterminePlugin
// Determines target plugin based widget info
// ---------------------------------------------------------------------------
//
CXnPluginData* DeterminePlugin( RPointerArray< CXnPluginData >& aPlugins, 
    const CHsContentInfo& aContentInfo ) 
    {    
    for ( TInt i = 0; i < aPlugins.Count(); i++ )
        {
        CXnPluginData* plugin( aPlugins[i] );
        
        if ( plugin->Occupied() && plugin->PluginId() == aContentInfo.PluginId() )
            {
            return plugin;
            }
        }
    
    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CXnEditor::NewL
// ---------------------------------------------------------------------------
// 
CXnEditor* CXnEditor::NewL( CXnViewManager& aViewManager, 
    const TDesC8& aUid )
    {
    CXnEditor* self = CXnEditor::NewLC( aViewManager, aUid );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CXnEditor::NewLC
// ---------------------------------------------------------------------------
// 
CXnEditor* CXnEditor::NewLC( CXnViewManager& aViewManager,
    const TDesC8& aUid )
    {
    CXnEditor* self = new ( ELeave ) CXnEditor( aViewManager );
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    return self;
    }

// ---------------------------------------------------------------------------
// CXnEditor::CXnEditor
// ---------------------------------------------------------------------------
// 
CXnEditor::CXnEditor( CXnViewManager& aViewManager )
    : iViewManager( aViewManager ),
      iWidgetsVisibilityState( ETrue )
    {
    iViewManager.AddObserver( *this );
    }

// ---------------------------------------------------------------------------
// CXnEditor::ConstructL
// ---------------------------------------------------------------------------
// 
void CXnEditor::ConstructL( const TDesC8& aUid )
    {
    iCpsWrapper = CCpsWrapper::NewL( *this );
    iHspsWrapper = CHspsWrapper::NewL( aUid, this );
    iRepository= CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    iOomSysHandler = CXnOomSysHandler::NewL();
    }

// ---------------------------------------------------------------------------
// CXnEditor::~CXnEditor
// ---------------------------------------------------------------------------
// 
CXnEditor::~CXnEditor()
    {
    iViewManager.RemoveObserver( *this );
    if( iPluginsCache.Count() )
        {
        iPluginsCache.ResetAndDestroy();
        }    
    delete iCpsWrapper;
    delete iHspsWrapper;
    delete iPublisherMap;
    delete iRepository;
    delete iOomSysHandler;
    }

// -----------------------------------------------------------------------------
// CXnEditor::IsCurrentViewFull
// -----------------------------------------------------------------------------
//
TBool CXnEditor::IsCurrentViewFull()
    {
    return IsViewFull( iViewManager.ActiveViewData() );
    }

// -----------------------------------------------------------------------------
// CXnEditor::FilterPluginsL
// -----------------------------------------------------------------------------
//
void CXnEditor::FilterPluginsL( CHsContentInfoArray& aContentInfoArray,
    TBool aIgnoreViewFull )
    {
    RPointerArray< CHsContentInfo >& list( aContentInfoArray.Array() );
    
    TBool viewFull = ( aIgnoreViewFull ? EFalse : IsCurrentViewFull() );
    
    for ( TInt i = 0; i < list.Count(); i++ )
        {
        CHsContentInfo* info( list[i] );

        info->SetCanBeAdded( EFalse );
        info->SetCanBeRemoved( EFalse );
        
        TInt result( 0 );

        if ( info->Type() != KKeyTemplate() )
            {   
            result = NonTemplateWidgetCanBeAddedRemovedL( *info );            
            }
        else
            {
            result = TemplateWidgetCanBeAddedRemovedL( *info );
            }
        
        if ( ( result & ECanBeAdded ) && !viewFull )
            {                        
            info->SetCanBeAdded( ETrue );            
            }
        
        if ( result & ECanBeRemoved )
            {
            info->SetCanBeRemoved( ETrue );
            }
        }            
    }

// ---------------------------------------------------------------------------
// CXnEditor::TemplateWidgetCanBeAddedRemovedL
// ---------------------------------------------------------------------------
// 
TInt CXnEditor::TemplateWidgetCanBeAddedRemovedL( 
    CHsContentInfo& aInfo )
	{
	TInt retval( 0 );
	
	TInt widgetCount( 0 );
	
	RPointerArray< CXnPluginData > plugins;
	CleanupClosePushL( plugins );
	
	if ( aInfo.IsWrt() )
	    {
	    // Get plugins globally from all pages
	    iViewManager.PluginDataL( plugins, ETrue );
	    }
	else
	    {
	    // Get plugins from current page
	    iViewManager.PluginDataL( plugins, EFalse );
	    }
	    	
	// Get widgets in current configuration
	for ( TInt i = 0; i < plugins.Count(); i++ )
		{
		CXnPluginData* plugin( plugins[i] );
		
		if ( !plugin->Occupied() )
		    {
		    continue;
		    }
		
		if ( plugin->PublisherName() == aInfo.PublisherId() )
			{
			widgetCount++;
			}
		}

	if ( widgetCount < aInfo.MaxWidgets() )
	    {
	    retval |= ECanBeAdded;
	    }
	
	CleanupStack::PopAndDestroy( &plugins );
	
	return retval;
	}

// ---------------------------------------------------------------------------
// CXnEditor::NonTemplateWidgetCanBeAddedRemovedL
//
// ---------------------------------------------------------------------------
// 
TInt CXnEditor::NonTemplateWidgetCanBeAddedRemovedL( 
    CHsContentInfo& aInfo )
    {    
    TInt retval( 0 );
    
    TInt widgetCount( 0 );

    CXnViewData& activeView( iViewManager.ActiveViewData() );

    RPointerArray< CXnPluginData >& plugins( activeView.PluginData() );

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );

        if ( !plugin->Occupied() )
            {
            continue;
            }        
        
        if ( plugin->PluginUid().CompareF( aInfo.Uid() ) == 0 )
            {
            widgetCount++;
            }
        }
    
    if ( widgetCount < aInfo.MaxWidgets() ||
        aInfo.MaxWidgets() == KMultiInstanceUnlimitedValue )
        {
        retval |= ECanBeAdded;        
        }
    
    return retval;
    }


// ---------------------------------------------------------------------------
// CXnEditor::FilterViewListL
// ---------------------------------------------------------------------------
//
void CXnEditor::FilterViewListL( CHsContentInfoArray& aContentInfoArray )
    {
    RPointerArray< CHsContentInfo >& list( aContentInfoArray.Array() );
    
    TBool canBeAdded( EFalse );
    CXnRootData& appData( iViewManager.ActiveAppData() );
    if ( appData.PluginData().Count() < appData.MaxPages() )
        {
        canBeAdded = ETrue;
        }    
    
    for ( TInt i = 0; i < list.Count(); i++ )
        {
        CHsContentInfo* info( list[i] );        
        info->SetCanBeAdded( canBeAdded );
        }
    }

// ---------------------------------------------------------------------------
// CXnEditor::RemoveUnRegisteredWidgetL
// ---------------------------------------------------------------------------
//
void CXnEditor::RemoveUnRegisteredWidgetL( const TDesC16& aPublisher )
    {
    RPointerArray< CXnPluginData > plugins;
    CleanupClosePushL( plugins );
    
    iViewManager.PluginDataL( plugins, ETrue );

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
        
        if( plugin->Occupied() && plugin->PublisherName() == aPublisher )
            {
            iViewManager.UnloadWidgetFromPluginL( *plugin );                                
            }
        }
    
    CleanupStack::PopAndDestroy( &plugins );
    }

// ---------------------------------------------------------------------------
// CXnEditor::RemoveUnInstalledWidgetL
// ---------------------------------------------------------------------------
// 
void CXnEditor::RemoveUnInstalledWidgetL( const CHsContentInfo& aContentInfo )
    {
    RPointerArray< CXnPluginData > plugins;
    CleanupClosePushL( plugins );
    
    iViewManager.PluginDataL( plugins, ETrue );
    
    CXnPluginData* plugin( DeterminePlugin( plugins, aContentInfo ) ); 
                            
    if ( plugin )
        {
        iViewManager.UnloadWidgetFromPluginL( *plugin );
        }  
    
    CleanupStack::PopAndDestroy( &plugins );
    }

// ---------------------------------------------------------------------------
// CXnEditor::ReplaceWidgetL
// ---------------------------------------------------------------------------
//
void CXnEditor::ReplaceWidgetL( CHsContentInfo& aContentInfo ) 
    {
    RPointerArray< CXnPluginData > plugins;
    CleanupClosePushL( plugins );
    
    iViewManager.PluginDataL( plugins, ETrue );
    
    CXnPluginData* match = NULL;
    
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
        
        if( plugin->PluginId() == aContentInfo.PluginId() )
            {
            if ( aContentInfo.Type() == KNullDesC8 )
                {
                aContentInfo.SetTypeL( plugin->Type() );
                }
            
            if ( aContentInfo.PublisherId() == KNullDesC )
                {
                aContentInfo.SetPublisherIdL( plugin->PublisherName() );
                }
            match = plugin;
            break;
            }
        }
    
    if( match )
        {
        iViewManager.ReplaceWidgetToPluginL( aContentInfo, *match );
        }
    
    CleanupStack::PopAndDestroy( &plugins );    
    }

// ---------------------------------------------------------------------------
// CXnEditor::IdFromCrep
// ---------------------------------------------------------------------------
//
TInt CXnEditor::IdFromCrep (TDes8& aUid) const
    {
    return iRepository->Get( KAICCPluginUIDKey, aUid );
    }

// ---------------------------------------------------------------------------
// CXnEditor::AddWidgetL
// ---------------------------------------------------------------------------
//
void CXnEditor::AddWidgetL()
    {
    if ( !CXnOomSysHandler::HeapAvailable( VIEW_MIN_MEM ) )
        {
    	OomSysHandler().HandlePotentialOomL();
    	return;
        }
    
    TBuf8<KOpaQDataLen> oPaqDataStr;
           
    MHsContentControlUi* ui( NULL );
    if ( IdFromCrep ( oPaqDataStr ) == KErrNone )
        {
        ui = iViewManager.AppUiAdapter().HsContentController( oPaqDataStr ); 
        }
    if( ui )
        {
        ui->SetContentController( this );
        ui->Activate();
        
        return;
        }
    
    CXnPluginData* plugin( NULL );

    if ( iTargetPlugin )
        {
        plugin = DeterminePlugin( iViewManager, iTargetPlugin );               
        }
    else
        {
        plugin = DeterminePlugin( iViewManager );
        }

    iTargetPlugin = NULL;
        
    CHsContentInfoArray* info = CHsContentInfoArray::NewL();
    CleanupStack::PushL( info );
           
    RPointerArray< CHsContentInfo >& widgets( info->Array() );
    
    // get installed widgets and template configurations from HSPS
    HspsWidgetPluginsL( widgets );
        
    // get installed widgets from HSPS (type: "template")    
    CpsWidgetPluginsL( widgets );
                
    // check whether the plugins can be added or removed
    FilterPluginsL( *info, ETrue );
                      
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 8 );
    CleanupStack::PushL( array );
    
    for ( TInt i = 0; i < widgets.Count(); i++ )
        {
        CHsContentInfo* widget( widgets[i] );
        
        if ( widget->CanBeAdded() )
            {
            TPtrC name( ParseWidgetName( *widget ) );
            
            array->InsertIsqAllowDuplicatesL( name );            
            }
        }

    const TDesC8& ns( iViewManager.ViewNode()->Namespace() );

    CXnNode* link( iViewManager.UiEngine().FindNodeByIdL( KDownload, ns ) );

    if ( link )
        {
        CXnText* textIf( NULL );

        XnComponentInterface::MakeInterfaceL( textIf, link->AppIfL() );

        if ( textIf )
            {
            const TDesC* text( textIf->Text() );

            if ( text )
                {
                // First item is always Download link
                array->InsertL( 0, *text );
                }
            }
        }

    // Display dialog
    TInt selectedIndex( 0 );

    CAknListQueryDialog* query =
        new ( ELeave ) CAknListQueryDialog( &selectedIndex );

    query->PrepareLC( R_LISTQUERY_ADD_WIDGET );

    query->SetItemTextArray( array );
    query->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Save dialog pointer for later usage
    iQuery = query;

    TBool linkSelected( EFalse );

    if ( query->RunLD() )
        {
        // Download link is always the first one, if it is defined
        if ( link && selectedIndex == 0 )
            {
            // Activate download link shortcut
            link->SetStateL( XnPropertyNames::style::common::KActive );
            link->UnsetStateL( XnPropertyNames::style::common::KActive );

            linkSelected = ETrue;
            }       
        else
            {
            if ( !plugin )
                {
                // No room for new widget, so error note.
                HBufC* msg = StringLoader::LoadLC( R_QTN_HS_ADD_WIDGET_FULL );                
                ShowErrorNoteL( *msg );                    
                CleanupStack::PopAndDestroy( msg );
                }            
            }
                    
        const TDesC& selectedName( ( *array )[selectedIndex] );

        for ( TInt i = 0; plugin && !linkSelected && i < widgets.Count(); i++ )
            {
            CHsContentInfo* widget( widgets[i] );

            TPtrC name( ParseWidgetName( *widget ) );

            if ( selectedName == name )
                {             
                TInt ret( iViewManager.LoadWidgetToPluginL( *widget, *plugin ) );
                
                if ( ret == KErrNone )
                    {
                    CXnNode *node( plugin->Owner()->LayoutNode() );

                    // Analyse added widget
                    TBool widgetOk( iViewManager.UiEngine().AnalyseAddedWidgetL(
                            *node ) );

                    if ( !widgetOk )
                        {
                        iViewManager.UnloadWidgetFromPluginL( *plugin );
                        
                        // Widget doesn't fit to UI 
                        HBufC* msg = StringLoader::LoadLC(
                            R_QTN_HS_ADD_WIDGET_FULL );
                        
                        ShowErrorNoteL( *msg );
                        
                        CleanupStack::PopAndDestroy( msg );
                        }                    
                    }
                
                break;
                }
            }
        }

    // Dialog ended and deleted by RunLD
    iQuery = NULL;

    CleanupStack::PopAndDestroy( 2, info ); // array
    }

// ---------------------------------------------------------------------------
// CXnEditor::RemoveWidgetL
// ---------------------------------------------------------------------------
//
void CXnEditor::RemoveWidgetL( CXnNode* aNode )    
    {
    if ( iTargetPlugin )
        {
        aNode = iTargetPlugin;               
        }

    iTargetPlugin = NULL;
    
    if ( !aNode )
        {
        return;
        }

    CXnPluginData* plugin( iViewManager.ActiveViewData().Plugin( aNode ) );

    if( plugin && plugin->Removable() )
        {
        TRAPD( err, err = iViewManager.UnloadWidgetFromPluginL( *plugin ) );
        
        if ( err != KErrNone )
            {
            HBufC* msg = StringLoader::LoadLC( 
                    R_QTN_HS_OPERATION_FAILED_NO_DISK );    
            ShowErrorNoteL( *msg );
            CleanupStack::PopAndDestroy( msg );
            }
        }
    }

// ---------------------------------------------------------------------------
// CXnEditor::ReorderWidgetsL
// ---------------------------------------------------------------------------
//
void CXnEditor::ReorderWidgetsL( RPointerArray< CXnNode >* aPluginArray )
    {    
    if ( !aPluginArray )
        {
        return;
        }
    
    CXnViewData& viewData( iViewManager.ActiveViewData() );

    const TDesC8& configurationId( viewData.ConfigurationId() );
  
    CDesC8ArrayFlat* ids = new ( ELeave ) CDesC8ArrayFlat( 6 );
    CleanupStack::PushL( ids );

    for ( TInt i = 0; i < aPluginArray->Count(); i++ )
        {
        CXnPluginData* plugin( viewData.Plugin( ( *aPluginArray )[i] ) );
        if ( plugin )
            {
            const TDesC8& id( plugin->PluginId() );
            
            if ( id != KNullDesC8 )
                {
                ids->AppendL( id );
                }
            }
        }        

    iHspsWrapper->MovePluginsL( configurationId, *ids );

    CleanupStack::PopAndDestroy( ids );
    }

// ---------------------------------------------------------------------------
// CXnEditor::PublisherInfoL
// ---------------------------------------------------------------------------
//
CPublisherInfo* CXnEditor::PublisherInfoL( const CHsContentInfo& aContentInfo )
    {
    if ( !iPublisherMap )
        {
        return NULL;
        }

    RPointerArray< CPublisherInfo >& publisherInfo(
        iPublisherMap->PublisherInfo() );
    
    const TDesC& publisherId( aContentInfo.PublisherId() );

    CPublisherInfo* info = NULL;
    
    for ( TInt i = 0; i < publisherInfo.Count(); i++ )
        {
        CPublisherInfo* temp = publisherInfo[i];

        if ( temp->PublisherId() == publisherId )
            {
            info = temp;
            break;
            }
        }

    return info;
    }

// ---------------------------------------------------------------------------
// CXnEditor::CpsWidgetPluginsL
// ---------------------------------------------------------------------------
//
void CXnEditor::CpsWidgetPluginsL( RPointerArray< CHsContentInfo >& aWidgets )
    {
    // Get publishers from CPS
    
    if( !iPublisherMap )
        {
        iPublisherMap = iCpsWrapper->GetTemplatedPublishersL();
        }
    
    RPointerArray< CPublisherInfo >& publisherInfo( iPublisherMap->PublisherInfo() );
    
    // Find templates for the published data
    for ( TInt i = 0; i < publisherInfo.Count(); i++ )
        {
        CPublisherInfo* info( publisherInfo[i] );

        for ( TInt j = 0; j < iPluginsCache.Count(); j++ )
            {        
            if( iPluginsCache[j]->Name().Length() > 0 )
                {               
                // 8 to 16bit conv
                HBufC* nameBuf = HBufC::NewLC( iPluginsCache[j]->Name().Length() );
                nameBuf->Des().Copy( iPluginsCache[j]->Name() );
                TBool matchingNames = ( nameBuf->Des() == info->TemplateType() );
                CleanupStack::PopAndDestroy();
                if ( matchingNames )
                    {
                
                    // Add published widget
                    CHsContentInfo* contentInfo = CHsContentInfo::NewLC();
    
                    contentInfo->SetNameL( info->WidgetName() );
                    contentInfo->SetPublisherIdL( info->PublisherId() );
                    contentInfo->SetMaxWidgets( info->MaxWidgets() );
                    contentInfo->SetUidL( iPluginsCache[j]->Uid() );
                    contentInfo->SetTypeL( iPluginsCache[j]->Type() );
                    contentInfo->SetDescriptionL( info->Description() );
                    contentInfo->SetIconPathL( info->LogoIcon() );                                
                    contentInfo->SetIsWrt( info->ContentType() == KWRTTemplate() );
                                    
                    aWidgets.AppendL( contentInfo );
                    
                    CleanupStack::Pop( contentInfo );
                    break;
                    }
                }
            }
        }        
    }


// ---------------------------------------------------------------------------
// CXnEditor::ToggleWidgetsVisibiltyL
// ---------------------------------------------------------------------------
//
TBool CXnEditor::ToggleWidgetsVisibiltyL()
    {
    RPointerArray< CXnPluginData >& plugins(
        iViewManager.ActiveViewData().PluginData() );

    TBool useEmpty( iViewManager.ActiveViewData().UseEmptyWidget() );
    
    TBool stateChanged( EFalse );

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
        
        if( !plugin->Removable() )
            {
            // Don't touch to non-removable widget
            continue;
            }

        if ( iWidgetsVisibilityState )
            {            
            // Currently visible, make invisible
            stateChanged = ETrue;

            SetPropertyL( *plugin->Owner()->LayoutNode(),
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KHidden );
            }
        else
            {
            // Currently invisible, make visible
            if ( plugin->Occupied() )
                {
                stateChanged = ETrue;
                
                SetPropertyL( *plugin->Owner()->LayoutNode(),
                    XnPropertyNames::style::common::KVisibility,
                    XnPropertyNames::style::common::visibility::KVisible );                
                }
            else if( useEmpty )
                {
                stateChanged = ETrue;
                
                SetPropertyL( *plugin->Owner()->LayoutNode(),
                    XnPropertyNames::style::common::KVisibility,
                    XnPropertyNames::style::common::visibility::KBlank );                                    
                }
            }
        }

    if ( stateChanged )
        {
        // Update
        iWidgetsVisibilityState = !iWidgetsVisibilityState;
        }

    return iWidgetsVisibilityState;
    }

// ---------------------------------------------------------------------------
// CXnEditor::WidgetsVisible
// ---------------------------------------------------------------------------
//
TBool CXnEditor::WidgetsVisible() const
    {
    return iWidgetsVisibilityState;
    }

// ---------------------------------------------------------------------------
// CXnEditor::SetTargetPlugin
// ---------------------------------------------------------------------------
//
void CXnEditor::SetTargetPlugin( CXnNode* aNode )
    {
    TBool editState( iViewManager.UiEngine().EditMode()->EditState() );
    
    if ( editState )
        {
        iTargetPlugin = aNode;
        }    
    }

// -----------------------------------------------------------------------------
// CXnEditor::NotifyViewActivatedL
// -----------------------------------------------------------------------------
//
void CXnEditor::NotifyViewActivatedL( const CXnViewData& aViewData )
    {
    RPointerArray< CXnPluginData >& plugins( aViewData.PluginData() );
    
    TBool emptyInUse( aViewData.UseEmptyWidget() );
    TBool editState( iViewManager.UiEngine().EditMode()->EditState() );
              
    iWidgetsVisibilityState = EFalse;
    
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin( plugins[i] );
        
        if ( plugin->Occupied() )
            {
            // At least one widget visible
            iWidgetsVisibilityState = ETrue;                       
            }
        
        if ( plugin->Occupied() || editState )
            {
            // Make widget visible
            SetPropertyL( *plugin->Owner()->LayoutNode(),
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KVisible );            
            }              
        else
            {            
            if ( emptyInUse )
                {
                // Make empty space blank
                SetPropertyL( *plugin->Owner()->LayoutNode(),
                    XnPropertyNames::style::common::KVisibility,
                    XnPropertyNames::style::common::visibility::KBlank );
                }
            else
                {
                // Nothing in this plugin hide
                SetPropertyL( *plugin->Owner()->LayoutNode(),
                    XnPropertyNames::style::common::KDisplay,
                    XnPropertyNames::style::common::display::KNone );                
                }
            }
        }                           
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyConfigureWidgetL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyConfigureWidgetL( const CHsContentInfo& aContentInfo,
    CXnPluginData& aPluginData )
    {
    if ( aContentInfo.Type() != KKeyTemplate )    	    
        {
        // Doesn't need configuration
        return;
        }

    const TDesC8& pluginId( aPluginData.PluginId() );

    CHspsConfiguration* pluginConf(
        iHspsWrapper->GetPluginConfigurationL( pluginId ) );

    CleanupStack::PushL( pluginConf );

    const CPublisherInfo* info = PublisherInfoL( aContentInfo );

    __ASSERT_DEBUG( info != NULL, Panic( EXnInvalidPublisherInfo ) );
    if ( !info )
        {
        User::Leave( KErrBadHandle );
        }
    RPointerArray< CItemMap > itemMapListIn;
    CleanupStack::PushL( TCleanupItem( DeleteItemMaps, &itemMapListIn ) );
    RPointerArray< CItemMap >& settings = pluginConf->Settings();

    HBufC8* publisherId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        info->PublisherId() );
    CleanupStack::PushL( publisherId );
    
    CItemMap* itemMap( 0 );
    CPropertyMap* property( 0 );
    CPropertyMap* propertyIn( 0 );

    for ( TInt i = 0; i < settings.Count(); ++i )
        {
        CItemMap* readItem = settings[i];
        itemMap = CItemMap::NewLC();
        itemMap->SetItemIdL( readItem->ItemId() );
        const TDesC8& itemName = readItem->ItemName();
        itemMap->SetItemNameL( itemName );

        RPointerArray< CPropertyMap >& properties = readItem->Properties();
        for ( int j = 0; j < properties.Count(); ++j )
            {
            property = properties[j];
            propertyIn = CPropertyMap::NewLC();
            propertyIn->SetNameL( property->Name() );
            // Dispatching is based on item name
            if (  itemName == KContentSource() )
                {
                propertyIn->SetValueL( *publisherId );
                }
            else if ( itemName ==  KContentData() )
                {
                // If this default template with full configuration
                TInt pos = property->Value().LocateReverse( KSepratorChar );
                if ( KErrNotFound != pos  )
                    {
                    propertyIn->SetValueL( property->Value() );
                    }
                else
                    {
                    HBufC8* contentData = HBufC8::NewLC( publisherId->Length()
                        + KSeperator().Length()
                        + property->Value().Length());
                    contentData->Des().Copy( *publisherId );
                    contentData->Des().Append( KSeperator );
                    contentData->Des().Append( property->Value() );
                    propertyIn->SetValueL( *contentData );
                    CleanupStack::PopAndDestroy( contentData );
                    }
                }
            else if ( itemName.Find( KPublisher ) != KErrNotFound )
                {
                propertyIn->SetValueL( *publisherId );
                }
            else if ( !KPubTrigger().Compare( itemName ) )
                {
                HBufC8* triggerData = HBufC8::NewLC(
                    publisherId->Length()
                    + KSeperator().Length()
                    + KPublisher().Length()
                    + KWidgetTriggerName().Length());
                triggerData->Des().Copy( *publisherId );
                triggerData->Des().Append( KSeperator );
                triggerData->Des().Append( KPublisher );
                triggerData->Des().Append( KWidgetTriggerName );

                propertyIn->SetValueL( *triggerData );
                CleanupStack::PopAndDestroy( triggerData );
                }
            else if ( !KTrigger().Compare( itemName ) )
                {
                // If this default template with full configuration
                TInt pos = property->Value().LocateReverse( KSepratorChar );
                if ( KErrNotFound != pos )
                    {
                    propertyIn->SetValueL( property->Value() );
                    }
                else
                    {
                    HBufC8* triggerData = HBufC8::NewLC(
                        publisherId->Length()
                        + KSeperator().Length()
                        + property->Value().Length()
                        + KOpen().Length()
                        + property->Value().Length()
                        + KClose().Length() );
                    triggerData->Des().Copy( *publisherId );
                    triggerData->Des().Append( KSeperator );
                    triggerData->Des().Append( property->Value() );
                    triggerData->Des().Append( KOpen );
                    triggerData->Des().Append( property->Value() );
                    triggerData->Des().Append( KClose );
                    propertyIn->SetValueL( *triggerData );
                    CleanupStack::PopAndDestroy( triggerData );
                    }
                }
            else
                {
                // unrecognized items are not handled
                }
            itemMap->AddPropertyMapL( propertyIn );
            CleanupStack::Pop( propertyIn );
            }
        itemMapListIn.AppendL( itemMap );
        CleanupStack::Pop( itemMap );
        }
    iHspsWrapper->SetPluginSettingsL( pluginId, itemMapListIn );

    CleanupStack::PopAndDestroy( publisherId );
    CleanupStack::PopAndDestroy( &itemMapListIn );
    CleanupStack::PopAndDestroy( pluginConf );
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyWidgetAdditionL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyWidgetAdditionL( const CXnPluginData& aPluginData )
    {
    CXnNode* node( aPluginData.Owner()->LayoutNode() );

    // At least one widget is visible
    iWidgetsVisibilityState = ETrue;
    
    // Ensure the new widget is visible
    SetPropertyL( *node,
        XnPropertyNames::style::common::KVisibility,
        XnPropertyNames::style::common::visibility::KVisible );        
    
    SetPropertyL( *node,
        XnPropertyNames::style::common::KDisplay,
        XnPropertyNames::style::common::display::KBlock );
    
    node->SetDirtyL( XnDirtyLevel::ELayoutAndRenderSiblings );
    
    WidgetListChanged();
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyWidgetRemovalL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyWidgetRemovalL( const CXnPluginData& aPluginData )
    {
    CXnNode* node( aPluginData.Owner()->LayoutNode() );

    CXnViewData& viewData( 
        static_cast< CXnViewData& >( *aPluginData.Parent() ) );
    
    TBool emptyInUse( viewData.UseEmptyWidget() );
    
    if ( emptyInUse )
        {        
        if ( iViewManager.UiEngine().IsEditMode() )
            {
            // Ensure the plugin which was holding 
            // the removed widget is visible when edit mode is active
            SetPropertyL( *node,
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KVisible );                   
            }
        else
            {
            // Ensure the plugin which was holding 
            // the removed widget is blank now
            SetPropertyL( *node,
                XnPropertyNames::style::common::KVisibility,
                XnPropertyNames::style::common::visibility::KBlank );            
            }                
        }
    else
        {
        // Ensure the plugin which was holding the removed widget is invisible
        SetPropertyL( *node,
            XnPropertyNames::style::common::KDisplay,
            XnPropertyNames::style::common::display::KNone );
                
        // Reodred layout tree by moving the plugin which was holding the
        // removed widget as the last one in layout tree's plugin the list
        CXnNode *parent( node->Parent() );

        RPointerArray< CXnNode >& children( parent->Children() );

        TInt nodeIndex( children.Find( node ) );

        children.Remove( nodeIndex );
        children.Insert( node, children.Count() - 1 );
        }
    
    node->SetDirtyL( XnDirtyLevel::ERender );
    
    iTargetPlugin = NULL;
    
    if ( aPluginData.Active() )
        {
        CXnViewData& viewData( iViewManager.ActiveViewData() );
        
        RPointerArray< CXnPluginData >& plugins( viewData.PluginData() );
        
        TBool occupied( EFalse );
        
        for ( TInt i = 0; i < plugins.Count(); i++ )
            {
            if ( plugins[i]->Occupied() && plugins[i]->Removable() )
                {
                occupied = ETrue;
                break;
                }
            }
        
        if ( !occupied )
            {
            // No more widgets in the active view
            iWidgetsVisibilityState = EFalse;
            }               
        }
    
    WidgetListChanged();
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyViewAdditionL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyViewAdditionL( const CXnPluginData& /*aPluginData*/ )
    {
    ViewListChanged();
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyViewRemovalL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyViewRemovalL( const CXnPluginData& /*aPluginData*/ )
    {
    ViewListChanged();
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyWidgetUnregisteredL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyWidgetUnregisteredL( const TDesC& aPublisher )
    {
    ResetCache();
    RemoveUnRegisteredWidgetL( aPublisher );
    WidgetListChanged();    
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyWidgetRegisteredL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyWidgetRegisteredL()
    {
    ResetCache();        
    WidgetListChanged();
    }

// ---------------------------------------------------------------------------
// CXnEditor::NotifyViewDeactivatedL
// ---------------------------------------------------------------------------
//
void CXnEditor::NotifyViewDeactivatedL( const CXnViewData& /*aViewData*/)
    {
    iTargetPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// CXnEditor::SetWallpaperL
// ---------------------------------------------------------------------------
//
void CXnEditor::SetWallpaperL()
    {
    // Display dialog
    TInt selectedIndex( 0 );

    CAknListQueryDialog* query =
        new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    CleanupStack::PushL( query );
    query->PrepareLC( R_LISTQUERY_CHANGE_WALLPAPER );

    if ( query->RunLD() )
        {
        if ( selectedIndex == 0 )
            {
            // set wallpaper. No need to check return value. If successful,
            // avkon calls SkinConfigurationChanged function
            AknsWallpaperUtils::SetIdleWallpaper(
                    KNullDesC,
                    NULL );
            }
        else if ( selectedIndex == 1 )
            {
            iViewManager.AppUiAdapter().ActivateLocalViewL( KWallpaperViewUid, KDummyUid, KSingle );                
            }
        else if ( selectedIndex == 2 )
            {
            iViewManager.AppUiAdapter().ActivateLocalViewL( KWallpaperViewUid, KDummyUid, KMulti );                
            }        
        }
    CleanupStack::Pop( query );
    }

// -----------------------------------------------------------------------------
// CXnEditor::HandleNotifyL
// Handles notifications from HSPS wrapper
// -----------------------------------------------------------------------------
//
TInt CXnEditor::HandleNotifyL(
    const TDesC8& aEvent,
    const TDesC8& /*aAppConfUid*/,
    const TDesC8& aPluginName,
    const TDesC8& /*aOrigUid*/,
    const TDesC8& aPluginUid,
    const TDesC8& aPluginId )
    {
    if ( aEvent == KEventPluginUnInstalled )
        {
        ResetCache();
                
        CHsContentInfo* info = CHsContentInfo::NewLC();

        info->SetNameL( aPluginName );
        info->SetUidL( aPluginUid );
        info->SetPluginIdL( aPluginId );

        RemoveUnInstalledWidgetL( *info );
        WidgetListChanged();

        CleanupStack::PopAndDestroy( info );
        }
    else if ( aEvent == KEventRootConfActivated )
        {
        iViewManager.AppUiAdapter().ReloadUiL();
        }
    else if ( aEvent == KEventPluginInstalled )
        {        
        ResetCache();
        
        WidgetListChanged();        
        }
    else if ( aEvent == KEventPluginUpdated )
        {
        ResetCache();
        
        // If the plugin is in use then reload the widget
        if ( aPluginId.Length() > 0 )
            {
            CHsContentInfo* info = CHsContentInfo::NewLC();    
            info->SetNameL( aPluginName );
            info->SetUidL( aPluginUid );
            info->SetPluginIdL( aPluginId );    
            ReplaceWidgetL( *info );                
            CleanupStack::PopAndDestroy( info );
            }
        WidgetListChanged();
        }
 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnEditor::WidgetListChanged
// -----------------------------------------------------------------------------
//
void CXnEditor::WidgetListChanged()
    {
    TBuf8<KOpaQDataLen> oPaqDataStr;
              
   MHsContentControlUi* ui( NULL );
   if ( IdFromCrep ( oPaqDataStr ) == KErrNone )
       {
       ui = iViewManager.AppUiAdapter().HsContentController( oPaqDataStr ); 
       }
    if( ui )
        {
        ui->NotifyWidgetListChanged();
        }
    MHsContentControl* srv( iViewManager.AppUiAdapter().HsContentControlSrv() );
    if ( srv )
        {
        srv->NotifyWidgetListChanged();
        }
    }

// -----------------------------------------------------------------------------
// CXnEditor::ViewListChanged
// -----------------------------------------------------------------------------
//
void CXnEditor::ViewListChanged()
    {
    TBuf8<KOpaQDataLen> oPaqDataStr;
              
   MHsContentControlUi* ui( NULL );
   if ( IdFromCrep ( oPaqDataStr ) == KErrNone )
       {
       ui = iViewManager.AppUiAdapter().HsContentController( oPaqDataStr ); 
       }
    if( ui )
        {
        ui->NotifyViewListChanged();
        }
    MHsContentControl* srv( iViewManager.AppUiAdapter().HsContentControlSrv() );
    if ( srv )
        {
        srv->NotifyViewListChanged();
        }
    }

// -----------------------------------------------------------------------------
// CXnEditor::HspsWrapper
// -----------------------------------------------------------------------------
//
CHspsWrapper& CXnEditor::HspsWrapper() const
    {
    return *iHspsWrapper;
    }

// -----------------------------------------------------------------------------
// CXnEditor::AppendPluginsL
// -----------------------------------------------------------------------------
//
void CXnEditor::AppendPluginsL(
        RPointerArray< hspswrapper::CPluginInfo > aPlugins,
        RPointerArray< CHsContentInfo >& aWidgets )    
    {    
    // Append plugins to the content info array
    for ( TInt i = 0; i < aPlugins.Count(); i++ )
       {
    
       // Block the empty and template plugins from the list
       if ( aPlugins[i]->Uid().CompareF( KEmptyWidgetUid ) == 0 
               || aPlugins[i]->Type() == KKeyTemplate )
           {           
           continue;
           }
       
       CHsContentInfo* contentInfo = CHsContentInfo::NewLC();
       contentInfo->SetNameL( aPlugins[i]->Name() );
       contentInfo->SetUidL( aPlugins[i]->Uid() );
       contentInfo->SetTypeL( aPlugins[i]->Type() );      
       if ( aPlugins[i]->Type() == KKeyWidget 
               || aPlugins[i]->Type() == KKeyTemplate )
           {
           contentInfo->SetMaxWidgets( aPlugins[i]->MultiInstance() );
           }
       contentInfo->SetDescriptionL( aPlugins[i]->Description() );
       contentInfo->SetIconPathL( aPlugins[i]->LogoIcon() );

       aWidgets.AppendL( contentInfo );
       CleanupStack::Pop( contentInfo );
       }    
    }

// -----------------------------------------------------------------------------
// CXnEditor::ResetCache
// -----------------------------------------------------------------------------
//
void CXnEditor::ResetCache()
    {
    // Force loading of widget/template plugin configurations
    iPluginsCache.ResetAndDestroy();
    
    // Forece reloading of CPS publishers 
    delete iPublisherMap;
    iPublisherMap = NULL;
    }

// -----------------------------------------------------------------------------
// CXnEditor::HspsApplicationPluginsL
// -----------------------------------------------------------------------------
//
void  CXnEditor::HspsApplicationPluginsL( RPointerArray< CHsContentInfo >& aWidgets )
    {
    RPointerArray< hspswrapper::CPluginInfo > plugins;
    CleanupStack::PushL( TCleanupItem( DeletePluginInfos, &plugins ) );
        
    iHspsWrapper->GetAppConfigurationsL( plugins );
    
    // Append plugins to the content info array
    AppendPluginsL( plugins, aWidgets ); 
    
    CleanupStack::PopAndDestroy( &plugins );
    }

// -----------------------------------------------------------------------------
// CXnEditor::HspsViewPluginsL
// -----------------------------------------------------------------------------
//
void CXnEditor::HspsViewPluginsL( RPointerArray< CHsContentInfo >& aWidgets )
    {
    RPointerArray< hspswrapper::CPluginInfo > plugins;
    CleanupStack::PushL( TCleanupItem( DeletePluginInfos, &plugins ) );
        
    iHspsWrapper->GetPluginsL( plugins, KPluginInterface, KView );
    
    // Append plugins to the content info array
    AppendPluginsL( plugins, aWidgets ); 
    
    CleanupStack::PopAndDestroy( &plugins );
    }

// -----------------------------------------------------------------------------
// CXnEditor::HspsWidgetPluginsL
// -----------------------------------------------------------------------------
//
void CXnEditor::HspsWidgetPluginsL( RPointerArray< CHsContentInfo >& aWidgets )
    {          
    __ASSERT_DEBUG( aWidgets.Count() == 0, User::Leave( KErrGeneral ) );
    
    // If widget/template plugins haven't been fetched yet
    if( iPluginsCache.Count() == 0 )
        {
        // Fetch the plugins into the runtime cache
        TRAPD( err, DoHspsWidgetPluginsL() );
        if( err )
            {            
            ResetCache();
            User::LeaveIfError( err );
            }  
        }    
        
    // Append plugins to the content info array
    AppendPluginsL( iPluginsCache, aWidgets );            
    }

// -----------------------------------------------------------------------------
// CXnEditor::DoHspsWidgetPluginsL
// -----------------------------------------------------------------------------
//
void CXnEditor::DoHspsWidgetPluginsL()
    {        
    iHspsWrapper->GetPluginsL( iPluginsCache, KPluginInterface, KKeyWidget );        
    iHspsWrapper->GetPluginsL( iPluginsCache, KPluginInterface, KKeyTemplate );            
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::WidgetListL( CHsContentInfoArray& aArray )
    {
    // append the list with native widget and template plugins from HSPS
    HspsWidgetPluginsL( aArray.Array() );
        
    // append the list with published template plugins from CPS 
    CpsWidgetPluginsL( aArray.Array() );

    // check whether the plugins can be added or removed
    FilterPluginsL( aArray, ETrue );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::WidgetListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray )
    {
    TInt err( KErrNone );
    RPointerArray< CXnPluginData > widgets;
    CleanupClosePushL( widgets );
    
    if ( aInfo.Type() == KApplication )
        {
        if ( aInfo.Uid().CompareF( iViewManager.ActiveAppData().PluginUid() ) == 0 )
            {
            // Get widgets included in active application configuration
            err = iViewManager.PluginDataL( KNullDesC8(), widgets );
            }
        else
            {
            // Invalid application configuration
            err = KErrArgument;
            }
        }
    else if ( aInfo.Type() == KView )
        {
        // Get widgets included in a view
        err = iViewManager.PluginDataL( aInfo.PluginId(), widgets );
        }
    else
        {
        err = KErrArgument;
        }
    
    if ( !err )
        {
        // Get installed widget content infos
        RPointerArray< CHsContentInfo > array;
        CleanupStack::PushL( TCleanupItem( DeleteContentInfo, &array ) );
        
        // get installed widgets and template configurations from HSPS
        HspsWidgetPluginsL( array );
                
        // get published widgets
        CpsWidgetPluginsL( array );
                
        // Create content info for each found widget
        for ( TInt i = 0; i < widgets.Count(); i++ )
            {
            CHsContentInfo* info = CreateContentInfoLC( *widgets[i], array );
            if ( info )
                {
                aArray.Array().AppendL( info );
                CleanupStack::Pop( info );
                }
            }
        CleanupStack::PopAndDestroy(); // array
        }
    
    CleanupStack::PopAndDestroy(); // widgets
        
    return err;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ViewListL( CHsContentInfoArray& aArray )
    {
    RPointerArray< CHsContentInfo >& array( aArray.Array() );
    
    
    // get installed view configurations from HSPS
    HspsViewPluginsL( array );
    
    FilterViewListL( aArray );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ViewListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray )
    {
    TInt err( KErrNone );

    if ( aInfo.Type() == KApplication )
        {
        if ( aInfo.Uid().CompareF( iViewManager.ActiveAppData().PluginUid() ) == 0 )
            {
            // Get list of views in active application configuration
            CXnRootData& appData( iViewManager.ActiveAppData() );
            RPointerArray< CXnPluginData >& views( appData.PluginData() );
            
            // Get installed view content infos
            RPointerArray< CHsContentInfo > array;
            CleanupStack::PushL( TCleanupItem( DeleteContentInfo, &array ) );
            HspsViewPluginsL( array );
                    
            // Create content info for each found view
            for ( TInt i = 0; i < views.Count(); i++ )
                {
                CHsContentInfo* info = CreateContentInfoLC( *views[i], array );
                if ( info )
                    {
                    CXnViewData* view = static_cast < CXnViewData* >( views[ i ] );
                    info->SetIsFull( IsViewFull( *view ) );
                    aArray.Array().AppendL( info );
                    CleanupStack::Pop( info );
                    }
                }
            CleanupStack::PopAndDestroy(); // array
            }
        else
            {
            // Invalid application configuration
            err = KErrArgument;
            }
        }
    else
        {
        // Invalid argument
        err = KErrArgument;
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::AppListL( CHsContentInfoArray& aArray )
    {
    RPointerArray< CHsContentInfo >& array( aArray.Array() );
    
    // get installed application configurations from HSPS
    HspsApplicationPluginsL( array );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::AddWidgetL( CHsContentInfo& aInfo )
    {
    TInt ret( KErrNone );    

    const TDesC8& type( aInfo.Type() );
    
    if ( ( type != KKeyWidget && type != KKeyTemplate ) ||
         aInfo.Uid() == KNullDesC8 )
        {
        // malformed content info
        return KErrArgument;
        }

    // the widget can not be added. Return proper error code
    if ( IsCurrentViewFull() )
        {
        return KHsErrorViewFull;
        }
    else if ( !aInfo.CanBeAdded() )
        {
        return KHsErrorMaxInstanceCountExceeded;
        }


    CXnPluginData* plugin( NULL );
    
    if( iTargetPlugin )
        {
        plugin = DeterminePlugin( iViewManager, iTargetPlugin );
        }
    else
        {
        plugin = DeterminePlugin( iViewManager );
        }
    
    iTargetPlugin = NULL;
    
    if ( !plugin )
        {
        return KErrGeneral;
        }
    
    ret = iViewManager.LoadWidgetToPluginL( aInfo, *plugin );
    
    if( ret == KErrNone )
        {
        CXnNode* node( plugin->Owner()->LayoutNode() );
        TBool widgetOk( iViewManager.UiEngine().AnalyseAddedWidgetL( *node ) );
    
        if( !widgetOk )
            {
            iViewManager.UnloadWidgetFromPluginL( *plugin );
            
            ret = KHsErrorDoesNotFit;
            }        
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::RemoveWidgetL( CHsContentInfo& aInfo )
    {
    if ( !aInfo.CanBeRemoved() || aInfo.PluginId() == KNullDesC8 || 
        ( aInfo.Type() != KKeyWidget && aInfo.Type() != KKeyTemplate ) )
        {
        return KErrArgument;
        }
    
    TInt retval( KErrNotFound );
    
    RPointerArray< CXnPluginData > plugins;
    CleanupClosePushL( plugins );
    
    // Search only from active view
    iViewManager.PluginDataL( plugins );
    
    CXnPluginData* plugin( DeterminePlugin( plugins, aInfo ) ); 
                            
    if ( plugin && plugin->Removable() )
        {
        retval = iViewManager.UnloadWidgetFromPluginL( *plugin );
        }  
    
    CleanupStack::PopAndDestroy( &plugins );
    
    return retval;
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::AddViewL( CHsContentInfo& aInfo )
    {
    if ( !aInfo.CanBeAdded() || aInfo.Uid() == KNullDesC8 || 
        aInfo.Type() != KView )
        {
        return KErrArgument;
        }
          
    return iViewManager.AddViewL( aInfo );
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::RemoveViewL( CHsContentInfo& aInfo )
    {
    if ( !aInfo.CanBeRemoved() || aInfo.PluginId() == KNullDesC8 || 
        aInfo.Type() != KView )
        {
        return KErrArgument;
        }
    
    return iViewManager.RemoveViewL( aInfo );
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ActivateViewL( CHsContentInfo& aInfo )
    {
    if ( aInfo.Type() != KView )
        {
        return KErrArgument;
        }
    
    return iViewManager.ActivateViewL( aInfo.PluginId() );
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ActivateAppL( CHsContentInfo& aInfo )
    {
    if ( aInfo.Type() != KApplication )
        {
        return KErrArgument;
        }
    
    return iViewManager.ActivateAppL( aInfo.Uid() );     
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ActiveViewL( CHsContentInfo& aInfo )
    {

    TInt err( KErrNone );
        
    CXnViewData& viewData( iViewManager.ActiveViewData() );
    
    CHspsConfiguration* view( iHspsWrapper->GetPluginConfigurationL( viewData.PluginId() ) );
    CleanupStack::PushL( view );
    if ( view )
        {
        aInfo.SetNameL( view->PluginInfo().Name() );
        aInfo.SetPluginIdL( viewData.PluginId() );
        aInfo.SetUidL( view->PluginInfo().Uid() );
        aInfo.SetTypeL( view->PluginInfo().Type() );
        aInfo.SetDescriptionL( view->PluginInfo().Description() );
        aInfo.SetIconPathL( view->PluginInfo().LogoIcon() );
        aInfo.SetIsFull( IsViewFull( viewData ) );        
        }
    else
        {
        err = KErrNotFound;
        }
        
    CleanupStack::PopAndDestroy( view );
    
    return err;     
    }

// -----------------------------------------------------------------------------
// from MHsContentController
// -----------------------------------------------------------------------------
//
TInt CXnEditor::ActiveAppL( CHsContentInfo& aInfo )
    {
    
    TInt err( KErrNone );
    CHspsConfiguration* app = iHspsWrapper->GetAppConfigurationL();
    CleanupStack::PushL( app );

    if ( app->PluginInfo().Uid().Length() > 0 )
        {
        aInfo.SetNameL( app->PluginInfo().Name() );
        aInfo.SetUidL( app->PluginInfo().Uid() );
        aInfo.SetTypeL( app->PluginInfo().Type() );
        aInfo.SetDescriptionL( app->PluginInfo().Description() );
        aInfo.SetIconPathL( app->PluginInfo().LogoIcon() );

        CXnRootData& appData( iViewManager.ActiveAppData() );
        if ( appData.PluginData().Count() < appData.MaxPages() )
            {
            aInfo.SetIsFull( EFalse );
            }
        else
            {
            aInfo.SetIsFull( ETrue );
            }
        }
    else
        {
        err = KErrNotFound;
        }
    
    CleanupStack::PopAndDestroy( app );
    return err;     
    }

// -----------------------------------------------------------------------------
// CXnEditor::CreateContentInfoLC
// -----------------------------------------------------------------------------
//
CHsContentInfo* CXnEditor::CreateContentInfoLC( CXnPluginData& aPlugin, 
    RPointerArray< CHsContentInfo >& aInfos )
    {
    CHsContentInfo* contentInfo( NULL );
    if ( aPlugin.Occupied() )
        {
        for ( TInt i = 0; i < aInfos.Count() && !contentInfo; i++ )
            {
            CHsContentInfo* info = aInfos[i];
            if ( aPlugin.PluginUid().CompareF( info->Uid() ) == 0 && 
                 ( ( aPlugin.PublisherName().Length() == 0 ) || 
                   ( aPlugin.PublisherName().CompareF( info->PublisherId() ) == 0 ) ) )
                {
                contentInfo = info->CloneL();
                CleanupStack::PushL( contentInfo );
                contentInfo->SetPluginIdL( aPlugin.PluginId() );
                contentInfo->SetCanBeRemoved( aPlugin.Removable() );
                }
            }
        }
    return contentInfo;
    }

// -----------------------------------------------------------------------------
// CXnEditor::IsViewFull
// -----------------------------------------------------------------------------
//
TBool CXnEditor::IsViewFull( CXnViewData& aViewData )
    {
    TBool isFull( ETrue );

    RPointerArray< CXnPluginData >& plugins( 
            aViewData.PluginData() );

    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CXnPluginData* plugin = plugins[ i ];

        if ( !plugin->Occupied() )
            {
            isFull = EFalse;
            break;
            }
        }
    
    return isFull;
    }

// -----------------------------------------------------------------------------
// CXnBackgroundManager::OOMSysHandler
// -----------------------------------------------------------------------------
//
CXnOomSysHandler& CXnEditor::OomSysHandler() const
    {
    __ASSERT_DEBUG( iOomSysHandler , User::Panic( _L("xneditor"), 0 ) );

    return *iOomSysHandler;
    }

// End of file
