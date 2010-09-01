/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies)..
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
* Declares statuspane's background cleaner.
*
*/

#ifndef __WMSPBGCLEANER_
#define __WMSPBGCLEANER_

// Forward declarations

/**
 * Statuspane background cleaner.
 * 
 * @class   CWmSpBgCleaner wmspbgcleaner.h
 */
NONSHARABLE_CLASS( CWmSpBgCleaner ) : public CCoeControl
    {
public:
    /**
     * Two-phased constructors.
     */
    static CWmSpBgCleaner* NewL();
    static CWmSpBgCleaner* NewLC();
    
    /** Destructor */
    ~CWmSpBgCleaner();
    
private:    
    /** constructor */
    CWmSpBgCleaner();
    
    /** 2nd phase constructor */
    void ConstructL();
    

protected: // from base class CCoeControl
    
    /**
     * Handles a change to the control's resources
     * 
     * @see CCoeControl::HandleResourceChange
     */
    void HandleResourceChange( TInt aType );

    /**
     * Sets the control's extent, specifying a rectangle.
     * 
     * @see CCoeControl::SizeChanged
     */
    void SizeChanged();

    /*
     * Draws the control.
     * 
     * @see CCoeControl::Draw
     */
    void Draw( const TRect& aRect ) const;

private:

    };

#endif // __WMSPBGCLEANERCTRL_
