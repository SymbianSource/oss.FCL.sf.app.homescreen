/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dialog renderer.
*
*/

// System includes
#include <e32property.h>
#include <aknnotedialog.h>
#include <avkon.rsg>
#include <StringLoader.h>

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include <activeidle2domainpskeys.h>
#include <AiNativeUi.rsg>
#include "aidialogrenderer.h"
#include "ainativeuiplugins.h"
#ifdef __COVER_DISPLAY
#include "aisecondarydisplayapi.h"
#endif


// AVKON headers
#include <AknNoteWrappers.h>             // for Note Wrappers
#include <AknPopup.h>                    // for Popup menus
#include <AknWaitDialog.h>               // for CAknWaitDialog
#include <StringLoader.h>                // for StringLoader
#include <eiktxlbm.h>                    // for CTextListBoxModel
#include <AknLists.h>                    // for AknPopupListEmpty
#include <bautils.h>                     // for BaflUtils

// Phonesettings headers
#include <PsetContainer.h>               // for CPsetContainer
#include <PsetNetwork.h>                 // for CPsetNetwork
#include <PsetSAObserver.h>              // for CPsetSAObserver
#include <gsmerror.h>                    // for GSM-specific error messages

#include <GsNetworkPluginRsc.rsg>        // for resource IDs
#include <CoreApplicationUIsSDKCRKeys.h> // for KCRUidCoreApplicationUIs, TCoreAppUIsNetworkConnectionAllowed
#include <featmgr.h>

const TInt KAIAlphaTagIconAdditionalChars = 5;
// Length of long network name.
const TInt KAIAlphaTagLongNameLength = 20;
// Length of short network name.
const TInt KAIAlphaTagShortNameLength = 10;

// array granularity
const TInt KAINetworkInfoArrayGranularity = 10;

// Error note type ID - for Net view internal usage only
const TInt KAIPPErrorNote = 5001;
// Confirmation note type ID - for Net view internal usage only
const TInt KAIPPConfirmationNote = 5002;
//Information note type ID - for Net view internal usage only
const TInt KAIPPInformationNote = 5003;

const TInt KAIPLMNListPopupMenuItems = 3;

// Some resources defined for GSNetworkPlugin are used
_LIT(KGSNetworkPluginResource, "z:\\resource\\GsNetworkPluginRsc.rsc");

enum TGSSoftKeys
	{
	EGSSoftkeyQuit = 1,
	EGSSoftkeyEmpty
	};


using namespace AiNativeUiController;

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiDialogRenderer::NewLC()
//
// ----------------------------------------------------------------------------
//
CAiDialogRenderer* CAiDialogRenderer::NewLC()
    {
    CAiDialogRenderer* self = new( ELeave ) CAiDialogRenderer;
    CleanupStack::PushL( self );
	self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAiDialogRenderer::~CAiDialogRenderer()
//
// ----------------------------------------------------------------------------
//
CAiDialogRenderer::~CAiDialogRenderer()
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdFfManualSelectionPopulatedPlmnList ) )
        {
        if( iRscIndex )
            {
            CEikonEnv::Static()->DeleteResourceFile( iRscIndex );
            }
        if ( iNetwork )
            {
            if ( iSearchForNetworksActive )
            {
            // Reset back to previously used network
            iNetwork->ResetNetworkSearch();
            }
        delete iNetwork;
        }

        if ( iNetworkArray )
            {
            iNetworkArray->Reset();
            }
        delete iNetworkArray;

        if ( iDlg )
            {
            delete iDlg;
            }
        if ( iNetworkPopupList )
            {
            iNetworkPopupList->CancelPopup();
            }

        if ( iSettingsContainer )
            {
            delete iSettingsContainer;
            }
        }

    FeatureManager::UnInitializeLib();

    delete iDialog;
    delete iText;
    }

// ----------------------------------------------------------------------------
// CAiDialogRenderer::CAiDialogRenderer()
//
// ----------------------------------------------------------------------------
//
CAiDialogRenderer::CAiDialogRenderer()
    {
    }

