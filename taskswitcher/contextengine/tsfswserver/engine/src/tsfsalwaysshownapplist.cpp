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
* Description:  File containing application list classes
 *
*/


//INCLUDES:
#include <apaid.h>
#include "tsfsalwaysshownapplist.h"

//CONSTANTS:
/* Array for all application uid that need to be always displayed (if they are running).
   Also, there will be no Close option for such apps. */
const TUint32 KTsFsUidArray[] =
    {
    0x0EFC346A //Search App Uid
/* never show menu app */    //,0x101F4CD2 // Menu App Uid
    };

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::NewL
// --------------------------------------------------------------------------
//
CTsFsAlwaysShownAppList* CTsFsAlwaysShownAppList::NewL()
    {
    CTsFsAlwaysShownAppList* self =
        new (ELeave) CTsFsAlwaysShownAppList;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::CTsFsAlwaysShownAppList
// --------------------------------------------------------------------------
//
CTsFsAlwaysShownAppList::CTsFsAlwaysShownAppList()
    {
    }

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::~CTsFsAlwaysShownAppList
// --------------------------------------------------------------------------
//
CTsFsAlwaysShownAppList::~CTsFsAlwaysShownAppList( )
    {
    iAppList.Close();
    iAppArcSession.Close();
    }

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::ConstructL
// --------------------------------------------------------------------------
//
void CTsFsAlwaysShownAppList::ConstructL( )
    {
    User::LeaveIfError( iAppArcSession.Connect() );
    }

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::InitializeAlwaysShownListL
// Applications that are always shown in fast swap are stored
// in different their own list whether they exist
// in the phone rom or not
// --------------------------------------------------------------------------
//
void CTsFsAlwaysShownAppList::InitializeAlwaysShownListL( )
    {
    iAppList.Reset();
    // order of always shown applications
    TApaAppInfo applicationInfo;
    for (TInt index = 0,
            sizeOfArray = ( sizeof(KTsFsUidArray) / sizeof( TUint32 ) );
            index < sizeOfArray; index++ )
        {
        // if exists in phone rom
        if ( iAppArcSession.GetAppInfo( applicationInfo,
                TUid::Uid( KTsFsUidArray[index] ) )!= KErrNotFound )
            {
            iAppList.AppendL( TUid::Uid( KTsFsUidArray[index] ) );
            }
        }
    }

// --------------------------------------------------------------------------
// CTsFsAlwaysShownAppList::IsAlwaysShownApp
// If application uid exists in the always shown application list
// --------------------------------------------------------------------------
//
TBool CTsFsAlwaysShownAppList::IsAlwaysShownApp( TUid aAppUid )
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
