/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ODT parser
*
*/
#include <aisystemuids.hrh>
#include <xnextrenderingpluginadapter.h>
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>

#include "xnextrenderingpluginwrapper.h"

#include "xnodt.h"
#include "xndomdocument.h"
#include "xndomnode.h"
#include "xndomattribute.h"
#include "xndomlist.h"
#include "xnnode.h"

#include "xnecomhandler.h"
#include "xnuiengine.h"
#include "xntype.h"
#include "xncontroladapter.h"
#include "xnviewcontroladapter.h"
#include "xnpopupcontroladapter.h"
#include "xnwidgetextensionadapter.h"
#include "xnscrollablecontroladapter.h"
#include "xnlistquerydialogadapter.h"

#include "xncomponent.h"
#include "xncomponentnodeimpl.h"
#include "xnviewnodeimpl.h"
#include "xnviewsnodeimpl.h"
#include "xnproperty.h"
#include "xncomponentfactory.h"
#include "xnarray.h"
#include "xnviewmanager.h"
#include "xnplugindata.h"
#include "xnrootdata.h"
#include "xnviewdata.h"

#include "xnodtparser.h"

_LIT8( KWidgetBgSkinId, "SKIN(268458534 9916)" );

// CONSTANTS
_LIT8( KXMLUIMLNodeName, "xmluiml" );
_LIT8( KViewsNodeName, "views" );
_LIT8( KViewNodeName, "view" );
_LIT8( KBoxNodeName, "box" );
_LIT8( KButtonNodeName, "button" );
_LIT8( KStylusPopupNodeName, "styluspopup" );
_LIT8( KImageNodeName, "image" );
_LIT8( KTooltipNodeName, "tooltip" );
_LIT8( KMenuBarNodeName, "menubar" );
_LIT8( KMenuNodeName, "menu" );
_LIT8( KMenuItemNodeName, "menuitem" );
_LIT8( KDynMenuItemNodeName, "dynmenuitem" );
_LIT8( KWidgetMenuItemNodeName, "widgetmenuitem" );
_LIT8( KActionsNodeName, "actions" );
_LIT8( KActionNodeName, "action" );
_LIT8( KTriggerNodeName, "trigger" );
_LIT8( KEventNodeName, "event" );
_LIT8( KTextNodeName, "text" );
_LIT8( KMarqueeNodeName, "marquee" );
_LIT8( KObjectNodeName, "object" );
_LIT8( KTracksterNodeName, "trackster" );
_LIT8( KPropertyNodeName, "property" );
_LIT8( KConfigurationNodeName, "configuration" );
_LIT8( KNewstickerNodeName, "newsticker" );
_LIT8( KAttributeNodeName, "attribute" );
_LIT8( KItemNodeName, "item" );
_LIT8( KDescNodeName, "desc" );
_LIT8( KTextZoomNodeName, "textzoom" );
_LIT8( KVolumeControlNodeName, "volumecontrol" );
_LIT8( KPluginNodeName, "plugin" );
_LIT8( KWidgetNodeName, "widget" );
_LIT8( KSliderNodeName, "slider" );
_LIT8( KClockNodeName, "clock" );
_LIT8( KClockAnalog, "analog" );
_LIT8( KClockDigital, "digital" );
_LIT8( KClockFace, "face" );
_LIT8( KClockFace12, "face12" );
_LIT8( KClockFace24, "face24" );
_LIT8( KClockDate, "date" );
_LIT8( KClockAmpm, "ampm" );
_LIT8( KClockDay, "day" );
_LIT8( KSoftkeyNodeName,"softkey" );
_LIT8( KConditionalTrigger,"conditional" );
_LIT8( KCondtionalOn, "true" );
_LIT8( KListQueryDialogNodeName, "listquerydialog" );
_LIT8( KSettingsConfigurationNodeName, "settingsconfiguration" );
_LIT8( KContentSourceNodeName, "contentsource" );
_LIT8( KMenuExtensionNodeName, "menuextension" );
_LIT8( KWidgetExtensionNodeName, "widgetextension" );
_LIT8( KTextEditorNodeName, "texteditor" );
_LIT8( KActionsHandlerNodeName, "actionshandler" );
_LIT8( KScrollableBoxNodeName, "scrollablebox" );
_LIT8( KPopUpNodeName, "popup" );

// LOCAL FUNCTION PROTOTYPES
#ifdef _XN3_DEBUG_
#include <flogger.h>
#include <utf.h>
static void PrintDomL( CXnDomDocument* aDomDocument );
#endif

static CXnNode* BuildRootNodeL( CXnUiEngine& aEngine );         

static CXnNode* FindAncestorWhoActAsParent( CXnDomNode& aSource );

static TBool CreateTriggerInLayoutNodeTree( CXnDomNode& aSource );

static void CopyPropertiesFromDomNodeToNodeL(
    CXnNode& aTarget,
    CXnDomNode& aSource,
    CXnDomStringPool& aStringPool );
static CXnNode* ConstructNodeFromPluginL(
    CXnEcomHandler& aEcomHandler,
    CXnDomNode& aSource );
