/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The engine class of MCS plugin. 
*
*/

// System includes
#include <gulicon.h>
#include <gslauncher.h>
#include <mcsmenufilter.h>
#include <mcsmenuoperation.h>
#include <mcsmenuiconutility.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <mcspluginres.rsg>
#include <apgcli.h> 
#include <apacmdln.h>
#include <gfxtranseffect/gfxtranseffect.h>      
#include <akntranseffect.h>
#include <schemehandler.h>
#include <viewcli.h>                // For CVwsSessionWrapper
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif
#include <aisystemuids.hrh>

// User includes
#include "carouselpluginengine.h"
#include "carouselplugin.h"
#include "carouselplugindata.h"
#include "carouselpluginuids.hrh"

// Constants
_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KSkin,         "skin" );
_LIT( KMif,          "mif" );
_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "mcspluginres.rsc" );
_LIT( KResPath, "\\resource\\" );
_LIT( KMMApplication, "mm://" );
_LIT( KHideExit2, "&exit=hide" );
_LIT( KSetFocusString, "!setfocus?applicationgroup_name=" );
_LIT( KApplicationGroupName, "applicationgroup_name" );
_LIT( KIcon, "icon" );
_LIT( KMenuAttrUndefUid, "0x99999991" );
_LIT( KMenuIconFile, "aimcsplugin.mif" );
_LIT( KMenuBookmarkIconId, "16386" );
_LIT( KMenuBookmarkMaskId, "16387" );
_LIT( KMenuMailboxIconId, "16388" );
_LIT( KMenuMailboxMaskId, "16389" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );
_LIT( KPrefix, "0x" );

const TUid KHomescreenUid = { AI_UID3_AIFW_COMMON };
const TUid KMMUid = { 0x101F4CD2 };
const TUid KMCSCmailUidValue = { 0x2001E277 };
const TUid KMCSCmailMailboxViewIdValue = { 0x2 };
const TUid KBrowserUid = { 0x10008D39 };

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// NextIdToken
// ----------------------------------------------------------------------------
//
static TPtrC NextIdToken( TLex& aLexer )
   {
   aLexer.SkipSpace();
   aLexer.Mark();
  
   while( !aLexer.Eos() && !aLexer.Peek().IsSpace() && aLexer.Peek() != ')' )
       {
       aLexer.Inc();
       }
   
   return aLexer.MarkedToken();
   }

// ----------------------------------------------------------------------------
// Shows note dailog, with the given resource.
// ----------------------------------------------------------------------------
//
static void ShowNoteDlgL( TInt aResource )
    {
    HBufC* temp = StringLoader::LoadLC( aResource );
    
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
    CAknNoteDialog::EConfirmationTone,
    CAknNoteDialog::ENoTimeout );
    CleanupStack::PushL( dialog );
    dialog->SetTextL( temp->Des() );
    dialog->ExecuteDlgLD( R_MCS_DISABLE_OPEN_ITEM_DLG );
    CleanupStack::Pop( dialog );
    CleanupStack::PopAndDestroy( temp );
    }

// ----------------------------------------------------------------------------
// Parses uid in Hexadecimal format from the given string.
// ----------------------------------------------------------------------------
//
TUid ParseHexUidFromString(const TDesC& aUidString )
    {
    TUid uid( KNullUid ); 
    const TInt pos( aUidString.FindF( KPrefix ) );
    
    if ( pos != KErrNotFound )
        {
        TLex lex( aUidString.Mid( pos + KPrefix().Length() ) );
        
        // Hex parsing needs unsigned int
        TUint32 value( 0 );
        const TInt parseResult( lex.Val( value, EHex ) );
        
        if ( parseResult == KErrNone )
            {
            TInt32 value32( value );
            uid.iUid = value32;   
            }
        }
    return uid;
    }

// ----------------------------------------------------------------------------
// Start transition effect. User has launched the application with the given uid.
// ----------------------------------------------------------------------------
//
void StartEffect( TUid aUid )
    {
    //start a full screen effect
    GfxTransEffect::BeginFullScreen( 
        AknTransEffect::EApplicationStart,
        TRect(), 
        AknTransEffect::EParameterType, 
        AknTransEffect::GfxTransParam( aUid,
        AknTransEffect::TParameter::EActivateExplicitContinue ));
    }

// ============================ MEMBER FUNCTIONS ===============================
// ----------------------------------------------------------------------------
// CMCSPluginEngine::CMCSPluginEngine
//
// ----------------------------------------------------------------------------
//
CCarouselPluginEngine::CCarouselPluginEngine( CCarouselPlugin& aPlugin, 
    const TDesC8& aInstanceUid ) 
    : iPlugin( aPlugin ), iInstanceUid( aInstanceUid )    
    {
    }
    
