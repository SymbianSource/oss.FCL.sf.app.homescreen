/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Background appearance drawer
*
*/


#ifndef _XNBGCONTROL_H
#define _XNBGCONTROL_H

// System includes
#include <e32base.h>
#include <coecntrl.h>

// User includes
#include "xnuistatelistener.h"

// Forward declarations

/**
 * Background appearance drawer
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnBgControl ) : public CCoeControl , public MXnUiStateObserver
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CXnBgControl* NewL();

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CXnBgControl* NewLC();

    /**
     * Destructor
     */
    ~CXnBgControl();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CXnBgControl();
    
public:
    // from CCoeControl

    /**
     * @see CCoeControl
     */    
    TInt CountComponentControls() const;

    /**
     * @see CCoeControl
     */    
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
    /**
     * @see CCoeControl
     */
    void SizeChanged();
    
    /**
     * @see CCoeControl
     */
    void Draw( const TRect& aRect ) const;
            
    /**
     * @see CCoeControl
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    
public:
    // new functions
    void SetCompoundControl( CCoeControl* aControl );
    
	/**
     * Service for removing grabbing controls
     */
    void ResetGrabbingL();
    
    /**
     * Prepares control for destroying
     */
    void PrepareDestroy();
  
private:  
	// new functions
    
	/**
     * Removes recursively grabbing controls
     */
    void RemoveGrabbingControL( const CCoeControl* aControl, const TPointerEvent& aEvent ) const;
    
private:
    // from MXnUiStateObserver
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyForegroundChanged( TForegroundStatus aStatus );
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyLightStatusChanged( TBool aLightsOn );
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyInCallStateChaged( TBool aInCall );   
    
private:
    // data
    
    /** Compound Control, Not owned */
    CCoeControl* iControl;
    
	/** stored point of EButton1Down */
    TPoint iHitpoint;
    
    };

#endif // _XNBGCONTROL_H
