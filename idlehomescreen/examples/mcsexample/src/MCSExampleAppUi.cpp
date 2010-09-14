/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

#include <MCSExample_0xEB753B18.rsg>
#include "MCSExample.hrh"
#include "MCSExample.pan"
#include "MCSExampleApplication.h"
#include "MCSExampleAppUi.h"
#include "MCSExampleAppView.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CMCSExampleAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin|CAknAppUi::EAknSingleClickCompatible);

    // Create view object
    iAppView = CMCSExampleAppView::NewL(ClientRect());
    }
// -----------------------------------------------------------------------------
// CMCSExampleAppUi::CMCSExampleAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMCSExampleAppUi::CMCSExampleAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppUi::~CMCSExampleAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CMCSExampleAppUi::~CMCSExampleAppUi()
    {
    if (iAppView)
        {
        delete iAppView;
        iAppView = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CMCSExampleAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case ECommand1:
            {

            // Load a string from the resource file and display it
            HBufC* textResource = StringLoader::LoadLC(R_COMMAND1_TEXT);
            CAknInformationNote* informationNote;

            informationNote = new (ELeave) CAknInformationNote;

            // Show the information Note with
            // textResource loaded with StringLoader.
            informationNote->ExecuteLD(*textResource);

            // Pop HBuf from CleanUpStack and Destroy it.
            CleanupStack::PopAndDestroy(textResource);
            }
            break;
        case ECommand2:
            {
            iAppView->ListAllItemsL();
            }
            break;
        case EHelp:
            {

            }
            break;
        case EAbout:
            {

            CAknMessageQueryDialog* dlg =
                    new (ELeave) CAknMessageQueryDialog();
            dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
            HBufC* title = iEikonEnv->AllocReadResourceLC(
                    R_ABOUT_DIALOG_TITLE);
            dlg->QueryHeading()->SetTextL(*title);
            CleanupStack::PopAndDestroy(); //title
            HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
            dlg->SetMessageTextL(*msg);
            CleanupStack::PopAndDestroy(); //msg
            dlg->RunLD();
            }
            break;
        default:
            Panic( EMCSExampleUi);
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CMCSExampleAppUi::HandleStatusPaneSizeChange()
    {
    iAppView->SetRect(ClientRect());
    }

CArrayFix<TCoeHelpContext>* CMCSExampleAppUi::HelpContextL() const
    {
#warning "Please see comment about help and UID3..."
    // Note: Help will not work if the application uid3 is not in the
    // protected range.  The default uid3 range for projects created
    // from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
    // can be self signed and installed on the device during testing.
    // Once you get your official uid3 from Symbian Ltd. and find/replace
    // all occurrences of uid3 in your project, the context help will
    // work. Alternatively, a patch now exists for the versions of 
    // HTML help compiler in SDKs and can be found here along with an FAQ:
    // http://www3.symbian.com/faq.nsf/AllByDate/E9DF3257FD565A658025733900805EA2?OpenDocument
#ifdef _HELP_AVAILABLE_
    CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    array->AppendL(TCoeHelpContext(KUidMCSExampleApp, KGeneral_Information));
    CleanupStack::Pop(array);
    return array;
#else
    return NULL;
#endif
    }

// End of File