static CXnNode* ConstructKnownBuiltinNodeL( CXnDomNode& aSource );
static CXnNode* ConstructGeneralNodeL( CXnDomNode& aSource );

// ============================= LOCAL FUNCTIONS ===============================

    
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

#ifdef _XN3_DEBUG_
_LIT( KLogFolder," xnlayoutengine" );
_LIT( KLogDom, "dom.log" );
#define _LOGT( aDescription ) RFileLogger::Write( \
    KLogFolder, KLogDom, EFileLoggingModeOverwrite, aDescription );
#define _LOGTFRM1( a, b ) RFileLogger::WriteFormat( \
    KLogFolder, KLogDom, EFileLoggingModeOverwrite, ( a ), ( b ) )
#define _LOGTFRM2( a, b, c ) RFileLogger::WriteFormat( \
    KLogFolder, KLogDom, EFileLoggingModeOverwrite, ( a ), ( b ), ( c ) );
#define _LOGTFRM3( a, b, c, d ) RFileLogger::WriteFormat( \
    KLogFolder, KLogDom, EFileLoggingModeOverwrite, ( a ), ( b ), ( c ), ( d ) );
static const TInt GetNodeDepth( CXnDomNode *aNode )
    {
    TInt depth = 0;
    CXnDomNode* node( aNode );
    while ( node )
        {
        node = node->Parent();
        if ( node )
            {
            depth++;
            }
        }
    return depth;
    }

//-----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
static HBufC8* GetFixedTextLC(
    const TDesC8& aText,
    const TInt aDepth,
    const TDesC8& aDelim )
    {
    HBufC8 *buf = HBufC8::NewL( aDepth * aDelim.Length() + aText.Length() + 1 );
    TInt i = 0;
    for ( ; i < aDepth; i++ )
        {
        buf->Des().Append( aDelim );
        }
    buf->Des().Append( aText );
    return buf;
    }

//-----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
static void PrintDomL( CXnDomDocument* aDomDocument )
    {
__UHEAP_MARK;
    TInt attrCount( 0 );
    TInt propCount( 0 );

    CXnDomDepthIterator* iter = 
            CXnDomDepthIterator::NewL( *aDomDocument->RootNode() );
    CleanupStack::PushL( iter );

    _LIT8( KDelim, "\t" );
    CXnDomNode* nodeParent( NULL );
    CXnDomNode* node = iter->First();
    while ( node )
        {
        const TInt depth = GetNodeDepth( node );

        HBufC8 *nameBuf = GetFixedTextLC( node->Name(), depth, KDelim );
        CleanupStack::PushL( nameBuf );
        const TDesC8& name = nameBuf->Des();

        HBufC8 *nsBuf = GetFixedTextLC( node->Namespace(), depth, KDelim );
        CleanupStack::PushL( nsBuf );
        const TDesC8& ns = nsBuf->Des();

        _LOGTFRM2( _L8( "%S -------------------------- %S\n" ), &name, &ns );

        CXnDomList& attrList = node->AttributeList();
        TInt length( attrList.Length() );
        attrCount += length;

        for ( TInt i = 0; i < length; i++ )
            {
            CXnDomAttribute* attr = static_cast< CXnDomAttribute* >(
                attrList.Item( i ) );

            HBufC8 *attrBuf = GetFixedTextLC( attr->Name(), depth,  KDelim );
            CleanupStack::PushL( attrBuf );
            const TDesC8& attrName = attrBuf->Des();

            const TDesC8& attrValue = attr->Value();

            _LOGTFRM2( _L8( "%S=%S\n" ), &attrName, &attrValue );

            CleanupStack::PopAndDestroy( attrBuf );
            }

        if ( node->Parent() )
            {
            nodeParent = node;
            }
        node = iter->NextL();

        CleanupStack::PopAndDestroy( nsBuf );
        CleanupStack::PopAndDestroy( nameBuf );
        }
    CleanupStack::PopAndDestroy( iter );

    _LOGTFRM1( _L8( "****DOM Size: %d ****\n" ), aDomDocument->Size() );
    _LOGTFRM1( _L8( "****DOM Node Count: %d ****\n" ), aDomDocument->DomNodeCount() );
    _LOGTFRM1( _L8( "****DOM Attribute Count: %d ****\n" ), attrCount );
__UHEAP_MARKEND;
    }

