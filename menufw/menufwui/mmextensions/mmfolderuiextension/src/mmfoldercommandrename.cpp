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
* Description:   Rename folder command
 *
*/


 
#include <liwcommon.h>

#include "mmfoldercommandrename.h"
#include "mmactionrequest.h"
#include "hnglobals.h"
#include "menudebug.h"
#include "hnconvutils.h"
#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "menudebug.h"
#include "hnservicehandler.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderCommandRename* CMmFolderCommandRename::NewL()
	{
	CMmFolderCommandRename* self = new( ELeave ) CMmFolderCommandRename();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderCommandRename::ConstructL()
	{
	BaseConstructL();
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderCommandRename::~CMmFolderCommandRename()
	{
	delete iServiceHandler;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmFolderCommandRename::CMmFolderCommandRename()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommandRename::HandleNotifyL( TInt /*aCmdId*/, 
                                TInt /*aEventId*/,
                                CLiwGenericParamList& /*aEventParamList*/,
                                const CLiwGenericParamList& /*aInParamList*/ )
    {  
    MMPERF(("CFolderUiExtPlugin::RenameFolderCommandL - END"));
	return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommandRename::ExecuteActionL( 
                                        CLiwGenericParamList* aEventParamList )
    {
  	MMPERF(("CFolderUiExtPlugin::RenameFolderCommandL - START"));
    TInt id = GetIntL( *aEventParamList, KIdAttrName8 );
    TPtrC currentName = GetStringL( *aEventParamList, KName8 );
    
    RBuf folder;
    CleanupClosePushL( folder );
    folder.CreateL( KMaxFileName );
    folder.Copy( currentName );

    MMPERF(("\t Show dialog"));
    TInt changed( KErrNone);
    TInt allowed( EValidationInvalid );
    do
        {
        changed = GetFolderNameL( folder );
        if (changed && folder.Length() > 0)
            {
            allowed = FolderNameValidationL( folder );
            }
        }
    while (allowed != EValidationOk && changed);
    
	if( changed && allowed == EValidationOk )
        {
        MMPERF(("\t\t Rename folder - START"));
        RenameFolderL( id , folder );
        CLiwGenericParamList* list = CLiwGenericParamList::NewLC();
        TLiwGenericParam command( KHnRequest, TLiwVariant( KDisableActionsForItem ) );
        TLiwGenericParam itemCustomId(
                KItemCustomId8, TLiwVariant( static_cast<TInt32>( id ) ) );
        list->AppendL( itemCustomId );
        list->AppendL( command );
        iActionRequest->HandleRequestL( *list );
        CleanupStack::PopAndDestroy( list );        
        MMPERF(("\t\t Rename folder - END"));
        }

	CleanupStack::PopAndDestroy( &folder );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CMmFolderCommandRename::RenameFolderL( TInt aId, RBuf& aFolderName )
    {
    HBufC8* id = HnConvUtils::NumToStr8LC( aId );
    
    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
            KInData8, KKeyTypeMap, KDefaultParentId );
    CleanupStack::PushL( inDataKey );
    
    AddSubKeyL( inDataKey, KIdParam, KKeyTypeInteger, *id );
    AddSubKeyL( inDataKey, KMcsTitleName8, KKeyTypeString, aFolderName );
    AddSubKeyL( inDataKey, KMcsShortName8, KKeyTypeString, aFolderName );
    AddSubKeyL( inDataKey, KMcsLongName8, KKeyTypeString, aFolderName );
    
	MCSQueryExecutionL( KCmdAdd8, inDataKey, NULL, this );

    CleanupStack::PopAndDestroy( inDataKey );
    CleanupStack::PopAndDestroy( id );
    }
	
// end of file

