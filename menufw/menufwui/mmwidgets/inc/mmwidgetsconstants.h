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
* Description:  Constants for the matrixmenu
*  Version     : %version: 33.1.9 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef MMWIDGETSCONSTANTS_H
#define MMWIDGETSCONSTANTS_H


#include <e32keys.h>

/**
 * Grid constants' namespace.
 */
namespace MmGrid
	{
	/**
	 * X component of the 4x3 layout.
	 */
	const TInt K4By3LayoutX = 4;

	/**
	 * Y component of the 4x3 layout.
	 */
	const TInt K4By3LayoutY = 3;

	/**
	 * X component of the 3x4 layout.
	 */

	const TInt K3By4LayoutX = 3;

	/**
	 * Y component of the 3x4 layout.
	 */
	const TInt K3By4LayoutY = 4;

	/**
	 * X component of the 4x5 layout.
	 */
	const TInt K4By5LayoutX = 4;

	/**
	 * Y component of the 5x4 layout.
	 */
	const TInt K4By5LayoutY = 5;

	/**
	 * X component of the 5x4 layout.
	 */
	const TInt K5By4LayoutX = 5;

	/**
	 * Y component of the 5x4 layout.
	 */
	const TInt K5By4LayoutY = 4;

	/**
	 * Below this value, default highlight in 3x4 grid
	 * is set up on the first item. Otherwize, on the default
	 * highlight.
	 */
	const TInt K3By4Threshold(5);

	/**
	 * Below this value, default highlight in 4x3 grid
	 * is set up on the first item. Otherwize, on the default
	 * highlight.
	 */
	const TInt K4By3Threshold(6);

	/**
	 * Below this value, default highlight in 5x4 grid
	 * is set up on the first item. Otherwize, on the default
	 * highlight.
	 */
	const TInt K5By4Threshold(8);

	/**
	 * Below this value, default highlight in 4x5 grid
	 * is set up on the first item. Otherwize, on the default
	 * highlight.
	 */
	const TInt K4By5Threshold(10);

	/**
	 * Default highlight in 3x4 grid.
	 */
	const TInt K3By4DefaultHighlight(4);

	/**
	 * Default highlight in 4x3 grid.
	 */
	const TInt K4By3DefaultHighlight(5);

	/**
	 * Default highlight in 5x4 grid.
	 */
	const TInt K5By4DefaultHighlight(7);

	/**
	 * Default highlight in 4x5 grid.
	 */
	const TInt K4By5DefaultHighlight(8);

	/**
	 * Number of rows in portrait/non-zoom mode;
	 */
	const TInt KRowsPortraitZoomNormal(4);

	/**
	 * Number of columns in landscape/non-zoom mode;
	 */
	const TInt KColsPortraitZoomNormal(3);

	/**
	 * Number of rows in portrait/non-zoom mode;
	 */
	const TInt KRowsLandscapeZoomNormal(3);

	/**
	 * Number of columns in landscape/non-zoom mode;
	 */
	const TInt KColsLandscapeZoomNormal(4);

	}

/**
 * Listbox constants' namespace.
 */
namespace MmListBox
	{
	/**
	 * Threshold value, determinign if a partial should
	 * be shown in a list.
	 */
	const TInt KPartialItemShowThreshold (0);

	/**
	 * Default highlight in 4x5 grid.
	 */
	const TInt KCols(1);
	}

/**
 * Drag and drop related constants.
 */
namespace MmEffects
	{
	/**
	 * Threshold for items shift.
	 */
	const TReal KShiftRatio(0.25);

	/**
	 * Drag and drop speed limit.
	 */
	const TInt KDragSpeedIgnoreThreshold(15 * 15);

	/**
	 * Number of frames to animate.
	 */
	const TInt KMaximumAnimationFramesCount( 6 );

	/**
	 * Number of frames to animate.
	 */
	const TInt KUsualAnimationFramesCount( 3 );

	/**
	 * Number of frames to animate on non-touch devices.
	 */
	const TInt KUsualAnimationFramesCountNonTouch( 2 );

	/**
	 * No animation frames count.
	 */
	const TInt KNoAnimationFramesCount( 1 );

	/**
	 * Delay between redrawing frames.
	 */
	const TInt KAnimationFrameDelay( 40000 );

	/**
	 * Text alpha during drag.
	 */
	const TInt KDragTextAlpha( 50 );

	/**
	 * Focus based scrolling item threshold.
	 */
	const TReal KFocusScrollingThreshold( 0.55 );

	/**
	 * Focus based scrolling item threshold for listbox landscape mode.
	 */
	const TReal KFocusScrollingThresholdListboxLanscape( 1.0 );

	/**
	 * Value of drag ignore.
	 */
	const TInt KDragIgnoreRectValue(200);

	/**
	 * Time between two butto repeat events.
	 */
	const TInt KDragRepeatRespawn(2);

	/**
	 * Determines the speed of focus-based scrolling in edit mode.
	 */
	const TInt KEditModeScrollingDelayFactor = 4000;
	}

/**
 * Templates' related constants.
 */
