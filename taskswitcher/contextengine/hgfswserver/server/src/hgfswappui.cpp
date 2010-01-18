/*
 * ===========================================================================
 *  Name        : hgfswappui.cpp
 *  Part of     : Huriganes / Fast Swap Server
 *  Description : appui implementation
 *  Version     : %version: sa1spcx1#6 %
 *
 *  Copyright © 2008 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ===========================================================================
 */

#include "hgfswappui.h"
#include "hgfswobservers.h"

// --------------------------------------------------------------------------
// CHgFswAppUi::~CHgFswAppUi
// --------------------------------------------------------------------------
//
CHgFswAppUi::~CHgFswAppUi()
    {
    // empty
    }

// --------------------------------------------------------------------------
// CHgFswAppUi::ConstructL
// --------------------------------------------------------------------------
//
void CHgFswAppUi::ConstructL()
    {
    CAknAppUi::BaseConstructL( ENoAppResourceFile | EAknEnableSkin );
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    windowGroup.EnableFocusChangeEvents();
    windowGroup.EnableGroupListChangeEvents();
    }

// --------------------------------------------------------------------------
// CHgFswAppUi::SetTaskListObserver
// --------------------------------------------------------------------------
//
void CHgFswAppUi::SetTaskListObserver( MHgFswTaskListObserver& aTaskListObserver )
    {
    iTaskListObserver = &aTaskListObserver;
    }

// --------------------------------------------------------------------------
// CHgFswAppUi::SetResourceObserver
// --------------------------------------------------------------------------
//
void CHgFswAppUi::SetResourceObserver( MHgFswResourceObserver& aResourceObserver )
    {
    iResourceObserver = &aResourceObserver;
    }

// --------------------------------------------------------------------------
// CHgFswAppUi::HandleWsEventL
// --------------------------------------------------------------------------
//
void CHgFswAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
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
// CHgFswAppUi::HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CHgFswAppUi::HandleResourceChangeL( TInt aType ) // codescanner::LFunctionCantLeave (virtual)
    {
    if ( iResourceObserver )
        {
        iResourceObserver->HandleResourceChange( aType );
        }
    }


// end of file
