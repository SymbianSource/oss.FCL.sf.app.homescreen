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
#include "xnnewstickercontrol.h"
#include "xnnewstickeradapter.h"
#include "xnproperty.h"

// CONSTANTS
   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewstickerControl::CXnNewstickerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerControl::CXnNewstickerControl( CXnNewstickerAdapter* aAdapter ) :     
    iCurrentTitleIndex( KErrNotFound ),
    iAdapter( aAdapter ), 
    iScrollLooping( ETrue )
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
// CXnNewstickerControl::SetScrollLooping()
// 
// -----------------------------------------------------------------------------
//
void CXnNewstickerControl::SetScrollLooping( TBool aLooping )
    {
    iScrollLooping = aLooping;
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
            iCurrentTitleIndex = KErrNotFound;
            }

        UpdateTitleL( KNullDesC, aIndex );      
        
        if( aIndex == iCurrentTitleIndex )
            {
            iCurrentTitleIndex = GetNextTitleWithContent( aIndex );
            }
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
TInt CXnNewstickerControl::LastIndexWithContent() const
    {
    for( TInt i(iTitleTexts.Count()-1); i>=0; --i )
        {
        const TDesC& title( *iTitleTexts[i] );
        
        if( title != KNullDesC() )
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
    iCurrentTitleIndex = KErrNotFound;
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
// CXnNewstickerControl::CurrentTitle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CXnNewstickerControl::CurrentTitle() const
    {
    return Title( iCurrentTitleIndex );
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
        const TDesC& title( *iTitleTexts[i] );
        
        if( title != KNullDesC() )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SelectTitle
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerControl::SelectTitle()
    {    
    TInt index( GetNextTitleWithContent( 0 ) );
    
    TInt currentIndex( iCurrentTitleIndex );
    
    iCurrentTitleIndex = index;
    
    if ( currentIndex != iCurrentTitleIndex )
        {
        if ( currentIndex != KErrNotFound )
            {
            iAdapter->ReportNewstickerEvent( 
                XnPropertyNames::action::trigger::name::KTitleScrolled, 
                currentIndex );        
            }
        
        if ( iCurrentTitleIndex != KErrNotFound )
            {
            iAdapter->ReportNewstickerEvent( 
                XnPropertyNames::action::trigger::name::KTitleToScroll, 
                iCurrentTitleIndex );            
            }    
        }
    
    return iCurrentTitleIndex;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::SelectNextTitle
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::SelectNextTitle()
    {
    TBool ret( EFalse );
    TInt lastIndex( iTitleTexts.Count() - 1 );
    TInt lastIndexWithContent( LastIndexWithContent() );
    
    TInt currentIndex( iCurrentTitleIndex );
                                
    if ( lastIndexWithContent == KErrNotFound )
        {               
        // Loop done
        ret = ETrue;                
        }
    else if ( !iScrollLooping && iCurrentTitleIndex == lastIndexWithContent )
        {
        // Loop done
        ret = ETrue;
        }        
    else
        {
        if ( iCurrentTitleIndex + 1 > lastIndexWithContent )
            {
            // At the end, get first
            iCurrentTitleIndex = 
                GetNextTitleWithContent( 0 );
            }
        else
            {
            // find next index with content
            iCurrentTitleIndex =
                    GetNextTitleWithContent( iCurrentTitleIndex + 1 );
            }               
        }
            
    if ( ret )
        {
        if ( iCurrentTitleIndex != KErrNotFound )
            {
            iAdapter->ReportNewstickerEvent( 
                XnPropertyNames::action::trigger::name::KTitleScrolled, 
                iCurrentTitleIndex );            
            }    
        }
    else
        {
        if ( currentIndex != iCurrentTitleIndex && currentIndex != KErrNotFound )
            {
            iAdapter->ReportNewstickerEvent( 
                XnPropertyNames::action::trigger::name::KTitleScrolled, 
                currentIndex );        
            }
        
        if ( currentIndex != iCurrentTitleIndex && iCurrentTitleIndex != KErrNotFound )
            {
            iAdapter->ReportNewstickerEvent( 
                XnPropertyNames::action::trigger::name::KTitleToScroll, 
                iCurrentTitleIndex );            
            }    
        }
              
   return ret;
   }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::PeekNextTitle
// -----------------------------------------------------------------------------
//
TPtrC CXnNewstickerControl::PeekNextTitle( TBool& aEndOfLoop ) const
    {
    TBool ret( EFalse );
    TInt lastIndex( iTitleTexts.Count() - 1 );
    TInt lastIndexWithContent = LastIndexWithContent();
           
    TPtrC retval( KNullDesC() );
    
    if ( lastIndexWithContent == KErrNotFound )
        {               
        // Loop done
        ret = ETrue;                
        }
    else if ( !iScrollLooping && iCurrentTitleIndex == lastIndexWithContent )
        {
        // Loop done
        ret = ETrue;
        
        retval.Set( CurrentTitle() );
        }        
    else
        {
        if ( iCurrentTitleIndex + 1 > lastIndexWithContent )
            {
            // At the end, get first
            retval.Set( Title( GetNextTitleWithContent( 0 ) ) );                
            }
        else
            {
            // find next index with content
            retval.Set( Title( GetNextTitleWithContent( iCurrentTitleIndex + 1 ) ) );                    
            }               
        }
    
    aEndOfLoop = ret;
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerControl::IsVisibleTitles
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerControl::IsVisibleTitles() const
    {
    for( TInt i = 0; i < iTitleTexts.Count(); i++ )
        {
        const TDesC& title( *iTitleTexts[i] );
        
        if( title != KNullDesC() )
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }


//  End of File  
