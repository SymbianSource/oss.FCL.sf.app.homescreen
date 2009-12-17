/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmGridContainer declaration
*  Version     : %version: MM_29.1.13 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMGRIDCONTAINER_H
#define C_MMGRIDCONTAINER_H

#include <e32std.h>
#include <e32base.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <AknGrid.h>
#include "mmwidgetcontainer.h"
#include "hnglobals.h"

class CMmGrid;
class CMmMarqueeAdapter;
class CEikColumnListBox;
class CNaviScrollTimer;
class CHnSuiteModel;
class CMmTemplateLibrary;
class CMmListBoxItemDrawer;

/**
 *  Container for Multimedia Menu grid widget 
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v5.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmGridContainer ) : public CMmWidgetContainer
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary template library for Drawer   
     */
    static CMmGridContainer* NewL( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary template library for Drawer
     */
    static CMmGridContainer* NewLC( const TRect& aRect, 
        MObjectProvider* aObjectProvider, CMmTemplateLibrary* aTemplateLibrary );
    
    /**
     * Destructor.
     * 
     * @since S60 v5.0
     */
    virtual ~CMmGridContainer();

    /**
     * Gets grid widget.
     *
     * @since S60 v5.0
     * @return Pointer to grid.
     */
    CAknGrid* Grid();
    
public: // from base class CMmWidgetContainer
 
    /**
     * Handle item addition.
     * 
     * @since S60 v3.0
     */
    void HandleItemAdditionL();
	
    /**
     * Handle item removal.
     * 
     * @since S60 v3.0
     */
    void HandleItemRemovalL(); 
    
    /**
     * Sets the default highlight in grid.
     *
     * @since S60 v5.0
     * @param aRedraw Is highlight to redraw.
     */
    IMPORT_C void SetDefaultHighlightL(  TBool aRedraw = ETrue  );
    
    /**
     * Checks if widget is in edit mode state.
     *
     * @since S60 v5.0
     * @param aIsEditMode Is grid in edit mode state.
     */
    void SetEditModeL( TBool aIsEditMode );
    
    /**
     * Derived from CCoeControl, called when container rectangle size is changed.
     *
     * @since S60 v5.0
     */
    void SizeChanged();   
    
    /**
     * Gets grid model.
     *
     * @since S60 v5.0
     * @return Grid model.
     */
    CMmListBoxModel* GetMmModel();
    
    /**
     * Sets empty text shown when grid is empty.
     *
     * @since S60 v5.0
     * @param aText Text to be shown when grid is empty.
     */
    IMPORT_C void SetEmptyTextL( const TDesC& aText );
    
    /**
     * Sets item drawer and view background context.
     *
     * @since S60 v5.0
     * @param aBgContext Background context.
     */
    void SetItemDrawerAndViewBgContext(
        CAknsBasicBackgroundControlContext * aBgContext );   
    
    /**
     * Sets up the grid layout.
     *
     * @since S60 v5.0
     */
    void SetupWidgetLayoutL();
    
    /**
     * Sets suite model.
     * 
     * @since S60 v5.0
     * @param aModel Suite model.
     */
    void SetSuiteModelL( CHnSuiteModel* aModel );
    
    /**
     * Set highlight visibility.
     * 
     * @since S60 v5.0
     * @param aVisible Visibility status. 
     */
    void SetHighlightVisibilityL( TBool aVisible );
    
    /**
     * Retrieve type of widget.
     * 
     * @since S60 v3.0
     * @return Type of widget.
     */ 
    THnSuiteWidgetType WidgetType();
    
    /**
     * Called when filp state changed.
     * 
     * @since S60 v3.0
     */ 
    void FlipStateChangedL();
    
    /**
      * Draws the widget view.
      *
      * @since S60 v5.0
      */
    virtual void DrawView();
    
    /**
     * Set the vertical item offset;
     * @param aOffset The offset to set to the widget.
     * 
     * @since S60 v5.0
     */
    virtual void SetVerticalItemOffset( TInt aOffset );

    /**
     * Gets the current widget vertical item offset.
     * 
     * @since S60 v5.0
     * @return The current widget vertical item offset.
     */
    virtual TInt VerticalItemOffset() const;
    
    /**
     * Tells if item is visible (fully or partially).
     * Overridden from the base class CMmWidgetContainer in order to
     * provide a reliable implementation for grid.
     * 
     * @param aItemIndex Item index.
     * @return ETrue if visible, EFalse otherwise.
     */
    TBool ItemIsVisible( TInt aItemIndex ) const;
    
private:
    /**
     * Default constructor.
     * 
     * @since S60 v5.0 
     */
    CMmGridContainer();
    
    /**
     * 2nd phase constructor.
     * 
     * @since S60 v5.0 
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary template library for Drawer
     */
    void ConstructL( const TRect& aRect, MObjectProvider* aObjectProvider,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Constructs listbox from resource, creates scrollbar and sets empty list background text.
     *
     * @since S60 v5.0
     * @param aTemplateLibrary template library for Drawer
     * @return Grid model.
     */
    CMmGrid* CreateGridL( CMmTemplateLibrary* aTemplateLibrary );
    
    /**
	 * Gets column count in current view.
	 * @returns Column count in current view.
	 */
    TInt ColumnsInCurrentView();
    
    /**
     * Gets row count in current view.
     * @returns Row count in current view.
     */
    TInt RowsInCurrentView();
    
    /**
	 * Updates current view's scrollbar thumbs.
	 */
    void UpdateViewScrollBarThumbs();
    
    /**
     * Gets base default highlight in landscape mode.
     * 
     * @param aLayout current layout of the grid
     * @returns default highlight
     */
    TInt LandscapeOrientationDefaultHighlight( TSize aLayout );

    /**
     * Gets base default highlight in portrait mode.
     * 
     * @param aLayout current layout of the grid
     * @returns default highlight
     */
    TInt PortraitOrientationDefaultHighlight( TSize aLayout );
    
    /**
     * Calculates offset for default highlight position based on
     * index of first visible item.
     * 
     * @returns offset for default highlight
     */
    TInt DefaultHighlightOffset();
    
private: // Data
	
    /**
     * Pointer to listbox control.
     * Own.
     */
    CMmGrid* iGrid;
    
    };

#endif // C_MMGRIDCONTAINER_H