namespace MmTemplateContants
	{
	/**
	 * If a custom (non LCT) template is used, this is
	 * its default scrollbar width.
	 */
	const TInt KCustomTemplateScrolbarWidth (25);

	const TInt KTemplateChildTextLength (64);

	const TInt KSubCellsCount (6);

	const TInt KItemSubCellsText (300);

	const TInt KMoveIndicatorStartingPos ( 32 );

	}

/**
 * Shortcut constants.
 */
namespace MmShortcutConstants
	{
	/**
	 * Possible scancodes when interpreting shortcut key.
	 */
	const TInt KScanCodes[] = { '1','2','3','4','5','6','7','8','9',
#ifndef __WINS__
        '*',
#else
        EStdKeyNkpAsterisk,
#endif
	    '0', EStdKeyHash };

	/**
	 * Number of available scancodes.
	 */
	const TInt KScanCodeCount = 12;
	}

namespace MmMarqueeConstants
	{
	const TInt KTextTrimmingThreshold (80);
	const TInt KClippingBufLength (80);
	const TInt KLoops (1);
	const TInt KScrollAmount (6);
	const TInt KScrollDelay (1000000);
	}
/**
 * Matrix Template Library panic notice.
 */
_LIT( KMtlPanic,  "MTL" );

_LIT8( KMmTitle8,       	"mul_title"         );
_LIT8( KMmDetail8,      	"mul_detail"        );
_LIT8( KMmIcon8,        	"mul_icon"          );
_LIT8( KMmBackdropIcon8,    "mul_icon_backdrop" );
_LIT8( KMmTemplate8,    	"mul_template"      );
_LIT8( KGrid8,           	"grid"              );
_LIT8( KListbox8,        	"listbox"           );

_LIT8( KEmpty8,        			"empty"            );
_LIT8( KOrientation8,        	"orientation"      );
_LIT8( KBase8,        			"base"             );
_LIT8( KText8,        			"text"             );
_LIT8( KLayout8,        		"layout"           );
_LIT8( KVariety8,        		"variety"          );
_LIT8( KHAlign8,                "halign"           );
_LIT8( KCenter8,                "center"           );
_LIT8( KLeft8,                  "left"             );
_LIT8( KRight8,                 "right"            );
_LIT8( KHeight8,        		"height"           );
_LIT8( KWidth8,        			"width"           );
_LIT8( KTextVisual8,        	"textvisual"           );
_LIT8( KImageVisual8,        	"imagevisual"           );
_LIT8( KImagePath8,        		"imagepath"           );
_LIT8( KGridLayout8,        	"gridlayout"           );
_LIT8( KEnabled8,        	"enabled"           );
_LIT8( KRow8,        			"row"           );
_LIT8( KCol8,        			"col"           );
_LIT8( KPositionX8,        		"positionx"           );
_LIT8( KPositionY8,        		"positiony"           );
_LIT8( KStyle8,        			"style"           );
_LIT8( KTargetValue8,        	"targetvalue"           );
_LIT8( KQfnPrimarySmall8,       "qfn_primarysmall"           );
_LIT8( KHorizontalAlign8,       "horizontalalign"           );
_LIT8( KLct8, 					"lct");
_LIT8( KHighlight8,      		"highlight"       );
_LIT8( KNoHighlight8,   		"nohighlight"    );
_LIT8( KLandscape8,        		"landscape"         );

_LIT8( KMmMoveIndicatorFrame8,    		"mul_move_indicator_frame"      	);
_LIT8( KMmMoveIndicatorArrowTop8,  		"mul_move_indicator_arrow_top"   	);
_LIT8( KMmMoveIndicatorArrowBottom8,	"mul_move_indicator_arrow_bottom"   );
_LIT8( KMmMoveIndicatorArrowLeft8,    	"mul_move_indicator_arrow_left"     );
_LIT8( KMmMoveIndicatorArrowRight8,   	"mul_move_indicator_arrow_right"    );
_LIT8( KMI8,        					"MI" 						        );

_LIT( KTab,         "\t"      	);
_LIT( KAkn, 		"akn"		);
_LIT( KUnderline, 	"_"			);
_LIT( KLct, 		"lct"		);
_LIT( KCustom, 		"custom"	);

_LIT( KZResource,        "z:\\resource\\"    );

_LIT( KFormat1,          "\t%S\t%S\t"        );
_LIT( KFormat2,          "%d\t%S\t%S\t"      );


