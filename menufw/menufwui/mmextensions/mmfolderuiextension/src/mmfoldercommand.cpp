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
* Description:   Interface of the folder command
 *
*/

#include <liwcommon.h>
#include <e32base.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <s32mem.h>

#include "mmfoldercommand.h"
#include "mmfoldercommandadd.h"
#include "mmfoldercommandrename.h"
#include "mmfoldercommanddelete.h"
#include "mmmovetofoldercommand.h"
#include "mmdialogmanager.h"
#include "hnliwutils.h"
#include "menudebug.h"
#include "hnutils.h"

#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "menudebug.h"
#include "hnconvutils.h"
#include "hnservicehandler.h"
#include "mmfolderuiextpluginconstants.h"
#include "mmpluginsharedresources.h"

#include <avkon.rsg>
#include <matrixmenu.rsg>
#include <mmfolderuiextplugin.rsg>


const TInt KCountNotAllowedChars = 9;
// Unicode character of sign obtained by press '0' key three times. Enter key.
const TUint16 KIllegalCharEnter = 0x21b2;
// Unicode line separator character obtained by pressing Shift + Enter keys
const TUint16 KIllegalCharLineSeparator = 0x2028;
// Space unicode character.
// Characters which ascii value is smaller than 32 are not allowed.
const TUint16 KSpace = ' '; //32;

// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmFolderCommand::~CMmFolderCommand()
    {
    iSerializedInputList.Close();
    iSerializedOutputList.Close();
    delete iGetListOutParam;
    delete iGetListInParam;
    iMcs.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::BaseConstructL()
	{
	// Init MCS session
	TBuf16<KMaxFileName> buf;
	buf.Copy(KMatrixMenuData);
	iMcs.OpenL(buf);
	iGetListInParam = CLiwGenericParamList::NewL();
	iGetListOutParam = CLiwGenericParamList::NewL();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::MCSQueryExecutionL(
        const TDesC8& aQueryName,
        const CHnMdBaseKey* aInputData,
        const CHnMdBaseKey* aFilter,
        MLiwNotifyCallback* aCallback )
    {
    RPointerArray< CHnMdBaseKey > data;
    CleanupClosePushL( data );
    data.AppendL( aInputData );
    if ( aFilter )
        {
        data.AppendL( aFilter );
        }

    CHnMdBaseKey* serviceData = HnMdKeyFactory::CreateL(
        KServiceContentName, KKeyTypeString, KMatrixMenuData );
    CleanupStack::PushL( serviceData );

    RPointerArray< CHnMdBaseKey> constructor;
    CleanupClosePushL( constructor );
    constructor.AppendL( serviceData );
    CLiwGenericParamList* constructorLiw = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* commandLiw = CLiwGenericParamList::NewLC();
    HnLiwUtils::SetGenericParamListL( constructor, *constructorLiw );
    HnLiwUtils::SetGenericParamListL( data, *commandLiw );

    TBuf8< KMaxLength > interface( KMCSDataSourceInterface8 );
    if ( !aQueryName.Compare( KCmdOrganize8 ) ||
            !aQueryName.Compare( KCmdExecuteAction8 ) )
        {
        interface.Copy( KMCSMenuContentInterface8 );
        }

    delete iServiceHandler;
    iServiceHandler = NULL;
    iServiceHandler = CHnServiceHandler::NewL(
            KMCSService8, interface, aQueryName,
            EServiceModeAsynchronous, constructorLiw, commandLiw );

    CleanupStack::Pop( commandLiw );
    CleanupStack::Pop( constructorLiw );

    iServiceHandler->ExecuteL( aCallback , 0 );

    CleanupStack::PopAndDestroy( &constructor );
    CleanupStack::PopAndDestroy( serviceData );
    CleanupStack::PopAndDestroy( &data );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmFolderCommand* CMmFolderCommand::FactoryL( const TDesC& aCommand )
    {
    CMmFolderCommand* ret = NULL;
    if( !aCommand.Compare( KAddFolderCmd ) )
		{
		ret = CMmFolderCommandAdd::NewL();
		}
    else if ( !aCommand.Compare( KRenameFolderCmd ) )
		{
		ret = CMmFolderCommandRename::NewL();
		}
    else if ( !aCommand.Compare( KDeleteFolderCmd ) )
		{
		ret = CMmFolderCommandDelete::NewL();
		}
    else if ( !aCommand.Compare( KDeleteAppCmd ) )
		{
		ret = CMmFolderDeleteApp::NewL();
		}
    else if ( !aCommand.Compare( KDeleteSuiteCmd ) )
		{
		ret = CMmFolderDeleteSuite::NewL();
		}
    else if ( !aCommand.Compare( KMoveToFolderCmd ) )
        {
        ret = CMmMoveToFolderCommand::NewL();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmFolderCommand::DoChangeNameL( RBuf& aFolderName, 
		TInt aDefaultNameLength, RBuf& aNumberInBracket, 
		CLiwGenericParamList& aFoldersNames, TLiwVariant& aVar,
		TInt aNumberToAppend )
    {
    TBool nameChanged( EFalse );
    TInt position( KErrNone );
    TInt count = aFoldersNames.FindFirst( position, KReturnValue8() )
        ->Value().AsList()->Count();
    aNumberInBracket.Delete( KOpenBracket().Length(), 
        aNumberInBracket.Length() - KOpenBracket().Length() );
    
    if ( aNumberToAppend < KMaxDigitNumber )
        {
        aNumberInBracket.AppendNum( 0 );
        }
    
    for ( TInt i = 0; i < count; i++ )
        {
        HnLiwUtils::GetVariantL( aFoldersNames, KLongNamePath8, i, aVar );
        TPtrC longName;
        aVar.Get( longName );
        if ( !aFolderName.Compare( longName ) )
            {
            aNumberInBracket.AppendNum( aNumberToAppend );
            aNumberInBracket.Append( KCloseBracket );
            aFolderName.Delete( aDefaultNameLength, 
                aFolderName.Length() - aDefaultNameLength );
            aFolderName.Append( aNumberInBracket );
            nameChanged = ETrue;
            break;
            }
        }
    return nameChanged;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::ChangeNameIfAlreadyExistsL( RBuf& aFolderName,
        CLiwGenericParamList* aFoldersNames, TInt aNumberToAppend )
    {
    if ( !aFoldersNames )
        {
        return;
        }
    TLiwVariant var;
    var.PushL();
    RBuf numberInBracket;
    CleanupClosePushL(numberInBracket);
    numberInBracket.CreateL( KMaxLength );
    numberInBracket.Append( KOpenBracket );
    numberInBracket.AppendNum( aNumberToAppend );
    numberInBracket.Append( KCloseBracket );
    TInt defaultNameLength = aFolderName.Length();
    
    while( DoChangeNameL( aFolderName, defaultNameLength, numberInBracket,
            *aFoldersNames, var, ++aNumberToAppend ) )
        ;
        
    CleanupStack::PopAndDestroy(&numberInBracket);
    CleanupStack::PopAndDestroy( &var );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmFolderCommand::GetFolderNameL( RBuf& aFolderName, 
        CLiwGenericParamList* aFoldersNames )
	{
	TBool res( EFalse );
	if( aFolderName.Length() == 0 )
		{
	    aFolderName.Append( iSharedResources->GetDefaultFolderName() );
	    ChangeNameIfAlreadyExistsL( aFolderName, aFoldersNames );
		}

	CAknTextQueryDialog* dialog =
	    CAknTextQueryDialog::NewL( aFolderName, CAknQueryDialog::ENoTone );
	
	dialog->PrepareLC( R_MENU_FILENAME_QUERY );
	dialog->SetPromptL( iSharedResources->GetEditFolderName() );

	SetDialogL( dialog );
    res = dialog->RunLD();
    SetDialogL( NULL );

	return res;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::GetFolderListL( TInt aItemId )
    {
    // Prepare data for GetList
    iGetListInParam->Reset();
    PrepareGetListInputL( aItemId );
    PrepareGetListFolderFilterL();
    ExternalizeInputListL();
    
    // Get folders list from MCS
    iMcs.GetListL( iSerializedInputList, iSerializedOutputList );
    InternalizeOutputListL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::GetListL( TInt aItemId )
    {
    // Prepare data for GetList
    iGetListInParam->Reset();
    PrepareGetListInputL( aItemId );
    PrepareGetListFilterL();
    ExternalizeInputListL();
    
    // Get folders list from MCS
    iMcs.GetListL( iSerializedInputList, iSerializedOutputList );
    InternalizeOutputListL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::DisplayErrorInfoL( const TDesC& aFolderName )
    {
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ELongTimeout );
    HBufC* msg = StringLoader::LoadLC(
            R_FLDR_CANNOT_DELETE_ITEM, aFolderName );
    dialog->SetTextL( msg->Des() );
    dialog->ExecuteDlgLD( R_MENU_EDIT_INTERRUPTION_INFO );

    CleanupStack::PopAndDestroy( msg );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::SetRequestInterface( MMmActionRequest* aActionRequest )
    {
    iActionRequest = aActionRequest;
    }

void CMmFolderCommand::SetSharedResources( CMmPluginSharedResources* aResources )
	{
	iSharedResources = aResources;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::SetDialogL( CAknQueryDialog* aDialog )
    {
    iDialog = aDialog;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::RemoveDialogL()
    {
    iLockDialog = ETrue;
    if( iDialog )
        {
        iDialog->HandleQueryEditorStateEventL( NULL,
            MAknQueryControlObserver::EEmergencyCallAttempted,
            MAknQueryControlObserver::EEditorEmpty );
        iDialog = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::AddSubKeyL( CHnMdBaseKey* aInDataKey,
                                   const TDesC8& aKeyName,
                                   const TDesC8& aKeyType,
                                   const TDesC8& aKeyValue )
    {
    CHnMdBaseKey* key = HnMdKeyFactory::CreateL(
    		aKeyName, aKeyType, aKeyValue );
    CleanupStack::PushL( key );
    aInDataKey->AddSubKeyL( key );
    CleanupStack::Pop( key );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CMmFolderCommand::AddSubKeyL( CHnMdBaseKey* aInDataKey, 
                                   const TDesC8& aKeyName, 
                                   const TDesC8& aKeyType, 
                                   const TDesC& aKeyValue )
    {
    CHnMdBaseKey* key = HnMdKeyFactory::CreateL(
    		aKeyName, aKeyType, aKeyValue );
    CleanupStack::PushL( key );
    aInDataKey->AddSubKeyL( key );
    CleanupStack::Pop( key );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMmFolderCommand::FolderNameValidationL(RBuf& aFolderName,
        CLiwGenericParamList* aFoldersNames)
    {
    TValidation allowed(EValidationOk);

    allowed = (TValidation) IllegalCharacterRemoveL(aFolderName, aFoldersNames);
    if (allowed != EValidationOk)
        {
        return allowed;
        }
    allowed = (TValidation) CheckValidFileNameL(aFolderName);
    if (allowed != EValidationOk)
        {
        // §qtn_fldr_bad_file_name§
        // data entry (for example data query New name: §qtn_fldr_item_name_prmpt§ ) is opened
        DisplayNameErrorInfoL(iSharedResources->GetMsgBadFileName());
        return allowed;
        }
    allowed = (TValidation) CheckAllowedCharsL(aFolderName);
    if (allowed != EValidationOk)
        {
        // information note "Name cannot contain
        // the characters < > \\ /"" |:*?" §qtn_fldr_illegal_characters§ is displayed.                 
        DisplayNameErrorInfoL(iSharedResources->GetMsgIllegalCharacters());
        }
    return allowed;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommand::IllegalCharacterRemoveL(RBuf& aFolderName,
        CLiwGenericParamList* aFoldersNames)
    {
    TValidation allowed( EValidationOk );
    // illegal characters are removed
    TInt pos( 0 );
    while ( pos < aFolderName.Length() )
        {
        if ( aFolderName[pos] == KIllegalCharEnter ||
             aFolderName[pos] == KIllegalCharLineSeparator )
            {
            aFolderName.Delete( pos, 1 );
            }
        else
            {
            ++pos;
            }
        }
    if (aFolderName.Length() == 0)
        {
        aFolderName.Append(iSharedResources->GetDefaultFolderName());
        ChangeNameIfAlreadyExistsL(aFolderName, aFoldersNames);
        return allowed;
        }
    // only space character is not allowed
    RBuf trimBuf;
    CleanupClosePushL(trimBuf);
    trimBuf.CreateL(aFolderName.Length());
    trimBuf.Copy(aFolderName);
    trimBuf.Trim();
    if (!trimBuf.Length())
        {
        allowed = EValidationInvalid;
        }
    CleanupStack::PopAndDestroy(&trimBuf);

    return allowed;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommand::CheckValidFileNameL( const TDes& aFolderName )
    {
    TValidation allowed(EValidationOk);
    _LIT( KSingleDot, ".");
    _LIT( KDoubleDot, "..");
    
    if (!aFolderName.Compare( KSingleDot) ||
            !aFolderName.Compare( KDoubleDot ) ||
            aFolderName.Length() > KMaxFileName)
        {
        allowed = EValidationInvalid;
        }
    else
        {
        // characters with ascii value less than 32
        for (TInt index(0); index < aFolderName.Length(); index++)
            {
            if (aFolderName[index] < KSpace)
                {
                allowed = EValidationInvalid;                
                }
            }
        }    
    return allowed;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommand::CheckAllowedCharsL( const TDes& aFolderName )
    {
    TValidation allowed(EValidationOk);

    for (TInt index(0); index < aFolderName.Length(); index++)
        {
        for (TInt c(0); c < KCountNotAllowedChars; c++)
            {
            if (TChar( KNotAllowedChars[c] ) == aFolderName[index])
                {
                allowed = EValidationNotAllowed;
                }
            }
        }

    return allowed;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::DisplayNameErrorInfoL( const TDesC& aMsg )
    {
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ELongTimeout );
    dialog->SetTextL( aMsg );
    dialog->ExecuteDlgLD( R_MENU_EDIT_MOVING_ABORT_NOTIFICATION );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::PrepareGetListInputL(TInt aItemId)
	{
    CLiwDefaultMap* inputMap = CLiwDefaultMap::NewLC();
    
    // Specify GetList input data
    inputMap->InsertL( KIdAttrName8, TLiwVariant( TInt32( aItemId ) ) );         	// Item id
    inputMap->InsertL( KRecursiveSearch8, TLiwVariant( TBool ( EFalse ) ) );	// Recursive Search
    inputMap->InsertL( KFlatResult8, TLiwVariant ( TBool ( ETrue ) ) );	   	// Flat GetList
    inputMap->InsertL( KParentOnly8, TLiwVariant ( TBool ( EFalse ) ) );	   	// Get children
    
    // Fill GetList input param list
    InsertToLiwListL(iGetListInParam, KInData8, inputMap);

    CleanupStack::PopAndDestroy(inputMap);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::PrepareGetListFolderFilterL()
	{
    // Specify GetList filter
	CLiwDefaultMap* folderFilter = CLiwDefaultMap::NewLC();
	folderFilter->InsertL( KTypeAttrName8, TLiwVariant( KMenuFolder ) ); // Folders
	
	folderFilter->InsertL( KHidden8, TLiwVariant( TBool(EFalse) ) ); // Not Hidden
	folderFilter->InsertL( KDeleteLocked8, TLiwVariant( TBool(EFalse) ) ); // Not locked
	
	// Fill GetList input param list
	InsertToLiwListL(iGetListInParam, KFilter8, folderFilter);
	
	CleanupStack::PopAndDestroy(folderFilter);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::PrepareGetListFilterL()
    {
    // Specify GetList filter
    CLiwDefaultMap* folderFilter = CLiwDefaultMap::NewLC();
    
    folderFilter->InsertL( KHidden8, TLiwVariant( TBool(EFalse) ) ); // Not Hidden
    folderFilter->InsertL( KMissing8, TLiwVariant( TBool(EFalse) ) ); // Not Missing
    
    // Fill GetList input param list
    InsertToLiwListL(iGetListInParam, KFilter8, folderFilter);
    
    CleanupStack::PopAndDestroy(folderFilter);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::InsertToLiwListL(CLiwGenericParamList* aList, 
		const TDesC8& aKey, const CLiwDefaultMap* aMap)
	{
    TLiwVariant liwVar;    
    liwVar.Set( aMap );
    liwVar.PushL();
    TLiwGenericParam livGenParam( aKey, liwVar );
    livGenParam.PushL();
    aList->AppendL ( livGenParam );
    CleanupStack::PopAndDestroy( &livGenParam );
    CleanupStack::PopAndDestroy( &liwVar ); 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::ExternalizeInputListL()
	{
	iSerializedInputList.Close();
	iSerializedOutputList.Close();
	iSerializedInputList.CreateL( iGetListInParam->Size() );
	RDesWriteStream datastrm( iSerializedInputList );
	CleanupClosePushL(datastrm);
	iGetListInParam->ExternalizeL(datastrm);
	datastrm.CommitL();
	CleanupStack::PopAndDestroy(&datastrm);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmFolderCommand::InternalizeOutputListL()
	{
	iGetListOutParam->Reset();
	if (iSerializedOutputList.Size())
		{
		RDesReadStream stream( iSerializedOutputList );
		CleanupClosePushL( stream );
		iGetListOutParam->InternalizeL( stream ) ;
		CleanupStack::PopAndDestroy(&stream);
		}
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderCommand::GetIntL(
        const CLiwGenericParamList& aParam, const TDesC8& aPath )
    {
    TInt pos( 0 );
    const TLiwGenericParam* param = aParam.FindFirst( pos, aPath );
    if( pos != KErrNotFound && param )
        {
        return param->Value().AsTInt32();
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPtrC CMmFolderCommand::GetStringL(
        const CLiwGenericParamList& aParam, const TDesC8& aPath )
    {
    TInt pos( 0 );
    const TLiwGenericParam* param = aParam.FindFirst( pos, aPath );
    if( pos != KErrNotFound && param )
        {
        return param->Value().AsDes();
        }
    TPtrC ret( KNullDesC16 );
    return ret; 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CMmFolderCommand::GetBoolL(
        const CLiwGenericParamList& aParam, const TDesC8& aPath )
    {
    TInt pos( 0 );
    const TLiwGenericParam* param = aParam.FindFirst( pos, aPath );
    if( pos != KErrNotFound && param )
        {
        return param->Value().AsTBool();
        }
    return EFalse; 
    }

// end of file