// ----------------------------------------------------------------------------
// CMCSPluginEngine::NewL
// 
// ----------------------------------------------------------------------------
//
CCarouselPluginEngine* CCarouselPluginEngine::NewL( CCarouselPlugin& aPlugin, 
    const TDesC8& aInstanceUid )
    {
    CCarouselPluginEngine* self = 
        new( ELeave ) CCarouselPluginEngine( aPlugin, aInstanceUid );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMCSPluginEngine::ConstructL
//
// ----------------------------------------------------------------------------
//
void CCarouselPluginEngine::ConstructL()
    {
    TFullName resourceFile( KResourceDrive );
    resourceFile.Append( KResPath );
    resourceFile.Append( KResourceFile );
    BaflUtils::NearestLanguageFile(
        CCoeEnv::Static()->FsSession(),
        resourceFile );
    iResourceOffset =
        CCoeEnv::Static()->AddResourceFileL( resourceFile );
    InitL();
    StartObservingL();
    
    // Get "Undefined" item
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );
    filter->HaveAttributeL( KMenuAttrUid, KMenuAttrUndefUid );
    iUndefinedItemHeader = FindMenuItemL( *filter );
    CleanupStack::PopAndDestroy( filter );
    iUndefinedItem = CMenuItem::OpenL( iMenu, iUndefinedItemHeader );    
    }

// ----------------------------------------------------------------------------
// CMCSPluginEngine::~CMCSPluginEngine
//
// ----------------------------------------------------------------------------
//
CCarouselPluginEngine::~CCarouselPluginEngine()
    {
    StopObserving();

    delete iPluginData;

    // Notifier close has to be before iMenu close
    iNotifier.Close();
    iMenu.Close();
    delete iWatcher;
    delete iNotifyWatcher;

    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );

    delete iUndefinedItem;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::InitL
