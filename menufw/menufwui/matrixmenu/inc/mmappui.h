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
* Description:  Application UI class
*  Version     : %version: MM_83.1.2.1.23.1.24 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMAPPUI_H
#define C_MMAPPUI_H

// INCLUDES
#include <gfxtranseffect/gfxtranseffect.h>     // for transition effects
#include <akntranseffect.h>                    // for transition effects

#include <e32hashtab.h>
#include <aknappui.h>
#include <akntoolbarobserver.h>
#include <eiklbo.h>
#include <AknsSrvClient.h>

#include "hncontrollerinterface.h"
#include "mmwidgetobserver.h"
#include "hnsuiteobserver.h"
#include "mmactionrequest.h"
#include "hnglobals.h"


// FORWARD DECLARATIONS

class CHnInterface;
class CHnSuiteModel;
class MHnMenuItemModelIterator;
class CAknToolbar;
class CMmWidgetContainer;
class CMmListBoxContainer;
class CMmGridContainer;
class CMmHighlightManager;
class CLiwGenericParamList;
class CHnItemModel;
class CMMExtensionManager;
class CMmTemplateLibrary;
class CMmNoMemory;
class CMmAppkeyHandler;
class CAknStylusPopUpMenu;

/**
 *  @ingroup group_matrixmenu
 *  Matrix Menu Application UI.
 *  Regular AVKON Application UI class, acts as controller for the application.
 *  <br>Implements:
 *  <ul>
 *  <li>MHnControllerInterface - for notifications about model changes.</li>
 *  <li>MAknToolbarObserver - to get notifications about toolbar events.</li>
 *  <li>MMmWidgetObserver -
 *      to get notifications about widget highlight changes.</li>
 *  </ul>
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CMmAppUi) : public CAknAppUi,
                            public MHnControllerInterface,
                            public MAknToolbarObserver,
                            public MMmWidgetObserver,
                            public MMmActionRequest,
                            public MAknsSkinChangeObserver
  {

public:

  /**
   * 2nd phase constructor.
   *
   * @since S60 v5.0
   */
  void ConstructL();

  /**
   * C++ default constructor. This needs to be public due to
   * the way the framework constructs the AppUi.
   *
   * @since S60 v5.0
   */
  CMmAppUi();

  /**
   * Virtual Destructor.
   *
   * @since S60 v5.0
   */
  ~CMmAppUi();

// from base class CEikAppUi

    /**
     * From CEikAppUi.
     * Handles a change to the application's resources which
     * are shared across the environment. This function calls
     * CEikAppUi::HandleResourceChangeL except when aType is
     * KEikDynamicLayoutVariantSwitch.
     *
     * @since S60 v5.0
     * @param aType The type of resources that have changed.
     */
    void HandleResourceChangeL( TInt aType );

    /**
     * From CEikAppUi.
     * Takes care of Command Line tail handling.
     *
     * @since S60 v5.0
     * @param aCommand Not used by this app.
     * @param aDocumentName Not used by this app.
     * @param aTail Command line tail.
     */
  TBool ProcessCommandParametersL( TApaCommand aCommand,
                  TFileName& aDocumentName,
                  const TDesC8& aTail );

    /**
     * From CAknAppUi.
     * This method is overriden to ignore certain commands in
     * certain situations. For not ignored commands it simply
     * calls the base class implementation.
     *
     * @param aCommand A command ID.
     */
    void ProcessCommandL(TInt aCommand);

  /**
     * From CEikAppUi.
     * Takes care of Command Line tail handling.
     *
     * @since S60 v5.0
     * @param aUid UID of the message.
     * @param aParams Message params.
     */
  void ProcessMessageL( TUid aUid, const TDesC8& aParams );

  /**
     * From MCoeMessageObserver.
     * Handles window server messages.
     *
     * @since S60 v5.0
     * @param aClientHandleOfTargetWindowGroup The window group that the message was sent to.
     * @param aMessageUid The message UID.
     * @param aMessageParameters The message parameters.
     * @return Indicates whether the message was handled or not handled by the function.
     */
    MCoeMessageObserver::TMessageResponse HandleMessageL(
         TUint32 aClientHandleOfTargetWindowGroup,
         TUid aMessageUid,
         const TDesC8& aMessageParameters );

  /**
     * From MEikMenuObserver (CEikAppUi).
     * Handles dynamic menu pane initialization.
     *
     * @since S60 v5.0
     * @param aResourceId Resource ID of the menu pane.
     * @param aMenuPane The menu pane itself.
     */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

