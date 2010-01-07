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

/**
 * Portal button ( ovi, operator ) class for Wm
 * 
 * @class   CWmPortalButton wmportalbutton.h
 */
NONSHARABLE_CLASS( CWmPortalButton ) : public CAknButton,
                                       public MConverterObserver 
    {
public:
    /*
     * Two-phased constructor.
     * 
     * @param aParent button title text
     * @param aText button title text
     * @param aUrl Url to open in browser when clicked.  
     * @param aButtonCtrlId type of button 
     */
    static CWmPortalButton* NewL(
            const CCoeControl* aParent,
            const TDesC& aText = KNullDesC,
            const TDesC& aUrl = KNullDesC, 
            TWmUiControlIds aButtonCtrlId = EOviPortal );
    
    /** Destructor */
    virtual ~CWmPortalButton();
                                      
public: // Functions from base class
    
    /**
     * Handles key events.
     * 
     * @see CCoeControl::OfferKeyEventL
     */
	TKeyResponse OfferKeyEventL( 
	        const TKeyEvent& aKeyEvent, TEventCode aType );
            
    /**
	 * Handles pointer events.
	 * 
	 * @see CCoeControl::HandlePointerEventL
	 */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
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
    
protected: // from MConverterObserver

    /** image conversin completed */
    void NotifyCompletion( TInt aError );
   
protected: // Constructors
    
    /** Constructor for performing 1st stage construction */
    CWmPortalButton( const TInt aFlags,
            TWmUiControlIds aButtonCtrlId = EOviPortal );
    
    /** 2nd phase constructor */
    void ConstructL( 
            const CCoeControl* aParent,
            const TDesC& aText = KNullDesC,
            const TDesC& aUrl = KNullDesC );
    
private:

    /** 
	 * Draws text over button
	 */
    void DrawText( CWindowGc& aGc,
                   const TDesC& aText, 
                   TAknTextComponentLayout& aLayout,
                   TInt aMargin ) const;

private: //data members
    /**
     * Button text
     */ 
    HBufC* iText;

    /**
     * Url assigned to button
     */ 
    HBufC* iUrl;

    /**
     * the image converter utility
     */    
    CWmImageConverter*  iImageConverter;

    /**
     * Parent control
     */ 
    CWmMainContainer* iWmMainContainer;

    /** 
     * Button ctrl id
     */ 
    TWmUiControlIds iButtonCtrlId;
 	};

#endif //___WMPORTALBUTTON_H__

// End of File
