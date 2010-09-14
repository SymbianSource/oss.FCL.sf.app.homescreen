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
class CXnNodePluginIf;

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
    * @param aAdapter Reference to clockadapter.
    * @param  aFormatFromLocale  Boolean flag to indicate whether the clock
    *                            format should be updated from the locale.
    * @param  aFormat Boolean flag that indicates whether the
    *                            clock is digital or analog.
    *
    * @return Newly constructed object.
    */
    static CXnClockControl* NewL( CXnClockAdapter& aAdapter,
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
    * @param aNode Node to draw
    * @param aAmpm Possible am/pm text to draw 
    */
    void Draw( CWindowGc& aGc, CXnNodePluginIf* aNode, CXnNodePluginIf* aAmpm );
    
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
    
    /**
    * Checks if clock format has changed 
    */
    void CheckClockFormatL();
    
    /**
    * Reset font and color 
    */
    void ResetFont();

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
    CXnClockControl( CXnClockAdapter& aAdapter,
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
    CXnClockAdapter&        iAdapter;            
    // Clock format
    TClockFormat            iClockFormat;
    // Time format
    TTimeFormat             iTimeFormat;
    // Flag indicating whether format clock from locale
    TBool                   iFormatFromLocale;
    };

#endif // _XNCLOCKCONTROL_H

// End of File
