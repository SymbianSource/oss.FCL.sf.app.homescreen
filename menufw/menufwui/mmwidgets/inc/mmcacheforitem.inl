/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*  Version     : 2 << Don't touch! Updated by Synergy at check-out.
*
*/

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TBool CMmCacheForItem::IsValid() const
    {
    return iIsValid;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void CMmCacheForItem::SetSubcellsSetupCode( TMmSubcellsSetupCode aSubcellsSetupCode )
    {
    iSubcellsSetupCode = aSubcellsSetupCode;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TMmSubcellsSetupCode CMmCacheForItem::GetSubcellsSetupCode() const
    {
    __ASSERT_DEBUG( iIsValid, User::Invariant() );
    return iSubcellsSetupCode;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void CMmCacheForItem::MarkAsValidL()
    {
    if ( !iIsValid )
        {
        iIsValid = ETrue;
        UpdateIconListL();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void CMmCacheForItem::MarkAsInvalid()
    {
    iIsValid = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TBool CMmCacheForItem::IsCurrent()
    {
    return iIsCurrent;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void CMmCacheForItem::SetCurrent( TBool aCurrent )
    {
    iIsCurrent = aCurrent;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline const TDesC& CMmCacheForItem::GetItemText() const
    {
    __ASSERT_DEBUG( iIsValid, User::Invariant() );
    return iItemText;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline RBuf& CMmCacheForItem::GetItemTextForModifications()
    {
    return iItemText;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TSize CMmCacheForItem::GetSize() const
    {
    __ASSERT_DEBUG( iIsValid, User::Invariant() );
    return iItemSize;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void CMmCacheForItem::SetSize( TSize aItemSize )
    {
    iItemSize = aItemSize;
    }
