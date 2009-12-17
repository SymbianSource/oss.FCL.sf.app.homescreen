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


#include "hnstringhandler.h"
#include "hnmdmodel.h"
#include "hnutils.h"
#include "hnmdlocalization.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HnStringHandler::LocaliseL( RBuf& aDestination, RBuf& aSource,
                                        THnMdCommonPointers* aCmnPtrs )
    {
    HnStringHandler::LocaliseL( aDestination, aSource, NULL, NULL, aCmnPtrs);
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HnStringHandler::LocaliseL( RBuf& aDestination,
                                 RBuf& aSource,
                                 CDesC16Array* aDesParams,
                                 CArrayFix<TInt>* aIntParams,
                                 THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdLocalization* localization(NULL);
    if (aCmnPtrs)
        {
        localization = aCmnPtrs->iLocalization;
        }
    else
        {
        localization =  THnMdCommonPointers::Static()->iLocalization;
        }

    HBufC* buf = localization->LoadL( aSource, aDesParams, aIntParams );
    aDestination.Assign( buf);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HnStringHandler::CompareIgnoreCaseL( const TDesC8& aLeft,
        const TDesC8& aRight )
    {
    TInt res( KErrGeneral );
    
    RBuf8 left;
    CleanupClosePushL( left );
    left.CreateL( aLeft.Length() );
    left.CopyUC( aLeft );

    RBuf8 right;
    CleanupClosePushL( right );
    right.CreateL( aRight.Length() );
    right.CopyUC( aRight );

    res = left.Compare( right );
    
    CleanupStack::PopAndDestroy( &left );
    CleanupStack::PopAndDestroy( &right );
    return res;
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HnStringHandler::CompareIgnoreCaseL( const TDesC& aLeft,
        const TDesC& aRight )
    {
    TInt res( KErrGeneral );
    
    RBuf left;
    CleanupClosePushL( left );
    left.CreateL( aLeft.Length() );
    left.CopyUC( aLeft );
    
    RBuf right;
    CleanupClosePushL( right );
    right.CreateL( aRight.Length() );
    right.CopyUC( aRight );

    res = left.Compare( right );
    
    CleanupStack::PopAndDestroy( &right );
    CleanupStack::PopAndDestroy( &left );
    
    return res;
    }

