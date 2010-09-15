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
* Declares main container control for Widget Manager application.
*
*/

#ifndef WMMAINCONTAINER_H
#define WMMAINCONTAINER_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include <coecobs.h>
#include <coecntrl.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <eiklbo.h>  // MEikListBoxObserver
#include <aknsfld.h>  // MAdaptiveSearchTextObserver
#include "wmwidgetloaderao.h"

// FORWARD DECLARATIONS
class CWmPlugin;
class CWmListBox;
class CAknsBasicBackgroundControlContext;
class CAknViewAppUi;
class CAknSearchField;
class CCoeControl;
class CAknSearchField;
class CWmStore;
class CWmMainContainerView;
class CWmConfiguration;

/**
 * Container class for WmMainContainer
 * 
 * @class    CWmMainContainer WmMainContainer.h
 */
NONSHARABLE_CLASS( CWmMainContainer ) : public CCoeControl,
                                        public MEikListBoxObserver,
                                        public MAdaptiveSearchTextObserver,
                                        public MWmWidgetloaderObserver
    {
public: // constructors and destructor
    
    /*
     * Two-phased constructor.
     * 
     * @param aRect container rect
     * @param aWmPlugin wm plugin
     */
    static CWmMainContainer* NewL( 
        const TRect& aRect,
        CWmPlugin& aWmPlugin );

    /*
     * Two-phased constructor.
     * 
     * @param aRect container rect
     * @param aWmPlugin wm plugin
     */
    static CWmMainContainer* NewLC( 
        const TRect& aRect,
        CWmPlugin& aWmPlugin );    

    /** Destructor */
    virtual ~CWmMainContainer();
    
private:
    /** constructor */
    CWmMainContainer(
        CWmPlugin& aWmPlugin );
    
    /** 2nd phase constructor */
    void ConstructL( 
            const TRect& aRect );

public: // new functions    
    
    /** 
     * @return true, if widgets loading operation is ongoing. 
     */
    TBool IsLoadingWidgets();
    
    /** 
     * @return true, if a widget in list is currently selected 
     */
    TBool WidgetSelected();
    
    /** 
     * @return true, ADD command is possible
     */
    TBool CanDoAdd(); 
    
    /** 
     * @return true, if UNINSTALL command is possible
     */
    TBool CanDoUninstall();
    
    /** 
     * @return true, if LAUNCH command is possible
     */
    TBool CanDoLaunch();

    /**
     * @return true, if FIND command is possible
     */
    TBool CanDoFind();
    
    /**
     * @return true, if SORT command is possible
     */
    TBool CanDoSort();
    
    /**
     * @return true, if DETAILS command is possible
     */
    TBool CanDoDetails();
    
    /**
     * @return true, if HELP command is possible
     */
    TBool CanDoHelp();
    
    /** 
     * executes widget details dialog launch 
     */
    void LaunchDetailsDialogL();

    /** 
     * executes widget addition to home screen 
     */
    void AddWidgetToHomeScreenL();
    
    /** 
     * executes widget launch 
     */
    void LaunchWidgetL();
    
    /** 
     * executes findbox activation 
     */
    void ActivateFindPaneL( TBool aActivateAdaptive = EFalse );

    /** 
     * executes findbox deactivation 
     */
    void DeactivateFindPaneL(TBool aLayout = ETrue);
    
    /** 
     * sorts the widget list in alphabetical order
     */
    void SortListAlphabeticallyL();

    /** 
     * executes widget uninstall 
     */
    void UninstallWidgetL();

    /**
     * Store menu command.
     */
    void HandleStoreCommandL();
    
    /**
     * Show widget manager help
     */
    void ShowHelpL();

    /** 
     * handles situation when widget list has changed.
     * in practice reloads the widget list from the widgets API
     * and redraws the screen.
     */
    void HandleWidgetListChanged();

public:

    /**
     * Handles focus changed events.
     *
     * @see CCoeControl::FocusChanged
     */    
    void FocusChanged( TDrawNow aDrawNow );    
    
    /**
     * Handles key events.
     * 
     * @see CCoeControl::OfferKeyEventL
     */
    TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );
    
    /**
     * Handles a change to the control's resources
     * 
     * @see CCoeControl::HandleResourceChange
     */
    void HandleResourceChange( TInt aType );
    
    /**
     * Handles pointer events.
     * 
     * @see CCoeControl::HandlePointerEventL
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * Moves focus to the widgets list, alternatively also setting the currently
     * selected item.
     * @param aIndex the item in widgets list to highlight during setting focus.
     *        By default the previously selected item will be highlighted.
     */
    void SetFocusToWidgetList( TInt aIndex = KErrUnknown );

    /*
     * Tells if user inputs should be handeled or not. If ret is EFalse wm 
     * can ignore user inputs. This is becouse closing view is async and user
     * might give some inputs before view is closed. 
     */
    TBool ClosingDown();
        
    /*
     * To set iClosingDown. See above.
     */
    void SetClosingDown( TBool aClosingDown );
    
    /**
     * access to WM list
     */
    CWmListBox& WmListBox();
    
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
    
    /**
    * Suppy Mop object for control context
    * @param TTypeUid aId
    */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

private: // from MEikListBoxObserver

    /**
     * List box observer interface.
     * 
     * @see MEikListBoxObserver::HandleListBoxEventL
     */
    void HandleListBoxEventL(
                        CEikListBox* aListBox,
                        TListBoxEvent aEventType);

private: // from MAdaptiveSearchTextObserver    
    /**
     * Adaptive search observer interface.
     * 
     * @see MAdaptiveSearchTextObserver::AdaptiveSearchTextChanged
     */
    void AdaptiveSearchTextChanged( CAknSearchField* aSearchField );
    
private: // New functions
    
    void DisplayAndLaunchStoresL();
    static void CleanupIconArray( TAny* aIconArray );

    void AddControlL( CCoeControl* aControl, TInt aControlId );
    void InitializeControlsL( const TRect& aRect );
    void LayoutControls();
    void StartLoadingWidgetsL();
    void RemoveCtrlsFromStack();
    CCoeControl* FindChildControlByPoint( const TPoint& aPoint );
    void HandleFindSizeChanged();
    TKeyResponse HandleListKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );
    TKeyResponse HandleSearchKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );
    
public: // from MWmWidgetloaderObserver
    void LoadDoneL( TBool aWidgetListChanged );
    
private:

    /**
     * the plugin root
     */
    CWmPlugin&              iWmPlugin;

    /**
     * the widgets list
     */
    CWmListBox*             iWidgetsList;

    /**
     * search filed
     */
    CAknSearchField*         iFindbox;
    
    /**
     * search filed visibility switch
     */
    TBool                    iFindPaneIsVisible;
    
    /**
     * background
     */
    CAknsBasicBackgroundControlContext* iBgContext;

    /**
     * AO for loading widgets into the list UI
     */
    CWmWidgetLoaderAo*      iWidgetLoader;

    /**
     * Currectly selected store.
     */
    CWmStore*               iSelectedStore;

    /**
     * If this is set to ETrue it means wmview is closing and all user inputs
     * should be ignored.
     */
    TBool                   iClosingDown;
    
#ifdef _WM_UNIT_TEST
    friend class CWmUnitTest;
#endif
    };

#endif // WMMAINCONTAINER_H

// End of File
