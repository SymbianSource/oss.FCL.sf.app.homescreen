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
* Description:  Date control
*
*/


#ifndef _XNDATECONTROL_H
#define _XNDATECONTROL_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CWindowGc;
class CXnClockAdapter;
class CXnNodePluginIf;
class CEnvironmentChangeNotifier;

enum TXnDateType {
                 EDate,
                 EDay
                 };

// Class declaration
/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CXnDateControl ) : public CBase
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @since S60 5.1
    *
    * @param aAdapter Clock adapter
    * @param aType Type of date. Could be date or day number
    *
    * @return Newly constructed object.
    */
    static CXnDateControl* NewL( CXnClockAdapter& aAdapter,
                                 TXnDateType aType );
                                              

    /**
    * Destructor.
    */
    virtual ~CXnDateControl();

public: // New functions
        
    /**
    * Draws the clock
    *
    * @param aGc Context where to draw
    * @param aNode Contains drawing info 
    */
    void DrawL( CWindowGc& aGc, CXnNodePluginIf* aNode );
    
    /**
    * Resets font and color values
    */
    void ResetFont();

private: // New functions

    /**
    * Constructs date string
    *
    * @return pointer to date string.
    */
    HBufC* ConstructDateStringL();
    
    /**
    * Constructs day number string
    *
    * @return pointer to day number string.
    */
    HBufC* ConstructDayStringL();

private:

    /**
    * C++ default constructor.
    */
    CXnDateControl( CXnClockAdapter& aAdapter,
                    TXnDateType aType );
                        
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    /**
    * Creates font
    * 
    * @param aNode Contains info of font
    * 
    * @return CAknLayoutFont
    */
    const CAknLayoutFont* CreateFontL( CXnNodePluginIf& aNode );
    
    /**
    * Creates font color
    * 
    * @param aNode Contains info of color
    */
    const TRgb& CreateColorL( CXnNodePluginIf& aNode );
    
    /**
    * Callback when enviroment changed.
    * 
    * @param aPtr Pointer to self
    * 
    * @return KErrNone
    */
    static TInt HandleCallBackL( TAny *aPtr );
    
    /**
    * Updated date/day string
    */
    void RefreshDateL();

private: // data

    // Holds date/day text, owned
    HBufC*	iDateText;
    // Clock adapter
    CXnClockAdapter& iAdapter;
    // Date font, not owned
    CFont*  iDateFont;
    // Font color
    TRgb    iDateColor;
    // Indicates whether color should be created
    TBool   iIsColorSet;
    // Environment notifier, owned
    CEnvironmentChangeNotifier* iEnvNotifier;
    // Type of date. Could be date or day number
    TXnDateType iDateType;
    };

#endif // _XNDATECONTROL_H

// End of File
