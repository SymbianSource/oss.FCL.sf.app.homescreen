/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Text scrolling functionality.
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknBidiTextUtils.h>

#include "xnnewstickercontrol.h"
#include "xnnewstickeradapter.h"


// CONSTANTS
   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CXnNewstickerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CXnNewstickerControl( CXnNewstickerAdapter* aAdapter ) :     
    iCurrentTitleIndex( -1 ),
    iAdapter( aAdapter )
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl* CXnNewstickerControl::NewL( CXnNewstickerAdapter* aAdapter )
    {
    CXnNewstickerControl* self = new(ELeave)CXnNewstickerControl( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self   
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::~CXnNewstickerControl()
// Destructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::~CXnNewstickerControl()
    {
    iTitleTexts.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::AppendTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::AppendTitleL( const TDesC& aTitle )
    {
    HBufC* title = aTitle.AllocLC();
    iTitleTexts.AppendL( title );
    CleanupStack::Pop( title );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::InsertTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::InsertTitleL( const TDesC& aTitle, TInt aIndex )
    {
    if( aIndex >= 0 && aIndex < iTitleTexts.Count() )
        {
        HBufC* title = aTitle.AllocLC();
        iTitleTexts.InsertL( title, aIndex );
        CleanupStack::Pop( title );
        }
    else
        {
        AppendTitleL( aTitle );
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::UpdateTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::UpdateTitleL( const TDesC& aTitle, TInt aIndex )
    {
    if( aIndex >= 0 && aIndex < iTitleTexts.Count() )
        {
        HBufC* oldTitle = iTitleTexts[ aIndex ];
        iTitleTexts.Remove( aIndex );
        
        delete oldTitle;
        oldTitle = NULL;
        
        HBufC* title = aTitle.AllocLC();
        iTitleTexts.InsertL( title, aIndex );
        CleanupStack::Pop( title );   
        }
    else
        {
        AppendTitleL( aTitle );
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::DeleteTitle
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::DeleteTitleL( TInt aIndex )
    {
    TInt count = iTitleTexts.Count();
    
    if( aIndex >= 0 && aIndex < count )
        {
        // If the last item will be deleted
        if(count == 1)
            {
            iAdapter->Stop();
            iCurrentTitleIndex = -1;
            }

        UpdateTitleL( KNullDesC, aIndex );      
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CurrentTitleIndex
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::CurrentTitleIndex() const
    {
    return iCurrentTitleIndex;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::Title
// -----------------------------------------------------------------------------
//
const TDesC& CXnNewstickerControl::Title( TInt aIndex ) const
    {
    if( aIndex >= 0 && aIndex < iTitleTexts.Count() )
        {
        return *iTitleTexts[ aIndex ];
        }

    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::LastIndexWithContent
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::LastIndexWithContent()
    {
    for( TInt i(iTitleTexts.Count()-1); i>=0; --i )
        {
        if ( iTitleTexts[ i ]->Des().Length() > 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::ClearTitles
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::ClearTitles()
    {
    iAdapter->Stop();
    iCurrentTitleIndex = -1;
    // Don't delete just clear the contents
    for( TInt i=0; i < iTitleTexts.Count(); i++ )
        {
        TRAP_IGNORE( UpdateTitleL( KNullDesC, i ) );
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::TitleCount
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::TitleCount() const
    {
    return iTitleTexts.Count();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SelectTitle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CXnNewstickerControl::SelectTitle()
    {
    if( IsVisibleTitles() ) 
        { 
        if( iCurrentTitleIndex < 0 || iCurrentTitleIndex >= iTitleTexts.Count() )
            {
            // Get the last title
            TInt index = 
                    GetNextTitleWithContent( iTitleTexts.Count() - 1, ETrue );
            iCurrentTitleIndex = index;
            }
        
        return Title( iCurrentTitleIndex );
        }  
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::GetNextTitleWithContent
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::GetNextTitleWithContent( TInt aStartSearch, 
        TBool aBackwards ) const
    {
    TInt dir = 1;
    if( aBackwards )
        {
        dir = -1;
        }
    
    for( TInt i = aStartSearch; i < iTitleTexts.Count() && i >= 0; i += dir )
        {
        if( iTitleTexts[i]->Compare( KNullDesC ) != KErrNone )
            {
            return i;
            }
        }
    
    return aStartSearch;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SetCurrentTitle
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::SetCurrentTitle( TBool aSetDefault )
    {
    TBool ret( EFalse );
    TInt lastIndex( iTitleTexts.Count() - 1 );
    TInt lastIndexWithContent = LastIndexWithContent();
    
    if( aSetDefault )
        {
        // set last index as default
        iCurrentTitleIndex = GetNextTitleWithContent( lastIndex, ETrue );
        }
    else if( iCurrentTitleIndex >= lastIndex )
        {
        // if iCurrentTitleIndex is in last position start from beginning
        iCurrentTitleIndex = GetNextTitleWithContent( 0 );
        }
    else
        {
        if ( iCurrentTitleIndex + 1 > lastIndexWithContent )
            {
            // if lastIndexWithContent is creater than next item 
            // it means all next items are empty strings
            iCurrentTitleIndex = lastIndexWithContent;
            }
        else
            {
            // find next index with content
            iCurrentTitleIndex
                = GetNextTitleWithContent( iCurrentTitleIndex + 1 );
            }
        }
    
    if( iCurrentTitleIndex == lastIndexWithContent || 
            lastIndexWithContent == KErrNotFound )
        {
        // loop done stop periodic timer.
        ret = ETrue;
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::IsVisibleTitles
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::IsVisibleTitles() const
    {
    for( TInt i=0; i < iTitleTexts.Count(); i++ )
        {
        if( iTitleTexts[i]->Compare( KNullDesC ) != KErrNone )
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }


//  End of File  
