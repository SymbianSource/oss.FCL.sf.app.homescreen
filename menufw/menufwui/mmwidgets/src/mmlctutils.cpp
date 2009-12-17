/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  highlight control class
*
*/


#include "mmlctutils.h"
#include "mmtemplatelibrary.h"
#include "mmwidgetsconstants.h"
#include <aknlayoutscalable_uiaccel.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include <AknFontId.h>
#include <AknLayoutFont.h>
#include <AknDef.hrh>

  
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//  
TAknWindowComponentLayout MmLCTUtils::ItemLayout( const TDesC8& aLayout, TInt aVariety )
	{
    if ( !aLayout.Compare(KListSingleLargeGraphicPane8))
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane(0, 0, 0);
        }
    else if ( !aLayout.Compare(KUiaccelAalistDoubleGraphicPane8))
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistDoublePane8))
        {
        return AknLayoutScalable_UiAccel::aalist_double_pane(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistSinglePane8))
        {
        return AknLayoutScalable_UiAccel::aalist_single_pane(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistGeneAdPane8))
        {
        return AknLayoutScalable_UiAccel::aalist_gene_ad_pane(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAagridCellImagePane8))
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane(aVariety, 0, 0);
        }
    else if ( !aLayout.Compare(KCellHcAppsPane8))
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane( aVariety, 0, 0);
        }
    else if ( !aLayout.Compare(KCellAppPane8))
    	{
    	return AknLayoutScalable_Avkon::cell_app_pane( aVariety, 0, 0 );
        }
    else if ( !aLayout.Compare(KListSingleHcAppsPane8))
        {
        return AknLayoutScalable_Avkon::list_single_hc_apps_pane( 0, aVariety );
        }
    else
        {
        User::Panic(KMLID, -1);
        }
    return TAknWindowComponentLayout();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//           
