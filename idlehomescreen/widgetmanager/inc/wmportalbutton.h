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
* Declares portal button for WidgetManager
*
*/

#ifndef ___WMPORTALBUTTON_H__
#define ___WMPORTALBUTTON_H__

#include <aknbutton.h>	// CAknButton
#include "wmimageconverter.h"
#include "wmcommon.h"

// FORWARD DECLARATIONS
class CWmMainContainer;
class CFbsBitmap;
class CWmConfiguration;
class CWmProcessMonitor;

/**
 * Portal button ( ovi, operator ) class for Wm
 * 
 * @class   CWmPortalButton wmportalbutton.h
 */
NONSHARABLE_CLASS( CWmPortalButton ) : public CAknButton,
                                       public MConverterObserver,
                                       public MCoeControlObserver
    {
public:
    /*
     * Two-phased constructor.
     * 
     * @param aParent button title text
     * @param aPortalButtonIndex index of this button (0 or 1)
     */
    static CWmPortalButton* NewL(
            const CCoeControl* aParent,
            TInt aPortalButtonIndex = 0 );
    
    /** Destructor */
    virtual ~CWmPortalButton();
                                      
public: // Functions from base class

    /*
     * Draws the control.
     * 
     * @see CCoeControl::Draw
     */
    void Draw( const TRect& /*aRect*/ ) const;
    
    /**
     * Sets the control's extent, specifying a rectangle.
     * 
     * @see CCoeControl::SizeChanged
     */
    void SizeChanged();
    
    /**
     * Executes action for button pressed
     */
    void ExecuteL();
    
protected: // from MConverterObserver

    /** image conversin completed */
    void NotifyCompletion( TInt aError );

protected: // from MCoeControlObserver

    /** Observes the button's own activity  */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
    
protected: // Constructors
    
    /** Constructor for performing 1st stage construction */
    CWmPortalButton( const TInt aFlags,
            TInt aPortalButtonIndex );
    
    /** 2nd phase constructor */
    void ConstructL( 
            CWmMainContainer* aParent,
            const TDesC& aText = KNullDesC,
            const TDesC& aIcon = KNullDesC );
    
private:

    /** size of button icon, defined in the layout */
    TSize LayoutIconSize() const;
    
    /** Draws text over button */
    void DrawText( CWindowGc& aGc,
                   const TDesC& aText, 
                   TAknTextComponentLayout& aLayout,
                   TInt aMargin ) const;
    /** Runs operator button action */
    void RunOperatorL( CWmConfiguration& aConf );
    
    /** Runs ovi store launcher */
    void RunOviL( CWmConfiguration& aConf );
    
    /**  Starts browser */ 
    void StartBrowserL( CWmConfiguration& aConf  );
    
private: //data members

    /**
     * the image converter utility
     */    
    CWmImageConverter*  iImageConverter;

    /**
     * Parent control
     */ 
    CWmMainContainer* iWmMainContainer;

    /** 
     * Button index
     */ 
    TInt iPortalButtonIndex;

    /** icon */
    CFbsBitmap* iButtonIcon;
    
    /** icon mask */
    CFbsBitmap* iButtonIconMask;
	
    /** Monitors process */
    CWmProcessMonitor* iProcessMonitor; 
 	};

#endif //___WMPORTALBUTTON_H__

// End of File
