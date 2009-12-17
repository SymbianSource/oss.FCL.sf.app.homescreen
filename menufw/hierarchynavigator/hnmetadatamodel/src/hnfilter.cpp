/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "hnfilter.h"
#include "hnitemid.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnFilter::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnFilter* CHnFilter::NewL( )
    {
    CHnFilter* self = CHnFilter::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnFilter* CHnFilter::NewLC( )
    {
    CHnFilter* self = new( ELeave ) CHnFilter();
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnFilter::CHnFilter()
    {
    iEvaluateSuite = ETrue;
    iSuiteId = KErrNotFound;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnFilter::~CHnFilter()
    {
    ResetSuiteName();
    ResetItemIds();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHnFilter::SetEvaluateSuiteL( TBool aEvaluate )
    {
    iEvaluateSuite = aEvaluate;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CHnFilter::IsEvaluateSuite()
    {
    return iEvaluateSuite;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
 const TDesC& CHnFilter::SuiteName() const
    {
    return iGenre;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnFilter::HasItemIds() const
    {
    return iItemIds.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnFilter::ResetIterator()
    {
    iIter = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CHnItemId* CHnFilter::GetNextItemId() const
    {
    ASSERT( iIter < iItemIds.Count() );
    return iItemIds[ iIter++ ];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnFilter::HasNextItemId()
    {
    return ( iIter < iItemIds.Count() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnFilter::ResetSuiteName()
    {
    iGenre.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnFilter::ResetItemIds()
    {
    iItemIds.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnFilter::SetSuiteNameL( const TDesC& aGenre )
    {
    ResetSuiteName();
    iGenre.CreateL( aGenre );
    iGenre.LowerCase();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnFilter::AppendItemIdL( CHnItemId* iItemId )
    {
    iItemIds.AppendL( iItemId ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnFilter::HasSuiteName() const
    {
    return iGenre.Length();
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnFilter::SetSuiteId( const TInt aSuiteId )
    {
    iSuiteId = aSuiteId;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnFilter::SuiteId() const
    {
    return iSuiteId;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnFilter::HasSuiteId() const
    {
    return (iSuiteId == KErrNotFound) ? EFalse : ETrue;
    }