// from base class MHnControllerInterface

    /**
     * From MHnControllerInterface.
     * Notifies UI to start a refresh after content change.
     *
     * @since S60 v5.0
     * @param aRefreshType Type of refresh.
     */
    void NotifyUiRefreshL( const THnUiRefreshType aRefreshType );

// from base class MHnSuiteObserver

    /**
     * From MHnSuiteObserver.
     * Trigger appropriate action on suite model.
     *
     * @since S60 v5.0
     * @param aCustomSuiteEvent Event to perform.
     * @param aModel Suite model where aCustomSuiteEvent should be envoked.
     */
    void HandleSuiteEventL( THnCustomSuiteEvent aCustomSuiteEvent,
                CHnSuiteModel* aModel );

// from base class MEikListBoxObserver

    /**
     * From MEikListBoxObserver.
     * Handles list box events.
     *
     * This pure virtual function is invoked by CEikListBox to
     * notify the observer of list box events.
     *
     * @since S60 v5.0
     * @param aListBox   The originating list box.
     * @param aEventType A code for the event. Further information
     *                   may be obtained by accessing the list box itself.
     */
    void HandleListBoxEventL( CEikListBox* aListBox,
            MEikListBoxObserver::TListBoxEvent aEventType );

// from base class MMmLongTapObserver

    /**
     * From MMmLongTapObserver.
     * Handles long tap event reported by widget container.
     *
     * @since S60 v5.0
     * @param aPenEventLocation The relative to screen pen event location.
     */
    void HandleLongTapEventL( const TPoint& aPenEventLocation );

// from base class MMmKeyEventObserver

    /**
     * From MMmKeyEventObserver.
     * Handles key press events.
     *
     * @since S60 v5.0
     * @param aKeyEvent Key event.
     * @param aType Event type.
     */
    TKeyResponse HandleKeyPressedL( const TKeyEvent &aKeyEvent,
        TEventCode aType );

// from base class MMmDragAndDropObserver

    /**
     * From MMmDragAndDropObserver.
     * Handles the start of dragging of an item.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of the dragged item.
     */
    void HandleDragStartL( TInt aModelItemIndex );

    /**
     * From MMmDragAndDropObserver.
     * Handles the event of dragging an item over a different one.
     * Used to change presentation of folder icons to indicate drop into action.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of the item.
     */
    void HandleDragOverL( TInt aModelItemIndex );

    /**
     * From MMmDragAndDropObserver.
     * Handles the stop of dragging of an item.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of the dragged item.
     */
    void HandleDragStopL( TInt aModelItemIndex );

    /**
     * From MMmDragAndDropObserver.
     * Handles update of dragged item index.
     *
     * @since S60 v5.0
     * @param aModelItemIndex New dragged item index.
     */
    void HandleDraggedIndexUpdatedL( TInt  aModelItemIndex  );

// from base class MAknToolbarObserver

    /**
     * From MAknToolbarObserver.
     * Handles toolbar events for a certain toolbar item.
     *
     * @since S60 v5.0
     * @param aCommand The command ID of some toolbar item.
     */
    void OfferToolbarEventL( TInt aCommand );

//    from base class MHnControllerInterface
    /**
     * From MHnControllerInterface.
     * Execute extension manager action.
     *
     * @since S60 v5.0
     * @param aUid UID of the ECom plugin's.
     * @param aCommand Command which has to be executed on the plugin.
     *                 Request to the Service.
     * @param aEventParamList List of the event parameters needed to
     *                        execute action. Extention Manager is its owner.
     * @return Number representing error code.
     */
    TInt ExecuteExtensionActionL( const TUid aUid , const TDesC& aCommand,
                CLiwGenericParamList* aEventParamList );

    /**
     * Reloads cascade menu map.
     */
    void ReloadCascadeMenuMapL();

    /**
     * Called by the skin server when skin content is changed and the
     * connected client wants to be informed.
     *
     * @since 2.0
     */
    void SkinContentChanged();

    /**
    * Called by the skin server to indicate that the current skin
    * configuration has changed.
    *
    * @param aReason Reason code.
    *
    * @since 2.5
    */
    void SkinConfigurationChanged(
        const TAknsSkinStatusConfigurationChangeReason aReason );

    /**
    * Called by the skin server to indicate that the available
    * skin packages have changed.
    *
    * @param aReason Reason code.
    *
    * @since 2.5
    */
    void SkinPackageChanged(
        const TAknsSkinStatusPackageChangeReason aReason );

    /**
     * From MMmDragAndDropObserver.
     * Method is invoked on the observer, when the move item event is needed.
     *
     * @since S60 v5.0
     * @param aRecipientId Index of item on which the move event invokes.
     * @param aEventParameters Event parameters.
     */
    void HandleTriggerMoveItemL( const TInt aRecipientId,
            CLiwGenericParamList* aEventParameters );

