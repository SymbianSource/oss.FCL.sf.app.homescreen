/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for controlling stylus popup menu.
*
*/


#include <aknpreviewpopupcontroller.h>
#include <eikmobs.h>
#include <aknutils.h>
#include <barsread.h>

#include "aistyluspopupmenu.h"
#include "aistyluspopupmenucontent.h"

const TInt KDefaultPopUpShowDelay = 0;
const TInt KDefaultPopUpHideDelay = 10000000; // 10 seconds


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenu::CAiStylusPopUpMenu( MEikMenuObserver* aObserver, 
    const TPoint& aPoint,
    CAknPreviewPopUpController* aPopup ) 
    : iPosition ( aPoint ),
     iMenuObserver( aObserver ),
     iPreviewPopup( aPopup )
    {
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::ConstructL
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::ConstructL()
    {
    iContent = CAiStylusPopUpMenuContent::NewL();
    
    iController = CAknPreviewPopUpController::NewL( *iContent,
            CAknPreviewPopUpController::ELayoutSubMenu );
    iController->SetPopUpShowDelay( KDefaultPopUpShowDelay );
    iController->SetPopUpHideDelay( KDefaultPopUpHideDelay );
    iController->SetPosition( iPosition );
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::NewL
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenu* CAiStylusPopUpMenu::NewL( 
                            MEikMenuObserver* aObserver, const TPoint& aPoint,
                            CAknPreviewPopUpController* aPopup )
    {
    if( !AknLayoutUtils::PenEnabled() )
        {
        return NULL;
        }

    CAiStylusPopUpMenu* self = CAiStylusPopUpMenu::NewLC( aObserver, 
                                                            aPoint,
                                                            aPopup );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::NewL
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenu* CAiStylusPopUpMenu::NewL( 
                            MEikMenuObserver* aObserver, const TPoint& aPoint )
    {
    if( !AknLayoutUtils::PenEnabled() )
        {
        return NULL;
        }

    CAiStylusPopUpMenu* self = CAiStylusPopUpMenu::NewLC( aObserver, 
                                                            aPoint, NULL );
    CleanupStack::Pop( self );
    return self;
    }
  
// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::NewLC
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenu* CAiStylusPopUpMenu::NewLC(
                            MEikMenuObserver* aObserver, const TPoint& aPoint,
                            CAknPreviewPopUpController* aPopup )
    {
    if( !AknLayoutUtils::PenEnabled() )
        {
        return NULL;
        }

    CAiStylusPopUpMenu* self = new ( ELeave ) CAiStylusPopUpMenu( aObserver,
                                                                    aPoint,
                                                                    aPopup );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenu::~CAiStylusPopUpMenu()
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        delete iController;
        delete iContent;
        }
    }


// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::AddMenuItemL
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::AddMenuItemL( const TDesC& aItem, 
                                                        const TInt aCommandId )
    {
    if(AknLayoutUtils::PenEnabled())
        {
        iContent->AddMenuItemL( aItem, aCommandId, *this );
        }
    }



// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::RemoveMenuItem
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::RemoveMenuItem( const TInt aCommandId )
    {
    if(AknLayoutUtils::PenEnabled())
        {
        iContent->RemoveMenuItem( aCommandId );
        }
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::SetItemDimmed
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::SetItemDimmed( const TInt aCommandId, const TBool aDimmed )
    {
    if(AknLayoutUtils::PenEnabled())
        {
        iContent->SetItemDimmed( aCommandId, aDimmed );
        }
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::ShowMenu
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::ShowMenu()
    {
    if(AknLayoutUtils::PenEnabled())
        {
        iController->ShowPopUp();
        }   
    }


// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::SetPosition
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::SetPosition( const TPoint& aPoint )
    {
    if(AknLayoutUtils::PenEnabled())
        {
        iController->SetPosition( aPoint );
        }
    }

// ---------------------------------------------------------------------------
// CAiStylusPopUpMenu::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::ConstructFromResourceL( TResourceReader&
                                                           aReader )
    {
    if(!AknLayoutUtils::PenEnabled())
        {
        return;
        }

    TInt count = aReader.ReadInt16();
    if ( count > KAiStylusMenuMaxItems )
        {
        // Only allowed number of items read from resources.
        count = KAiStylusMenuMaxItems;
        }
    TInt commandId;
    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC text = aReader.ReadTPtrC();
        commandId = aReader.ReadInt32();
        iContent->AddMenuItemL( text, commandId, *this );
        aReader.ReadInt32(); // extension link
        iController->UpdateContentSize();
        }
    }

// ---------------------------------------------------------------------------
// From class MCoeControlObserver.
// Handles stylus tap events by forwarding the event to the observer.
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenu::HandleControlEventL( CCoeControl* aControl,
                                                        TCoeEvent aEventType )
    {
    if(!AknLayoutUtils::PenEnabled())
        {
        return;
        }

    if ( aControl && aEventType == EEventStateChanged )
        {
        if ( iPreviewPopup )
            {
            iPreviewPopup->HidePopUp();
            }
        iController->HidePopUp();
        iMenuObserver->ProcessCommandL( iContent->CommandId( aControl ) );
        }
    }