void CAiDialogRenderer::ConstructL()
    {
    FeatureManager::InitializeLibL();

    if ( FeatureManager::FeatureSupported( KFeatureIdFfManualSelectionPopulatedPlmnList ) )
        {
        iSettingsContainer = CPsetContainer::NewL();
        iNetwork = iSettingsContainer->CreateNetworkObjectL( *this );

        iApprovedNetwork = EFalse;
        CheckAndCreateDlgL( EFalse );
        iSearchForNetworksActive = EFalse;
        iPLMNListPopulateShowIsHandling = EFalse;

        // Open the resource file.  Assume that the resource is stored on the same
        // drive the DLL is installed on.
        TFileName file;
        Dll::FileName( file );
        TParse parse;
        parse.Set( KGSNetworkPluginResource, &file, NULL );
        file = parse.FullName();
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        BaflUtils::NearestLanguageFile( eikonEnv->FsSession(), file );
        iRscIndex = eikonEnv->AddResourceFileL( file );
        }
    }

void CAiDialogRenderer::DoPublishL( CHsContentPublisher& aPlugin,
     TInt aContent, TInt aResource, TInt /*aIndex*/  )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KDeviceStatusPluginUid )
    	{
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentSIMRegStatus:
	            {
	            TInt idleState;
	            if ( RProperty::Get(
	                    KPSUidAiInformation,
	                    KActiveIdleState,
	                    idleState ) != KErrNone )
	                {
	                idleState = EPSAiBackground;
	                }
                if( aResource == EAiDeviceStatusResourceSIMRegFail )
                    {
    	            if( iShowDialog )
	                    {
	                    break;
	                    }
	                HBufC* temp = StringLoader::LoadL( 
	                    R_ACTIVEIDLE_SIM_REGISTRATION_FAILED );
	                delete iText;
	                iText = temp;
#ifdef __COVER_DISPLAY
	                iDialogId = EAiSimRegistrationFailed;
#endif
	                iShowDialog = ETrue;
                    }
	            if ( idleState == EPSAiForeground )
                    {
                    ShowDialogL();
                    }
	            break;
	            }

            case EAiDeviceStatusContentNetRegStatus:
                 {
                 MPsetNetworkSelect::TSelectMode mode =
                         MPsetNetworkSelect::ENetSelectModeAutomatic;

                 if ( iNetwork )
                     {
                     iNetwork->GetNetworkSelectMode( mode );
                     }

                 if ( mode == MPsetNetworkSelect::ENetSelectModeManual )
                     {
                     if ( aResource == EAiDeviceStatusResourceNetRegFail )
                         {
                         PopulatePLMNListL();
                         }
                     }
                 break;
                 }


	        default:
	            {
	            User::Leave( KErrNotFound );
	            break;
	            }
	        };
    	}
    else
   		{
   		User::Leave( KErrNotFound );
   		}
    }