#endif
// -----------------------------------------------------------------------------
// BuildRootNodeL()
// Builds root node
// -----------------------------------------------------------------------------
//
static CXnNode* BuildRootNodeL( CXnUiEngine& aEngine )           
    {
    CXnType* type = CXnType::NewL( KXMLUIMLNodeName );
    CleanupStack::PushL( type );

    CXnNodeImpl* impl = CXnNodeImpl::NewL( type ); 
        
    CleanupStack::Pop( type );
    CleanupStack::PushL( impl );

    CXnNode* node = CXnNode::NewL();
    node->SetRootNodeImpl( impl );
    CleanupStack::Pop( impl );

    node->SetUiEngine( aEngine );
    
    return node;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnNode* FindAncestorWhoActAsParent( CXnDomNode& aSource )
    {
    CXnNode* ret( NULL );
    CXnDomNode* parent = aSource.Parent();

    while ( !ret && parent )
        {
        ret = parent->LayoutNode();
        parent = parent->Parent();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CreateTriggerInLayoutNodeTree( CXnDomNode& aSource )
    {
    TBool ret( EFalse );

    CXnDomAttribute* attribute = static_cast< CXnDomAttribute* >(
        aSource.AttributeList().FindByName( KConditionalTrigger ) );

    if ( attribute && attribute->Value() == KCondtionalOn )
        {
        ret = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CopyDomPropertiesFromDomNodeToNodeL
// -----------------------------------------------------------------------------
//
static void CopyDomPropertiesFromDomNodeToNodeL(
    CXnNode& aTarget,
    CXnDomNode& aSource )
    {
    CXnDomList& propertyList = aSource.PropertyList();
    for ( TInt count = propertyList.Length() - 1; count >= 0; --count )
        {
        CXnDomProperty* domProperty = static_cast< CXnDomProperty* >(
            propertyList.Item( count ) );
        CXnProperty* property = CXnProperty::NewSharedL( domProperty );
        CleanupStack::PushL( property );
        aTarget.InitializePropertyL( property );
        CleanupStack::Pop( property );
        }
    }

// -----------------------------------------------------------------------------
// CopyDomAtributesFromDomNodeToNodeL
// -----------------------------------------------------------------------------
//
static void CopyDomAttributesFromDomNodeToNodeL(
    CXnNode& aTarget,
    CXnDomNode& aSource,
    CXnDomStringPool& aStringPool )
    {
    CXnDomList& attributeList = aSource.AttributeList();
    for ( TInt count = attributeList.Length() - 1; count >= 0; --count )
        {
        CXnDomAttribute* attribute = static_cast< CXnDomAttribute* >(
            attributeList.Item( count ) );
#ifdef _DEBUG
        const TDesC8& name = attribute->Name();
        const TDesC8& attrValue = attribute->Value();
#endif
        if ( attribute->ValueStringPoolIndex() == KErrNotFound )
            {
            continue;
            }

        if ( attribute->NameStringPoolIndex() == KErrNotFound )
            {
            continue;
            }
         CXnDomProperty* clone =
            CXnDomProperty::NewL( attribute->NameStringPoolIndex(), &aStringPool );
        CleanupStack::PushL( clone );
        CXnDomPropertyValue* propertyValue =
            CXnDomPropertyValue::NewL( &aStringPool );
        CleanupStack::PushL( propertyValue );
        propertyValue->SetStringPoolIndexL(
            CXnDomPropertyValue::EString, attribute->ValueStringPoolIndex() );
        CXnDomList& propertyList = clone->PropertyValueList();
        propertyList.AddItemL( propertyValue );
        CleanupStack::Pop( propertyValue );
        CXnProperty* property = CXnProperty::NewL( clone );
        CleanupStack::Pop( clone );
        CleanupStack::PushL( property );
        aTarget.InitializePropertyL( property );
        CleanupStack::Pop( property );
        }

    //aSource.DeleteAttributeList();
    // delete attributes from dom node tree
    if ( aSource.Name() != KTriggerNodeName )
        {
        CXnDomList& attrList = aSource.AttributeList();
        while ( attrList.Length() )
            {
            attrList.DeleteItem( attrList.Length() - 1 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CopyPropertiesFromDomNodeToNodeL
// -----------------------------------------------------------------------------
//
static void CopyPropertiesFromDomNodeToNodeL(
    CXnNode& aTarget,
    CXnDomNode& aSource,
    CXnDomStringPool& aStringPool )
    {
    CopyDomPropertiesFromDomNodeToNodeL( aTarget, aSource );
    CopyDomAttributesFromDomNodeToNodeL( aTarget, aSource, aStringPool );
    }

// -----------------------------------------------------------------------------
// ConstructNodeFromPluginL
// -----------------------------------------------------------------------------
//
static CXnNode* ConstructNodeFromPluginL(
    CXnEcomHandler& aEcomHandler,
    CXnDomNode& aSource )
    {
    CXnNode* returnValue = NULL;
    const TDesC8& name = aSource.Name();

    CXnArray* componentFactories = CXnArray::NewL();
    CleanupStack::PushL( componentFactories );

    TUid interfaceUid = { AI3_RENDERING_PLUGIN_ECOM_FACTORY_UID };
    aEcomHandler.PluginsL( interfaceUid, name, *componentFactories );
    TInt count = componentFactories->Container().Count();
    TBool componentNodeImplNeeded = ETrue;
    // invoke factory chain
    for ( TInt i = 0; i < count; ++i )
        {
        MXnComponentFactory* factory = static_cast< MXnComponentFactory* >(
            componentFactories->Container()[i] );
        componentNodeImplNeeded = factory->DoesNodeNeedComponentImplL( name );
        }
    CleanupStack::PopAndDestroy( componentFactories );
    if ( componentNodeImplNeeded )
        {
        CXnType* type = CXnType::NewL( name );
        CleanupStack::PushL( type );
        CXnComponentNodeImpl* impl = CXnComponentNodeImpl::NewL( type );
        CleanupStack::Pop( type );
        CleanupStack::PushL( impl );
        returnValue = CXnNode::NewL();
        returnValue->SetComponentNodeImpl( impl );
        CleanupStack::Pop( impl );
        }
    else
        {
        if ( name == KActionsNodeName ||
             name == KActionNodeName ||
             name == KEventNodeName ||
             name == KTriggerNodeName ||
             name == KMenuNodeName ||
             name == KMenuItemNodeName ||
             name == KPropertyNodeName ||
             name == KSoftkeyNodeName )
            {
            return ConstructGeneralNodeL( aSource );
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// ConstructKnownBuiltinNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* ConstructKnownBuiltinNodeL( CXnDomNode& aSource )
    {
    CXnNode* returnValue = NULL;
    const TDesC8& name = aSource.Name();
    if ( name == KMenuNodeName ||
         name == KMenuItemNodeName ||
         name == KPropertyNodeName ||
         name == KSoftkeyNodeName ||
         name == KTriggerNodeName )
        {
        return ConstructGeneralNodeL( aSource );
        }
    CXnType* type = CXnType::NewL( name );
    CleanupStack::PushL( type );
    if ( name == KViewsNodeName )
        {
        CXnViewsNodeImpl* impl = CXnViewsNodeImpl::NewL( type );
        CleanupStack::Pop( type );
        CleanupStack::PushL( impl );
        returnValue = CXnNode::NewL();
        returnValue->SetViewsNodeImpl( impl );
        CleanupStack::Pop( impl );
        }
    else if ( name == KViewNodeName )
        {
        CXnViewNodeImpl* impl = CXnViewNodeImpl::NewL( type );
        CleanupStack::Pop( type );
        CleanupStack::PushL( impl );
        returnValue = CXnNode::NewL();
        returnValue->SetViewNodeImpl( impl );
        CleanupStack::Pop( impl );
        }
    else if ( name == KBoxNodeName ||
              name == KButtonNodeName ||
              name == KImageNodeName ||
              name == KMenuBarNodeName ||
              name == KTooltipNodeName ||
              name == KMarqueeNodeName ||
              name == KTextNodeName ||
              name == KObjectNodeName ||
              name == KTracksterNodeName ||
              name == KNewstickerNodeName ||
              name == KSliderNodeName ||
              name == KClockNodeName ||
              name == KClockAnalog ||
              name == KClockDigital ||
              name == KClockFace ||
              name == KClockFace12 ||
              name == KClockFace24 ||
              name == KClockDate ||
              name == KClockAmpm ||
              name == KClockDay ||
              name == KStylusPopupNodeName ||
              name == KPopUpNodeName ||
              name == KWidgetExtensionNodeName ||
              name == KTextEditorNodeName )
        {
        CXnComponentNodeImpl* impl = CXnComponentNodeImpl::NewL( type );
        CleanupStack::Pop( type ) ;
        CleanupStack::PushL( impl );
        returnValue = CXnNode::NewL();
        returnValue->SetComponentNodeImpl( impl );
        CleanupStack::Pop( impl );
        }
    else
        {
        CleanupStack::PopAndDestroy( type );

        if ( name == KVolumeControlNodeName ||
             name == KWidgetNodeName ||
             name == KPluginNodeName )
            {
            CXnType* type = CXnType::NewL( KBoxNodeName );
            CleanupStack::PushL( type );
            CXnComponentNodeImpl* impl = CXnComponentNodeImpl::NewL( type );
            CleanupStack::Pop( type );
            CleanupStack::PushL( impl );
            returnValue = CXnNode::NewL();
            returnValue->SetComponentNodeImpl( impl );
            CleanupStack::Pop( impl );
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// ConstructGeneralNodeL
// -----------------------------------------------------------------------------
//
static CXnNode* ConstructGeneralNodeL( CXnDomNode& aSource )
    {
    CXnType* type = CXnType::NewL( aSource.Name() );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    CleanupStack::PushL( impl );
    CXnNode* tmpNode = CXnNode::NewL();
    tmpNode->SetImpl( impl );
    CleanupStack::Pop( impl );
    return tmpNode;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnODTParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnODTParser* CXnODTParser::NewL( CXnViewManager& aManager,
    CXnEcomHandler& aEcomHandler )
    {
    CXnODTParser* self = new ( ELeave ) CXnODTParser( aManager, aEcomHandler ); 
            
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnODTParser::CXnODTParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnODTParser::CXnODTParser( CXnViewManager& aManager,
    CXnEcomHandler& aEcomHandler )
    : iManager( aManager ), iUiEngine( aManager.UiEngine() ),
      iEcomHandler( aEcomHandler )
    {
    }

// -----------------------------------------------------------------------------
// CXnODTParser::~CXnODTParser
// Destructor.
// -----------------------------------------------------------------------------
//
CXnODTParser::~CXnODTParser()
    {
    }

// -----------------------------------------------------------------------------
// CXnODTParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnODTParser::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnODTParser::LoadRootL
// -----------------------------------------------------------------------------
//
void CXnODTParser::LoadRootL( CXnRootData& aRootData, TUid /*aAppUid*/ )        
    {    
    CXnDomNode* root( aRootData.Owner() );
    
    CXnDomStringPool* sp( root->StringPool() );
    
    CXnAppUiAdapter& appui( iManager.AppUiAdapter() );
    
    // Build root
    CXnNode* node( BuildRootNodeL( iUiEngine ) );
    
    // Let root nodes to know each other
    CXnDomNode* dom( aRootData.ODT()->DomDocument().RootNode() );    
    dom->SetLayoutNode( node );
    node->SetDomNode( dom );
        
    CreateNodesL( root, *sp, aRootData );

    // root doesn't have any controls   
    
    aRootData.SetOccupied();
    }

// -----------------------------------------------------------------------------
// CXnODTParser::LoadViewL
// -----------------------------------------------------------------------------
//
void CXnODTParser::LoadViewL( CXnViewData& aViewData )
    {    
    // <view> element
    CXnDomNode* view( aViewData.Node() );
    
    CXnDomStringPool* sp( view->StringPool() );

    // from <view> element
    CreateNodesL( view, *sp, aViewData );       
    CreateControlsL( view, aViewData );

    // By default make controls invisible
    CXnControlAdapter* adapter( view->LayoutNode()->Control() );
    
    if( adapter )
        {
        adapter->MakeVisible( EFalse );
        }    
        
    aViewData.SetOccupied();
    }

// -----------------------------------------------------------------------------
// CXnODTParser::DestroyView
// -----------------------------------------------------------------------------
//
void CXnODTParser::DestroyView( CXnViewData& aViewData )
    {
    CXnNode* view( aViewData.Node()->LayoutNode() );
    
    // Destroy view and all its child nodes
    CXnNode* parent( view->Parent() );
    
    RPointerArray< CXnNode >& children( parent->Children() );
    
    TInt index( children.Find( view ) );
    
    if ( index != KErrNotFound )
        {
        children.Remove( index );
        
        delete view;
        view = NULL;
        
        CXnDomNode* owner( aViewData.Owner() );
        owner->DeleteChild( aViewData.Node() );
        }    
    }

// -----------------------------------------------------------------------------
// CXnODTParser::LoadWidgetL
// -----------------------------------------------------------------------------
//
void CXnODTParser::LoadWidgetL( CXnPluginData& aPluginData )
    {   
    if ( aPluginData.Empty() )
        {
        // Holds "empty" widget
        return;
        }
    
    // <widget> element
    CXnDomNode* widget( aPluginData.Node() );
    
    CXnDomStringPool* sp( widget->StringPool() );

    // from <widget> element 
    CreateNodesL( widget, *sp, aPluginData );           
    CreateControlsL( widget, aPluginData );    

    HandleWidgetBackgroundL( widget->LayoutNode() );

    // By default make controls invisible
    CXnControlAdapter* adapter( widget->LayoutNode()->Control() );
    
    if( adapter )
        {
        adapter->MakeVisible( EFalse );
        }   
    
    aPluginData.SetOccupied();
    }

// -----------------------------------------------------------------------------
// CXnODTParser::DestroyWidget
// -----------------------------------------------------------------------------
//
void CXnODTParser::DestroyWidgetL( CXnPluginData& aPluginData )
    {
    CXnViewData& parent( 
        static_cast< CXnViewData& >( *aPluginData.Parent() ) );
    
    // Check is any of plugin data's controls in views draw chain and remove
    CXnControlAdapter* control( parent.Node()->LayoutNode()->Control() );

    RPointerArray< CXnControlAdapter >& controls( control->ChildAdapters() );

    RPointerArray< CXnControlAdapter > pluginControls;
    CleanupClosePushL( pluginControls );

    aPluginData.ControlsL( pluginControls );

    // Remove from view chain if found
    for ( TInt i = 0; i < pluginControls.Count(); i++ )
        {
        CXnControlAdapter* control( pluginControls[i] );

        TInt index( controls.Find( control ) );

        if ( index != KErrNotFound )
            {
            controls.Remove( index );
            }
        }

    CleanupStack::PopAndDestroy( &pluginControls );
    
    CXnNode* widgetNode( aPluginData.Node()->LayoutNode() );
        
    CXnNode* owner( widgetNode->Parent() );
    
    CXnControlAdapter* widgetControl( widgetNode->Control() );
    
    RPointerArray< CXnControlAdapter >& 
        childAdapters( owner->Control()->ChildAdapters() );
    
    childAdapters.Remove( childAdapters.Find( widgetControl ) ); 
    
    childAdapters.Compress();
    
    RPointerArray< CXnNode >& children( owner->Children() );
    
    TInt index( children.Find( widgetNode ) );
    
    // Destroy widget and all its child nodes
    if ( index != KErrNotFound )
        {
        children.Remove( index );
        
        delete widgetNode;
        widgetNode = NULL;
       
        CXnDomNode* owner( aPluginData.Owner() );
        owner->DeleteChild( aPluginData.Node() );
        }       
    }

// -----------------------------------------------------------------------------
// CXnODTParser::CreateNodesL
// Creates node tree
// -----------------------------------------------------------------------------
//
void CXnODTParser::CreateNodesL(
    CXnDomNode* aSourceNode,
    CXnDomStringPool& aSp,    
    CXnPluginData& aPluginData )
    {
    if ( aSourceNode )
        {
        ConstructNodeL( *aSourceNode, aSp, aPluginData );
                     
        CXnDomList& childList( aSourceNode->ChildNodes() );

        TInt count( childList.Length() );

        for ( TInt i = 0; i < count; ++i )
            {
            CreateNodesL(
                static_cast< CXnDomNode* >( childList.Item( i ) ),
                aSp, aPluginData );
            }                              
        }
    }

// -----------------------------------------------------------------------------
// CXnODTParser::ConstructNodeL
// Creates layout node and control for it
// -----------------------------------------------------------------------------
//
void CXnODTParser::ConstructNodeL(
    CXnDomNode& aSource,
    CXnDomStringPool& aSp,    
    CXnPluginData& aPluginData )
    {
    const TDesC8& name( aSource.Name() );
    
    if ( name == KActionsNodeName ||
         name == KActionNodeName ||
         name == KEventNodeName ||
         name == KXMLUIMLNodeName ||
         name == KAttributeNodeName ||
         name == KItemNodeName )
        {
        return;
        }

    if ( name == KTriggerNodeName )
        {
        if ( !CreateTriggerInLayoutNodeTree( aSource ) )
            {
            return;
            }
        }

    const TDesC8& parentName( aSource.Parent()->Name() );

    if ( name == KPropertyNodeName &&
         ( parentName == KTriggerNodeName ||
           parentName == KEventNodeName ||
           parentName == KPropertyNodeName ||
           parentName == KAttributeNodeName ) )
        {
        return;
        }

    CXnNode* node( ConstructKnownBuiltinNodeL( aSource ) ); 

    if ( !node )
        {
        node = ConstructNodeFromPluginL( iEcomHandler, aSource );
        
        if ( !node )
            {
            node = ConstructGeneralNodeL( aSource );
            
            if ( !node )
                {
                return;
                }            
            }
        }

    if ( name == KMenuBarNodeName ||
         name == KListQueryDialogNodeName ||
         name == KMenuNodeName ||
         name == KMenuItemNodeName ||
         name == KDynMenuItemNodeName ||
         name == KWidgetMenuItemNodeName ||
         name == KPropertyNodeName ||
         name == KDescNodeName ||
         name == KTextZoomNodeName ||
         name == KConfigurationNodeName ||
         name == KSoftkeyNodeName ||
         name == KTriggerNodeName ||
         name == KSettingsConfigurationNodeName ||
         name == KContentSourceNodeName ||
         name == KMenuExtensionNodeName ||
         name == KStylusPopupNodeName ||
         name == KActionsHandlerNodeName )         
        {
        node->SetLayoutCapable( EFalse );
        }

    CXnNode* parent( FindAncestorWhoActAsParent( aSource ) );

    if( !parent )
        {
        delete node;
        
        return;
        }
    
    parent->AddChildL( node );

    if ( !parent->IsLayoutCapable() )
        {
        node->SetLayoutCapable( EFalse );
        }
    
    aSource.SetLayoutNode( node );
    node->SetDomNode( &aSource );

    node->SetUiEngine( iUiEngine );

    CopyPropertiesFromDomNodeToNodeL( *node, aSource, aSp );
           
    if ( name == KTriggerNodeName )
        {
        if ( iUiEngine.IsEditMode() )
            {
            node->SetStateL( XnPropertyNames::style::common::KEdit );
            }
        }
            
    // Mark adaptive layoutable nodes
    TInt adaptive( XnAdaptive::ENone );

    CXnProperty* width( node->WidthL() );

    if ( width )
        {
        const TDesC8& w( width->StringValue() );

        if ( w == XnPropertyNames::style::common::KAdaptive )
            {
            adaptive = XnAdaptive::EWidth;
            }
        }

    CXnProperty* height( node->HeightL() );

    if ( height )
        {
        const TDesC8& h( height->StringValue() );

        if ( h == XnPropertyNames::style::common::KAdaptive )
            {
            adaptive |= XnAdaptive::EHeight;
            }
        }

    if ( adaptive != XnAdaptive::ENone )
        {
        node->SetAdaptiveL( adaptive );
        }
       
    // Collect nodes, which nav-index property is 'appearance' to
    // an array
    CXnProperty* navindex( node->NavIndexL() );
    
    if ( navindex &&
         navindex->StringValue() == XnPropertyNames::style::common::KAppearance )
        {
        aPluginData.SetAppearanceNodeL( node );
        }
    
    // Collect initial focus nodes.
    CXnProperty* initialFocusProp( node->InitialFocusL() );
    
    if ( initialFocusProp )
        {
        aPluginData.SetInitialFocusNodeL( node );
        }
    }

// -----------------------------------------------------------------------------
// CXnODTParser::CreateControlsL()
// Run control creation 1st phase
// -----------------------------------------------------------------------------
//
void CXnODTParser::CreateControlsL( CXnDomNode* aNode,
    CXnPluginData& aPluginData )
    {
    if ( aNode )
        {
        ConstructControlL( aNode->LayoutNode(), aPluginData );
        
        CXnDomList& childList( aNode->ChildNodes() );

        TInt count( childList.Length() );

        for ( TInt i = 0; i < count; ++i )
            {
            CreateControlsL(
                static_cast< CXnDomNode* >( childList.Item( i ) ), aPluginData );                
            }                              
        }
    }

// -----------------------------------------------------------------------------
// CXnODTParser::ConstructControlL()
// Run control creation 1st phase
// -----------------------------------------------------------------------------
//
void CXnODTParser::ConstructControlL( CXnNode* aNode,
    CXnPluginData& aPluginData )
    {
    if( !aNode )
        {
        return;
        }
    
    CXnComponentNodeImpl* impl( aNode->ComponentNodeImpl() );
    CXnViewNodeImpl* viewImpl( aNode->ViewNodeImpl() );

    TBool needsCreation( EFalse );

    if ( ( impl && impl->DoesComponentNeedCreation() ) || viewImpl )
        {
        needsCreation = ETrue;
        }

    const TDesC8& name( aNode->Type()->Type() );

    if ( needsCreation )
        {
        // Try if builtin control can be created
        if ( !CreateBuiltInControlL( *aNode, name ) )
            {
            // Not builtin, invoke factory chain
            CreateFactoryControlL( *aNode, name );
            }

        // Fill plugin data
        aPluginData.SetControlL( aNode );
        }

    if ( name == KContentSourceNodeName )
        {
        aPluginData.SetContentSourceNodeL( aNode );
        }
    }

// -----------------------------------------------------------------------------
// CXnODTParser::CreateExternalControlL()
// Creates external control
// -----------------------------------------------------------------------------
//
TBool CXnODTParser::CreateExternalControlL( CXnNode& aNode,
    const TDesC8& aName )
    {
    CXnExtRenderingPluginAdapter* adapter( NULL );
    
    // Get implementations via ECom
    RImplInfoPtrArray plugins;
    CleanupResetAndDestroyPushL( plugins );

    REComSession::ListImplementationsL( 
    	TUid::Uid( AI3_EXTERNAL_RENDERING_PLUGIN_ECOM_UID ), plugins );

    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CImplementationInformation* information( plugins[i] );

        if( information->DataType().Compare( aName ) == 0 )
            {
            adapter = CXnExtRenderingPluginAdapter::NewL( 
            	information->ImplementationUid() );
            break;
            } 
        }
    CleanupStack::PopAndDestroy(); // plugins
    
    if( adapter )
        {
        CXnComponent* component = CXnComponent::NewL();
        CleanupStack::PushL( component );

        CXnControlAdapter* parentAdapter( NULL );

        // Find parent control
        CXnNode* parent( aNode.Parent() );

        for ( ; parent; parent = parent->Parent() )
            {
            parentAdapter = parent->Control();

            if ( parentAdapter )
                {
                break;
                }
            }
        
        if( parentAdapter )
            {
            CXnExtRenderingPluginWrapper* wrapper = 
                CXnExtRenderingPluginWrapper::NewL( aNode.PluginIfL(), *adapter );
            CleanupStack::PushL( wrapper );
    
            parentAdapter->AppendChildL( *wrapper, aNode );
            CleanupStack::Pop( wrapper );
            
            component->SetControlAdapter( wrapper );
    
            aNode.ComponentNodeImpl()->SetComponent( component );
    
            component->SetNode( aNode.PluginIfL() );
    
            wrapper->SetComponent( component );
    
            wrapper->SetComponentsToInheritVisibility( ETrue );
    
            wrapper->ActivateL();
            
            CleanupStack::Pop( component );
    
            return ETrue;
            }
        else
            {
            CleanupStack::Pop( component );
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }
    }

// CXnODTParser::CreateFactoryControlL()
// Creates factory control
// -----------------------------------------------------------------------------
//
void CXnODTParser::CreateFactoryControlL( CXnNode& aNode,
    const TDesC8& aName )
    {
    CXnArray* factories = CXnArray::NewL();
    CleanupStack::PushL( factories );

    TUid interfaceUid = { AI3_RENDERING_PLUGIN_ECOM_FACTORY_UID };

    iEcomHandler.PluginsL( interfaceUid, aName, *factories );

    CXnComponent* component( NULL );

    TInt count( factories->Container().Count() );
    
    // Invoke factory chain
    for ( TInt i = 0; i < count; i++ )
        {
        MXnComponentFactory* factory =
            static_cast< MXnComponentFactory* >(
                ( factories->Container() )[i] );
                 
        if ( factory->CreateXnComponentL( aNode.PluginIfL(), component ) == 
            MXnComponentFactory::EXnFactoryResponseComponentConstructed )
             
            {
            // All done
            break;
            }
        }

    CleanupStack::PopAndDestroy( factories );
    }

// -----------------------------------------------------------------------------
// CXnODTParser::CreateBuiltInControlL()
// Creates builtin control
// -----------------------------------------------------------------------------
//
TBool CXnODTParser::CreateBuiltInControlL( CXnNode& aNode,
    const TDesC8& aName )
    {
    if ( aName != KBoxNodeName && aName != KButtonNodeName &&
         aName != KStylusPopupNodeName && aName != KScrollableBoxNodeName&& 
         aName != KWidgetExtensionNodeName && 
         aName != KPopUpNodeName && aName != KViewNodeName &&
		 aName != XnPropertyNames::listquerydialog::KListQueryDialog )         
        {
        return CreateExternalControlL( aNode, aName );
        }

    CXnComponent* component = CXnComponent::NewL();
    CleanupStack::PushL( component );

    CXnControlAdapter* parentAdapter( NULL );

    CXnProperty* position( aNode.PositionL() );

    if ( position )
        {
        const TDesC8& value( position->StringValue() );

        if ( value == XnPropertyNames::style::common::position::KFloating )
            {
            // Find view node and append floating control to its compound
            for ( CXnNode* node = &aNode; node; node = node->Parent() )
                {
                if ( node->ViewNodeImpl() )
                    {
                    parentAdapter = node->Control();

                    break;
                    }
                }
            }
        }

    if ( !parentAdapter )
        {
        // Find parent control
        CXnNode* parent( aNode.Parent() );

        for ( ; parent; parent = parent->Parent() )
            {
            parentAdapter = parent->Control();

            if ( parentAdapter )
                {
                break;
                }
            }
        }
    
    CXnControlAdapter* adapter( NULL );

    if( aName == KViewNodeName )
        {
        adapter = CXnViewControlAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );
        }
    else if( aName == KStylusPopupNodeName )
        {
        adapter = CXnPopupControlAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );
        }
    else if( aName == KWidgetExtensionNodeName ||
             aName == KPopUpNodeName )
        {               
        adapter = CXnWidgetExtensionAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );        
        CXnProperty* prop = CXnProperty::NewL( 
            XnPropertyNames::style::common::KPosition,
            XnPropertyNames::style::common::position::KFloating,
            CXnDomPropertyValue::EString, 
            *aNode.UiEngine()->ODT()->DomDocument().StringPool() );
        CleanupStack::PushL( prop );
        aNode.SetPropertyL( prop );                    
        CleanupStack::Pop( prop );                                                        
        }
    else if( aName == KScrollableBoxNodeName )
        {               
        adapter = CXnScrollableControlAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );
        }
    else if( aName == XnPropertyNames::listquerydialog::KListQueryDialog )
        {
        CleanupStack::PopAndDestroy( component );
        CXnListQueryDialog* listQuery = CXnListQueryDialog::NewL();
        component = static_cast<CXnComponent*>( listQuery );
        CleanupStack::PushL( component );
        adapter = CXnListQueryDialogAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );
        }
    else
        {
        adapter = CXnControlAdapter::NewL( aNode.PluginIfL() );
        CleanupStack::PushL( adapter );
        }
    
    if ( parentAdapter )
        {
        parentAdapter->AppendChildL( *adapter, aNode );
        }
    
    component->SetControlAdapter( adapter ); // ovnership transferred
    CleanupStack::Pop( adapter );

    __ASSERT_DEBUG( adapter, User::Leave( KErrGeneral ) );
    
    if( aName == KViewNodeName )
        {
        aNode.ViewNodeImpl()->SetComponent( component );
        }
    else
        {
        aNode.ComponentNodeImpl()->SetComponent( component );
        }

    component->SetNode( aNode.PluginIfL() );

    adapter->SetComponent( component );

    adapter->SetComponentsToInheritVisibility( ETrue );

    adapter->ActivateL();
    
    CleanupStack::Pop( component );

    return ETrue;
    }

// --------------------------------------------------------------------------
// CXnODTParser::HandleWidgetBackgroundL()
//
// --------------------------------------------------------------------------
//
void CXnODTParser::HandleWidgetBackgroundL( CXnNode* aWidgetNode )
    {
    if( aWidgetNode )
        {
        CXnUiEngine* uiengine = aWidgetNode->UiEngine();
        CXnProperty* bgImageProp = aWidgetNode->BackgroundImageL();
        CXnProperty* bgColorProp = aWidgetNode->BackgroundColorL();
        
        // Set default bg color if it has not been defined by a widget
        if( !bgImageProp && !bgColorProp && uiengine )
            {
            CXnDomPropertyValue* value = CXnDomPropertyValue::NewL(
                uiengine->ODT()->DomDocument().StringPool() );
            CleanupStack::PushL( value );
            value->SetStringValueL( CXnDomPropertyValue::EString, KWidgetBgSkinId );
        
            CXnProperty* bgColor = CXnProperty::NewL(
                XnPropertyNames::appearance::common::KBackGroundColor,
                value, *uiengine->ODT()->DomDocument().StringPool() );
        
            CleanupStack::Pop( value );
            CleanupStack::PushL( bgColor );
            aWidgetNode->SetPropertyL( bgColor );
            CleanupStack::Pop( bgColor );
            }
        }
    }

// End of file

