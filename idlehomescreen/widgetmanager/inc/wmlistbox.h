/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Declares widget listbox for WidgetManager
*
*/

#ifndef ___WMLISTBOX_H__
#define ___WMLISTBOX_H__

// INCLUDES
#include <eiklbi.h>
#include <eiktxlbx.h>
#include <eiktxlbm.h>
#include <eikfrlb.h>
#include <eiktxlbm.h>
#include <aknlists.h>

#include <touchfeedback.h>
#include <touchlogicalfeedback.h>
#include <aknlongtapdetector.h> 
#include "wmwidgetdataobserver.h"
#include "wmwidgetdata.h"
#include "wmwidgetorderdata.h"

// FORWARD DECLARATIONS
class CWmListBox;
class CWmPlugin;
class CListBoxView;
class CFormattedCellListBoxData;
class CAknLongTapDetector;
class CWmListModel;
class CWmWidgetData;
class CAknStylusPopUpMenu;
class MEikMenuObserver;
class CWmListItemDrawer;

// CLASS DECLARATIONS

/**
 * CWmListItemDrawer
 */
NONSHARABLE_CLASS( CWmListItemDrawer ): public CFormattedCellListBoxItemDrawer
	{
public:
    /** Two-phased constructor. */
	static CWmListItemDrawer* NewL(
                CWmPlugin& aWmPlugin,
                MTextListBoxModel* aTextListBoxModel,
                const CFont* aFont,
                CFormattedCellListBoxData* aFormattedCellData,
                CWmListBox* aListBox  );

    /** Destructor. */
    ~CWmListItemDrawer();
    
    /** Default logo image. */
    const CFbsBitmap* DefaultLogoImage();
    /** Default logo mask. */
    const CFbsBitmap* DefaultLogoMask();

    /**
     * sets item height according to layout 
     */
    void UpdateItemHeight();

public:
    /**
     * from CFormattedCellListBoxItemDrawer:
     * draws a single list box item
     */
    void DrawItem(
                TInt aItemIndex, TPoint aItemRectPos, 
                TBool aItemIsSelected, TBool aItemIsCurrent, 
                TBool aViewIsEmphasized, TBool aViewIsDimmed ) const;
    
private:
    /** Constructor for performing 1st stage construction */
    CWmListItemDrawer( 
                CWmPlugin& aWmPlugin,
                MTextListBoxModel* aTextListBoxModel,  
                const CFont* aFont,  
                CFormattedCellListBoxData* aFormattedCellData,
                CWmListBox* aListBox );

    /** 2nd phase constructor */
    void ConstructL();
    
private: // data members
    /** 
     * the plugin root (not owned) 
     */
    CWmPlugin& iWmPlugin;

    /** 
     * pointer to the list box data (not owned) 
     */
    CFormattedCellListBoxData* iCellData;

    /** 
     * pointer to the listbox (not owned) 
     */
    CWmListBox* iListBox;
    
    /** 
     * default logo (when widget does not have one) bitmap 
     */
    CFbsBitmap*         iDefaultLogoImage;
    
    /** 
     * default logo (when widget does not have one) mask 
     */
    CFbsBitmap*         iDefaultLogoImageMask;
    
	};


// CLASS DECLARATIONS
/**
 * CWmListBox
 */
