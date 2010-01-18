/*
 * ============================================================================
 *  Name        : hgfsalwaysshownapplist.cpp
 *  Part of     : Hg Teleport
 *  Description : File containing application list classes
 *  Version     : %version: 4 %
 *
 *  Copyright 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

//INCLUDES:
#include <apaid.h>
#include "hgfsalwaysshownapplist.h"

//CONSTANTS:
/* Array for all application uid that need to be always displayed (if they are running).
   Also, there will be no Close option for such apps. */
const TUint32 KHgFsUidArray[] =
    {
    0x102750F0 // Idle App Uid
    ,0x0EFC346A //Search App Uid
/* never show menu app */    //,0x101F4CD2 // Menu App Uid
    };

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::NewL
// --------------------------------------------------------------------------
//
CHgFsAlwaysShownAppList* CHgFsAlwaysShownAppList::NewL()
    {
    CHgFsAlwaysShownAppList* self =
        new (ELeave) CHgFsAlwaysShownAppList;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::CHgFsAlwaysShownAppList
// --------------------------------------------------------------------------
//
CHgFsAlwaysShownAppList::CHgFsAlwaysShownAppList()
    {
    }

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::~CHgFsAlwaysShownAppList
// --------------------------------------------------------------------------
//
CHgFsAlwaysShownAppList::~CHgFsAlwaysShownAppList( )
    {
    iAppList.Close();
    iAppArcSession.Close();
    }

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::ConstructL
// --------------------------------------------------------------------------
//
void CHgFsAlwaysShownAppList::ConstructL( )
    {
    User::LeaveIfError( iAppArcSession.Connect() );
    }

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::InitializeAlwaysShownListL
// Applications that are always shown in fast swap are stored
// in different their own list whether they exist
// in the phone rom or not
// --------------------------------------------------------------------------
//
void CHgFsAlwaysShownAppList::InitializeAlwaysShownListL( )
    {
    iAppList.Reset();
    // order of always shown applications
    TApaAppInfo applicationInfo;
    for (TInt index = 0,
            sizeOfArray = ( sizeof(KHgFsUidArray) / sizeof( TUint32 ) );
            index < sizeOfArray; index++ )
        {
        // if exists in phone rom
        if ( iAppArcSession.GetAppInfo( applicationInfo,
                TUid::Uid( KHgFsUidArray[index] ) )!= KErrNotFound )
            {
            iAppList.AppendL( TUid::Uid( KHgFsUidArray[index] ) );
            }
        }
    }

// --------------------------------------------------------------------------
// CHgFsAlwaysShownAppList::IsAlwaysShownApp
// If application uid exists in the always shown application list
// --------------------------------------------------------------------------
//
TBool CHgFsAlwaysShownAppList::IsAlwaysShownApp( TUid aAppUid )
    {
    for ( TInt index = 0, count = iAppList.Count ( ); index < count; index++ )
        {
        if ( iAppList[index] == aAppUid )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// end of file