TAknTextComponentLayout MmLCTUtils::TextLayout( const TDesC8& aLayout, TInt aVariety )
    {
    if ( aLayout.Compare(KUiaccelAalistSinglePaneT18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_single_pane_t1(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoubleGraphicPaneT18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_t1(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoubleGraphicPaneT28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_t2(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoublePaneT18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_pane_t1(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoublePaneT28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_pane_t2(aVariety);
        }
    else if ( aLayout.Compare(KlistSingleLargeGraphicPaneT18) == 0 )
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane_t1(aVariety);
        }       
    else if ( aLayout.Compare(KUiaccelAagridCellImagePaneT18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane_t1(aVariety);
        }
    else if ( !aLayout.Compare(KCellHcAppsPaneT18))
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane_t1( aVariety, 0, 0);
        }
    else if ( !aLayout.Compare(KListSingleHcAppsPaneT18))
		{
		return AknLayoutScalable_Avkon::list_single_hc_apps_pane_t1( aVariety );
		}
    else if ( !aLayout.Compare(KCellAppsPaneT18))
		{
		return AknLayoutScalable_Avkon::cell_app_pane_t1( aVariety, 0, 0 );
		}
    else
        {
        User::Panic(KMLID, -1);
        }
    return TAknTextComponentLayout();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TAknWindowComponentLayout MmLCTUtils::GraphicLayout( const TDesC8& aLayout, TInt aVariety )
    {
    if ( aLayout.Compare(KUiaccelAalistSinglePaneG18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_single_pane_g1(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistSinglePaneG28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_single_pane_g2(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoubleGraphicPaneG1) == 0 )
        {
	    return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_g1(aVariety);
	    }
    else if ( aLayout.Compare(KUiaccelAalistDoubleGraphicPaneG28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_g2(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoubleGraphicPaneG38) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_g3(aVariety);
        }        
    else if ( aLayout.Compare(KUiaccelAalistDoublePaneG18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_pane_g1(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAalistDoublePaneG28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aalist_double_pane_g2(aVariety);
        } 
    else if ( aLayout.Compare(KlistSingleLargeGraphicPane8) == 0 )
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane(aVariety, 0,0);
        }
    else if ( aLayout.Compare(KlistSingleLargeGraphicPaneG18) == 0 )
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1(aVariety);
        }
    else if ( aLayout.Compare(KlistSingleLargeGraphicPaneG2Cp28) == 0 )
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane_g2_cp2(aVariety);
        } 
    else if ( aLayout.Compare(KlistSingleLargeGraphicPaneG4Cp28) == 0 )
        {
        return AknLayoutScalable_Avkon::list_single_large_graphic_pane_g4_cp2(aVariety);
        } 
    else if ( aLayout.Compare(KUiaccelAagridCellImagePaneG18) == 0 )
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane_g1(aVariety);
        } 
    else if ( aLayout.Compare(KUiaccelAagridCellImagePaneG28) == 0 )
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane_g2(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAagridCellImagePaneG38) == 0 )
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane_g3(aVariety);
        }
    else if ( aLayout.Compare(KUiaccelAagridCellImagePaneG48) == 0 )
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane_g4(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAagridCellImagePane8))
        {
        return AknLayoutScalable_UiAccel::aagrid_cell_image_pane(aVariety, 0, 0);
        }
    else if ( !aLayout.Compare(KUiaccelAalistDoubleGraphicPane8))
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistDoubleGraphicPaneG68))
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_g6(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistDoubleGraphicPaneG78))
        {
        return AknLayoutScalable_UiAccel::aalist_double_graphic_pane_g7(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistGeneAdPaneG18))
        {
        return AknLayoutScalable_UiAccel::aalist_gene_ad_pane_g1(aVariety);
        }
    else if ( !aLayout.Compare(KUiaccelAalistGeneAdPaneG18))
        {
        return AknLayoutScalable_UiAccel::aalist_gene_ad_pane_g1(aVariety);
        }
    else if ( !aLayout.Compare( KCellHcAppsPaneG18))
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane_g1( aVariety, 0, 0);
        }
    else if ( !aLayout.Compare( KCellHcAppsPaneG28))
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane_g2( aVariety, 0, 0);
        }
    else if ( !aLayout.Compare( KCellHcAppsPaneG38))
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane_g3( aVariety, 0, 0);
        }
    else if ( !aLayout.Compare( KListSingleHcAppsPaneG18))
        {
        return AknLayoutScalable_Avkon::list_single_hc_apps_pane_g1( aVariety );
        }
    else if ( !aLayout.Compare( KListSingleHcAppsPaneG28))
        {
        return AknLayoutScalable_Avkon::list_single_hc_apps_pane_g2( aVariety );
        }
    else if ( !aLayout.Compare( KCellAppsPaneG18))
        {
        return AknLayoutScalable_Avkon::cell_app_pane_g1( aVariety, 0, 0 );
        }
    else if ( !aLayout.Compare( KCellAppsPaneG28))
        {
        return AknLayoutScalable_Avkon::cell_app_pane_g2( aVariety, 0, 0 );
        }
    else
        {
        User::Panic(KMLID, -1);
        }
    return TAknWindowComponentLayout();        
    }    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TAknLayoutScalableParameterLimits MmLCTUtils::LCTParameterLimits( const TDesC8& aLayout, TInt aVariety )
	{
    if ( aLayout.Compare(KCellHcAppsPane8) == 0 )
        {
        return AknLayoutScalable_Avkon::cell_hc_apps_pane_ParamLimits( aVariety );
        }
    else if ( aLayout.Compare(KCellAppPane8) == 0 )
        {
        return AknLayoutScalable_Avkon::cell_app_pane_ParamLimits( aVariety );
        }
    else
        {
        User::Panic(KMLID, -1);
        }
    return TAknLayoutScalableParameterLimits();
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MmLCTUtils::SetupTextSubCellTemplate( TSize aItemSize, 
                              TTemplateChild& aSubCellTemplate )
    {
    TAknTextComponentLayout textLayout = TextLayout( aSubCellTemplate.iLct, aSubCellTemplate.iVariety);                
    TAknTextLineLayout textlineLayout = textLayout.LayoutLine();
    TAknLayoutText t;
    t.LayoutText( aItemSize, textlineLayout );
    aSubCellTemplate.iTextAlign = AknLayoutUtils::TextAlignFromId( textlineLayout.iJ );
    aSubCellTemplate.iRectAccordingToParent = t.TextRect();
    aSubCellTemplate.iFontId =  (TAknLogicalFontId)textlineLayout.FontId();
    aSubCellTemplate.iIsImage = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MmLCTUtils::SetupGraphicSubCellTemplate( TSize aItemSize, 
                              TTemplateChild& aSubCellTemplate )
    {
    TAknWindowComponentLayout graphicLayout = GraphicLayout (
			aSubCellTemplate.iLct, aSubCellTemplate.iVariety);
	TAknLayoutRect r;           
    r.LayoutRect( aItemSize, graphicLayout.LayoutLine() );
    aSubCellTemplate.iTextAlign = CGraphicsContext::ECenter ;
    aSubCellTemplate.iRectAccordingToParent = r.Rect();
    aSubCellTemplate.iIsImage = ETrue;
    aSubCellTemplate.iFontId = EAknLogicalFontSecondaryFont;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TSize MmLCTUtils::GetLCTSize (const TDesC8& aLCTTemplate, TInt aVariety,
		const TRect aParentRect, TAknWindowLineLayout& aWindowLayout)
	{
    aWindowLayout = MmLCTUtils::ItemLayout( aLCTTemplate, aVariety);
    TAknLayoutRect layRect;
    layRect.LayoutRect(aParentRect, aWindowLayout);
    return layRect.Rect().Size();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TSize MmLCTUtils::GetLayoutSize( const TDesC8& aLCTTemplate, TInt aVariety )
	{
    TInt cols(0);
    TInt rows(0);
    cols = MmLCTUtils::LCTParameterLimits( aLCTTemplate, aVariety ).LastColumn() + 1;
    rows = MmLCTUtils::LCTParameterLimits( aLCTTemplate, aVariety ).LastRow() + 1;
    if ( aLCTTemplate.Compare(KCellAppPane8) == 0 && aVariety == 3 )
    	{
    	cols = 6;
    	rows = 2;
    	}
    return TSize( cols, rows);
	}

// End of file