private:

    /**
     * Enum defining possible exit types for ExitMenuL method.
     *
     * @since S60 v5.0
     */
    enum TExitType
        {
        EExitReally,
        EExitToIdle,
        EExitToPhone
        };

    /**
     * Enum defining possible exit types for ExitMenuL method.
     *
     * @since S60 v5.0
     */
    enum TExitKeyType
        {
        EExitKeyApplication,
        EExitKeyRed,
        ECommingFromBackground
        };

    /**
     * Defiens edit mode state.
     *
     * @since S60 v5.0
     */
    enum TEditModeStatus
        {
        ENoEditMode,
        ETransitionToEditMode,
        EEditMode,
        ETransitionFromEditMode,
        EFastTransitionFromEditMode
        };

    /**
   * Indicates if edit mode is on.
   *
   * @since S60 v5.0
   * @return Returns edit mode status.
   */
    TBool IsEditMode();

    /**
   * Adjusts highlight when entering or leaving edit mode.
   *
   * @since S60 v5.0
   * @param aOriginalHighlight Highlight that would normally be set
   * @return A modified value of highlight
   */
    TInt AdjustEditModeHighlightL( TInt aOriginalHighlight );

    /**
   * Manages the container lookup.
   *
   * @since S60 v5.0
   * @return Pointer to the container to be loaded, of NULL
   * 		   if a new container should be created.
   */
    CMmWidgetContainer* GetAppropriateContainerToLoadL();

    /**
     * Submits an asynchronous service request to show user a message
     * when Matrix Menu is out of memory.
     *
     * @since S60 v5.0
     */
    void HandleOutOfMemoryL();

    /**
     * Resets MM App UI to the initial state (most of the fields).
     *
     * @since S60 v5.0
     */
    void ResetToInitialStateL();

    /**
     * Refreshes menu pane if needed
     */
    void HideMenuPaneIfVisibleL();

