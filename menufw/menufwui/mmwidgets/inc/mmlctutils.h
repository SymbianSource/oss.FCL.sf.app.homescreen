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
* Description:  
*
*/



#ifndef MMLCTUTILS_H
#define MMLCTUTILS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <aknlayoutscalable_uiaccel.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknDef.hrh>
#include "mmtemplatelibrary.h"

// CLASS DECLARATION

/**
 *  Utilities for LCT scalable layout support in Multimedia Menu.
 * 
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
class MmLCTUtils
	{
	
public:

	/**
	 * Gets the window component layout for the given layout name and variety.
	 * 
	 * @since S60 v3.0
	 * @param aLayout Name of the layout.
	 * @param aVariety Variety for the layout.
	 * @return Window component layout.
	 */
	static TAknWindowComponentLayout ItemLayout (const TDesC8& aLayout,
			TInt aVariety);
	
    /**
     * Gets the text component layout for the given layout name and variety.
     * 
     * @since S60 v3.0
     * @param aLayout Name of the layout.
     * @param aVariety Variety for the layout.
     * @return Text component layout.
     */
	static TAknTextComponentLayout
			TextLayout (const TDesC8& aLayout, TInt aVariety);
	
    /**
     * Gets the Window component layout for the given layout name and variety. Used for setting up graphical subcells.
     * 
     * @since S60 v3.0
     * @param aLayout Name of the layout.
     * @param aVariety Variety for the layout.
     * @return Window component layout.
     */
	static TAknWindowComponentLayout GraphicLayout (const TDesC8& aLayout,
			TInt aVariety);
	
    /**
     * Sets up template data for text subcell.
     * 
     * @since S60 v3.0
     * @param aItemSize Size of the parent item.
     * @param aSubCellTemplate Template data to be set.
     */
	static void SetupTextSubCellTemplate (TSize aItemSize,
			TTemplateChild& aSubCellTemplate);
	
    /**
     * Sets up template data for graphic subcell.
     * 
     * @since S60 v3.0
     * @param aItemSize Size of the parent item.
     * @param aSubCellTemplate Template data to be set.
     */
	static void SetupGraphicSubCellTemplate (TSize aItemSize,
			TTemplateChild& aSubCellTemplate);
	
    /**
     * Calculates the item size using LCT.
     * 
     * @since S60 v3.0
     * @param aLCTTemplate Name of the layout.
     * @param aVariety Variety for the layout.
     * @param aParentRect Parent rectangle.
     * @param aWindowLayout Window component layout to be set.
     * @return Size of the item calculated by LCT.
     */
	static TSize GetLCTSize (const TDesC8& aLCTTemplate, TInt aVariety,
			const TRect aParentRect, TAknWindowLineLayout& aWindowLayout );

    /**
     * Calculates the layout size using LCT.
     * 
     * @since S60 v3.0
     * @param aLCTTemplate Name of the layout template.
     * @param aVariety Variation of the layout.
     * @return Size of the layout (e.g 3x4) calculated by LCT.
     */
	static TSize GetLayoutSize( const TDesC8& aLCTTemplate, TInt aVariety );
    
	/**
     * Gets the structure of parameters for a given layout.
     * 
     * @since S60 v3.0
     * @param aLayout Name of the layout template.
     * @param aVariety Variation of the layout.
     * @return 
     */
	static TAknLayoutScalableParameterLimits LCTParameterLimits( const TDesC8& aLayout, TInt aVariety );
	};

#endif // MMLCTUTILS_H

