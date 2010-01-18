/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application configuration composer
*
*/

// System includes
#include <s32file.h>

// User includes
#include "xncomposer.h"
#include "xnplugindata.h"
#include "xnrootdata.h"
#include "xnviewdata.h"
#include "xnodt.h"
#include "xnproperty.h"
#include "xnresource.h"
#include "xndomdocument.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xndomattribute.h"
#include "xnpanic.h"
#include "xnplugindefs.h"
#include "xnviewmanager.h"
#include "xneditor.h"
#include "xnbackgroundmanager.h"

// Constants
_LIT8( KStateWaitConfirmation, "WaitForConfirmation" );
_LIT8( KStateError, "Error" );

_LIT8( KXmluiml, "xmluiml" );
_LIT8( KApp, "application" );
_LIT8( KViews, "views" );
_LIT8( KWidget, "widget" );
_LIT8( KUseEmptyWidget, "use_empty_widget" );
_LIT8( KRemovable, "removable" );

_LIT8( KEmptyWidgetUid, "0x2001F47F" );

_LIT( KCDrive, "C:" );
_LIT8( KTagXuikon, "xuikon" );

using namespace hspswrapper;

// ======== LOCAL FUNCTIONS ========
// --------------------------------------------------------------------------
// ItemValueL
// Get property value from configuration.
// --------------------------------------------------------------------------
//
static HBufC* ItemValueL( CHspsConfiguration& aConfiguration, const TDesC8& aItemId, const TDesC8& aName  )
    {
    HBufC* ret = NULL;

    RPointerArray<CItemMap>& settingsList = aConfiguration.Settings();    
    for( TInt i = 0; i < settingsList.Count(); i++ )
        {
        CItemMap* setting = settingsList[i];
        if( !setting )
            {
            continue;
            }
        
        if( setting->ItemId() == aItemId )
            {
            RPointerArray<CPropertyMap>& properties = setting->Properties();
            for( TInt j = 0; j < properties.Count(); j++ )
                {
                CPropertyMap* property = properties[j];
                if( !property )
                    {
                    continue;
                    }
                
                if( property->Name() == aName )
                    {
                    ret = HBufC::NewL( property->Value().Length() );
                    ret->Des().Copy( property->Value() );
                    break;
                    }
                }            
            break;
            }    
        }        
    return ret;
    }