// ----------------------------------------------------------------------------
// CAiDialogRenderer::DoCleanL()
//
// ----------------------------------------------------------------------------
//
void CAiDialogRenderer::DoCleanL( CHsContentPublisher& /*aPlugin*/, 
    TInt aContent )
    {
    switch( aContent )
        {
        case EAiDeviceStatusContentSIMRegStatus:
            {
            delete iDialog;
            iDialog = NULL;
            iShowDialog = EFalse;
            break;
            }
        default:
            {
            User::Leave( KErrNotFound );
            break;
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CAiDialogRenderer::ShowDialogL()
//
// ----------------------------------------------------------------------------
//  
void CAiDialogRenderer::ShowDialogL()
    {
    if ( iDialog )
        {
        return;
        }

    iDialog = 
        new (ELeave) CAknNoteDialog( 
            reinterpret_cast< CEikDialog** >( &iDialog ),
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ENoTimeout );

#ifdef __COVER_DISPLAY
    iDialog->PublishDialogL( iDialogId, KAICategory );
#endif

    iDialog->PrepareLC( R_AKN_INFORMATION_NOTE_DIALOG );

    iDialog->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_EMPTY );
    iDialog->SetTextL( *iText );
    iDialog->RunLD();
    }
    
// ----------------------------------------------------------------------------
// CAiDialogRenderer::FocusObtainedL()
//
// ----------------------------------------------------------------------------
//    
void CAiDialogRenderer::FocusObtainedL()
    {
    if( iShowDialog )
        {
        ShowDialogL();
        }
    }

void CAiDialogRenderer::PopulatePLMNListL()
    {
    // Ignore any subsequent EAiDeviceStatusResourcePLMNListPopulateShow content while one is handling
    if ( iPLMNListPopulateShowIsHandling )
		{
		return;
		}

    iPLMNListPopulateShowIsHandling = ETrue;
    iSearchForNetworksActive = ETrue;
    iNetwork->GetAvailableNetworksL();
    }

// ---------------------------------------------------------------------------
//
// While request to find networks is processed
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::HandleSearchingNetworksL(
                      MPsetNetworkInfoObserver::TServiceRequest aRequest )
    {
    // Create a network info array if doesn't exist
    if ( !iNetworkArray )
        {
     	iNetworkArray = new ( ELeave )
        CNetworkInfoArray( KAINetworkInfoArrayGranularity );
     	}

	if ( aRequest != MPsetNetworkInfoObserver::EServiceRequestNone )
		{
		CheckAndCreateDlgL( EFalse );
		iSearchForNetworksActive = ETrue;
		TInt res = iDlg->ExecuteLD( R_SEARCHING_NOTE );
		if ( res == EGSSoftkeyQuit )
			{
			iNetwork->CancelProcess();
			iPLMNListPopulateShowIsHandling = EFalse;
			}
		}
		else // If EServiceRequestNone received, hide the "Searching..." note
			{
			delete iDlg;
			iDlg = NULL;
			}
	}

// ---------------------------------------------------------------------------
//
// Handles network info
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::HandleNetworkInfoReceivedL(
     const CNetworkInfoArray* aInfoArray, const TInt /* aResult */ )
	{
	const TInt itemsCount = aInfoArray->Count();

	// First delete old ones
	iNetworkArray->Delete( 0, iNetworkArray->Count() );

	// Then insert found networks
	for ( TInt i = 0; i < itemsCount; i++ )
		{
		MPsetNetworkSelect::TNetworkInfo info = aInfoArray->At( i );
		iNetworkArray->InsertL( i, info );
		}

	NetworkListL(); // After search complete, show results
	}

// ---------------------------------------------------------------------------
//
// While request to change network is processed, a note is shown
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::HandleRequestingSelectedNetworkL( TBool aOngoing )
	{
	if ( aOngoing )
		{
		CheckAndCreateDlgL( ETrue );

		if ( iDlg->ExecuteLD( R_REQUESTING_NOTE ) == EGSSoftkeyQuit )
			{
			iNetwork->CancelProcess();

			CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog(
			 CAknNoteDialog::EConfirmationTone,
			 CAknNoteDialog::EShortTimeout );
			dlg->ExecuteLD( R_REGISTRATION_INTERRUPTED );
			iApprovedNetwork = EFalse;
			iPLMNListPopulateShowIsHandling = EFalse;
			}
		}
	else
		{
		delete iDlg;
		iDlg = NULL;
		}
	}

// ---------------------------------------------------------------------------
//
// Handle network change
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::HandleNetworkChangedL(
                 const MPsetNetworkSelect::TNetworkInfo& aCurrentInfo,
                 const RMobilePhone::TMobilePhoneRegistrationStatus& aStatus,
                 const TInt aResult )
	{
    switch ( aStatus )
		{
	     case RMobilePhone::ERegisteredOnHomeNetwork:
	         {
	         ShowNoteL( R_HOME_NETWORK_SELECTED, KAIPPConfirmationNote );
	         PurgeNetworkList();
	         break;
	         }
	     case RMobilePhone::ERegisteredRoaming:
	         {
	         HBufC* stringholder = NULL;
	         if ( aCurrentInfo.iLongName.Length() > 0 &&
	             aCurrentInfo.iLongName.Length() <=
	             MPsetNetworkSelect::ENetLongNameSize )
	             {
	             stringholder = StringLoader::LoadLC( R_NETSL_NETWORKSELECTED,
	                                                  aCurrentInfo.iLongName );
	             }
	         else if (aCurrentInfo.iShortName.Length() > 0 &&
	             aCurrentInfo.iShortName.Length() <=
	             MPsetNetworkSelect::ENetShortNameSize )
	             {
	             stringholder = StringLoader::LoadLC( R_NETSL_NETWORKSELECTED,
	                                                  aCurrentInfo.iShortName );
	         }

	         CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
	         if ( stringholder )
	             {
                 note->ExecuteLD( *stringholder );
                 CleanupStack::PopAndDestroy( stringholder );
	             }
	         
	         //After list has been used, clear it.
	         PurgeNetworkList();
	         break;
	         }
	     case RMobilePhone::ERegistrationUnknown:
	     case RMobilePhone::ENotRegisteredNoService:
	     case RMobilePhone::ENotRegisteredEmergencyOnly:
	     case RMobilePhone::ENotRegisteredSearching:
	     case RMobilePhone::ERegisteredBusy:
	     case RMobilePhone::ERegistrationDenied:
	     default:
	         break;
	    }

	iPLMNListPopulateShowIsHandling = EFalse;
	iSearchForNetworksActive = EFalse;
	}

// ---------------------------------------------------------------------------
//
// Handles errors.
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::HandleNetworkErrorL(
	const MPsetNetworkInfoObserver::TServiceRequest aRequest,
	const TInt aError )
	{
	TInt resourceTxt = KErrNone;

	switch ( aRequest )
		{
		case MPsetNetworkInfoObserver::EServiceRequestGetNetworkInfo:
		    {
            iNetwork->CancelProcess();
            iApprovedNetwork = EFalse;
            break;
		    }
		case MPsetNetworkInfoObserver::EServiceRequestSetNetwork:
		    {
		    }
		default:
		    {
		   break;
		    }
		}

	TBool ignore = EFalse;

	switch ( aError )
		{
		case KErrGsmNetCauseCallActive:
		    {
            ignore = ETrue; // Do not show an error
            break;
		    }
		case KErrGsm0707NoNetworkService:
		    {
            resourceTxt = R_NO_NETWORK_FOUND;
            break;
		    }
		case KErrGsmOfflineOpNotAllowed:
		    {
            resourceTxt = R_OFFLINE_MODE;
            break;
		    }
		default:
		    {
            resourceTxt = R_NO_NETWORK_ACCESS;
            break;
		    }
		}

	if ( !ignore )
		{
		ShowNoteL( resourceTxt, KAIPPErrorNote );
		iApprovedNetwork = EFalse;
		}
	}

// ---------------------------------------------------------------------------
//
// Creates list of Network providers
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::NetworkListL()
     {
     //if no net items were found, do not show list
     if ( iNetworkArray->Count() > 0 )
         {
         while ( !iApprovedNetwork )
             {
             AknPopupListEmpty<CEikFormattedCellListBox>* list;
             list = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
             CleanupStack::PushL( list );

             if ( iNetworkPopupList )
                 {
                 iNetworkPopupList->CancelPopup();
                 iNetworkPopupList = NULL;
                 } //

             iNetworkPopupList =
                 CAknPopupList::NewL( list, R_AVKON_SOFTKEYS_OK_CANCEL );

             list->ConstructL( iNetworkPopupList,
                               CEikListBox::ELeftDownInViewRect );
             list->CreateScrollBarFrameL( ETrue );
             list->ScrollBarFrame()->SetScrollBarVisibilityL(
                 CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

             // Fill in the list texts
             CDesCArrayFlat* items =
                 new ( ELeave ) CDesCArrayFlat( KAIPLMNListPopupMenuItems );
             CleanupStack::PushL( items );

             const TInt itemsCount = iNetworkArray->Count();

             for ( TInt loop = 0; loop < itemsCount; loop++ )
                 {
                 HBufC* insertString =
                     HBufC::NewLC(
                         KAIAlphaTagLongNameLength + KAIAlphaTagIconAdditionalChars );
                 MPsetNetworkSelect::TNetworkInfo info =
                     iNetworkArray->At( loop );

                 if ( info.iLongName.Length() <= 0 ||
                      info.iLongName.Length() > KAIAlphaTagLongNameLength )
                     {
                     // Short name received
                     if ( info.iShortName.Length() > 0 &&
                         info.iShortName.Length() <= KAIAlphaTagShortNameLength )
                         {
                         insertString->Des().Append( info.iShortName );
                         } //
                     } //
                 // Long name received
                 else
                     {
                     insertString->Des().Append( info.iLongName );
                     } //

                 items->AppendL( *insertString );
                 CleanupStack::PopAndDestroy();
                 }

             CTextListBoxModel* model = list->Model();
             model->SetItemTextArray( items );
             model->SetOwnershipType( ELbmOwnsItemArray );
             CleanupStack::Pop( items ); // Listbox model now owns this

             // Set title for list
             SetTitleToPopupL( *iNetworkPopupList, R_NETSL_FOUNDOPERATORS );

             TInt res = 0;
             // Show the PLMN list
             if ( items->Count() )
                 {
                 res = iNetworkPopupList->ExecuteLD();
                 } //

             iNetworkPopupList = NULL;
             iApprovedNetwork = ETrue; // User might want to quit
             TInt selection = list->CurrentItemIndex();

             CleanupStack::PopAndDestroy( list );

             if ( res )
                 {
                 MPsetNetworkSelect::TNetworkInfo info =
                     iNetworkArray->At( selection );
                 info.iMode = MPsetNetworkSelect::ENetSelectModeManual;
                 iNetwork->SelectNetworkL( info );
                 } //
             else // User selected "Cancel"
                 {
                 // Reset back to previously used network
                 iNetwork->ResetNetworkSearch();
                 iSearchForNetworksActive = EFalse;
                 // After list has been used, clear it
                 PurgeNetworkList();
                 iPLMNListPopulateShowIsHandling = EFalse;
                 } //
             }
         }

     iApprovedNetwork = EFalse; // Initialize before new search
     }

// ---------------------------------------------------------------------------
//
// Creates dialog, if it is does not exist yet.
//
// ---------------------------------------------------------------------------
//
void CAiDialogRenderer::CheckAndCreateDlgL( TBool aDelayOff )
	{
	if ( !iDlg )
	   {
	   iDlg = new ( ELeave ) CAknWaitDialog(
	       reinterpret_cast<CEikDialog**> ( &iDlg ), aDelayOff );
	   }
	}

// ---------------------------------------------------------------------------
//
// Sets a title to a given popup list.
//
// ---------------------------------------------------------------------------
void CAiDialogRenderer::SetTitleToPopupL( CAknPopupList& aList, TInt aTitleID )
	{
	HBufC* text = StringLoader::LoadLC( aTitleID );

	aList.SetTitleL( *text );

	CleanupStack::PopAndDestroy( text );
	text = NULL;
	}

// ---------------------------------------------------------------------------
//
// Shows note.
//
// ---------------------------------------------------------------------------
//
void CAiDialogRenderer::ShowNoteL( TInt aResourceId, TInt aType )
	{
	HBufC* string = StringLoader::LoadLC( aResourceId );

	switch ( aType )
		{
		case KAIPPErrorNote:
		   {
		   CAknErrorNote* note = new ( ELeave ) CAknErrorNote ( ETrue );
		   note->ExecuteLD( *string );
		   break;
		   }
		case KAIPPConfirmationNote:
		   {
		   CAknConfirmationNote* note =
		       new ( ELeave ) CAknConfirmationNote( ETrue );
		   note->ExecuteLD( *string );
		   break;
		   }
		case KAIPPInformationNote:
		   {
		   CAknInformationNote* note =
		       new ( ELeave ) CAknInformationNote( ETrue );
		   note->ExecuteLD( *string );
		   break;
		   }
		default:
		   break;
		}

	CleanupStack::PopAndDestroy( string );
	}

// ---------------------------------------------------------------------------
//
// Empties fetched network provider's list
//
// ---------------------------------------------------------------------------
//
void CAiDialogRenderer::PurgeNetworkList()
	{
	if ( iNetworkArray )
		{
		iNetworkArray->Reset();
		delete iNetworkArray;
		iNetworkArray = NULL;
		}
	}

// End of file
