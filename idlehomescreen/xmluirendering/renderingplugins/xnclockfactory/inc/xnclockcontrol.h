/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock control
*
*/


#ifndef _XNCLOCKCONTROL_H
#define _XNCLOCKCONTROL_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CWindowGc;
class CXnClockAdapter;
class CXnClockFace;

// Class declaration
/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CXnClockControl ) : public CBase
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @since S60 5.1
    *
    * @param  aContainerWindow   Container window for the clock component.
    *
    * @param  aFormatFromLocale  Boolean flag to indicate whether the clock
    *                            format should be updated from the locale.
    *
    * @param  aContextPaneClock  Boolean flag that indicates whether the
    *                            clock is in the context pane.
    *
    * @return Newly constructed object.
    */
    static CXnClockControl* NewL( CXnClockAdapter* aAdapter,
                                  const TBool aFormatFromLocale,
                                  const TClockFormat aFormat );
                                              

    /**
    * Destructor.
    */
    virtual ~CXnClockControl();

public: // New functions

    /**
    * Updates the double buffer and redraws the clock.
    */
    void UpdateDisplay();

    /**
    * Sets the format of the clock.
    *
    * @param  aFormatFromLocale  Wheter clock is getting format from locale
    * @param  aFormat  New clock format.
    */
    void SetFormatL( const TBool aFormatFromLocale, 
                     const TClockFormat aFormat );

    /**
    * Gets the clock format 
    *    
    * @return Clock format.
    */    
    TClockFormat Format() const;
        
    /**
    * Draws the clock
    *
    * @param aGc Context where to draw
    * @param aRect Rect Clock rect 
    */
    void Draw( CWindowGc& aGc, const TRect& aRect );
    
    /**
    * Starts the clock timer.
    * Used when the clock gains visibility.
    */
    void StartTimer();

    /**
    * Stops the clock timer.
    * Used when the clock loses visibility.
    */
    void StopTimer();

private: // New functions

    /**
    * Timer callback for clock updates
    *
    * @param  aThis  Pointer to the skinnable clock instance.
    *
    * @return Always returns @c KErrNone.
    */
    static TInt TimerCallback( TAny* aThis );

private:

    /**
    * C++ default constructor.
    */
    CXnClockControl( CXnClockAdapter* aAdapter,
                     const TBool aFormatFromLocale,
                     const TClockFormat aFormat );
                        
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private: // data

    // Timer, owned
    CPeriodic*              iTimer;
    // Clock face, owned
    CXnClockFace*           iFace;
    // Clock adapter, not owned
    CXnClockAdapter*        iAdapter;            
    // Clock format
    TClockFormat            iClockFormat;    
    // Flag indicating whetger format clock from locale
    TBool                   iFormatFromLocale;    
    };

#endif // _XNCLOCKCONTROL_H

// End of File
