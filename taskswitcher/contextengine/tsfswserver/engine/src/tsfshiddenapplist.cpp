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
* Description:  Class that checks if chosen applications should be hidden
*                from the task swap.
*
*/


// INCLUDES
#include "tsfshiddenapplist.h"
#include <apgwgnam.h>

#include <eikenv.h>

// the maximum size of array (25 32-bit UIDs equal 100 bytes)
const TInt KMaxHiddenApps = 25;

/**
 * Category id for the hidden application list.
 */
const TUid KPSUidUikon = { 0x101F8773 };
const TUint32 KUikAppHiddenList  = 0x00000010;

/**
 * Apps that are always hidden, no matter what settings the system reports.
 */
const TUint32 KTsFsAlwaysHiddenUidArray[] =
    {
    0x100056CF, // screensaver
    0x102750F0 // Idle App Uid
    };

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::NewL
// -----------------------------------------------------------------------------
//
CTsFsHiddenAppList* CTsFsHiddenAppList::NewL(
        MTsFsHiddenAppListObserver& aObserver )
    {
    CTsFsHiddenAppList* self =
        new ( ELeave ) CTsFsHiddenAppList( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::NewLC
// -----------------------------------------------------------------------------
//
CTsFsHiddenAppList* CTsFsHiddenAppList::NewLC(
        MTsFsHiddenAppListObserver& aObserver )
    {
    CTsFsHiddenAppList* self = CTsFsHiddenAppList::NewL( aObserver );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::~CTsFsHiddenAppList
// -----------------------------------------------------------------------------
//
CTsFsHiddenAppList::~CTsFsHiddenAppList()
    {
    Cancel();

    ResetArray();
    delete iHiddenAppList;

    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::IsHiddenL
// -----------------------------------------------------------------------------
//
TBool CTsFsHiddenAppList::IsHiddenL( TUid aAppUid,
                                     const RWsSession& aWsSession,
                                     TInt aWgId )
    {
    TBool ret = EFalse;

    CApaWindowGroupName* windowName =
                        CApaWindowGroupName::NewLC( aWsSession, aWgId );

    if ( windowName->Hidden() )
        {
        ret = ETrue;
        }
    else
        {
        for ( TInt i = 0, ie = iHiddenAppList->Count(); i != ie; ++i )
            {
            if ( aAppUid.iUid == iHiddenAppList->At( i ) )
                {
                ret = ETrue;
                break;
                }
            }
        if ( !ret )
            {
            for ( TInt i = 0,
                    arrCnt = sizeof( KTsFsAlwaysHiddenUidArray ) / sizeof( TUint32 );
                    i != arrCnt; ++i )
                {
                if ( KTsFsAlwaysHiddenUidArray[i] == aAppUid.iUid )
                    {
                    ret = ETrue;
                    break;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( windowName );
    return ret;
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::CTsFsHiddenAppList
// -----------------------------------------------------------------------------
//
CTsFsHiddenAppList::CTsFsHiddenAppList(
        MTsFsHiddenAppListObserver& aObserver )
            :CActive( CActive::EPriorityHigh ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::ConstructL
// -----------------------------------------------------------------------------
//
void CTsFsHiddenAppList::ConstructL()
    {
    const TInt KMaxGranularity = 4;
    CActiveScheduler::Add( this );

    iHiddenAppList = new (ELeave) CArrayFixFlat<TInt>( KMaxGranularity );
    User::LeaveIfError( iProperty.Attach( KPSUidUikon, KUikAppHiddenList, EOwnerThread ) );

    // must not call RunL directly from here as things are not fully initialized yet
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::ResetArray
// -----------------------------------------------------------------------------
//
void CTsFsHiddenAppList::ResetArray()
    {
    if ( iHiddenAppList )
        {
        iHiddenAppList->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::UpdateListL
// -----------------------------------------------------------------------------
//
void CTsFsHiddenAppList::UpdateListL()
    {
    ResetArray();

    // hidden list array
    TBuf16 <2*KMaxHiddenApps> hiddenList;
    iProperty.Get( hiddenList );

    for ( TInt i = 0; i < hiddenList.Length(); i = i + 2 )
        {
        TUint32 uid = 0x00000000;
        uid = hiddenList[i];
        uid = uid << 16; // codescanner::magicnumbers
        uid |= hiddenList[i+1];

        if ( uid != 0 )
            {
            iHiddenAppList->AppendL( uid );
            }
        else
            {
            break;
            }
        }

    iObserver.HiddenAppListUpdated();
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::DoCancel
// -----------------------------------------------------------------------------
//
void CTsFsHiddenAppList::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CTsFsHiddenAppList::RunL
// -----------------------------------------------------------------------------
//
void CTsFsHiddenAppList::RunL()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    UpdateListL();
    }


// end of file
