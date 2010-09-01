/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  appui implementation
 *
*/


#include "tsfswappui.h"
#include "tsfswobservers.h"

// --------------------------------------------------------------------------
// CTsFswAppUi::~CTsFswAppUi
// --------------------------------------------------------------------------
//
CTsFswAppUi::~CTsFswAppUi()
    {
    // empty
    }

// --------------------------------------------------------------------------
// CTsFswAppUi::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswAppUi::ConstructL()
    {
    CAknAppUi::BaseConstructL( ENoAppResourceFile | EAknEnableSkin );
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    windowGroup.EnableFocusChangeEvents();
    windowGroup.EnableGroupListChangeEvents();
    }

// --------------------------------------------------------------------------
// CTsFswAppUi::SetTaskListObserver
// --------------------------------------------------------------------------
//
void CTsFswAppUi::SetTaskListObserver( MTsFswTaskListObserver& aTaskListObserver )
    {
    iTaskListObserver = &aTaskListObserver;
    }

// --------------------------------------------------------------------------
// CTsFswAppUi::SetResourceObserver
// --------------------------------------------------------------------------
//
void CTsFswAppUi::SetResourceObserver( MTsFswResourceObserver& aResourceObserver )
    {
    iResourceObserver = &aResourceObserver;
    }

// --------------------------------------------------------------------------
// CTsFswAppUi::HandleWsEventL
// --------------------------------------------------------------------------
//
void CTsFswAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
    {
    TInt eventType = aEvent.Type();

    if ( eventType == EEventFocusGroupChanged
            || eventType == EEventWindowGroupListChanged )
        {
        if ( iTaskListObserver )
            {
            iTaskListObserver->UpdateTaskList();
            }
        }

    CAknAppUi::HandleWsEventL( aEvent, aDestination );
    }

// --------------------------------------------------------------------------
// CTsFswAppUi::HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CTsFswAppUi::HandleResourceChangeL( TInt aType ) // codescanner::LFunctionCantLeave (virtual)
    {
    if ( iResourceObserver )
        {
        iResourceObserver->HandleResourceChange( aType );
        }
    }


// end of file
