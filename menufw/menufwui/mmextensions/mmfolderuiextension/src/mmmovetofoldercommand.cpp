/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Add folder command
 *
*/


#include <liwcommon.h>

#include "mmmovetofoldercommand.h"
#include "hnglobals.h"
#include "menudebug.h"
#include "hnconvutils.h"
#include "hnliwutils.h"
#include "hnutils.h"

#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "menudebug.h"
#include "mmactionrequest.h"
#include "hnservicehandler.h"
#include "mmpluginsharedresources.h"

#include <StringLoader.h>
#include <liwcommon.h>
#include <avkon.rsg>
#include <avkon.mbg>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <aknnotedialog.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmfolderuiextplugin.rsg>
#include <matrixmenu.rsg>

_LIT( KFolderClosed, "0\t" );
_LIT( KFolderOpened, "1\t" );
const TInt KSoftkeyBack( 0 );

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmMoveToFolderCommand* CMmMoveToFolderCommand::NewL()
    {
    CMmMoveToFolderCommand* self = new( ELeave ) CMmMoveToFolderCommand();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::ConstructL()
    {
    BaseConstructL();
    iLockDialog = EFalse;
    iFlagLeave = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmMoveToFolderCommand::~CMmMoveToFolderCommand()
    {
    iItemName.Close();
    iFolderName.Close();
    iWidgetType.Close();
    iOpenedHierarchy.ResetAndDestroy();
    iOpenedHierarchy.Close();
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmMoveToFolderCommand::CMmMoveToFolderCommand()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmMoveToFolderCommand::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt aEventId,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    if (aEventId != KLiwEventCanceled)
        {
        NotificationMovingCompleteL();
        iItemName.Close();
        iFolderName.Close();
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::ExecuteActionL(
        CLiwGenericParamList* aEventParamList )
    {

    //
    // Prepare move to folder operation
    MMPERF(("CMmMoveToFolderCommand::ExecuteActionL - START"));
    iItemId = GetIntL( *aEventParamList, KIdAttrName8 );
    iParentId = GetIntL( *aEventParamList, KFolderAttrName8 );
    TPtrC widget = GetStringL( *aEventParamList, KWidgetAttrName8 );
    TPtrC currentTempName = GetStringL( *aEventParamList, KName8 );

    iItemName.Close();
    iItemName.CreateL( currentTempName );

    iWidgetType.Close();
    iWidgetType.CreateL( widget );

    CMmFolder* rootFolder = CMmFolder::NewL();
    CleanupStack::PushL( rootFolder );

    rootFolder->SetNameL( iSharedResources->GetRootFolder());
    rootFolder->SetId( KRootId );
    iOpenedHierarchy.AppendL( rootFolder );
    CleanupStack::Pop( rootFolder );
    iLockDialog = EFalse;

    // Excute move to folder operation
    TInt err( KErrNone );

  // Get folder list form MCS
  GetFolderListL( CurrentFolder()->Id() );

  RPointerArray< CMmFolder > folders;
  CleanupResetAndDestroyPushL( folders );

  // Get folders from GetList output
  GetFoldersL(folders);

    // if foler already exists, reopen options menu
    do
      {
      TRAP( err, ExecuteMoveToFolderL());
      }
    while ( err == KErrAlreadyExists && folders.Count() > 0 );

    // if any different leave was trapped, pass it on
    if ( err != KErrNone )
      {
      User::Leave( err );
      }

    CleanupStack::PopAndDestroy( &folders );
    //MMPERF(("CFolderUiExtPlugin::AddFolderCommandL - END"));
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmFolder* CMmMoveToFolderCommand::CurrentFolder()
    {
    ASSERT( iOpenedHierarchy.Count() > 0 );
    return iOpenedHierarchy[ iOpenedHierarchy.Count() - 1 ];
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::MoveToFolderL( TInt aId, TInt aFolderId,
                                            TInt aBeforeItemId )
    {
    if( aFolderId != iParentId )
        {
        CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
            KInData8, KKeyTypeMap, KDefaultParentId8 );
        CleanupStack::PushL( inDataKey );
        CHnMdBaseKey* listIds = HnMdKeyFactory::CreateL(
            KIdsList, KKeyTypeList, KNullDesC8 );

        HBufC8* id = HnConvUtils::NumToStr8LC( aId );
        AddSubKeyL( listIds, KIdParam, KKeyTypeBoolean, *id );
        CleanupStack::PopAndDestroy();

        inDataKey->AddSubKeyL( listIds );
        HBufC8* folderId = HnConvUtils::NumToStr8LC( aFolderId );
        AddSubKeyL( inDataKey, KFolderId8, KKeyTypeBoolean, *folderId );
        CleanupStack::PopAndDestroy( folderId );

        if( aBeforeItemId != KErrNotFound )
            {
            HBufC8* itemId = HnConvUtils::NumToStr8LC( aBeforeItemId );
            AddSubKeyL( inDataKey, KBeforeItemId8, KKeyTypeBoolean, *itemId );
            CleanupStack::PopAndDestroy( itemId );
            }
        MCSQueryExecutionL( KCmdOrganize8, inDataKey, NULL, this );
        iOpenedHierarchy.ResetAndDestroy();

        CleanupStack::PopAndDestroy( inDataKey );
        CLiwGenericParamList* list = CLiwGenericParamList::NewLC();
        TLiwGenericParam command( KHnRequest, TLiwVariant( KDisableActionsForItem ) );
        TLiwGenericParam itemCustomId(
                KItemCustomId8, TLiwVariant( static_cast<TInt32>( aId ) ) );
        list->AppendL( itemCustomId );
        list->AppendL( command );
        iActionRequest->HandleRequestL( *list );
        CleanupStack::PopAndDestroy( list );
        }
    else
        {
        NotificationItemAlreadyStoredL();
        if( iOpenedHierarchy.Count() > 0 && !iLockDialog )
            {
            if( CurrentFolder()->Id() == iParentId && iParentId != KRootId )
                {
                StepBackInHierarchy();
                }
            else if ( iParentId == KRootId )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::SelectFolderL(
        const RPointerArray< CMmFolder >& aFolderContent )
    {
    CDesCArrayFlat* folders =
        new( ELeave )CDesCArrayFlat( aFolderContent.Count() + 1 );
    CleanupStack::PushL( folders );
    RBuf currentFolder;
    CleanupClosePushL( currentFolder );
    currentFolder.CreateL(
        KFolderOpened().Length() + CurrentFolder()->Name().Length() );
    currentFolder.Append( KFolderOpened );
    currentFolder.Append( CurrentFolder()->Name() );
    folders->AppendL( currentFolder );

    for( int i( 0 ); i<aFolderContent.Count(); i++ )
        {
        CMmFolder* folder = aFolderContent[ i ];
        RBuf name;
        CleanupClosePushL( name );
        name.CreateL( KFolderClosed().Length() + folder->Name().Length() );
        name.Append( KFolderClosed );
        name.Append( folder->Name() );
        folders->AppendL( name );
        CleanupStack::PopAndDestroy( &name );
        }

    TInt selected( SelectFolderFromListL(
        folders, CurrentFolder()->Id() == KRootId ) );

    switch( selected )
        {
        case EAknSoftkeyBack:
            {
            if( iOpenedHierarchy.Count() > 1 && !iLockDialog )
                {
                StepBackInHierarchy();
                break;
                }
            }
        case EAknSoftkeyExit:
            {
            iItemName.Close();
            iFolderName.Close();
            iOpenedHierarchy.ResetAndDestroy();
            break;
            }
        default:
            {
            if( selected > 0 )
                {
                CMmFolder* folder = CMmFolder::NewL();
                CleanupStack::PushL( folder );
                folder->SetNameL( aFolderContent[ selected - 1 ]->Name() );
                folder->SetId( aFolderContent[ selected - 1 ]->Id() );
                iOpenedHierarchy.AppendL( folder );
                CleanupStack::Pop( folder );
                }
            else
                {
                iFolderName.Close();
                iFolderName.CreateL( CurrentFolder()->Name() );
                MoveToFolderL( iItemId, CurrentFolder()->Id() );
                }
            break;
            }
        }

    CleanupStack::PopAndDestroy( &currentFolder );
    CleanupStack::PopAndDestroy( folders );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmMoveToFolderCommand::SelectFolderFromListL(
        CDesCArrayFlat* aItems, TBool aIsRoot )
    {
    TInt selected( KErrNotFound );

    CAknListQueryDialog* dialog =
        new ( ELeave ) CAknListQueryDialog( &selected );
    if( aIsRoot )
        {
        dialog->PrepareLC( R_MENU_EDIT_MOVE_TO_FOLDER_SELECT_CANCEL_QUERY );
        }
    else
        {
        dialog->PrepareLC( R_MENU_EDIT_MOVE_TO_FOLDER_SELECT_BACK_QUERY );
        }
    dialog->SetItemTextArray( aItems );
    dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );
    CArrayPtr<CGulIcon>* icons = GetFolderIconsL();
    dialog->SetIconArrayL( icons );
    dialog->ListBox()->SetCurrentItemIndexAndDraw( 0 );

    if( !iLockDialog )
        {
        SetDialogL( dialog );
        TInt softkey ( KErrNotFound );
        softkey = dialog->RunLD();
        SetDialogL( NULL );

        if( softkey == KSoftkeyBack )
            {
            selected = EAknSoftkeyBack;
            }
        else if( selected < 0 ||
                ( softkey != EAknSoftkeyOk && softkey != EAknSoftkeySelect ) )
            {
            selected = EAknSoftkeyExit;
            }
        }
    else
        {
        CleanupStack::PopAndDestroy( dialog );
        selected = EAknSoftkeyExit;
        }

    return selected;
    }


// ---------------------------------------------------------------------------
// Action resolving function.
// ---------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CMmMoveToFolderCommand::GetFolderIconsL()
    {
    CArrayPtr<CGulIcon>* icons = new( ELeave )CAknIconArray( 10 );
    CleanupStack::PushL( icons );
    CFbsBitmap* folderClosed     = NULL;
    CFbsBitmap* folderClosedMask = NULL;

    AknIconUtils::CreateIconL( folderClosed, folderClosedMask,
            AknIconUtils::AvkonIconFileName(),
            EMbmAvkonQgn_prop_folder_small,
            EMbmAvkonQgn_prop_folder_small_mask );
    CleanupStack::PushL( folderClosed );
    CleanupStack::PushL( folderClosedMask );
    icons->AppendL( CGulIcon::NewL( folderClosed, folderClosedMask ) );
    CleanupStack::Pop( 2 );

    CFbsBitmap* folderOpened     = NULL;
    CFbsBitmap* folderOpenedMask = NULL;
    AknIconUtils::CreateIconL( folderOpened, folderOpenedMask,
            AknIconUtils::AvkonIconFileName(),
            EMbmAvkonQgn_prop_folder_current,
            EMbmAvkonQgn_prop_folder_current_mask );
    CleanupStack::PushL( folderOpened );
    CleanupStack::PushL( folderOpenedMask );
    icons->AppendL( CGulIcon::NewL( folderOpened, folderOpenedMask ) );
    CleanupStack::Pop( 3 );
    return icons;
    }


// ---------------------------------------------------------------------------
// Action resolving function.
// ---------------------------------------------------------------------------
//
HBufC* CMmMoveToFolderCommand::GetFromResourceLC( TInt aIdentifier )
    {
    HBufC* string = StringLoader::LoadLC( aIdentifier );
    return string;
    }


// ---------------------------------------------------------------------------
// Action resolving function.
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::NotificationMovingCompleteL()
    {
    CDesCArrayFlat* items = new(ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( items );
    items->AppendL( iItemName );
    items->AppendL( iFolderName );

    HBufC* msg = StringLoader::LoadLC( R_APPS_NOTE_MOVE_TO_FOLDER, *items );
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ELongTimeout );
    dialog->SetTextL( msg->Des() );
    dialog->ExecuteDlgLD( R_MENU_EDIT_MOVING_COMPLETE_NOTIFICATION );

    CleanupStack::PopAndDestroy( msg );
    CleanupStack::PopAndDestroy( items );
    }


// ---------------------------------------------------------------------------
// Action resolving function.
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::NotificationItemAlreadyStoredL()
    {
    HBufC* message = StringLoader::LoadLC( R_APPS_NOTE_ITEM_ALREADY_STORED );
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ELongTimeout );
    dialog->SetTextL( message->Des() );
    dialog->ExecuteDlgLD( R_MENU_EDIT_MOVING_ABORT_NOTIFICATION );

    CleanupStack::PopAndDestroy( message );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::StepBackInHierarchy()
  {
  // Remove last folder from iOpenedHierarchy
  CMmFolder* folder = CurrentFolder();
  iOpenedHierarchy.Remove( iOpenedHierarchy.Count() - 1 );
  delete folder;
  }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::ExecuteMoveToFolderL()
  {
    while (iOpenedHierarchy.Count() > 0)
      {
      // Get folder list form MCS
      GetFolderListL( CurrentFolder()->Id() );

      RPointerArray< CMmFolder > folders;
      CleanupResetAndDestroyPushL( folders );

      // Get folders from GetList output
      GetFoldersL(folders);

      // Perform further folder selection or move operation
      if( folders.Count() > 0 )
        {
        SelectFolderL( folders );
        }
      else
        {
        iFolderName.Close();
        iFolderName.CreateL( CurrentFolder()->Name() );
        MoveToFolderL( iItemId, CurrentFolder()->Id() );
        }
      CleanupStack::PopAndDestroy( &folders );
      }
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmMoveToFolderCommand::GetFoldersL(RPointerArray<CMmFolder >& aFolderArray)
  {
  TInt pos( 0 );
  TInt count = iGetListOutParam->FindFirst(
      pos, KReturnValue8() )->Value().AsList()->Count();
  TLiwVariant var; var.PushL();

  // Get propper widget names depending from view type
  // and append to aFolderArray
  for( TInt i = 0; i < count; i++ )
    {
    HnLiwUtils::GetVariantL( *iGetListOutParam, KIdPath8, i, var );
    TInt32 id;
    var.Get( id );

    if (id != iItemId)
      {
      CMmFolder* folder = CMmFolder::NewL();
      CleanupStack::PushL( folder );

      folder->SetId( id );

      TPtrC title;
      if( !iWidgetType.Compare( KWidgetTypeList ) )
        {
        HnLiwUtils::GetVariantL(
            *iGetListOutParam, KLongNamePath8, i, var );
        }
      else if( !iWidgetType.Compare( KWidgetTypeGrid ) )
        {
        HnLiwUtils::GetVariantL(
            *iGetListOutParam, KShortNamePath8, i, var );
        }
      else
        {
        HnLiwUtils::GetVariantL(
            *iGetListOutParam, KAppGroupNamePath8, i, var );
        }
      var.Get( title );
      folder->SetNameL( title );
      aFolderArray.AppendL( folder );

      CleanupStack::Pop( folder );
      }
    }
  CleanupStack::PopAndDestroy(&var);
  }

// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolder* CMmFolder::NewL()
    {
    CMmFolder* self = new( ELeave ) CMmFolder();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolder::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolder::~CMmFolder()
    {
    iName.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmFolder::CMmFolder()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolder::SetNameL( const TDesC& aName )
    {
    iName.Close();
    iName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CMmFolder::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolder::SetId( TInt aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmFolder::Id()
    {
    return iId;
    }

// end of file

