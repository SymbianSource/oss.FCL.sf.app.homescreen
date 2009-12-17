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
*  Version     : %version: MM_24.1.8 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef MMTEMPLATELIBRARY_H
#define MMTEMPLATELIBRARY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h> //RHashMap
#include <avkon.hrh>
#include <xmlengdom.h>
#include <gdi.h>
#include <eiksbfrm.h>
#include <AknLayout2Def.h>
#include "mmwidgetsconstants.h"
#include <AknDef.hrh>

// default templates
// grid
_LIT8( KDefaultGridLogicalTemplate, "logical_template_3" );
// list
_LIT8( KDefaultListLogicalTemplate, "list_single_hc_apps_pane" );


// STRUCT DECLARATION

/**
 *  Struct used to store template child data
 * 
 * @since S60 v5.0
 * @ingroup group_mmwidgets
 */
class TTemplateChild
    {
    
public:
	
    /**
     * Rectangle of subcell according to position in item.
     */
    TRect iRectAccordingToParent;
    
    /**
     * Data type represented by the subcell.
     */
    TBufC8<64> iData;
    
    /**
     * The LCT template used to calculate the subcell position.
     */
    TBufC8<64> iLct;
    
    /**
     * The variety for the lct template.
     */
    TInt iVariety;
    
    /**
     * Depicts if the subcell is an image or text.
     */
    TBool iIsImage;
    
    /**
     * The font id used for displaying text.
     */
    TAknLogicalFontId iFontId;
    
    /**
     * The type of align for the text.
     */
    CGraphicsContext::TTextAlign  iTextAlign;
    
    /**
     * The additional Id for a subcell element used to differentiate it
     * from others. e.g the backdrop image has a different visual id 
     * since it is displayed only in edit mode.
     */
    TImageVisualId iImageVisualId;
    
    /**
     * The type of horizontal align for the icon.
     */
    TManualAlign iHAlign;

    };

// CLASS DECLARATION

/**
 *  Used by Multimedia Menu to load template data.
 *  
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmTemplateLibrary ) : public CBase
	{
public:
	// Constructors and destructor
    
    /**
     * Destructor.
     */
    ~CMmTemplateLibrary();

	/**
	 * Two-phased constructor.
	 */
	IMPORT_C static CMmTemplateLibrary* NewL();
	
	/**
     * Two-phased constructor.
     */
	IMPORT_C static CMmTemplateLibrary* NewLC();

private:

	/**
	 * Constructor for performing 1st stage construction.
	 */
	CMmTemplateLibrary();

	/**
	 * Default symbian 2nd stage constructor.
	 */
	void ConstructL();

