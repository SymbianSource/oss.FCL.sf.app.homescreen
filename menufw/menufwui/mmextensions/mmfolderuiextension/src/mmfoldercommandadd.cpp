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
* Description:  Add folder command
 *  Version     : %version: 19.1.7 % << Don't touch! Updated by Synergy at check-out.
 *
*/

 
#include <liwcommon.h>

#include "mmfoldercommandadd.h"
#include "hnglobals.h"
#include "menudebug.h"
#include "hnconvutils.h"

#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "menudebug.h"
#include "mmactionrequest.h"
#include "hnservicehandler.h"

_LIT8( KItemLock, "item_lock" );

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderCommandAdd* CMmFolderCommandAdd::NewL()
	{
	CMmFolderCommandAdd* self = new( ELeave ) CMmFolderCommandAdd();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderCommandAdd::ConstructL()
	{
	BaseConstructL();
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderCommandAdd::~CMmFolderCommandAdd()
	{
	delete iServiceHandler;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmFolderCommandAdd::CMmFolderCommandAdd()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommandAdd::HandleNotifyL( TInt /*aCmdId*/, 
                                TInt aEventId,
                                CLiwGenericParamList& aEventParamList,
                                const CLiwGenericParamList& /*aInParamList*/ )
    {  
    if( aEventId != KLiwEventCanceled )
        {	
	    TInt pos( 0 );
        aEventParamList.FindFirst( pos, KIdAttrName8 );
        if (pos != KErrNotFound)
            {   
            CLiwGenericParamList* list = CLiwGenericParamList::NewLC();
            TLiwGenericParam command( KHnRequest, TLiwVariant( KSetFocus ) );
            TLiwGenericParam dataId(
                    KItemCustomId8, aEventParamList[pos].Value() );
            TLiwGenericParam dataParentId(
                    KSuiteCustomId8, TLiwVariant( iParent ) );
                      
            list->AppendL( dataId );
            list->AppendL( dataParentId );
            list->AppendL( command );
            
            iActionRequest->HandleRequestL( *list );
            CleanupStack::PopAndDestroy( list );
            }
	    MMPERF(("CFolderUiExtPlugin::AddFolderCommandL - END"));
		}
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommandAdd::ExecuteAddFolderL()
	{
    MMPERF(("\t Show dialog - START"));
    RBuf folder;
    CleanupClosePushL( folder );
    folder.CreateL( KMaxFileName );    
    
    TInt changed( KErrNone);
    TInt allowed( EValidationInvalid );
    do
        {
        changed = GetFolderNameL(folder, iGetListOutParam);
        if (changed && folder.Length() > 0)
            {
            allowed = FolderNameValidationL(folder, iGetListOutParam);
            }
        }
    while (allowed != EValidationOk && changed);
    MMPERF(("\t Show Dialog - END"));
    if (changed && allowed == EValidationOk)
        {
        MMPERF(("\t\t Create folder - START"));
        CreateFolderL( folder );
        MMPERF(("\t\t Create folder - END"));
        }
    CleanupStack::PopAndDestroy( &folder );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommandAdd::ExecuteActionL( 
                                       CLiwGenericParamList* aEventParamList )
    {
	MMPERF(("CFolderUiExtPlugin::AddFolderCommandL - START"));
	ASSERT( aEventParamList );
	TBool lock = GetBoolL( *aEventParamList, KItemLock );
	if( !lock )
	    {
	    iParent = GetIntL( *aEventParamList, KItemTypeFolder8 );
	    GetFolderListL( iParent );
	    ExecuteAddFolderL();
	    }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommandAdd::CreateFolderL( const TDesC& aFolder )

    {
	HBufC8* parentTemp = HnConvUtils::NumToStr8LC( iParent );

    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
        KInData8, KKeyTypeMap, KDefaultParentId );
    CleanupStack::PushL( inDataKey );

    AddSubKeyL( inDataKey, KType8, KKeyTypeString, KMenuFolder8 );
    AddSubKeyL( inDataKey, KDeleteLocked8, KKeyTypeBoolean, KStringFalse8 );
    AddSubKeyL( inDataKey, KMcsAppGroupName8, KKeyTypeString, aFolder );
    AddSubKeyL( inDataKey, KMcsTitleName8, KKeyTypeString, aFolder );
    AddSubKeyL( inDataKey, KMcsShortName8, KKeyTypeString, aFolder );
    AddSubKeyL( inDataKey, KMcsLongName8, KKeyTypeString, aFolder );
    AddSubKeyL( inDataKey, KParentId8, KKeyTypeInteger, *parentTemp );

    MCSQueryExecutionL( KCmdAdd8, inDataKey, NULL, this );

    CleanupStack::PopAndDestroy( inDataKey );
    CleanupStack::PopAndDestroy( parentTemp );
    }
	
// end of file

