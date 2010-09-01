/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Define the Util API for a external standby screen
*
*/



// INCLUDES
#include <aknlayout.cdl.h>
#include <skinlayout.cdl.h>

// -----------------------------------------------------------------------------
// CExtrStandbyScr::Draw
// -----------------------------------------------------------------------------
//
inline void CExtrStandbyScr::Draw(const TRect& /*aRect*/) const
     {
     if (IsDimmed())
        {
        return ;
        }

    TRect rect( Rect() );

    CWindowGc& gc = SystemGc();

    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    MAknsSkinInstance* aknsInst = AknsUtils::SkinInstance();

    // draw(clear) background
    //AknsDrawUtils::Background( aknsInst, cc, gc, rect );
    // Draw background starting from correct offset. May cause flicker if derived class
    // has not registered control position with AknsUtils::RegisterControlPosition
    AknsDrawUtils::Background( aknsInst, cc, this, gc, rect );

    if( IsFocused() )
        {
        // draw highlight
        TAknLayoutRect highlightParentRect;
        highlightParentRect.LayoutRect( rect,
                                        AknLayout::Highlight_for_other_Active_Idle_items_Line_1( rect ) );

        TAknLayoutRect highlightRectCenter;
        highlightRectCenter.LayoutRect( highlightParentRect.Rect(),
                                        SkinLayout::List_highlight_skin_placing__general__Line_1( highlightParentRect.Rect() ) );

        TAknLayoutRect highlightRectTl;
        highlightRectTl.LayoutRect( highlightParentRect.Rect(),
                                    SkinLayout::List_highlight_skin_placing__general__Line_2() );

        TAknLayoutRect highlightRectBr;
        highlightRectBr.LayoutRect( highlightParentRect.Rect(),
                                    SkinLayout::List_highlight_skin_placing__general__Line_5() );

        TRect highlightRect( highlightRectTl.Rect().iTl, highlightRectBr.Rect().iBr );

        if( !AknsDrawUtils::DrawFrame( aknsInst,
                                       gc,
                                       highlightRect,
                                       highlightRectCenter.Rect(),
                                       KAknsIIDQsnFrList,
                                       KAknsIIDQsnFrListCenter ) )
            {
            // no skin - draw normal list pane highlight
            TAknLayoutRect highlighRect;

            // shadow
            highlighRect.LayoutRect( highlightParentRect.Rect(),
                                     AknLayout::List_pane_highlight_graphics__various__Line_1( highlightParentRect.Rect() ) );
            highlighRect.DrawRect( gc );

            // highlight
            highlighRect.LayoutRect( highlightParentRect.Rect(),
                                     AknLayout::List_pane_highlight_graphics__various__Line_2( highlightParentRect.Rect() ) );
            highlighRect.DrawRect( gc );
            }
        }

    TAknLayoutRect separatorRect;
    separatorRect.LayoutRect( rect, AknLayout::First_general_event_elements_Line_1() );

    TRgb lineColour( separatorRect.Color() );

    AknsUtils::GetCachedColor( aknsInst,
                               lineColour,
                               KAknsIIDQsnLineColors,
                               EAknsCIQsnLineColorsCG6 /* column separator lines */ );

    gc.SetPenColor( lineColour );
    gc.SetPenStyle( CWindowGc::ESolidPen );

    TInt separatorWidth( separatorRect.Rect().Width() -1 );
    TPoint dot( separatorRect.Rect().iTl );

    for( ; dot.iX < separatorWidth; dot.iX+=2 )
        {
        // draw the dots
        gc.Plot( dot );
        }
    }
