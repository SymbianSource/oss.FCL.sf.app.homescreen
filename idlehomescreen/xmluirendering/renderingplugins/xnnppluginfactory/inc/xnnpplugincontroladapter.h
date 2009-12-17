/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The plugin can use this interface to communicate with the browser.
*
*/



#ifndef CPLUGINCONTROLADAPTER_H
#define CPLUGINCONTROLADAPTER_H

//  INCLUDES
#include <PluginAdapterInterface.h>

// CLASS DECLARATION

/**
*  @ingroup group_xnnppluginfactory
* 
*  The plugin can use this interface to communicate with the browser.
*
*  @lib xn3nppluginfactory.lib
*  @since Series 60 3.1
*/
/* Note that inheritance order must be MPluginAdapter, CBase don't change it */
class CXnNPPluginControlAdapter : public MPluginAdapter, public CBase
    {        
    public: // Functions from base classes

        /**
        * Gets the pointer to the minor version number of the plugin.
        * @since Series 60 3.1
        * @return 1.
        */
        TUint16 GetVersion() { return 1; };

        /**
        * Called when the plug-in has completed its creation.
        * This function is called from the constructL function of the plug-in.
        * @since Series 60 3.1
        * @param aPluginControl A pointer to the plug-in control window
        * @return None
        */
        void PluginConstructedL( CCoeControl* aPluginControl );

        /**
        * Called after the plug-in has been deleted.
        * This function is called from the destructor of the plug-in.
        * @since Series 60 3.1
        * @return None
        */
        void PluginFinishedL();

#ifndef __SERIES60_30__

        /**
        * Called by the plug-in to open a URL in the parent frame.
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @param aUrl Name of the URL to open
        * @return None
        */
        inline void OpenUrlL( const TDesC& /*aUrl*/ ) {};

        /**
        * Called by the plug-in to open a URL in a named window or frame.
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @param aUrl Name of the URL to open
        * @param aNewTargetWindowName The name of the target window or frame
        * @return None
        */
        inline void OpenUrlInTargetWindowL( const TDesC& /*aUrl*/,
                                     const TDesC& /*aNewTargetWindowName*/ ) {};

        /**
        * Specifies whether or not the frame has a previous navigation history item.
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @return true if the frame has a previous navigation history item false otherwise
        */
        inline TBool HasNavigationHistoryPrevious() const { return EFalse; };

        /**
        * Informs the browser to load the next navigation history URL in its parent frame.
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @return None
        */
        inline void LoadNavigationHistoryPreviousL() {};

        /**
        * Returns whether the frame has a next navigation history item. 
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @return Returns whether the frame has a next navigation history item. 
        */
        inline TBool HasNavigationHistoryNext() const { return EFalse; };

        /**
        * Informs the browser to load the next navigation history URL in its parent frame.
        * This function is not implemented in Series 60.
        * @since Series 60 3.1
        * @return None
        */
        inline void LoadNavigationHistoryNextL() {};

#endif // __SERIES60_30__
        /**
        * Returns the window system level control object for the plug-in.
        * @since Series 60 3.1
        * @return A pointer to the parent control window.
        */
        CCoeControl* GetParentControl();

        /**
        * Returns the observer for the plug-in control.
        * @since Series 60 3.1
        * @return Observer that the plug-in can use to send events to the browser.
        */
        MCoeControlObserver* GetParentControlObserver();

        /**
        * Sets the plug-in notifier allowing the plug-in to control portions of the browser.
        * @since Series 60 3.1
        * @param aNotifier Pointer to an interface that the browser can use to send 
        *                  events to the plug-in.
        * @return None
        */
        void SetPluginNotifier( MPluginNotifier *aNotifier );
        /**
        * Tells the plugin adapter if plugin content can accept user interaction.  This function
        * is called from the plugin.
        * @since Series 60 3nd Edition
        * @param None
        * @return None
        */
        void SetContentInteractive(TBool /*aInteractive*/ );
        
    
    public:	// Data
        // Pointer to Browser Plugins control adapter, not owned
        CCoeControl* iNPPControlAdapter;
        // Pointer to parent control adapter, not owned
        CCoeControl* iParentControlAdapter;
        // Set by plugin. Notifies plugin through MPluginNotifier functions
        MPluginNotifier* iPluginNotifier;
};

#endif      // CPLUGINCONTROLADAPTER_H

// End of File