// from base class CEikAppUi

  /**
   * From CEikAppUi.
   * Takes care of command handling.
   *
   * @since S60 v5.0
   * @param aCommand Command to be handled.
   */
  void HandleCommandL( TInt aCommand );

  /**
     * From CEikAppUi.
     * Handles Window Server events.
     *
     * @since S60 v5.0
     * @param aEvent Event to be handled.
     * @param aDestination Destination control.
     */
    void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

  /**
     * Indicates if root (main) view is displayed.
     *
     * @since S60 v5.0
     * @return ETrue if root displayed, EFalse otherwise.
     */
  TBool IsRootdisplayedL();

  /**
   * Handles pen down events for an item.
   *
   * @since S60 v5.0
     * @param aIndex Item index.
     */
    void HandleHighlightItemPenDownL( TInt aIndex );

    /**
     * Handles single click event on widget item.
     *
     * @since S60 v5.0
     * @param aIndex Clicked item index.
     */
    void HandleHighlightItemSingleClickedL( TInt aIndex );

    /**
     * Handles drag event.
     *
     * @since S60 v5.0
     */
    void HandleEventDraggingActionedL();

    /**
     * Handles click (select) events for an item.
     *
     * @since S60 v5.0
     * @param aIndex Item index.
     */
    void HandleHighlightItemDoubleClickedL( TInt aIndex );

    /**
     * Handles suite model initialized event.
     *
     * @param aModel Suite model that has been initialized.
     * @since S60 v5.0
     */
    void HandleSuiteModelInitializedL( CHnSuiteModel* aModel );

    /**
     * Handles suite update events.
     *
     * @since S60 v5.0
     */
    void UpdateL();

    /**
     * Handles suite remove events.
     *
     * @since S60 v5.0
     */
    void RemoveL();

    /**
     * Handles suite insert events.
     *
     * @since S60 v5.0
     */
    void InsertL();

  /**
     * Handles the "Back" softkey event.
     *
     * @since S60 v5.0
     */
  void HandleBackCommandL();

    /**
     * Refreshes toolbar.
     *
     * @since S60 v5.0
     */
    void RefreshToolbarL();

    /**
     * Updates app's presentation to show desired suite content.
     *
     * @since S60 v5.0
     */
    void ShowSuiteL();

    /**
     * Creates new container for suite model.
     *
     * @since S60 v5.0
     * @param aSuiteModel Suite model to load.
     * @param aContainerToLoad Target pointer for the new container.
     * @return ETrue if operation is succesful, EFalse otherwise
     */
    TBool CreateNewContainerL( CHnSuiteModel* aSuiteModel,
            CMmWidgetContainer*& aContainerToLoad );

    /**
     * Forwards the passed event code to Hierarchy Navigator for handling.
     *
     * @since S60 v5.0
     * @param aEvent Event code.
     */
    void ForwardEventToHNL( TInt aEvent );

    /**
     * Forwards the passed event code to Hierarchy Navigator for handling
     * regarding the current suite's item referred to by itemid.
     *
     * @since S60 v5.0
     * @param aEvent Event code.
     * @param aItemId Item ID.
     * @param aEventParams Additional parameters.
     */
    void ForwardEventToHNL( TInt aEvent, TInt aItemId,
            CLiwGenericParamList* aEventParams = NULL );

    /**
     * Sets the Status Pane and CBA to indicate the current open suite.
     *
     * @param aReset Indicates if status pane should be reset.
     * @since S60 v5.0
     */
    void RefreshUiPanesL( TBool aReset = EFalse );

    /**
     * Checks recieved messages and forwards to HN.
     *
     * @since S60 v5.0
     * @param aMessage The message.
     */
    void HandleMessageL( const TDesC8& aMessage );

    /**
     * Gets the resource ID of the first free cascade menu pane container.
     * Resource containers are defined in nsui.rss under
     * r_main_menu_pane_cascade_* tags.
     *
     * @since @since S60 v5.0
     * @return The ID of the first available cascade manu pane resource
     *          container, or KErrNotFound if none available.
     */
    TInt GetNextCascadeMenuResourceId();

    /**
     * Handles the update of current shown container.
     *
     * @since S60 v5.0
     * @param aWidgetContainer The widget container to be shown.
     */
    void HandlePresentationChangeL( CMmWidgetContainer* aWidgetContainer );

    /**
     * Handles the switching of visible containers.
     *
     * @since S60 v5.0
     * @param aWidgetContainer The widget to switch to.
     */
    void HandleWidgetChangeRefreshL( CMmWidgetContainer* aWidgetContainer );

    /**
     * Handles the refresh if no widget change occured.
     * Just model on the stack changed.
     *
     * @since S60 v5.0
     */
    void HandleNoWidgetChangeRefreshL();

    /**
     * Shows or hides toolbar depending on context.
     *
     * @since S60 v5.0
     */
    void HandleToolbarVisibilityL();

    /**
     * Updates the toolbar content.
     *
     * @since S60 v5.0
     */
    void UpdateToolbarL();

    /**
     * Sets the Edit Mode.
     *
     * @since S60 v5.0
     * @param aIsEditMode ETrue if edit mode needs to be turned on.
     */
    void SetEditModeL( TBool aIsEditMode );

    /**
     * Returns TAT/Kastor effect rectangle.
     *
     * @since S60 v5.0
     * @param aItemModel Model of the selected item.
     * @param aItemId Item ID.
     * @return Screen rect of the selected item.
     */
    TRect GetKastorRectL( CHnItemModel* aItemModel, TInt aItemId );

    /**
     * Returns TAT/Kastor effect depends on selected item.
     *
     * @since S60 v5.0
     * @param aItemModel Model of the selected item.
     * @return Effect id depends on item type and status.
     */
    TInt GetKastorEffectL( CHnItemModel* aItemModel );

    /**
     * Handles exiting Matrix.
     *
     * @since S60 v5.0
     * @param aExitType type of exit.
     */
    void ExitMatrix( TExitType aExitType );

    /**
     * Cleans up before exiting Matrix.
     * Stops edit mode and resets to root.
     *
     * @param aExitKey Red key or application key.
     * @since S60 v5.0
     */
    void CleanupForExitL( TExitKeyType aExitKey );

    /**
     * Manages bringing homescreen and relevant apps to foreground.
     *
     * @since S60 v5.0
     * @param aExitType type of exit.
     */
    void ShowHomescreenL( TExitType aExitType );

    /**
     * Places the homescreen window underneath the menu window.
     *
     * This method puts the homescreen window just below the matrix menu
     * window (assuming that matrix menu is running in the foreground) so
     * that if matrix gets closed and disappears from the screen, then
     * homescreen will be visible immediately. If it is impossible to do
     * so (for any reason), this method will call ShowHomescreenL.
     *
     * @since S60 v5.0
     */
    void PrepareHomescreenForMatrixExitL();

    /**
     * Opens the desired application.
     * Used in exiting Matrix.
     *
     * @since S60 v5.0
     * @param aUid UID of the app to open.
     */
    void OpenAppL( const TUid aUid );

    /**
     * Loads default root suite
     *
     * @since S60 v5.0
     */
    void InitializeL();

    /**
     * Refreshes Cba according to current context.
     *
     * @since S60 v5.0
     */
    void RefreshCbaL();

    /**
     * MSK text.
     *
     * @since S60 v5.0
     */
    void SetMiddleSoftKeyL();

    /**
     * Starts fullscreen.
     * @param aKastorEffect Id of the TAT/Kastor effect.
     *
     * @since S60 v5.0
     */
    void StartLayoutSwitchFullScreen( TInt aKastorEffect
        = AknTransEffect::ELayoutSwitchStart);

    /**
     * Ends fullscreen
     *
     * @since S60 v5.0
     */
    void EndFullScreen();

    // From MMActionRequest

    /**
     * Enables to issue a request to Hn engine.
     *
     * @since S60 v5.0
     * @param aParam Input Parameters.
     * @param aOutput Target list for output.
     */
     void HandleRequestL( const CLiwGenericParamList& aParam,
                         CLiwGenericParamList* aOutput = NULL );

     /**
      * Resets map of widget containers.
      *
      * @since S60 v5.0
      */
     void ResetContainerMap();

     /**
      * Resets map of widget container to root.
      *
      * @since S60 v5.0
      */
     void ResetContainerMapToRootL();

     /**
      * Resets view to root.
      *
      * @return ETrue if view was not displaying root suite already, EFalse otherwise
      */
     TBool ResetToRootL();

     /**
      * Refreshes icons after skin changing
      */
     void RefreshIconsL();

     /**
      * Forward event to hierarchy navigator.
      *
      * @param aOffset Highlight offset.
      */
     void HandleHighlightOffsetL( TInt aOffset );

     /**
      * Replaces ETransitionFromEditMode or EFastTransitionFromEditMode to ENoEditMode
      */
     void ClearTransitionFromEditModeFlag();
     /**
       * Method inherited from CEikAppUi to handle
       * out of memory problem.
       *
       * @param aError KErrNoMemory is handled.
       * @param aExtErr Not used.
       * @param aErrorText Not used.
       * @param aContextText Not used.
       * @return EErrorNotHandled in order to display proper information.
       */
     TErrorHandlerResponse HandleError( TInt aError,
                                        const SExtendedError &aExtErr,
                                        TDes &aErrorText,
                                        TDes &aContextText );

     /**
      * Update widget data for number of items changed.
      *
      * @param aChange type of change.
      */
     void HandleNumberOfItemsChangedL( TItemsChangeType aChange );

     /**
      * Sets the highlighted item in gui objects accordingly to what
      * is stored in the model. The exact result of calling this
      * method depends on @c iMakeHightlightedItemFullyVisible
      * flag.
      */
     void ApplyHighlightFromModelL();

     /**
      * Called when EFocusGained message is received by menu.
      * Should be called only from HandleWsEvent.
      */
     void HandleFocusGainedL();

     /**
      * Called when application is sent to foreground.
      * Should be called only from HandleWsEvent.
      */
     void HandleFullOrPartialForegroundGainedL();

     /**
      * Called when application is sent to background.
      * Should be called only from HandleWsEvent.
      */
     void HandleFullOrPartialForegroundLostL();

     /**
      * Called when EFocusLost message is received by menu.
      * Should be called only from HandleWsEvent.
      */
     void HandleFocusLostL();