public:

	/**
	 * Gets the size of template item according to highlight and orientation.
	 * 
	 * @since S60 v3.0
	 * @param aWidgetType The widget type.
	 * @param aTemplate The current template for the item. 
	 * @param aLandscapeOrientation Is view in landscape orientation.
	 * @param aHighlighted Is item highlighted.
	 * @param aParentRect The bounding rect in which the item is drawn.
	 * 
	 * @return size of item
	 */
	TSize GetSize( TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
			TBool aLandscapeOrientation = 0, TBool aHighlighted = 0, 
			TRect aParentRect = TRect() );
    
    /**
     * Gets the size of template.
     * 
     * @since S60 v3.0
     * @param aItemSize Item size to be set.
     * @param aWidgetType The widget type.
	 * @param aTemplate The current template for the item. 
	 * @param aLandscapeOrientation Is view in landscape orientation. 
	 * @param aHighlighted Is item highlighted.
	 * @param aParentRect The bounding rect in which the item is drawn.
	 * 
     * @return error code
     */
	TInt GetSize(  TSize& aItemSize, TMmWidgetType aWidgetType, 
	        const TDesC8& aTemplate, TBool aLandscapeOrientation, 
	        TBool aHighlighted , TRect aParentRect = TRect() );
    
    /**
     * Get array of templates for subcells ( item children ). 
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aArray The array of subcell children to be set.
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation.
     * @param aHighlighted Is item highlighted.
     * @param aIsEditMode Is in edit mode. 
     * 
     */
	void GetChildrenL( TMmWidgetType aWidgetType,
			RArray< TTemplateChild >& aArray, const TDesC8& aTemplate, 
			TBool aLandscapeOrientation = 0, TBool aHighlighted = 0, TBool aIsEditMode = 0 );
    
    /**
     * Get the layout size ( Grid ) e.g. 3x4
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation.
     * 
     * @return the size of the layout.
     */
	TSize GetLayoutSizeL( TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
			TBool aLandscapeOrientation );
    
    /**
     * Get rect of move indicator according to item (as reference position).
     * The rect is relevant to the item in point (0,0). 
     * e.g for item size 20x360 the rect can be (-5,-5):(365,25).
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation. 
     * @param aHighlighted Is item highlighted.
     * 
     * @return rect of move indicator area.
     */
	TRect GetMoveIndicatorRect( TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
			TBool aLandscapeOrientation, TBool aHighlighted );
    
    /**
     * Get children templates for move indicator. 
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aArray Array of children templates.
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation.
     * @param aHighlighted Is item highlighted.
     * 
     */
	void GetMoveIndicatorChildrenL( TMmWidgetType aWidgetType,
			RArray< TTemplateChild >& aArray, const TDesC8& aTemplate, TBool aLandscapeOrientation, TBool aHighlighted );
    
    /**
     * Get size of item for LCT template.
     * 
     * @since S60 v3.0
     * @param aLCTTemplate The LCT template name.
     * @param aVariety The variety for aLCTTemplate.
     * @param aWindowLayout The window layout to be used to calculet the size.
     * @param aLandscapeOrientation ETrue if view in landscape orientation otherwise EFalse.
     * 
     * @return item size
     */
	TSize GetLCTSize( const TDesC8& aLCTTemplate, TInt aVariety, TAknWindowLineLayout& aWindowLayout, TBool aLandscapeOrientation );
    
    /**
     * Clean the cache
     * 
     * @since S60 v3.0
     */
	void CleanAndClearCache( );

    /**
     * The template content is loaded. LCT templates have highest priority. 
     * If there is no LCT template, a custom template is loaded.
     * 
     * @since S60 v3.0
     * @param aTemplate The current template for the item.
     * @param aWidgetType The widget type.
     * @param aMmTemplateType Type of template (LCT/custom).
     * 
     * @return Template content.
     */
	HBufC8* GetTemplateContentL( const TDesC8& aTemplate, TMmWidgetType aWidgetType, TMmTemplateType& aMmTemplateType );
    
    /**
     * Sets scrollbar visibility so template library is aware.
     * 
     * @since S60 v3.0
     * @param aIsScrollbarVisible Is scrollbar visible.
     */
    void SetScrollbarVisibilityL( TBool aIsScrollbarVisible );
    
    /**
     * Gets scrollbar visibility.
     * 
     * @since S60 v3.0
     * @return Is scrollbar visible.
     */
    TBool GetScrollbarVisibility();
    
    /**
     * Sets the current zoom so template library can retrieve the appropriate templates.
     * 
     * @since S60 v3.0
     * @param aZoom Is The current zoom to be set.
     */
	void SetZoom( TAknUiZoom aZoom =  EAknUiZoomAutomatic );
	
	/**
     * Sets the scrollbar width so template library can calculate the width of the item correctly.
     * 
     * @since S60 v3.0
     * @param aScrollbarWidth The current width of the scrollbar.
     */
	void SetScrollbarWidthL( TInt aScrollbarWidth );
	
	/**
	 * Gets scrollbar width.
	 * 
	 * @since S60 v3.0
	 * @return scrollbar width
	 */
	TInt ScrollbarWidth();
	
	/**
     * Gets the parent rect for given orientation from which templates were calculated.
     * 
     * @since S60 v3.0
     * @param aLandscapeOrientation The current landscape orientation.
     * @return Parent rect for given orientation.
     */
	TRect GetParentRect( TBool aLandscapeOrientation );
	