//
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::InitL()
    {
    iMenu.OpenL( KMyMenuData );
    iPluginData = CCarouselPluginData::NewL( *this, iInstanceUid );
    iWatcher = CCarouselPluginWatcher::NewL( CCarouselPluginWatcher::EOperation );
    
    TInt err( iNotifier.Open( iMenu ) ); 
    
    if ( err == KErrNone )
        {
        iNotifyWatcher = CCarouselPluginWatcher::NewL( CCarouselPluginWatcher::ENotify );
        
        iNotifier.Notify( 0,
            RMenuNotifier::EItemsAddedRemoved,
            iNotifyWatcher->iStatus );
        iNotifyWatcher->WatchNotify( this );
        }
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::StartObservingL
//
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::StartObservingL()
    {
    // registering to mailbox db. changes observing
    iMsvSession = CMsvSession::OpenAsObserverL( *this );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::StopObserving
//
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::StopObserving()
    {       
    delete iMsvSession;
    iMsvSession = NULL;    
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::MenuDataL
//
// ---------------------------------------------------------------------------
//
CCarouselData& CCarouselPluginEngine::MenuDataL( const TInt& aIndex )
    {
    return iPluginData->DataItemL( aIndex );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::MenuItemCount
//
// ---------------------------------------------------------------------------
//
TInt CCarouselPluginEngine::MenuItemCount()
    {
    return iPluginData->DataCount();
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::FindMenuItemL
// Returns the menu item header, which matches the given filter.
// ---------------------------------------------------------------------------
//
TMenuItem CCarouselPluginEngine::FindMenuItemL( CMenuFilter& aFilter )
    {    
    TMenuItem item;
    const TInt root( iMenu.RootFolderL() );
    
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    
    iMenu.GetItemsL( items, root, &aFilter, ETrue );
    
    if ( items.Count() > 0 )
        {
        item = items[0];
        }

    CleanupStack::PopAndDestroy( &items );
    
    return item;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::FetchMenuItemL
// Returns the actual menu item for the given header.
// ---------------------------------------------------------------------------
//
CMenuItem* CCarouselPluginEngine::FetchMenuItemL( CCarouselData& aData )
    {
    CMenuItem* item = NULL;
    
    TRAP_IGNORE( item = CMenuItem::OpenL( iMenu, aData.MenuItem().Id() ) );
    
    if( item )
        {
        return item; 
        }
    
    if( aData.MenuItem().Type() == KMenuTypeUrl )
        {
        item = CreateBkmItemL( aData );
        }
    else if( aData.MenuItem().Type() == KMenuTypeMailbox )
        {
        item = CreateMailboxItemL( aData);
        }
    
    return item;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMenuItem* CCarouselPluginEngine::CreateBkmItemL( CCarouselData& aData )
    {
    CMenuItem* item( NULL );
    if( aData.MenuItem().Id() != KErrNotFound )
        {        
        item = CMenuItem::CreateL( iMenu, KMenuTypeUrl, 0, 0 );
        CleanupStack::PushL( item );
        item->SetAttributeL( KMenuAttrLongName, aData.Name() );
        item->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
        item->SetAttributeL( KMenuAttrIconId, KMenuBookmarkIconId );
        item->SetAttributeL( KMenuAttrMaskId, KMenuBookmarkMaskId );
        CleanupStack::Pop( item );
        }
    return item;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMenuItem* CCarouselPluginEngine::CreateMailboxItemL( CCarouselData& aData )
    {
    CMenuItem* item( NULL );
    if( aData.MenuItem().Id() != KErrNotFound )
        {   
        item = CMenuItem::CreateL( iMenu, KMenuTypeMailbox, 0, 0 );
        CleanupStack::PushL( item );
        item->SetAttributeL( KMenuAttrLongName, aData.Name() );
        item->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
        item->SetAttributeL( KMenuAttrIconId, KMenuMailboxIconId );
        item->SetAttributeL( KMenuAttrMaskId, KMenuMailboxMaskId );
        CleanupStack::Pop( item );
        }
    return item;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ItemIconL
// Returns MCS default icon if attribute is 'icon' else parses the
// skin definition from attribute and sets attributes to aMenuItem.
// ---------------------------------------------------------------------------
//
CGulIcon* CCarouselPluginEngine::ItemIconL( CMenuItem* aMenuItem, 
    const TDesC& aAttr )
    {
    
    CMenuItem* menuItem;
  
    // check if item exists in MCS
    if ( aMenuItem )
        { 
        TUint32 flags = aMenuItem->Flags();
        TUint32 isHidden = flags & TMenuItem::EHidden;
        TUint32 isMissing = flags & TMenuItem::EMissing;

        if ( iUndefinedItem && ( isHidden || isMissing ) )
            {
            menuItem = iUndefinedItem;
            }
        else
            {
            menuItem = aMenuItem;
            }
        }
    else 
        {
        // item not found in MCS. Use Undefined Icon
        menuItem = iUndefinedItem;
        }

    CAknIcon* icon( NULL );
    CGulIcon* gIcon( NULL );
    TBool exists( ETrue );
    
    if ( aAttr != KIcon )
        {
        // Resolve secondary icon definition from attribute
        TPtrC iconDef( menuItem->GetAttributeL( aAttr, exists ) );
    
        if ( exists )
            {
            exists = ConstructMenuItemForIconL( iconDef, *menuItem );
            }
        }
    
    if ( exists )
        {
        icon = MenuIconUtility::GetItemIconL( *menuItem );
    
        if ( icon )
            {
            CleanupStack::PushL( icon );
            
            gIcon = CGulIcon::NewL( icon->Bitmap(), icon->Mask() );
            
            // Detach and delete
            icon->SetBitmap( NULL );
            icon->SetMask( NULL );    
            
            CleanupStack::PopAndDestroy( icon );
            }
        }
    
    return gIcon;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ItemTextL
// Returns text string for the given attribute
// ---------------------------------------------------------------------------
//
TPtrC CCarouselPluginEngine::ItemTextL( CMenuItem* aMenuItem, const TDesC& aAttr )
    {

    CMenuItem* menuItem;

    // check if item exists in MCS
    if ( aMenuItem )
        {
        TUint32 flags = aMenuItem->Flags();
        TUint32 isHidden = flags & TMenuItem::EHidden;
        TUint32 isMissing = flags & TMenuItem::EMissing;

        // if item is hidden or missing (mmc card removed)
        // use "Undefined" text instead
        if ( iUndefinedItem && ( isHidden || isMissing ) )
            {
            menuItem = iUndefinedItem;
            }
        else
            {
            menuItem = aMenuItem;
            }
        }
    else 
        {
        // item not found in MCS. Use "Undefined" text
        menuItem = iUndefinedItem;
        }
    
    TBool exists( KErrNotFound );
    
    TPtrC name( menuItem->GetAttributeL( aAttr, exists ) );
    
    if ( exists )
        {
        return name;
        }

    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::LaunchItemL
// Calls the open command for the given menu item header
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::LaunchItemL( const TInt& aIndex )
    {
    if ( iBackupRestore )
        {
        ShowNoteDlgL( R_MCS_DISABLE_OPEN_ITEM ); 
        return;
        }

    CCarouselData& dataItem( iPluginData->DataItemL( aIndex ) );
    // run item based on its type
    TPtrC type( dataItem.MenuItem().Type());

    // run folder
    if ( type == KMenuTypeFolder )
        {
        LaunchFolderItemL( dataItem );
        }
    else if( type == KMenuTypeUrl )
        {
        LaunchBookmarkItemL( dataItem );
        }
    else if( type == KMenuTypeMailbox )
        {
        LaunchMailboxItemL( dataItem );
        }
    else
        {
        LaunchMCSItemL( dataItem );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::LaunchFolderItemL( CCarouselData& aData )
    {
    CMenuItem* item = NULL;
    TRAP_IGNORE( item = CMenuItem::OpenL( iMenu, aData.MenuItem().Id() ) );
    
    // item does not exist at all in MCS
    if ( item == NULL )
        {
        ShowNoteDlgL( R_MCS_DISABLE_OPEN_ITEM_MISSING );
        return;
        }
    
    CleanupStack::PushL( item );
    
    StartEffect( KMMUid );
    
    // message for MM application
    HBufC8* message; 

    // prepare message for launching folder
    TBool hasApplicationGroupName( EFalse );
    
    TPtrC applicationGroupName( item->GetAttributeL(
        KApplicationGroupName, hasApplicationGroupName ) );
                                                      
    if ( !hasApplicationGroupName )
        {
        return;
        }
    
    message = HBufC8::NewLC( KMMApplication().Length() + 
                             KSetFocusString().Length() +
                             applicationGroupName.Length() + 
                             KHideExit2().Length() );

    message->Des().Copy( KMMApplication );
    message->Des().Append( KSetFocusString );
    message->Des().Append( applicationGroupName );
    message->Des().Append( KHideExit2 );

    // find MM application
    TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
    TApaTask task( taskList.FindApp( KMMUid ) );

    if ( task.Exists() )
        {
        // MM is already running in background - send APA Message
        task.SendMessage( 
            TUid::Uid( KUidApaMessageSwitchOpenFileValue ), *message );
        }
    else
        { 
        // MM not running yet - use Command Line Tail
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        
        User::LeaveIfError( appArcSession.Connect() );
        
        TApaAppInfo appInfo;
        TInt err( appArcSession.GetAppInfo( appInfo, KMMUid ) );
        
        if ( err == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            cmdLine->SetTailEndL( *message );
            appArcSession.StartApp( *cmdLine );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        CleanupStack::PopAndDestroy( &appArcSession ); 
        }
    CleanupStack::PopAndDestroy( message );
    CleanupStack::PopAndDestroy( item );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::LaunchBookmarkItemL( CCarouselData& aData )
    {
    StartEffect( KBrowserUid );
    
    CSchemeHandler* urlHandler = CSchemeHandler::NewL( aData.Value());    
    CleanupStack::PushL( urlHandler );
    urlHandler->HandleUrlStandaloneL();
    CleanupStack::PopAndDestroy( urlHandler );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::LaunchMailboxItemL( CCarouselData& aData )
    {
    TInt id( aData.MenuItem().Id());
    if ( id == KErrNotFound )
        {
        ShowNoteDlgL( R_MCS_DISABLE_OPEN_ITEM_MISSING );
        return;
        }
    
    StartEffect( KMCSCmailUidValue );
    
    TUid uId = TUid::Uid( id );
    const TVwsViewId viewId( KMCSCmailUidValue, KMCSCmailMailboxViewIdValue );
    CVwsSessionWrapper* vwsSession = CVwsSessionWrapper::NewL();
    vwsSession->CreateActivateViewEvent( viewId, uId, KNullDesC8() );
    delete vwsSession;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::LaunchMCSItemL( CCarouselData& aData )
    {
    if( iWatcher->IsActive())
        {
        return;
        }
    CMenuItem* item = NULL;
    TRAP_IGNORE( item = CMenuItem::OpenL( iMenu, aData.MenuItem().Id() ) );

    // item does not exist at all in MCS
    if ( item == NULL )
        {
        ShowNoteDlgL( R_MCS_DISABLE_OPEN_ITEM_MISSING );
        return;
        }

    CleanupStack::PushL( item );
    
    TBool attrExists = ETrue;
    TPtrC uid = item->GetAttributeL( KMenuAttrUid, attrExists );

    // trying to run hidden or missing application (e.g. unistalled app 
    // or app on MMC which was removed )
    // -> We display a note for a user that this is not possible¨
    TUint32 isHidden = item->Flags() & TMenuItem::EHidden;
    TUint32 isMissing = item->Flags() & TMenuItem::EMissing;

    if ( ( attrExists && uid == KMenuAttrUndefUid ) || isHidden || isMissing )
        {
        CleanupStack::PopAndDestroy( item );
        ShowNoteDlgL( R_MCS_DISABLE_OPEN_ITEM_MISSING );
        return;
        }
    
    if ( attrExists )
        {      
        StartEffect( ParseHexUidFromString( uid ));
        }
    
    // run application/shortcut
    CMenuOperation* operation( item->HandleCommandL(
        KMenuCmdOpen, KNullDesC8, iWatcher->iStatus ) );
    iWatcher->Watch( operation );
    CleanupStack::PopAndDestroy( item );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::HandleNotifyL
// Handle the change in Menu Content
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::HandleNotifyL()
    {
    TInt count( iPluginData->DataCount() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        CCarouselData& data( iPluginData->DataItemL( i ) );
        data.SetDirty( ETrue );
        }
    
    iPlugin.PublishL();
    
    // Notification must be activated again
    iNotifyWatcher->Cancel();
    iNotifier.Notify( 0,
        RMenuNotifier::EItemsAddedRemoved,
        iNotifyWatcher->iStatus );

    iNotifyWatcher->WatchNotify( this );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::HandleSessionEventL
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::HandleSessionEventL( TMsvSessionEvent aEvent, 
    TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
        case EMsvEntriesDeleted:
            {
            CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>( aArg1 );
            TInt count( sel->Count());
            for( TInt i = 0; i < count; i++ )
                {
                iPluginData->RemoveDataL( sel->At( i ) );
                }
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::SetBackupRestore
//
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::SetBackupRestore( TBool aBackupRestore )
    {
    iBackupRestore = aBackupRestore;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ShowSettingsL
// Launch General Settings plugin
// ---------------------------------------------------------------------------
//
void CCarouselPluginEngine::ShowSettingsL()
    { 
    TUid uid = {AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN};
    
    CGSLauncher* launcher = CGSLauncher::NewLC();
    launcher->LaunchGSViewL ( uid, KHomescreenUid, iInstanceUid );
    CleanupStack::PopAndDestroy( launcher );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ConstructMenuItemForIconL
// Syntax: skin(major minor):mif(filename bimapId maskId)
// ---------------------------------------------------------------------------
//
TBool CCarouselPluginEngine::ConstructMenuItemForIconL( const TDesC& aPath, 
    CMenuItem& aMenuItem )
   {
   TInt pos( aPath.Locate( ':' ) );
   if ( pos == KErrNotFound )
       {
       pos = aPath.Length();
       }
   TPtrC skin( aPath.Left( pos ) );
   TInt sf( skin.FindF( KSkin ) );
   
   if ( sf == KErrNotFound )
       {
       return EFalse;
       }
   
   TPtrC temp( skin.Mid( sf + KSkin().Length() ) );
   TLex input( temp );   
   input.SkipSpace();
   
   if ( !input.Eos() && input.Peek() == '(' )
       {
       input.Inc();
       }
   TPtrC majorId( NextIdToken( input ) );
   TPtrC minorId( NextIdToken( input ) );
   
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMajorId, majorId );
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMinorId, minorId );
   
   if ( aPath.Length() > pos && 
      ( aPath.Mid( pos + 1 ).FindF( KMif ) != KErrNotFound ) )
       {
       TPtrC mif( aPath.Mid( pos + 1 ) );
       TInt mf( mif.FindF( KMif ) );
       
       TLex input1( mif.Mid( mf + KMif().Length() ) );
       input1.SkipSpace();
       
       if ( !input1.Eos() && input1.Peek() == '(' )
          {
          input1.Inc();
          }
       
       TPtrC file( NextIdToken( input1 ) );
       TPtrC bitmapId( NextIdToken( input1 ) );
       TPtrC maskId( NextIdToken( input1 ) );
       
       aMenuItem.SetAttributeL( KMenuAttrIconFile, file );
       aMenuItem.SetAttributeL( KMenuAttrIconId, bitmapId );
       aMenuItem.SetAttributeL( KMenuAttrMaskId, maskId );
       }
   
   return ETrue;
   }

// End of file
