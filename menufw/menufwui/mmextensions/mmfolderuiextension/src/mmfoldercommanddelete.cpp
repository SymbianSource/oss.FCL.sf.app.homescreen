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
* Description:   Delete folder command
 *
*/

 
#include <liwcommon.h>
#include <e32base.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <AknQueryDialog.h>
#include <AknWaitDialog.h> 

#include "mmfoldercommanddelete.h"
#include "mmdialogmanager.h"
#include "mmactionrequest.h"
#include "hnglobals.h"
#include "hnliwutils.h"
#include "menudebug.h"
#include "hnutils.h"

#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "menudebug.h"
#include "hnconvutils.h"
#include "hnservicehandler.h"
#include "mmpluginsharedresources.h"

#include <avkon.rsg>
#include <matrixmenu.rsg>
#include <mmfolderuiextplugin.rsg>


_LIT8( KChildrenCount8, "children_count" );

// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderCommandDelete* CMmFolderCommandDelete::NewL()
	{
	CMmFolderCommandDelete* self = new( ELeave ) CMmFolderCommandDelete();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderCommandDelete::ConstructL()
	{
	BaseConstructL();
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderCommandDelete::~CMmFolderCommandDelete()
	{
	delete iServiceHandler;
    if( iWaitDialog )
        {
        delete iWaitDialog;
        }
	}

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMmFolderCommandDelete::CMmFolderCommandDelete()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommandDelete::HandleNotifyL(  TInt /*aCmdId*/, 
                                TInt aEventId,
                                CLiwGenericParamList& /* aEventParamList */,
                                const CLiwGenericParamList& /*aInParamList*/ )
    {  
    if( aEventId != KLiwEventCanceled  )
        {
        StopWaitDialogL();
        MMPERF(("CFolderUiExtPlugin::DeleteFolderCommandL - END"));
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommandDelete::ExecuteActionL(
        CLiwGenericParamList* aEventParamList )
    {
    MMPERF(("CFolderUiExtPlugin::DeleteFolderCommandL - START"));
    TInt id = GetIntL( *aEventParamList, KIdAttrName8 );
    TPtrC currentName = GetStringL( *aEventParamList, KName8 );
    TInt32 childrenCount = GetIntL( *aEventParamList, KChildrenCount8 );
	
	if( childrenCount == 0 )
	    {
        if( DeleteQueryL( currentName ) )
            {
            DeleteItemFromMcsL( id, currentName );

            CLiwGenericParamList* list = CLiwGenericParamList::NewLC();
            TLiwGenericParam command( KHnRequest, TLiwVariant( KDisableActionsForItem ) );
            TLiwGenericParam itemCustomId(
                    KItemCustomId8, TLiwVariant( static_cast<TInt32>( id ) ) );
            list->AppendL( itemCustomId );
            list->AppendL( command );
            iActionRequest->HandleRequestL( *list );
            CleanupStack::PopAndDestroy( list );
        	}
	    }
	else
	    {
	    DisplayErrorInfoL( currentName );
	    }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CMmFolderCommandDelete::MoveFocusBeforeDeleteL()
    {
    CLiwGenericParamList* list = CLiwGenericParamList::NewLC();
    TLiwGenericParam command( KHnRequest, TLiwVariant( KMoveFocusBeforeDelete ) );

    list->AppendL( command );
    
    iActionRequest->HandleRequestL( *list );
    CleanupStack::PopAndDestroy( list );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CMmFolderCommandDelete::StartWaitDialogL( const TDesC& aLabel )
    {
    if( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
 
    // For the wait dialog
    iWaitDialog = new (ELeave) CAknWaitDialog(
        REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) );
    iWaitDialog->SetCallback( this );
    iWaitDialog->SetTextL( aLabel );
    iWaitDialog->ExecuteLD( R_MENU_WAIT_DIALOG );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CMmFolderCommandDelete::DialogDismissedL(TInt /*aButtonId*/)
    {
    // No implementation required.
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CMmFolderCommandDelete::StopWaitDialogL()
    {
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL(); 
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
TBool CMmFolderCommandDelete::DeleteQueryL( const TDesC& aFolderName )
	{
	HBufC* msg = StringLoader::LoadLC(
			R_MENU_EDIT_DELETE_QUERY_TEXT, aFolderName );
	CAknQueryDialog* dialog = CAknQueryDialog::NewL();
	SetDialogL( dialog );
	TBool result( dialog->ExecuteLD( R_MENU_EDIT_DELETE_QUERY, *msg ) );
	SetDialogL( NULL );
    CleanupStack::PopAndDestroy( msg );
    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CMmFolderCommandDelete::DeleteItemFromMcsL(
        TInt aId, const TDesC& aFolderName )
    {
	MMPERF(("\t\t Delete folder - START"));
	
	GetListL( aId );
    TInt pos( 0 );
    TInt count( 0 );
    const TLiwGenericParam* res =
            iGetListOutParam->FindFirst( pos, KReturnValue8() );
    if( pos != KErrNotFound && res )
        {
        count = res->Value().AsList()->Count();
        }
    
    if( count == 0 )
        {
        StartWaitDialogL( iSharedResources->GetMsgFolderDeleting() );
        CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
            KInData8, KKeyTypeMap, KDefaultParentId8 );
        CleanupStack::PushL( inDataKey );

        HBufC8* id = HnConvUtils::NumToStr8LC( aId );
        AddSubKeyL( inDataKey, KIdParam, KKeyTypeInteger, *id );
        MCSQueryExecutionL( KCmdDelete8, inDataKey, NULL, this );

        CleanupStack::PopAndDestroy( id );
        CleanupStack::PopAndDestroy( inDataKey );
        }
    else
        {
        HBufC* message = StringLoader::LoadLC(
                R_FLDR_CANNOT_DELETE_ITEM, aFolderName );
        CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
                CAknNoteDialog::EConfirmationTone,
                CAknNoteDialog::ELongTimeout );
        dialog->SetTextL( message->Des() );
        dialog->ExecuteDlgLD( R_MENU_EDIT_MOVING_ABORT_NOTIFICATION );
        
        CleanupStack::PopAndDestroy( message );
        }
	
	MMPERF(("\t\t Delete folder - END"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CMmFolderCommandDelete::UninstallL( TInt aId )
    {
    MMPERF(("\t\t Delete folder - START"));
    
    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
        KInData8, KKeyTypeMap, KDefaultParentId8 );
    CleanupStack::PushL( inDataKey );

    HBufC8* id = HnConvUtils::NumToStr8LC( aId );
    AddSubKeyL( inDataKey, KIdParam, KKeyTypeInteger, *id );
    AddSubKeyL( inDataKey, KAction, KKeyTypeString8, KActionRemove );
    MCSQueryExecutionL( KCmdExecuteAction8, inDataKey, NULL, this );

    CleanupStack::PopAndDestroy( id );
    CleanupStack::PopAndDestroy( inDataKey );
    
    MMPERF(("\t\t Delete folder - END"));
    }


// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteSuite* CMmFolderDeleteSuite::NewL()
	{
	CMmFolderDeleteSuite* self = new( ELeave ) CMmFolderDeleteSuite();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderDeleteSuite::ConstructL()
	{
	}


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteSuite::~CMmFolderDeleteSuite()
	{
	}

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteSuite::CMmFolderDeleteSuite()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderDeleteSuite::ExecuteActionL(
        CLiwGenericParamList* /* aEventParamList */ )
    {
//    TPtrC currentName = GetStringL( *aEventParamList, KName8 );
    }

// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteApp* CMmFolderDeleteApp::NewL()
	{
	CMmFolderDeleteApp* self = new( ELeave ) CMmFolderDeleteApp();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderDeleteApp::ConstructL()
	{
	
	}


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteApp::~CMmFolderDeleteApp()
	{
	
	}

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMmFolderDeleteApp::CMmFolderDeleteApp()
	{
	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderDeleteApp::ExecuteActionL(
        CLiwGenericParamList* aEventParamList )
    {
    MMPERF(("CMmFolderDeleteApp::DeleteAppCommandL - START"));
    TInt id = GetIntL( *aEventParamList, KIdAttrName8 );
    UninstallL( id );
    }

// end of file

