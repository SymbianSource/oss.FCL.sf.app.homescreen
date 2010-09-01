/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Optimizer module for CXnDomStringPool.
*
*/



// INCLUDE FILES
#include "xndomstringpooloptimizer.h"

const TInt KMaxEstimateThreshold = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizerEntry::TXnDomStringPoolOptimizerEntry
// -----------------------------------------------------------------------------
//
TXnDomStringPoolOptimizerEntry::TXnDomStringPoolOptimizerEntry( TInt aIndex,
        const TDesC8& aString) :         
        iIndex( aIndex ),
        iString( aString )
    {
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::AddEntryL
// -----------------------------------------------------------------------------
//
void TXnDomStringPoolOptimizer::AddEntryL( TXnDomStringPoolOptimizerEntry& aEntry )
    {
    TBool positionFound = EFalse;
    
    if( iEntries.Count() > 0 )
        {    
        const TInt estimate = FindInsertionIndexEstimate(
                aEntry.iString,
                0,
                iEntries.Count() - 1 );        
        
        if( estimate != KErrNotFound )
            {
            for( TInt i = estimate; i < iEntries.Count(); i++ )
                {
                if( aEntry.iString.Compare( iEntries[i].iString ) < 0 )
                    {
                    positionFound = ETrue;
                    iEntries.InsertL( aEntry, i );
                    break;
                    }
                }
            }
        }
    
    if( !positionFound )
        {
        iEntries.AppendL( aEntry );
        }
    }
    
// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::TXnDomStringPoolOptimizerGetIndex
// -----------------------------------------------------------------------------
//
TInt TXnDomStringPoolOptimizer::GetIndex( const TDesC8& aString )
    {
    if( iEntries.Count() == 0 )
        {
        return KErrNotFound;
        }    
    
    TInt index = FindEntry( aString, 0, iEntries.Count() - 1 );
    if( index >= 0 && index < iEntries.Count() )
        {
        return iEntries[index].iIndex;
        }
    else
        {
        // Error code.
        return index;
        }
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::Close
// -----------------------------------------------------------------------------
//
void TXnDomStringPoolOptimizer::Close()
    {
    iEntries.Close();
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::Count
// -----------------------------------------------------------------------------
//
TInt TXnDomStringPoolOptimizer::Count()
    {
    return iEntries.Count();
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::Entry
// -----------------------------------------------------------------------------
//
TXnDomStringPoolOptimizerEntry& TXnDomStringPoolOptimizer::Entry(
        const TInt aIndex )
    {    
    return iEntries[ aIndex ];
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::Reset
// -----------------------------------------------------------------------------
//
void TXnDomStringPoolOptimizer::Reset()
    {
    iEntries.Reset();
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::FindEntry
// -----------------------------------------------------------------------------
//
TInt TXnDomStringPoolOptimizer::FindEntry( const TDesC8& aString,
        const TInt aLeft,
        const TInt aRight )
    {    
    if( aLeft > aRight )
        {
        return KErrNotFound;
        }
    
    const TUint middle = ( aLeft + aRight ) >> 1; // >> 1 means "divided by two".    
    TXnDomStringPoolOptimizerEntry& entryAtMiddle = iEntries[ middle ];    
    const TInt comparisonResult = aString.Compare( entryAtMiddle.iString );
    
    if( comparisonResult > 0 )
        {
        return FindEntry( aString, middle + 1, aRight);
        }
    else if( comparisonResult < 0 )
        {
        return FindEntry( aString, aLeft, middle - 1 );
        }
    else
        {
        return  middle;
        }    
    }

// -----------------------------------------------------------------------------
// TXnDomStringPoolOptimizer::FindEntry
// -----------------------------------------------------------------------------
//
TInt TXnDomStringPoolOptimizer::FindInsertionIndexEstimate( const TDesC8& aString,
        const TInt aLeft,
        const TInt aRight )
    {
    if( ( aRight - aLeft ) <= KMaxEstimateThreshold )
        {
        return aLeft;
        }
    
    const TUint middle = ( aLeft + aRight ) >> 1;      
         
    TXnDomStringPoolOptimizerEntry& entryAtMiddle = iEntries[ middle ];    
    const TInt comparisonResult = aString.Compare( entryAtMiddle.iString );
        
    if( comparisonResult > 0 )
        {
        return FindInsertionIndexEstimate( aString, middle, aRight);
        }
    else if( comparisonResult < 0 )
        {
        return FindInsertionIndexEstimate( aString, aLeft, middle );
        }
    else
        {
        // Should not go here. There should be only one of a kind in the list.
        return KErrNotFound;
        }
    }

//  End of File  
