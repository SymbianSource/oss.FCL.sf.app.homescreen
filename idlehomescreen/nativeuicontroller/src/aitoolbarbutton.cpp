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
* Description:  Touch Toolbar button
*
*/


#include <aknbutton.h>
#include <gulicon.h>

#include "aitoolbarbutton.h"

using namespace AiNativeUiController;

CAiToolbarButton* CAiToolbarButton::NewL()
    {
    CAiToolbarButton* self = new (ELeave) CAiToolbarButton;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CAiToolbarButton::ConstructL()
    {
    iIconNotOwned = EFalse;
    iTooltip = KNullDesC().AllocL();
    }

CAiToolbarButton::~CAiToolbarButton()
    {

    if ( !iIconNotOwned )
        {
        delete iIcon;
        iIcon = NULL;
        }
    delete iTooltip;
    iTooltip = NULL;
    }

CAiToolbarButton::CAiToolbarButton()
    {
    // no implementation
    }

void CAiToolbarButton::SetShortcutId( TInt aId )
    {
    iShortcutId = aId;
    }

TInt CAiToolbarButton::ShortcutId()
    {
    return iShortcutId;
    }

TDesC& CAiToolbarButton::Tooltip()
    {
    return *iTooltip;
    }


CGulIcon* CAiToolbarButton::Icon( TBool aTransferOwnership )
    {
    iIconNotOwned = aTransferOwnership;
    CGulIcon* icon = iIcon;
    if(iIconNotOwned)
        {
        iIcon = NULL;
        }

    return icon;
    }

void CAiToolbarButton::SetTooltipL( const TDesC& aTooltip )
    {
    HBufC *temp = aTooltip.AllocL();
    delete iTooltip;
    iTooltip = temp;
    }

void CAiToolbarButton::SetIcon( CGulIcon *aIcon, TBool aTransferOwnership )
    {
    if ( !iIconNotOwned )
        {
        delete iIcon;
        iIcon = NULL;
        }
    iIconNotOwned = aTransferOwnership;
    iIcon = aIcon;
    }

void CAiToolbarButton::SetIconExternallyOwned( TBool aValue )
    {
    iIconNotOwned = aValue;
    }

CAknButton* CAiToolbarButton::ButtonL()
    {
    // Icon ownership transferred to CAknButton
    return CAknButton::NewL( Icon( ETrue ),
                             NULL,
                             NULL,
                             NULL,
                             KNullDesC,
                             Tooltip(),
                             KAknButtonSizeFitText | KAknButtonReportOnLongPress,
                             0 );
    }