_LIT( KMLID,          "MLID"      );
_LIT8( KListSingleLargeGraphicPane8,        "list_single_large_graphic_pane"           );
_LIT8( KUiaccelAalistDoubleGraphicPane8,        "uiaccel_aalist_double_graphic_pane"           );
_LIT8( KUiaccelAalistDoublePane8,        "uiaccel_aalist_double_pane"           );
_LIT8( KUiaccelAalistSinglePane8,        "uiaccel_aalist_single_pane"           );
_LIT8( KUiaccelAalistGeneAdPane8,        "uiaccel_aalist_gene_ad_pane"           );
_LIT8( KUiaccelAalistSinglePaneT18,        "uiaccel_aalist_single_pane_t1"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneT18,        "uiaccel_aalist_double_graphic_pane_t1"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneT28,        "uiaccel_aalist_double_graphic_pane_t2"           );
_LIT8( KUiaccelAalistDoublePaneT18,        "uiaccel_aalist_double_pane_t1"           );
_LIT8( KUiaccelAalistDoublePaneT28,        "uiaccel_aalist_double_pane_t2"           );
_LIT8( KlistSingleLargeGraphicPaneT18,        "list_single_large_graphic_pane_t1"           );
_LIT8( KUiaccelAalistSinglePaneG18,        "uiaccel_aalist_single_pane_g1"           );
_LIT8( KUiaccelAalistSinglePaneG28,        "uiaccel_aalist_single_pane_g2"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneG1,        "uiaccel_aalist_double_graphic_pane_g1"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneG28,        "uiaccel_aalist_double_graphic_pane_g2"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneG38,        "uiaccel_aalist_double_graphic_pane_g3"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneG68,        "uiaccel_aalist_double_graphic_pane_g6"           );
_LIT8( KUiaccelAalistDoubleGraphicPaneG78,        "uiaccel_aalist_double_graphic_pane_g7"           );
_LIT8( KUiaccelAalistDoublePaneG18,        "uiaccel_aalist_double_pane_g1"           );
_LIT8( KUiaccelAalistDoublePaneG28,        "uiaccel_aalist_double_pane_g2"           );
_LIT8( KlistSingleLargeGraphicPane8,        "list_single_large_graphic_pane"           );
_LIT8( KlistSingleLargeGraphicPaneG18,        "list_single_large_graphic_pane_g1"           );
_LIT8( KlistSingleLargeGraphicPaneG2Cp28,        "list_single_large_graphic_pane_g2_cp2"           );
_LIT8( KlistSingleLargeGraphicPaneG4Cp28,        "list_single_large_graphic_pane_g4_cp2"           );
_LIT8( KUiaccelAalistGeneAdPaneG18,        "uiaccel_aalist_gene_ad_pane_g1"           );

_LIT8( KUiaccelAagridCellImagePane8,        "uiaccel_aagrid_cell_image_pane"           );
_LIT8( KUiaccelAagridCellImagePaneT18,        "uiaccel_aagrid_cell_image_pane_t1"           );
_LIT8( KUiaccelAagridCellImagePaneG18,        "uiaccel_aagrid_cell_image_pane_g1"           );
_LIT8( KUiaccelAagridCellImagePaneG28,        "uiaccel_aagrid_cell_image_pane_g2"           );
_LIT8( KUiaccelAagridCellImagePaneG38,        "uiaccel_aagrid_cell_image_pane_g3"           );
_LIT8( KUiaccelAagridCellImagePaneG48,        "uiaccel_aagrid_cell_image_pane_g4"           );

_LIT8( KCellHcAppsPane8,        "cell_hc_apps_pane"           );
_LIT8( KCellHcAppsPaneT18,        "cell_hc_apps_pane_t1"           );
_LIT8( KCellHcAppsPaneG18,        "cell_hc_apps_pane_g1"           );
_LIT8( KCellHcAppsPaneG28,        "cell_hc_apps_pane_g2"           );
_LIT8( KCellHcAppsPaneG38,        "cell_hc_apps_pane_g3"           );

_LIT8( KCellAppPane8,        "cell_app_pane"           );
_LIT8( KCellAppsPaneG18,        "cell_app_pane_g1"           );
_LIT8( KCellAppsPaneT18,        "cell_app_pane_t1"           );
_LIT8( KCellAppsPaneG28,        "cell_app_pane_g2"           );

_LIT8( KListSingleHcAppsPane8,        "list_single_hc_apps_pane"           );
_LIT8( KListSingleHcAppsPaneT18,        "list_single_hc_apps_pane_t1"           );
_LIT8( KListSingleHcAppsPaneG18,        "list_single_hc_apps_pane_g1"           );
_LIT8( KListSingleHcAppsPaneG28,        "list_single_hc_apps_pane_g2"           );
/**
 * Defines the type of a widget.
 */
enum TMmWidgetType
    {
    EWidgetTypeNone = 0,
    EGrid,
    EListbox,
    ECoverflow
    };

enum TImageVisualId
    {
    EImageVisualIdNormalMode = 0x00000000,
    EImageVisualIdSwapMode = 0x00000001,
    EImageVisualIdEditMode = 0x00000002
    };

/**
 * Defines the type of a template.
 */
enum TMmTemplateType
    {
    ETemplateTypeCustom = 0,
    ETemplateTypeLCT
    };

/**
 * Defines the type of a floating item.
 */
enum TMmFloatingItemType
    {
    EDrag,
    EPostDragRefreshItem,
    EPostHighlightChangeRefreshItem,
    ESwapTransition,
    EDragTransition,
    EZoomTransition
    };

const TInt KDelayInSeconds1 = 1000000;
const TInt KDelayInSeconds6 = 6000000;
const TInt KDelayInfinite = -1;

enum TManualAlign
    {
    EManualAlignUndefined,
    EManualAlignLeft,
    EManualAlignCenter,
    EManualAlignRight
    };

#endif // MMWIDGETSCONSTANTS_H
