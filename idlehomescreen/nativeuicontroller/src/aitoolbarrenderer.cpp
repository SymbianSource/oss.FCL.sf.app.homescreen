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
* Description:  Touch Toolbar
*
*/


#include "aitoolbarrenderer.h"
#include "aifweventhandler.h"
#include "aicontentobserver.h"
#include "activeidle2domaincrkeys.h"
#include "activeidle2domainpskeys.h"
#include "aiscutplugindomaincrkeys.h"
#include <aipropertyextension.h>
#include "ainativeuiplugins.h"

#include <AiNativeUi.rsg>
#include <akntoolbar.h>
#include <aknbutton.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <e32property.h>
#include <aknappui.h>

#include "centralrepository.h"


#include "debug.h"

using namespace AiNativeUiController;

// Action to send to framework
_LIT( KOpenAppByIndex,              "Shortcut/LaunchByIndex(%d)" );
_LIT( KOpenAppByIndexAlternate,     "Shortcut/LaunchByIndexAlternate(%d)");

const TInt KOpenAppByIndexParameterLength = 2;

const TInt KFirstButton  = 0;
const TInt KSecondButton = 1;
const TInt KThirdButton  = 2;

const TInt KRadix = 10;

static TInt IndexLength( TInt aIndex )
    {
    TInt length = 0;

    if ( aIndex < 0 )
        {
        ++length;
        }

    do
        {
        aIndex /= KRadix;
        ++length;
        }
    while ( aIndex != 0 );

    return length;
    }

static HBufC* CreateEventStringL( const TDesC& aFrom, TInt aEventIndex )
    {
    HBufC* buffer = NULL;

    TInt bufferLength = aFrom.Length() -
                        KOpenAppByIndexParameterLength +
                        IndexLength( aEventIndex );

    buffer = HBufC::NewL(bufferLength);

    TPtr bufferPtr = buffer->Des();
    bufferPtr.Format( aFrom, aEventIndex );

    return buffer;
    }

// ========== MEMBER FUNCTIONS ================================================

CAiToolbarRenderer::CAiToolbarRenderer( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar )
    : iToolbar( aToolbar ),
    iAiFwEventHandler( aAiFwEventHandler )

    {
    // no implementation required
    }

void CAiToolbarRenderer::ConstructL( )
    {
    iToolbarEnabled = EFalse;

    TInt err = KErrNone;

    CRepository *cr = CRepository::NewLC( TUid::Uid( KCRUidActiveIdleLV ) );
    err = cr->Get( KAITouchToolbarVisible, iToolbarEnabled );
    CleanupStack::PopAndDestroy( cr );
    __PRINT (__DBG_FORMAT("XAI: Toolbar value in cenrep %d err = %d"), iToolbarEnabled , err);

    // Hide the toolbar because it is disabled via cenrep
    if ( err != KErrNone || !iToolbarEnabled )
        {
        iToolbar.SetToolbarObserver( NULL );
        iToolbar.SetToolbarVisibility( EFalse, EFalse );
        }
    else
        {
        iToolbar.SetToolbarObserver( this );
        iToolbar.SetToolbarVisibility( ETrue, EFalse );
        }

    }

CAiToolbarRenderer* CAiToolbarRenderer::NewLC( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar )
    {
    CAiToolbarRenderer* self = new (ELeave) CAiToolbarRenderer( aAiFwEventHandler, aToolbar );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

CAiToolbarRenderer* CAiToolbarRenderer::NewL( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar )
    {
    CAiToolbarRenderer* self = CAiToolbarRenderer::NewLC( aAiFwEventHandler, aToolbar );
    CleanupStack::Pop(); // self
    return self;
    }

CAiToolbarRenderer::~CAiToolbarRenderer()
    {
    // We don't want anymore events from toolbar
    iToolbar.SetToolbarObserver( NULL );
    iButtons.ResetAndDestroy();
    }

void CAiToolbarRenderer::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* /*aToolbar*/ )
    {
    // do nothing
    }

