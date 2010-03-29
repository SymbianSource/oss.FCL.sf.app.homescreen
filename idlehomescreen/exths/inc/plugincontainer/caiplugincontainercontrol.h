/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       It is the control container for the AI plugins
*
*/


#ifndef CAIPLUGINCONTROL_H
#define CAIPLUGINCONTROL_H

#include <e32base.h>
#include <cenrepnotifyhandler.h>

#include <babackup.h>

#include "extrstandbycontainerif.h"

#include "ExtrStandbyScr.h"

#include "propertyobserver.h"

enum TAIPluginAlignment
    {
    EAITop = 1,
    EAIBottom
    };

// Background layers
enum TIdleBgLayers
    {
    EAIBgLayerBackground     = 0,
    EAIBgLayerWallpaper      = 1,
    EAIBgLayersN             = 2
    };

enum TAIPluginState
    {
    EAIPluginOff = 0,
    EAIPluginOn,
    EAIPluginError
    };

// Structure containing plugin information
struct TPluginInfo
    {
    CExtrStandbyScr*        iPlugin;
    TBool                   iResizeable ;
    TBool                   iOnScreen ;
    TUid                    iPluginId;
    TAIPluginAlignment      iAlignment;
    TAIPluginState          iState;
    TInt                    iWeight;
    TInt32                  iCRKeyId;
    };

class CAknsLayeredBackgroundControlContext;

// CLASS DECLARATION
/**
*  It is the control for the Active Idle.
*
*  @since 2.6
*/
NONSHARABLE_CLASS(CAIPluginContainerControl) :
    public CExtrStandbyContainerIF,
    public MCoeControlObserver,
    public MCenRepNotifyHandlerCallback,
    public MPropertyChangeHandler
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aViewId View identifier.
        * @param aContainer Container object.
        * @return Pointer to the CAIPluginContainerControl object.
        */
        static CAIPluginContainerControl* NewL(CCoeControl *aContainer) ;
        /**
        * Destructor.
        */
        ~CAIPluginContainerControl();

    public: // From CCoeControl

        /**
        * From CCoeControl, returns the amount of component controls.
        * @return amount of component controls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, returns a control by index.
        * @param aIndex Index of the control.
        * @return Control by index.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, informs that the size is changed to update the
        * sizes of component controls appropriately.
        */
        void SizeChanged();

        /**
        * From CCoeControl, informs that the focus is changed
        */
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * From CCoeControl, draws the contents of the view.
        * @param aRect Area to be redrawn.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl, offers a key event to the view.
        * @param  aKeyEvent Information about the key event.
        * @param  aType Type of the event.
        * @return Information which indicates whether the keypress
        *           was processed or not.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * From CCoeControl : Notifier for resourse changing
        * @param aType : Type of resource change
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CCoeControl : Sets all the plugins dimmed. When dimmed the plugin must not draw anythings
        * @param aDimmed
        */
        void SetDimmed(TBool aDimmed);


        /**
        * From CCoeControl
        * @param TTypeUid aId
        */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    public: // from CExtrStandbyContainerIF

        /**
        * Start loading plugin
        */
        void StartL();
        /**
        * Sets mode for External Homescreen (Full screen or normal)
        */
        void SetFullScreenMode(TBool aFullScreenMode);

    public: // from MCoeControlObserver

        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    public: // From PropertyObserver
        /**
         * Called when a backup or restore operation either starts or ends.
         * @param aBackupOperationAttributes Operation attributes
         */
        void HandlePropertyChange(const TUid& aCategory, const TUint& aKey, const TInt& aValue);

    protected:  // From MCenRepNotifyHandlerCallback

        void HandleNotifyGeneric(TUint32 aId);

    private:

        /**
        * C++ constructor.
        */
        CAIPluginContainerControl();

        /**
        * Symbian OS constructor.
        */
        void ConstructL(CCoeControl* aContainer);


        /**
        * change the plugin focus
        */
        void ChangeFocusPluginL( TInt aDirection );

        /**
        * focus on the default focusing plugin
        */
        void FocusOnDefaultFocusingPlugin(TDrawNow aDrawNow=ENoDrawNow);

        /**
        * Calculate the rect for each plugin and draw
        */
        void CalculateRectPlugins();

        /**
        * Handle the Top Plugin
        */
        void HandleTopPlugins(TInt& aTopPluginX, TInt& aBottomPluginX);

        /**
        * Handle the bottom Plugin
        */
        void HandleBottomPlugins(TInt& aTopPluginX, TInt& aBottomPluginX);

        /**
        * Calculate the rect for a plugin (top or bottom allignment)
        */
        void HandlePlugin(TInt aIndex,
                          TAIPluginAlignment aAlign,
                          TInt& aTopPluginX,
                          TInt& aBottomPluginX);

        /**
        * Return the next visible plugin with content
        */
        TInt NextVisiblePlugin(TInt aStartIndex);

        /**
        * Return the previous visible plugin with content
        */
        TInt PreviousVisiblePlugin(TInt aStartIndex);

        /**
        * Return the index of the reduceable plugin with the lower weight
        */
        TInt FindReduceablePlugin(TInt aOverRunHeight, TInt aCurrentIndex);

        /**
        * Return the index of the plugin with the lower weight
        */
        TInt FindLowerWeightPlugin(TInt aCurrentIndex);

        /**
        * Reduce the plugin and move the others plugin till the current
        */
        void ReducePlugin(TInt aPluginToBeReducedIndex,TInt aCurrentIndex,TInt aReduceHeight);

        /**
        * Hide the plugin
        */
        void HidePlugin(TInt aPluginIndex);

        /**
        * Read Plugin Configuration
        */
        void ReadPluginsConfigurationsL();

        /**
        * Load all the plugins
        */
        void LoadPluginsL();

        /**
        * Load a specific plugin
        */
        void LoadPluginL(TInt aIndex);

        /**
        * Destroy all the plugins
        */
        void DestroyPlugins();

        /**
        * Resets plugin state to default one
        */
        void ResetToDefault( TPluginInfo& aPluginInfo );

        /*
        * Checks in which mode AI1 will be executed
        * fullscreen or normal
        * @since S60 3.2
        * @return TBool
        */
        TBool IsInFullScreenModeL();

        /*
        * updates background bitmap
        * @since S60 3.2
        * @return void
        */
        void UpdateBackgroundL();

    private:    // Data

        // Current plugin Index
        TInt            iIndexPlugin;

        // Default focusing plugin Index
        TInt            iIndexDefaultFocusingPlugin;

        // Contains array of plugin loaded.
        RPointerArray<TPluginInfo> iPlugins;

        // Current plugin info
        TPluginInfo*    iCurrentPlugin ;

        // Container
        CCoeControl*    iContainer; //not own

        //Flag for a status change notification.
        // This flag is used only if the notification come when the component is dimmed
        TBool           iContentStatusChangedNotification ;

        // Previous rect of the AI container
        TRect           iPrevRect;

        CRepository*            iAIRepository;
        CCenRepNotifyHandler*   iNotifyHandler;

        // ?one_line_short_description_of_data
        CPropertyObserver* iBackupObserver;

        // Background context.
        CAknsLayeredBackgroundControlContext* iBgContext;

        // Current AI mode, fullscreen - ETrue / normal - EFalse
        TBool iIsFullScreenMode;

        // Flag for checking if FW already started
        TBool iStarted;
   };

#endif      // CAIPLUGINCONTROL_H

// End of File