private: // Data

    /**
     * Grid's Container Widget.
     * Own.
     */
    //CMmWidgetContainer* iGridContainer;

    /**
     * List's Container Widget.
     * Own.
     */
    //CMmWidgetContainer* iListboxContainer;

    /**
     * Current visible Widget.
     * Not own.
     */
    CMmWidgetContainer* iCurrentContainer;

    /**
     * Hash map containig widget containers, with corresponding
     * suite id.
     */
    RHashMap< TInt, CMmWidgetContainer* > iContainerMap;

    /**
     * Array of container pointers, whose deletion should
     * be delayed.
     */
    RPointerArray< CMmWidgetContainer > iGarbage;

    /**
     * Current suite model
     * Not own.
     */
    CHnSuiteModel* iCurrentSuiteModel;

    /**
     * Hierarchy Navigator
     * Own.
     */
    CHnInterface* iHNInterface;

    /**
     * Hash map for dynamic initialization of cascade menu panes.
     * Contains item iterator pointers indexed with cascade pane resource IDs.
     * MHnMdMenuItemIterator items not owned.
     */
    RHashMap<TInt, MHnMenuItemModelIterator*> iCascadeMenuMap;

    /**
     * Indicates the edit mode status.
     */
    TEditModeStatus iEditModeStatus;

    /**
     * Pointer to application's custom toolbar.
     * Own.
     */
    CAknToolbar* iToolbar;

    /**
     * Index of dragged item.
     */
    TInt iItemDragged;

    /**
     * Model ID of the dragged item.
     */
    TInt iIdDragged;

    /**
     * Locks select events for highlight based scrolling.
     */
    TBool iKeyClickLocked;

    /**
     * Indicates if TAT/Kastor effect is started.
     */
    TBool iIsKastorEffectStarted;

    /**
     * Extension manager.
     * Own.
     */
    CMMExtensionManager* iMmExtManager;

  /**
     * Screen state.
     */
    TBool iScreenOn;

    /**
     * Skin change needed
     */
    TBool iSkinChangeNeeded;

    /**
     * Template library.
     */
    CMmTemplateLibrary* iTemplateLibrary;

    /**
    * Skin server session.
    */
    RAknsSrvSession iSkinSrvSession;

    /**
     * Skin change in progress
     */
    TBool iSkinChangeInProgress;

    /**
     * Widget container that contains only an empty listbox.
     * It is displayed only when there is no other container that
     * could be displayed. It is needed to avoid ugly flicker
     * effects where homescreen becomes visible for a fraction of
     * second.
     */
    CMmWidgetContainer* iDummyContainer;

    /**
     * A template library for iDummyContainer;
     */
    CMmTemplateLibrary* iDummyTemplateLib;

    /**
     * Out-of-memory condition handler.
     */
    CMmNoMemory* iOutOfMemoryHandler;

    /**
     * This flag is intended for indicating that the highlithted item must
     * be fully visible. It is only used in two cases:
     * 1. When an item has been drag-and-dropped into a folder (the item
     *    moved into the folder must be fully visible when the target suite
     *    is displayed).
     * 2. When turning on edit mode (the item highlighted must be fully
     *    visible once edit mode is turned on).
     */
    TBool iMakeHightlightedItemFullyVisible;

    /**
     * An low priority active object used as a helper to detect frequent and
     * quick appkey pressing.
     * Owns.
     */
    CMmAppkeyHandler* iAppkeyHandler;

    /**
     * ETrue if Matrix menu has focus i.e. it is in foreground and is not eclipsed
     * by any popup note that is not originating from Matrix menu.
     */
    TBool iHasFocus;

    /**
     * ETrue if Matrix is hidden from TS which is used on startup.
     */
    TBool isHiddenFromFS;

    /**
     * Own.
     * Popup menu displayed after long tap.
     */
    CAknStylusPopUpMenu* iPopupMenu;

    };

#endif // C_MMAPPUI_H

// End of File

