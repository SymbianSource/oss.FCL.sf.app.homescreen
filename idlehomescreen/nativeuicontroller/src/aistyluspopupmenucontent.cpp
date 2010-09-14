/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content that is shown inside a stylus popup menu.
*
*/


#include <aknbutton.h>
#include <eikon.hrh>
#include <aknlayoutscalable_avkon.cdl.h>

#include "aistyluspopupmenucontent.h"

const TInt KItemArrayGranularity = 10;

// Item shown or dimmed
const TInt KShown = 0;
const TInt KHidden = 0x02;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenuContent::CAiStylusPopUpMenuContent() :
    iItems( KItemArrayGranularity ),
    iCommands( KItemArrayGranularity )
    {

    }


// ---------------------------------------------------------------------------
// CAiStylusPopUpMenuContent::ConstructL
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::ConstructL()
    {
    for ( TInt i=0; i<KAiStylusMenuMaxItems; i++ )
        {
        iVisibility[i] = KShown;
        }
    }


// ---------------------------------------------------------------------------
// CAiStylusPopUpMenuContent::NewL
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenuContent* CAiStylusPopUpMenuContent::NewL()
    {
    CAiStylusPopUpMenuContent* self = CAiStylusPopUpMenuContent::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CAiStylusPopUpMenuContent::NewLC
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenuContent* CAiStylusPopUpMenuContent::NewLC()
    {
    CAiStylusPopUpMenuContent* self =
                                    new ( ELeave ) CAiStylusPopUpMenuContent;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAiStylusPopUpMenuContent::~CAiStylusPopUpMenuContent()
    {
    iItems.ResetAndDestroy();
    iCommands.Reset();
    }


// ---------------------------------------------------------------------------
// Adds a new menu item to the array of items.
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::AddMenuItemL( const TDesC& aItem,
    const TInt aCommandId, MCoeControlObserver& aObserver )
    {
    if ( iItems.Count() >= KAiStylusMenuMaxItems )
        {
        return;
        }
    TInt flags = 0;
    flags = KAknButtonTextLeft | KAknButtonSizeFitText | KAknButtonNoFrame;
    CAknButton* item = CAknButton::NewL( NULL, NULL, NULL, NULL, aItem,
        KNullDesC, flags, 0 );
    CleanupStack::PushL( item );
    item->SetTextFont( AknLayoutUtils::FontFromId(
        AknLayoutScalable_Avkon::
            list_single_popup_submenu_pane_t1( 0 ).LayoutLine().FontId() ) );
    item->SetBackground( Background() );
    item->SetObserver( &aObserver );
    iItems.AppendL( item );
    CleanupStatck::Pop( item );
    iCommands.AppendL( aCommandId );    
    SizeChanged();
    }


// ---------------------------------------------------------------------------
// Removes the menu item based on the given command id and frees the
// memory occupied by it.
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::RemoveMenuItem( const TInt aCommandId )
    {
    TInt itemIndex;
    for ( itemIndex = 0; itemIndex < iItems.Count(); itemIndex++ )
        {
        if ( iCommands[itemIndex] == aCommandId )
            {
            delete iItems[itemIndex];
            iItems.Remove( itemIndex );
            iCommands.Remove( itemIndex );
            iVisibility[itemIndex] = KShown;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Hides / unhides the menu item based on the given command id
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::SetItemDimmed( const TInt aCommandId, const TBool aDimmed )
    {
    TInt itemIndex;
    for ( itemIndex = 0; itemIndex < iItems.Count(); itemIndex++ )
        {
        if ( iCommands[itemIndex] == aCommandId )
            {
            if ( aDimmed )
                {
                iVisibility[itemIndex] = KHidden;
                }
            else
                {
                iVisibility[itemIndex] = KShown;
                }
            SizeChanged();
            }
        }
    }

// ---------------------------------------------------------------------------
// Returns the command id of the specified menu item.
// ---------------------------------------------------------------------------
//
TInt CAiStylusPopUpMenuContent::CommandId( CCoeControl* aControl ) const
    {
    TInt retVal = 0;
    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        if ( iItems[i] == aControl )
            {
            retVal = iCommands[i];
            }
        }
    return retVal;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// Calculates the total size needed to display the items.
// ---------------------------------------------------------------------------
//
TSize CAiStylusPopUpMenuContent::MinimumSize()
    {
    TInt totalHeight = 0; // height of all menu items combined.
    TInt width = 0; // Width of the widest menu item.
    TSize itemSize;

    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        if ( iVisibility[i] == KShown )
            {
            itemSize = iItems[i]->MinimumSize();
            if ( itemSize.iWidth > width )
                {
                width = itemSize.iWidth;
                }
            totalHeight += itemSize.iHeight;
            }
        }
    return TSize( width, totalHeight );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// CAiStylusPopUpMenuContent::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CAiStylusPopUpMenuContent::CountComponentControls()  const
    {
    TInt count ( 0 );

    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        if ( iVisibility[i] == KShown )
            {
            count++;
            }
        }
    return count;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// CAiStylusPopUpMenuContent::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CAiStylusPopUpMenuContent::ComponentControl(
                                                        TInt aIndex ) const
    {
    TInt count ( 0 );

    for ( TInt i = 0; i< iItems.Count(); i++ )
        {
        // Skip dimmed item(s)
        if ( iVisibility[i] == KShown )
            {
            if ( count == aIndex )
                {
                return iItems[i];
                }
            count++;
            }
        }

    // Should never come here
    return NULL;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// CAiStylusPopUpMenuContent::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        // Implementation when graphics are ready.
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// CAiStylusPopUpMenuContent::SizeChanged
// ---------------------------------------------------------------------------
//
void CAiStylusPopUpMenuContent::SizeChanged()
    {
    if ( iItems.Count() == 0 )
        {
        return;
        }
    // Note:  LAF data missing at the moment
    TRect rect = Rect();
    TInt itemHeight = iItems[0]->Size().iHeight;
    TPoint topLeft = rect.iTl;
    TSize itemSize( 0, 0 );

    // Position items starting from the topmost item
    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        // Skip hidden item(s)
        if ( iVisibility[i] == KShown )
            {
            // CAknButton::MinimumSize returns the size needed by the item text.
            itemSize = iItems[i]->MinimumSize();
            iItems[i]->SetRect( TRect( topLeft, itemSize ) );
            topLeft.iY += itemHeight; // Move next item down.
            }
        }
    }
