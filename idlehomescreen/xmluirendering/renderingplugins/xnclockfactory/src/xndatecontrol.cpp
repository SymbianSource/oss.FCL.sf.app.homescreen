/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock control
*
*/


// System includes
#include <e32std.h>
#include <debug.h>
#include <AknLayoutFont.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h>
#include <StringLoader.h>
#include <bacntf.h>

// User includes
#include "xndatecontrol.h"
#include "xncontroladapter.h"
#include "xnclockadapter.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"
#include "xnuienginepluginif.h"
#include "c_xnutils.h"

// Constants
const TInt KMaxDateStringLength = 100;
const TInt KMaxDayNumberStringLength = 10;

_LIT( KDateFont, "EAknLogicalFontSecondaryFont" );
_LIT( KTimeFormat, "%0U %1U" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDateControl::CXnDateControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDateControl::CXnDateControl( CXnClockAdapter& aAdapter,
                                TXnDateType aType )
    : iAdapter( aAdapter ), iDateType( aType )
    {
    }

// -----------------------------------------------------------------------------
// CXnDateControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDateControl::ConstructL()
    {
    // Create enviroment notifier
    iEnvNotifier = CEnvironmentChangeNotifier::NewL( 
        CActive::EPriorityStandard, TCallBack( HandleCallBackL, this ) );
    // Start listening notifications
    iEnvNotifier->Start();
    }

// -----------------------------------------------------------------------------
// CXnDateControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnDateControl* CXnDateControl::NewL( CXnClockAdapter& aAdapter,
                                      TXnDateType aType )
    {
    CXnDateControl* self = new (ELeave) CXnDateControl( aAdapter, aType );
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CXnDateControl::~CXnDateControl()
    {
    delete iDateText;
    delete iEnvNotifier;
    }

// -----------------------------------------------------------------------------
// CXnDateControl::DrawL
// -----------------------------------------------------------------------------
//       
void CXnDateControl::DrawL( CWindowGc& aGc, CXnNodePluginIf* aNode )
    {
    if ( !aNode ||
         !iDateText ||
         ( aNode && aNode->Rect().IsEmpty() ) )
        {
        return;
        }
    
    const CAknLayoutFont* dateFont( CreateFontL( *aNode ) );
    if ( !dateFont )
        {
        return;
        }
    
    const TRgb& color( CreateColorL( *aNode ) );
    
    CGraphicsContext::TTextAlign align = iAdapter.GetTextAlignL( *aNode );
    
    const TInt deltaHeight( aNode->Rect().Height() - dateFont->TextPaneHeight() );
    TInt offset( dateFont->TextPaneTopToBaseline() + deltaHeight / 2 );
    
    aGc.SetPenColor( color );
    aGc.UseFont( dateFont );
    aGc.DrawText( *iDateText, aNode->Rect(), offset, align );
    aGc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CXnDateControl::ConstructDateStringL
// -----------------------------------------------------------------------------
//
HBufC* CXnDateControl::ConstructDateStringL()
    {
    // Construct date string using date format from resource file
    CCoeEnv* coeEnv = CCoeEnv::Static();

    if( !coeEnv )
        {
        User::Leave( KErrNotReady );
        }

    TTime time;
    time.HomeTime();

    HBufC* aiDateString = HBufC::NewLC( KMaxDateStringLength );
    HBufC* aiDateFormatString = HBufC::NewLC( KMaxDateStringLength );
    aiDateFormatString->Des().Copy( KTimeFormat() );
    // TODO: when localisation is ready read format string from resource
    //HBufC* aiDateFormatString = StringLoader::LoadLC( R_ACTIVEIDLE_TIME_FORMAT,
    //                                                    coeEnv );
    HBufC* dateStringBuf = HBufC::NewLC( KMaxDateStringLength );
    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_SHORT_WITH_ZERO,
                                                    coeEnv );
    TPtr dateString( dateStringBuf->Des() );    
    time.FormatL( dateString, *dateFormatString );
    CleanupStack::PopAndDestroy( dateFormatString );

    //now dateString contains string which is formatted using
    //R_QTN_DATE_USUAL_WITH_ZERO

    // To arabic
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateString );

    TPtr aiDateStringPtr = aiDateString->Des();
    
    TDayNameAbb wkDayAbb = TDayNameAbb();
    wkDayAbb.Set(time.DayNoInWeek());
        
    //add date to string
    StringLoader::Format( aiDateStringPtr, *aiDateFormatString, 1,dateString );

    //reuse dateString
    dateString.Copy( aiDateStringPtr );

    //add weekday to string
    StringLoader::Format( aiDateStringPtr, dateString, 0, wkDayAbb );

    CleanupStack::PopAndDestroy( dateStringBuf );//dateStringBuf, aiDateFormatString
    CleanupStack::PopAndDestroy( aiDateFormatString );

    CleanupStack::Pop( aiDateString );
    return aiDateString;
    }

// -----------------------------------------------------------------------------
// CXnDateControl::ConstructDayStringL
// -----------------------------------------------------------------------------
//
HBufC* CXnDateControl::ConstructDayStringL()
    {
    TTime time;
    time.HomeTime();

    HBufC* aiDayString = HBufC::NewLC( KMaxDayNumberStringLength );
    aiDayString->Des().Num( time.DayNoInMonth() + 1 );
    // To arabic
    TPtr dayStringPtr = aiDayString->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dayStringPtr );
    CleanupStack::Pop( aiDayString );
    return aiDayString;
    }

// -----------------------------------------------------------------------------
// CXnDateControl::CreateFontL
// -----------------------------------------------------------------------------
//
const CAknLayoutFont* CXnDateControl::CreateFontL( CXnNodePluginIf& aNode )
    {    
    if ( !iDateFont )
        {
        iAdapter.CreateFontL( aNode, KDateFont, iDateFont );
        }
    return CAknLayoutFont::AsCAknLayoutFontOrNull( iDateFont );
    }

// -----------------------------------------------------------------------------
// CXnDateControl::CreateColorL
// -----------------------------------------------------------------------------
//
const TRgb& CXnDateControl::CreateColorL( CXnNodePluginIf& aNode )
    {
    if ( !iIsColorSet )
        {
        iAdapter.CreateColorL( aNode, iDateColor );
        iIsColorSet = ETrue;
        }
    return iDateColor;
    }

// -----------------------------------------------------------------------------
// CXnDateControl::ResetFont
// -----------------------------------------------------------------------------
//
void CXnDateControl::ResetFont()
    {
    iDateFont = NULL;
    iIsColorSet = EFalse;
    }

// -----------------------------------------------------------------------------
// CXnDateControl::RefreshDateL
// -----------------------------------------------------------------------------
//
void CXnDateControl::RefreshDateL()
    {
    delete iDateText;
    iDateText = NULL;
    if ( iDateType == EDate )
        {
        iDateText = ConstructDateStringL();
        }
    else if ( iDateType == EDay )
        {
        iDateText = ConstructDayStringL();
        }
    }

// -----------------------------------------------------------------------------
// CXnDateControl::HandleCallBackL
// -----------------------------------------------------------------------------
//
TInt CXnDateControl::HandleCallBackL( TAny *aPtr )
    {
    CXnDateControl* self = static_cast<CXnDateControl*>( aPtr );
    
    if( self )
        {
        TInt changes( self->iEnvNotifier->Change() );
        
        if ( changes & 
            ( EChangesLocale | EChangesMidnightCrossover | EChangesSystemTime ) )
            {
            self->RefreshDateL();
            }       
        }    
        
    return KErrNone;
    }

//  End of File