private:
	/**
	 * Get the lookup text.
	 * 
	 * @since S60 v3.0
	 * @param aTemplate The current template for the item.
	 * @param aWidgetType The widget type.
	 * @param aLandscapeOrientation Is view in landscape orientation. 
	 * @param aHighlighted Is item highlighted.
	 * 
	 * @return Lookup text. 
	 */
	HBufC8* LookupText( const TDesC8& aTemplate, TMmWidgetType aWidgetType,
        TAknUiZoom aZoom, TBool aLandscapeOrientation = EFalse, 
        TBool aHighlighted = EFalse );	
    
    /**
     * Get the lookup text for layout size.
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation. 
     * 
     * @return lookup layout text.
     */
	HBufC8* LookupLayoutText( TMmWidgetType aWidgetType, TAknUiZoom aZoom, 
        const TDesC8& aTemplate, TBool aLandscapeOrientation );
    
	/**
     * Get the lookup text for move indicator.
     * 
     * @since S60 v3.0
     * @param aLookupText The lookup text to be used to create the lookup indicator text.
     * @return lookup Indicator text.
     */
	HBufC8* LookupIndicatorText( const TDesC8& aLookupText );

    /**
     * Load the template to cache.
     * 
     * @since S60 v3.0
     * @param aWidgetType The widget type.
     * @param aTemplate The current template for the item.
	 * @param aLandscapeOrientation The current orientation.
     */
	void LoadTemplateL( TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
			TBool aLandscapeOrientation );
	
    /**
     * Get template file content.
     * 
     * @since S60 v3.0
     * @param aContent Descriptor to which the file content should be set.
     * @param aTemplate The current template for the item.
     * @param aWidgetType The widget type.
     * @param aMmTemplateType Type of template (LCT/custom).
     * @param aLoadCustomTemplate Value to be set depending on template type loaded (LCT/custom).
     */
	void GetTemplateFileContentL (HBufC8*& aContent, const TDesC8& aTemplate,
		TMmWidgetType aWidgetType, TMmTemplateType& aMmTemplateType,
		TBool aLoadCustomTemplate = EFalse);
		
    /**
     * Process xml element.
     * 
     * @since S60 v3.0
     * @param aMmTemplateType Type of template (LCT/custom).
     * @param aElement Element ( xml ).
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation. 
     * @param aHighlighted Is item highlighted.
     * 
     */
	void ProcessElementL (TMmTemplateType aMmTemplateType,
		TXmlEngElement aElement, const TDesC8& aTemplate,
		TBool aLandscapeOrientation, TBool aHighlighted);	
    
	/**
     * Process the LCT template.
     * 
     * @since S60 v3.0
     * @param aElement Element (xml).
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation. 
     * @param aHighlighted Is item highlighted.
     */
	void ProcessLCTTemplateElementL( TXmlEngElement aElement, 
	                                         const TDesC8& aTemplate, 
	                                         TBool aLandscapeOrientation,
	                                         TBool aHighlighted );

    /**
     * Process the custom template.
     * 
     * @since S60 v3.0
     * @param aElement Element (xml)
     * @param aTemplate The current template for the item.
     * @param aLandscapeOrientation Is view in landscape orientation. 
     * @param aHighlighted Is item highlighted.
     */
	void ProcessCustomTemplateElementL( TXmlEngElement aElement, 
	                                         const TDesC8& aTemplate, 
	                                         TBool aLandscapeOrientation,
	                                         TBool aHighlighted );

    /**
     * Setup LCT template data.
     * 
     * @since S60 v3.0
     * @param aChildTemplate Template where the data should be set up. 
     * @param aChildElement Element (xml).
     * @param aItemSize Item size.
     */
	void SetupLCTTemplateL(TTemplateChild& aChildTemplate, TXmlEngElement& aChildElement, TSize aItemSize );
    
	/**
     * Setup custom template. 
     * 
     * @since S60 v3.0
     * @param aChildTemplate Template where the data should be set up. 
     * @param aChildElement Element (xml).
     */
	void SetupCustomTemplateL(TTemplateChild& aChildTemplate, TXmlEngElement& aChildElement );
    
	/**
     * Set the template visual id.
     * 
     * @since S60 v3.0
     * @param aChildTemplate Template data.
     */
	void SetupTemplateVisualId(TTemplateChild& aChildTemplate );
    
	/**
     * Setup the item children template data for move indicator and cache the data.
     * 
     * @since S60 v3.0
     * @param aLookupString Lookup string.
     * @param aItemSize Item size.
     */
	void SetupMoveIndicatorTemplateChildrenL( const TDesC8& aLookupString, TSize aItemSize );
    
	/**
     * Adjusts the item size loaded to template cache according to layout size and orientation.
     * This method adjusts the item width so that items occupy the view correctly.
     * 
     * @since S60 v3.0
     * @param aSize The size of the item to be adjusted. 
     * @param aLayoutSize The layout of the widget e.g 3x4 
     * @param aLandscapeOrientation The orientation of the template being processed.
     */
	void AdjustItemWidth( TSize& aSize, TSize aLayoutSize, TBool aLandscapeOrientation );

	/**
     * Updates the parent rect according to orientation.
     * 
     * @since S60 v3.0
     * @param aParentRect The current parent rectangle.
     * @param aLandscapeOrientation The current orientation.
     */
	void UpdateParentRect( TRect aParentRect, TBool aLandscapeOrientation );
    
	/**
     * Sets the parent rect for given orientation.
     * 
     * @since S60 v3.0
     * @param aLandscapeOrientation Orientation for which parent rect shoul be set.
     * @param aParentRect The parent rect of current orientation.
     */
	void SetParentRect( TBool aLandscapeOrientation, TRect aParentRect);
    
	/**
     * Appends the edit mode template (backdrop icon) to the array of template children passed to the item drawer for drawing.
     * 
     * @since S60 v3.0
     * @param aTemplateArray The template array to which the edit mode template is appended.
     * @param aSize The size of the item for which the edit mode template should be calculated.
     */
	void AppendEditModeTemplateL( RArray< TTemplateChild >& aTemplateArray, TSize aSize );
    
	/**
     * Adjusts the item size according to layout, LCT item size and orientation.
     * 
     * @since S60 v3.0
     * @param aItemSize The LCT item size.
     * @param aLayoutSize The layout size to be used to calculate item size.
     * @param aLandscapeOrientation The orientation for which the template is loaded.
     */
	void AdjustItemSize(TSize& aItemSize,  TSize aLayoutSize, TBool aLandscapeOrientation );
    
	/**
     * Get the current zoom for displaying templates.
     * 
     * @since S60 v3.0
     * @return The current zoom.
     */
	TAknUiZoom Zoom( );
    
	/**
     * Caches the layout size
     * 
     * @since S60 v3.0
     * @param aLayoutSize The layout size to ba cached.
     * @param aTemplate The template for the cached layout size.
     * @param aLandscapeOrientation The orientation for which the template is loaded.
     * @return The current zoom.
     */
	void CacheLayoutSizeL( TSize aLayoutSize, const TDesC8& aTemplate, TBool aLandscapeOrientation );
	
	/**
     * Adjusts icon's align to layout (left, center, right).
     * 
     * @since S60 v5.0
     * @param aParentSize The LCT parent item size.
     * @param aTemplateChild Template data.
     */
    void AdjustIconPosition(TSize aParentSize,
                    TTemplateChild& aTemplateChild);

private: //Members

	/**
	 * Widget type of template being loaded.
	 */
    TMmWidgetType iWidgetType;
    
    /**
     * Template type which is being loaded (LCT/custom).
     */
    TMmTemplateType iMmTemplateType;
    
    /**
     * Template sizes map.
     */
    RHashMap< HBufC8*, TSize> iTemplateSizesMap;
    
    /**
     * Item children template map.
     */
    RHashMap< HBufC8*, RArray<TTemplateChild> > iTemplateChildrenMap;
    
    /**
     * Move indicator rects map.
     */
    RHashMap< HBufC8*, TRect> iMoveIndicatorRectsMap;
    
    /**
     * Parent bounding rect.
     */
    TRect iParentRectPortrait;
	
    /**
     * Parent bounding rect.
     */
    TRect iParentRectLandscape;
    
    /**
     * Scrollbar is visible.
     */
    TBool iIsScrollbarVisible;
	
    /**
     * Scrollbar width.
     */
    TInt iScrollbarWidth;
	
    /**
     * Zoom type.
     */
    TAknUiZoom iZoom;

	};
#endif // MMTEMPLATELIBRARY_H
