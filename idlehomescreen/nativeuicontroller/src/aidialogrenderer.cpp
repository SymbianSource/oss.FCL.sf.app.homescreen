/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
    return self;
    }

// ----------------------------------------------------------------------------
// CAiDialogRenderer::~CAiDialogRenderer()
//
// ----------------------------------------------------------------------------
//
CAiDialogRenderer::~CAiDialogRenderer()
    {
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
    
// ----------------------------------------------------------------------------
// CAiDialogRenderer::DoPublishL()
//
// ----------------------------------------------------------------------------
//
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
	        default:
	            {
	            User::Leave( KErrNotFound );
	            break;
	            }
	        }
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

// End of file
