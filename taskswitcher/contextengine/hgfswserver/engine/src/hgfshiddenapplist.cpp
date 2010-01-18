/*
* ============================================================================
*  Name        : hgfshiddenapplist.cpp
*  Part of     : Hg Teleport / Fast Swap module
*  Description : Class that checks if chosen applications should be hidden
*                from the task swap.
*  Version     : %version: sa1spcx1#3 %
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

// INCLUDES
#include "hgfshiddenapplist.h"
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
const TUint32 KHgFsAlwaysHiddenUidArray[] =
    {
    0x100056CF // screensaver
    };

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::NewL
// -----------------------------------------------------------------------------
//
CHgFsHiddenAppList* CHgFsHiddenAppList::NewL(
        MHgFsHiddenAppListObserver& aObserver )
    {
    CHgFsHiddenAppList* self =
        new ( ELeave ) CHgFsHiddenAppList( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::NewLC
// -----------------------------------------------------------------------------
//
CHgFsHiddenAppList* CHgFsHiddenAppList::NewLC(
        MHgFsHiddenAppListObserver& aObserver )
    {
    CHgFsHiddenAppList* self = CHgFsHiddenAppList::NewL( aObserver );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::~CHgFsHiddenAppList
// -----------------------------------------------------------------------------
//
CHgFsHiddenAppList::~CHgFsHiddenAppList()
    {
    Cancel();

    ResetArray();
    delete iHiddenAppList;

    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::IsHiddenL
// -----------------------------------------------------------------------------
//
TBool CHgFsHiddenAppList::IsHiddenL( TUid aAppUid,
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
                    arrCnt = sizeof( KHgFsAlwaysHiddenUidArray ) / sizeof( TUint32 );
                    i != arrCnt; ++i )
                {
                if ( KHgFsAlwaysHiddenUidArray[i] == aAppUid.iUid )
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
// CHgFsHiddenAppList::CHgFsHiddenAppList
// -----------------------------------------------------------------------------
//
CHgFsHiddenAppList::CHgFsHiddenAppList(
        MHgFsHiddenAppListObserver& aObserver )
            :CActive( CActive::EPriorityHigh ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::ConstructL
// -----------------------------------------------------------------------------
//
void CHgFsHiddenAppList::ConstructL()
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
// CHgFsHiddenAppList::ResetArray
// -----------------------------------------------------------------------------
//
void CHgFsHiddenAppList::ResetArray()
    {
    if ( iHiddenAppList )
        {
        iHiddenAppList->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::UpdateListL
// -----------------------------------------------------------------------------
//
void CHgFsHiddenAppList::UpdateListL()
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
// CHgFsHiddenAppList::DoCancel
// -----------------------------------------------------------------------------
//
void CHgFsHiddenAppList::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CHgFsHiddenAppList::RunL
// -----------------------------------------------------------------------------
//
void CHgFsHiddenAppList::RunL()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    UpdateListL();
    }


// end of file