NONSHARABLE_CLASS( CWmListBox ): public CEikFormattedCellListBox,
                      public MWmWidgetDataObserver
	{
public:
    
    /**
     * Two-phased constructor.
     */
	static CWmListBox* NewL(
	        CWmPlugin& aWmPlugin,
            const TRect& aRect,
            const CCoeControl* aParent,  
            TInt aFlags = 0 );

    /** Destructor. */
    ~CWmListBox();

public: // API for manipulating list content
    
    /** Default Logo. */
    const CFbsBitmap* DefaultLogo();
    /** Default Logo mask. */
    const CFbsBitmap* DefaultMask();

    /** 
     * Findbox visibility
     * 
     * @param aVisibility sets find pane visibility
     */
    inline void SetFindPaneIsVisible( TBool aVisibility );
    
    /** 
     * Currently selected item index 
     * 
     * @return return currently selected item 
     */
    TInt CurrentListBoxItemIndex();
    
    /** 
     * real index for listbox item if findbox is active 
     * 
     * @return return index from original list for currently selected item
     */
    TInt RealIndex( TInt aIndex );
    
    /** 
     * number of items in the widget data array (also items on the list) 
     * 
     * @return count of widget data array
     */
    inline TInt WidgetDataCount();
    
    /** 
     * currently selected item
     * @return the selected item, or NULL if nothing selected
     */
    CWmWidgetData* WidgetData();
    
    /** 
     * gets an item by index 
     * 
     * @param aItemIndex index for item to return
     */
    CWmWidgetData& WidgetData( TInt aItemIndex );

    /**
     * gets the constant widget data array
     */
    inline const RWidgetDataValues& WidgetDataArray();

    /** 
     * inserts widget data to the model. Takes ownership of the object.
     * The widget data will appear to correct location in natural order:
     * - in previously saved position, if it has been viewed before
     *   (widget data must have a pointer to persistent widget order)
     * - in case this is a new widget, it will appear topmost
     * - in case there is no persistent widgets order or it is empty,
     *   the widget is added in alphabetical order
     * 
     * NOTE: caller must call HandleItemAdditionL() for the listbox after
     * adding all widget datas to update the listbox correctly!
     * 
     * @param aWidget Data data to add
     * @param aRedraw listbox update flag 
     */
    void AddWidgetDataL( CWmWidgetData* aWidgetData, TBool aRedraw = ETrue );
    
    /** 
     * removes widget data from given index in the model
     * The method reorganises the list model and size.
     * Drawing is still needed to update the appearance.
     * 
     * @param aItemIndex index to remove
     * */
    void RemoveWidgetData( TInt aItemIndex/*, TBool aRedraw = ETrue*/ );
    
    /** 
     * requests to redraw item in given index position 
     * 
     * @param aItemIndex index to redraw
     */
    void RedrawItem( TInt aItemIndex );

    /** sort order of widgets in the list */
    enum TSortOrder
        {
        /** sort list as it was on last run */
        EStoredOrder,
        /** sort alphabetically */
        EAlphabetical,
        /** show latest installed first */
        ELatestFirst
        };

    /** 
     * Sorts the listbox's visible array 
     * 
     * @param aOrder sort order
     */
    void DoSortToVisibleArray( TSortOrder aOrder );
    
    /**
     * Responds to layout switched.
     */
    void HandleLayoutChanged();

    /**
     * Returns size of logo 
     * 
     * @return Size of logo rect
     */
    TSize LogoSize();

    /**
     * gets the constant widget order data array
     */
    inline const ROrderArray& OrderDataArray();
    
    /** 
     * Sorts the order array
     * 
     * @param aOrder sort order
     */
    void DoSortToOrderData( TSortOrder aOrder );
    
    /**
     * Adds order data to array
     */
    void AddOrderDataL( CWmWidgetOrderData* aOrderData );
    
    /*
     * Returns orderdata object by index
     */
    CWmWidgetOrderData* OrderData( TInt aItemIndex );
    
private: // from CEikTextListBox
    /**
     * Creates the item drawer.
     * 
     * @see CEikTextListBox::CreateItemDrawerL
     */
	void CreateItemDrawerL();

protected: // from base class CCoeControl
    
    /**
     * Sets the control's extent, specifying a rectangle.
     * 
     * @see CCoeControl::SizeChanged
     */
    void SizeChanged();

private: // from base class CCoeControl

    /*
     * Draws the control.
     * 
     * @see CCoeControl::Draw
     */
    void Draw( const TRect& aRect ) const;
	
private:
  	 /** Constructor for performing 1st stage construction */
    CWmListBox( CWmPlugin& aWmPlugin );

    /** 2nd phase constructor */
    void ConstructL(
            const TRect& aRect,
            const CCoeControl* aParent,
            TInt aFlags );
    
private:
    /** 
     * linear order for sorting, depending on the sort type being used 
     */
    TLinearOrder<CWmWidgetData> SortOrder( TSortOrder aOrder );
    
    /** 
     * linear order for sorting, depending on the sort type being used 
     */
    TLinearOrder<CWmWidgetOrderData> SortOrderToOrderData( TSortOrder aOrder );
    
protected: // from MWmWidgetDataObserver

    /** 
     * widget needs redrawing 
     * 
     * @param aWidgetData item to redraw
     */
    void HandleWidgetDataChanged( CWmWidgetData* aWidgetData );

private:

    /** 
     * the plugin root (not owned) 
     */
    CWmPlugin&          iWmPlugin;

    /** 
     * array of widget data objects (the list model) 
     */
    RWidgetDataValues   iVisibleWidgetArray;

    /** 
     * the currently active item ADD button's rectangle, for push recognition 
     */
    TRect               iButtonRect;

    /** 
     * state of list box find pane 
     */
    TBool               iFindPaneIsVisible;
	
    /** size of logo rect in list item */
    TSize               iLogoSize;
    
    /** 
     * array of widget order objects
     */
    ROrderArray         iOrderDataArray;
	};
#include "wmlistbox.inl"

#endif ___WMLISTBOX_H__

// End of File