// ---------------------------------------------------------------------------
// Finds recursively node by name
// @return    returns pointer to desired node, NULL if nothing found 
// ---------------------------------------------------------------------------
//
CXnDomNode* FindNodeByName( CXnDomNode* aNode, const TDesC8& aName )
    {
    if ( !aNode )
        {        
        return NULL;
        }

    if ( aNode->Name() == aName )
        {
        return aNode;
        }

    CXnDomList& list( aNode->ChildNodes() );

    for ( TInt i = 0; i < list.Length() ; i++ )
        {
        CXnDomNode* retval( FindNodeByName(
                static_cast< CXnDomNode* >( list.Item( i ) ), aName ) );

        if ( retval )
            {
            return retval;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// FindNodeById
// Finds recursively node by id
// @return    returns pointer to desired node, NULL if nothing found 
// ---------------------------------------------------------------------------
//
CXnDomNode* FindNodeById( CXnDomNode* aNode, const TDesC8& aId )
    {
    if ( !aNode )
        {        
        return NULL;
        }

    CXnDomList& attributes( aNode->AttributeList() );

    CXnDomAttribute* id(
        static_cast< CXnDomAttribute* >(
                attributes.FindByName( XnPropertyNames::common::KId ) ) );

    if ( id && id->Value() == aId )
        {
        return aNode;
        }

    CXnDomList& list( aNode->ChildNodes() );

    for ( TInt i = 0; i < list.Length() ; i++ )
        {
        CXnDomNode* retval( FindNodeById(
                static_cast< CXnDomNode* >( list.Item( i ) ), aId ) );

        if ( retval )
            {
            return retval;
            }
        }

    return NULL;
    }

// ----------------------------------------------------------------------------
// UpdateResourcesPathL
// ----------------------------------------------------------------------------
//
static void UpdateResourcesPathL( CArrayPtrSeg< CXnResource >& aList, 
    TPtrC aNewPath )
    {
    TFileName newFilename;

    for ( TInt i = 0; i < aList.Count(); i++ )
        {
        CXnResource& res( *aList[i] );
        const TDesC& filename( res.FileName() );

        TParsePtrC fileParser( filename );
        TPtrC nameAndExt( fileParser.NameAndExt() );

        newFilename = aNewPath;
        newFilename.Append( nameAndExt );

        res.SetFileNameL( newFilename );
        }
    }

// --------------------------------------------------------------------------
// UpdateSettingsL
// 
// --------------------------------------------------------------------------
//
static void UpdateSettingsL( const TDesC8& aElementId,
    RPointerArray< CPropertyMap >& aProperties, CXnDomNode& aRootNode )
    {    
    CXnDomNode* node( FindNodeById( &aRootNode, aElementId ) );

    if( !node )
        {
        return;
        }

    CXnDomStringPool* sp( node->StringPool() );

    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {
        CPropertyMap* property( aProperties[i] );

        const TDesC8& name( property->Name() );
        const TDesC8& value( property->Value() );

        CXnDomList& attributes( node->AttributeList() );

        CXnDomAttribute* attribute(
            static_cast< CXnDomAttribute* >( attributes.FindByName( name ) ) );

        if ( attribute )
            {
            // Update current attribute
            attribute->SetValueL( value );
            }
        else
            {
            // Need to create new attribute
            attribute = CXnDomAttribute::NewL( name, sp );
            CleanupStack::PushL( attribute );

            attribute->SetValueL( value );

            node->AttributeList().AddItemL( attribute );
            CleanupStack::Pop( attribute );
            }
        }
    }


// --------------------------------------------------------------------------
// UpdatePluginFromSettingsL()
// Updates plugin from settings
// --------------------------------------------------------------------------
//
static void UpdatePluginFromSettingsL( CHspsConfiguration& aConfiguration,
    CXnDomNode& aRootNode )
    {
    RPointerArray< CItemMap >& settings( aConfiguration.Settings() );

    for ( TInt i = 0; i < settings.Count(); i++ )
        {
        CItemMap* setting( settings[i] );

        const TDesC8& itemId( setting->ItemId() );

        RPointerArray< CPropertyMap >& properties( setting->Properties() );

        UpdateSettingsL( itemId, properties, aRootNode );
        }
    }

// --------------------------------------------------------------------------
// MergeLD()
// --------------------------------------------------------------------------
//
static CXnDomNode* MergeLD( CXnDomNode* aRoot,  
    CXnPluginData& aPluginData, TBool aMergeView = EFalse )
    {
    CXnDomNode* parent( aPluginData.Owner() );
    
    CXnDomNode* childToFind( NULL );

    if ( aMergeView )
        {
        childToFind = FindNodeByName( aRoot, KView );
        }
    else
        {
        childToFind = FindNodeByName( aRoot, KWidget );
        }
    
    if( !childToFind || aRoot->Name() != KXmluiml )
        {
        delete aRoot;
        
        return NULL;
        }

    aPluginData.SetNode( childToFind );
    
    CXnDomList& list( aRoot->ChildNodes() );
           
    TInt index( list.ItemIndex( *childToFind ) );   
    
    CXnDomNode* node =
        static_cast< CXnDomNode* >( list.Item( index ) );
    
    list.RemoveItem( index );

    // sets namespace recursively
    node->SetOwnershipL( aPluginData.PluginId() );
    
    // Takes ownership
    parent->AddChildL( node );

    delete aRoot;
    aRoot = NULL;
        
    if ( aMergeView )
        {
        // Return the last view
        TInt index( parent->ChildNodes().Length() - 1 );

        return ( static_cast< CXnDomNode* >(
            parent->ChildNodes().Item( index ) ) );
        }
    else
        {
        // Return <widget>
        return node;
        }
    }

// --------------------------------------------------------------------------
// UseEmptyWidget
// Determines whether to use empty widget in this view configuration
// --------------------------------------------------------------------------
//
static TBool UseEmptyWidget( CXnDomNode& aView )
    {    
    CXnDomAttribute* attribute(
        static_cast< CXnDomAttribute* >( 
                aView.AttributeList().FindByName( KUseEmptyWidget ) ) );

    if ( attribute && attribute->Value() == XnPropertyNames::KTrue )
        {
        return ETrue;
        }
    
    return EFalse;
    }

// --------------------------------------------------------------------------
// Removable
// Determines whether this plugin is removable
// --------------------------------------------------------------------------
//
static TBool Removable( CXnDomNode& aPlugin )
    {    
    CXnDomAttribute* attribute(
        static_cast< CXnDomAttribute* >( 
                aPlugin.AttributeList().FindByName( KRemovable ) ) );

    if ( attribute && attribute->Value() == XnPropertyNames::KFalse )
        {
        return EFalse;
        }
    
    return ETrue;
    }

// ======== MEMBER FUNCTIONS ========
// --------------------------------------------------------------------------
// CXnComposer::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CXnComposer* CXnComposer::NewL( CHspsWrapper& aWrapper )
    {
    CXnComposer* self = new ( ELeave ) CXnComposer( aWrapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CXnComposer::~CXnComposer
// Destructor
// --------------------------------------------------------------------------
//
CXnComposer::~CXnComposer()
    {
    iFs.Close();
    }

// --------------------------------------------------------------------------
// CXnComposer::CXnComposer
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CXnComposer::CXnComposer( CHspsWrapper& aWrapper )
    : iWrapper( aWrapper )
    {
    }

// --------------------------------------------------------------------------
// CXnComposer::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CXnComposer::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// --------------------------------------------------------------------------
// CXnComposer::ComposeRootL()
// Composes the application root configuration
// --------------------------------------------------------------------------
//
CXnODT* CXnComposer::ComposeRootL( CXnRootData& aRootData )
    {
    // Not owned
    iODT = NULL;
    
    // Get application configuration
    CHspsConfiguration* configuration( iWrapper.GetAppConfigurationL() );
    CleanupStack::PushL( configuration );

    const TDesC8& state( configuration->PluginInfo().ConfigurationState() );
    
    aRootData.SetConfigurationIdL( configuration->ConfId() );
           
    if ( state.CompareF( KStateError ) == 0 || 
        configuration->Resources().Count() == 0 )
        {
        CleanupStack::PopAndDestroy( configuration );
        
        return NULL;       
        }
        
    // Get application configuration odt resource
    CXnODT* odt = CXnODT::NewL();
    CleanupStack::PushL( odt );
    
    // Save for later usage, not owned
    iODT = odt;
    
    // Find a resource which can be internalized
    const CObjectMap* resourceObject = FindObject( 
            configuration->Resources(), KTagXuikon() );
    if ( !resourceObject )
        {
        CleanupStack::PopAndDestroy( 2, configuration );
        
        return NULL;
        }
    
    CXnDomNode* root( GetOdtL( *resourceObject, aRootData ) );       
    if ( !root )
        {        
        CleanupStack::PopAndDestroy( 2, configuration );
        
        return NULL;
        }
    
    odt->DomDocument().SetRootNode( root );
      
    // "views" node is the owner node of view nodes
    CXnDomNode* views( FindNodeByName( root, KViews ) );
        
    if ( !views )
        {
        CleanupStack::PopAndDestroy( 2, configuration );
        
        return NULL;        
        }

    CPluginInfo& info( configuration->PluginInfo() );

    aRootData.SetOwner( root );
    aRootData.SetNode( views );
    
    aRootData.SetPluginNameL( info.Name() );
    aRootData.SetPluginUidL( info.Uid() );
    
    aRootData.SetPluginTypeL( KApp );
    
    aRootData.SetMaxPages( info.MaxChild() );

    // Get plugins (= views) in this application configuration
    RPointerArray< CPluginMap >& plugins( configuration->PluginMaps() );

    if ( ( plugins.Count() == 0 ) || ( aRootData.MaxPages() <= 0 ) )
        {
        CleanupStack::PopAndDestroy( 2, configuration );
        
        return NULL;
        }

    RPointerArray< CXnPluginData >& array( aRootData.PluginData() );
    
    TInt index( 0 );
    
    for ( TInt i = 0; ( i < plugins.Count() ) && ( i < aRootData.MaxPages() );
        i++ )
        {
        CPluginMap* plugin( plugins[i] );
        
        CXnViewData* view = CXnViewData::NewLC( aRootData );

        // Give view data ownership to CXnRootData
        array.AppendL( view );                                 
        CleanupStack::Pop( view );
       
        view->SetOwner( views );  
        
        view->SetPluginIdL( plugin->PluginId() );
                
        if ( plugin->ActivationState() )
            {            
            index = i;            
            }

        view->SetLockingStatus( plugin->LockingStatus() );
        }

    CXnViewData* initial( static_cast< CXnViewData* >( array[ index ] ) );
    
    initial->SetInitial();
        
    CleanupStack::Pop( odt );
    
    CleanupStack::PopAndDestroy( configuration );
                    
    // Ownership is granted to the caller
    return odt;          
    }

// --------------------------------------------------------------------------
// CXnComposer::ComposeViewL()
// Composes a view
// --------------------------------------------------------------------------
//
TInt CXnComposer::ComposeViewL( CXnViewData& aViewData )
    {    
    TInt retval( KXnErrPluginFailure );
    
    if ( aViewData.PluginId() == KNullDesC8 )
        {
        return retval;
        }
    
    // Get view configuration
    CHspsConfiguration* configuration(
        iWrapper.GetPluginConfigurationL( aViewData.PluginId() ) );
    CleanupStack::PushL( configuration );

    const TDesC8& state( configuration->PluginInfo().ConfigurationState() );
                     
    if ( state.CompareF( KStateError ) == 0 || 
        configuration->Resources().Count() == 0 )
        {               
        CleanupStack::PopAndDestroy( configuration );
        
        return retval;
        }
                  
    aViewData.SetConfigurationIdL( configuration->ConfId() );    
    aViewData.SetPluginStateL( KStateWaitConfirmation );
    
    // Find a resource which can be internalized
    const CObjectMap* resourceObject = FindObject( 
                configuration->Resources(), KTagXuikon );
    if ( !resourceObject )
        {
        CleanupStack::PopAndDestroy( configuration );
        
        return retval;
        }
    
    CXnDomNode* root( GetOdtL( *resourceObject, aViewData ) );
    if ( !root )
        {               
        CleanupStack::PopAndDestroy( configuration );
        
        return retval;        
        }

    root->SetNamespaceL( aViewData.PluginId() );
    
    // Merge view to root. Note that root will be obsolete after this call
    CXnDomNode* viewRoot( MergeLD( root, aViewData, ETrue ) );
    
    if ( viewRoot )
        {                                                     
        // Finalise view data              
        CPluginInfo& info( configuration->PluginInfo() );
        aViewData.SetPluginNameL( info.Name() );
        aViewData.SetPluginUidL( info.Uid() );

        aViewData.SetPluginTypeL( KView );
        
        // Find first <plugin> element from this view
        CXnDomNode* pluginNode( FindNodeByName( viewRoot, KPlugin ) );

        UpdatePluginFromSettingsL( *configuration, *viewRoot );
                
        aViewData.SetUseEmptyWidget( UseEmptyWidget( *viewRoot )  );   

        // Read wallpaper image path from HSPS
        CXnBackgroundManager& bgManager = aViewData.ViewManager().
            Editor().BgManager();
        
        // if page specific wallpaper feature is enabled
        if( bgManager.ActivatedL() )
            {
            HBufC* bgImage = ItemValueL( *configuration, KWallpaper, KPath );
            CleanupStack::PushL( bgImage );
            if( bgImage && bgImage->Length() > 0 )
                {
                bgManager.CacheWallpaperL( bgImage->Des(), aViewData );
                }
            CleanupStack::PopAndDestroy( bgImage );
            }
        if ( pluginNode )
            {            
            // This assumes all <plugin> elements are siblings
            CXnDomNode* pluginParent( pluginNode->Parent() );   
            
            CXnDomList& list( pluginParent->ChildNodes() );
        
            // Get plugins configuration
            RPointerArray< CPluginMap >& plugins( configuration->PluginMaps() );
                    
            RPointerArray< CXnPluginData >& array( aViewData.PluginData() );

            TInt count( 0 );
            
            for ( TInt i = 0; i < list.Length(); i++ )
                {
                CXnDomNode* node =
                    static_cast< CXnDomNode* >( list.Item( i ) );
        
                // We are interested only about <plugin> elements
                if ( node->Name() == KPlugin )
                    {
                    // Create plugin data
                    CXnPluginData* widget = CXnPluginData::NewLC( aViewData );
            
                    // Give ownership to view data
                    array.AppendL( widget );
                    CleanupStack::Pop( widget );
                            
                    // <plugin> element
                    widget->SetOwner( node );
                    
                    if ( count < plugins.Count() )
                        {                        
                        widget->SetPluginIdL( plugins[ count ]->PluginId() );
                        }
                    
                    count++;
                    }        
                }                           
            }
        
        retval = KErrNone;
        }
    
    CleanupStack::PopAndDestroy( configuration );
    
    return retval;
    }

// --------------------------------------------------------------------------
// ComposeWidgetL()
// Composes a widget
// --------------------------------------------------------------------------
//
TInt CXnComposer::ComposeWidgetL( CXnPluginData& aPluginData )     
    {    
    TInt retval( KXnErrPluginFailure );
    
    if ( aPluginData.PluginId() == KNullDesC8 )
        {
        return retval;
        }
    
    // Get widget configuration
    CHspsConfiguration* configuration(
        iWrapper.GetPluginConfigurationL( aPluginData.PluginId() ) );
    CleanupStack::PushL( configuration );

    // Empty widget is not composed at all
    if ( configuration->PluginInfo().Uid().CompareF( KEmptyWidgetUid ) == 0 )
        {
        aPluginData.SetEmptyL( aPluginData.PluginId() );
        
        CleanupStack::PopAndDestroy( configuration );
        
        return KErrNone;
        }
    
    const TDesC8& state( configuration->PluginInfo().ConfigurationState() );
                     
    if ( state.CompareF( KStateError ) == 0 || 
        configuration->Resources().Count() == 0 )
        {               
        CleanupStack::PopAndDestroy( configuration );
        
        return retval;       
        }
    
    aPluginData.SetConfigurationIdL( configuration->ConfId() );    
    aPluginData.SetPluginStateL( KStateWaitConfirmation );
                              
    // Find a resource which can be internalized
    const CObjectMap* resourceObject = FindObject( 
                configuration->Resources(), KTagXuikon );
    if ( !resourceObject )
        {
        CleanupStack::PopAndDestroy( configuration );
        
        return retval;
        }
    
    CXnDomNode* root( GetOdtL( *resourceObject, aPluginData ) );
    if ( !root )
        {
        CleanupStack::PopAndDestroy( configuration );        

        return retval;        
        }

    // Update plugin namespace
    root->SetNamespaceL( aPluginData.PluginId() );

    // Nested plugins are not supported currently. 
    // Note root will be obsolete after this call.
    CXnDomNode* widgetRoot( MergeLD( root, aPluginData ) );

    if ( widgetRoot )
        {
        // Finalise plugin data
        CPluginInfo& info( configuration->PluginInfo() );
        
        aPluginData.SetPluginNameL( info.Name() );
        aPluginData.SetPluginUidL( info.Uid() );

        aPluginData.SetPluginTypeL( info.Type() );

        UpdatePluginFromSettingsL( *configuration, *widgetRoot );
        
        if ( info.Type() == KKeyTemplate )
            {
            CXnDomNode* node( FindNodeByName( widgetRoot, KContentSourceNode ) );
            
            if ( node )
                {
                CXnDomList& attributes( node->AttributeList() );
                 
                CXnDomAttribute* attribute( static_cast< CXnDomAttribute* >(
                    attributes.FindByName( KName ) ) );
                
                if ( attribute )
                    {
                    aPluginData.SetPublisherNameL( attribute->Value() );
                    }
                }
            }
        
        aPluginData.SetRemovable( Removable( *widgetRoot ) );

        retval = KErrNone;                       
        }
    
    CleanupStack::PopAndDestroy( configuration );
    
    return retval;
    }

// --------------------------------------------------------------------------
// CXnComposer::GetOdtL()
//
// --------------------------------------------------------------------------
//
CXnDomNode* CXnComposer::GetOdtL( const CObjectMap& aObject,
    CXnPluginData& aPluginData )
    {
    if( !iODT )
        {
        return NULL;
        }
    
    CXnDomNode* root( NULL ); 
    
    TBuf8< KMaxFileName > resource;
    TFileName privatePath;
    TFileName filename;

    // Get private path
    User::LeaveIfError( iFs.PrivatePath( privatePath ) );

    // Get resource path and name
    resource = aObject.Path();
    resource.Append( aObject.NameL() );

    // Copy from TBuf8 to TBuf
    filename.Copy( resource );

    // insert private path and
    filename.Insert( 0, privatePath );

    // ... c-drive
    filename.Insert( 0, KCDrive );

    RFile file;

    User::LeaveIfError( file.Open( iFs, filename, EFileShareReadersOnly ) );
    CleanupClosePushL( file );

    TInt size( 0 );
    file.Size( size );

    if ( size )
        {
        CFileStore* store( CDirectFileStore::FromLC( file ) );

        RStoreReadStream instream;
        CleanupClosePushL( instream );

        instream.OpenLC( *store, store->Root() );

        // Consume header
        CXnODT::InternalizeHeaderL( instream );
        
        CArrayPtrSeg< CXnResource >* list( 
            CXnODT::InternalizeResourceListL( instream ) );
        
        aPluginData.SetResources( list );

        // Set correct resource path
        UpdateResourcesPathL( 
            *list, TParsePtrC( filename ).DriveAndPath() );            
                        
        root = iODT->InternalizeDomDocumentL( instream );

        // Destroy the stream object, close the instream, destroy store
        CleanupStack::PopAndDestroy( 3, store );
        }

    CleanupStack::PopAndDestroy( &file );

    return root;
    }

// --------------------------------------------------------------------------
// CXnComposer::FindObject()
// Finds an object from the provided array 
// --------------------------------------------------------------------------
//
const CObjectMap* CXnComposer::FindObject( 
        RPointerArray<CObjectMap>& aResourceObjects,
        const TDesC8& aTag ) const
    {
    CObjectMap* object = NULL;
    
    const TInt count = aResourceObjects.Count();
    if ( count )
        {   
        
        // Find an object with the provided tag
        for( TInt objectIndex=0; objectIndex < count; objectIndex++ )
            {
            CObjectMap* o = aResourceObjects[ objectIndex ];
            if ( o->Tag() == aTag )
                {
                object = o;
                break;
                }
            }
        
        // If the tag was not found, return first object
        if( !object )
            {
            object = aResourceObjects[ 0 ];
            }
        }
    
    return object;
    }

//  End of File
