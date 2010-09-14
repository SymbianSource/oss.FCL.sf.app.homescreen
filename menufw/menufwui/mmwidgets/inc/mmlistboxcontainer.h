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
* Description:  CMmListBoxContainer
*  Version     : %version: MM_21.1.11 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMLISTBOXCONTAINER_H
#define C_MMLISTBOXCONTAINER_H

#include <eiklbo.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <AknsListBoxBackgroundControlContext.h>

#include "mmwidgetcontainer.h"
#include "mmwidgetsconstants.h"
#include "hnglobals.h"

class CMmListBox;
class CMmMarqueeAdapter;
class CEikColumnListBox;
class CNaviScrollTimer;
class CHnSuiteModel;
class CMmTemplateLibrary;

/**
 *  Container for Multimedia Menu listbox widget.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v5.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmListBoxContainer ) : public CMmWidgetContainer
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary Template library for drawer. 
     */
    static CMmListBoxContainer* NewL( const TRect& aRect, 
        MObjectProvider* aObjectProvider, 
        CMmTemplateLibrary* aTemplateLibrary );    

    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary Template library for drawer. 
     */
    static CMmListBoxContainer* NewLC( const TRect& aRect, 
        MObjectProvider* aObjectProvider,
        CMmTemplateLibrary* aTemplateLibrary );
    
    /**
     * Destructor.
     * 
     * @since S60 v5.0
     */
    virtual ~CMmListBoxContainer();
    
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
     * Set the default highlight in listbox.
     *
     * @since S60 v5.0
     * @param aRedraw Is highlight to redraw.
     */
    IMPORT_C void SetDefaultHighlightL( TBool aRedraw = ETrue );
    
    /**
     * Called to set widget in edit mode state.
     *
     * @since S60 v5.0
     * @param aIsEditMode Is edit mode to be set.
     */
    void SetEditModeL( TBool aIsEditMode );
            
    /**
     * Gets the listbox model.
     *
     * @since S60 v5.0
     * @return listbox model
     */
    CMmListBoxModel* GetMmModel();
    	
    /**
     * Sets the empty text for empty list.
     *
     * @since S60 v5.0
     * @param aText Text to be set when list is empty.
     */
    IMPORT_C void SetEmptyTextL(const TDesC& aText);
    
    /**
     * Set background context for item drawer and view.
     *
     * @since S60 v5.0
     * @param aBgContext Background context.
     */
    void SetItemDrawerAndViewBgContext(
        CAknsBasicBackgroundControlContext * aBgContext );
   
    /**
     * Sets up the layout for the widget.
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
     * Gets widget type.
     * 
     * @since S60 v5.0
     * @return Widget type. 
     */
    THnSuiteWidgetType WidgetType();

    /**
     * Sets a proper flag if the widget is brought to foreground.
     * 
     * @param  aCommingFromBackground indicates whether the widget is brought to foreground.
     */
    virtual void SetWidgetCommingFromBackground( TBool aCommingFromBackground );
	
public:
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
        
protected: // from CMmWidgetContainer
    
    /**
     * Checks whether given point collides with specific item's re-order area.
     * 
     * Please see @c CMmWidgetContainer::PointInItemReorderAreaL documentation
     * for details.
     * 
     * @since S60 v5.0
     */
    TBool PointInItemReorderAreaL( TInt aItemIndex, TPoint aPoint );
    
private: 
    /**
     * Default constructor.
     * 
     * @since S60 v5.0 
     */
    CMmListBoxContainer();    

    /**
     * 2nd phase constructor.
     * 
     * @since S60 v5.0 
     * @param aRect Rectangle of widget control.
     * @param aObjectProvider Object provider.
     * @param aTemplateLibrary Template library for drawer. 
     */
    void ConstructL( const TRect& aRect, MObjectProvider* aObjectProvider,
        CMmTemplateLibrary* aTemplateLibrary );

    /**
     * Constructs listbox from resource, creates scrollbar 
     * and sets empty list background text.
     *
     * @since S60 v5.0
     * @param aTemplateLibrary Template library for drawer. 
     * @return List box widget.
     */
    CMmListBox* CreateListboxL( CMmTemplateLibrary* aTemplateLibrary );
    
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

private: // members
    /**
     * Pointer to listbox widget.
     * Owned.
     */
    CMmListBox* iListBox;
    
    };

#endif //__MMLISTBOXCONTAINER_H__