void CAiToolbarRenderer::OfferToolbarEventL( TInt aCommand )
    {
    TInt modifiers = iToolbar.EventModifiers();


    if ( aCommand >= 0 && aCommand < iButtons.Count())
        {
        CAiToolbarButton* button = iButtons[aCommand];
        if ( button )
            {

            HBufC *event = NULL;
            if ( modifiers == CAknToolbar::ELongPress )
                {
                event = CreateEventStringL( KOpenAppByIndexAlternate, button->ShortcutId() );
                CleanupStack::PushL( event );
                }
            else
                {
                event = CreateEventStringL( KOpenAppByIndex, button->ShortcutId() );
                CleanupStack::PushL( event );
                }
            iAiFwEventHandler.HandlePluginEvent( *event );
            CleanupStack::PopAndDestroy( event );
            }
        }
    }

void CAiToolbarRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                     TInt aContent,
                                     const TDesC16& aText,
                                     TInt aIndex )
    {
    // Only interested on certain publish events
    if (  aPlugin.PublisherInfoL()->iUid == KShortcutPluginUid &&
          aContent == EAiScutContentShortcutToolbarCaption &&
        ( aIndex == EAiScutToolbarFirst ||
          aIndex == EAiScutToolbarSecond ||
          aIndex == EAiScutToolbarThird )
        )
        {
        CAiToolbarButton* button = ButtonL( aIndex );
        if ( button )
            {
            button->SetTooltipL(aText);
            iCommitted = EFalse;
            }
        }
    }

void CAiToolbarRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                     TInt aContent,
                                     const TDesC8& aBuf,
                                     TInt aIndex )
    {
    // Only interested on certain publish events
    if (  aPlugin.PublisherInfoL()->iUid == KShortcutPluginUid &&
          aContent == EAiScutContentShortcutToolbarIcon &&
        ( aIndex == EAiScutToolbarFirst ||
          aIndex == EAiScutToolbarSecond ||
          aIndex == EAiScutToolbarThird )
        )
        {
        CGulIcon* icon = MAiContentObserver::UnpackPtr<CGulIcon>( aBuf );
        CAiToolbarButton* button = NULL;
        if ( !icon )
            {
            User::Leave(KErrArgument);
            }
        // Replace the old button with a new button
        button = ButtonL( aIndex );
        if ( button )
            {
            button->SetIcon(icon);
            iCommitted = EFalse;
            }
        // icons ownership is transferred to us
        // so we need to delete the icon in error situations
        else
            {
            delete icon;
            }
        }
    else
        {
        // We have received stuff we cannot publish.
        User::Leave(KErrNotFound);
        }
    }

CAiToolbarButton* CAiToolbarRenderer::ButtonL( TInt aShortcutId )
    {
    TInt index = KErrNotFound;
    CAiToolbarButton *button = NULL;
    switch ( aShortcutId )
        {
        case EAiScutToolbarFirst:
            index = KFirstButton;
            break;
        case EAiScutToolbarSecond:
            index = KSecondButton;
            break;
        case EAiScutToolbarThird:
            index = KThirdButton;
            break;
        default:
            break;
        }

    if ( index == KErrNotFound )
        {
        User::Leave(KErrNotFound);
        }
    // No button yet => Create new
    if ( index >= iButtons.Count() )
        {
        button = CAiToolbarButton::NewL();
        button->SetShortcutId( aShortcutId );
        iButtons.Append( button );
        }

    // Use the old button
    else if( index >= KFirstButton && index < iButtons.Count() )
        {
        button = iButtons[index];
        button->SetShortcutId( aShortcutId );
        }
    return button;
    }


void CAiToolbarRenderer::TransactionCommittedL()
    {
    if ( !iCommitted )
        {
        for(TInt i = 0; i < iButtons.Count(); i++)
            {
            CAiToolbarButton *button = iButtons[i];
            if ( button && button->Icon())
                {
                // remove the old button
                iToolbar.RemoveItem(i);
                // and add new one
                // transfers ownership of button->ButtonL() to toolbar
                iToolbar.AddItemL( button->ButtonL(), EAknCtButton, i, 0, i );
                }
            }
         iCommitted = ETrue;
        }
    }


// End of File.
