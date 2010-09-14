/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Xuikon clock face.
*
*/


#ifndef _XNCLOCKFACE_H
#define _XNCLOCKFACE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CXnClockAdapter;
class CXnNodePluginIf;
class CWindowGc;

// CLASS DECLARATIONS
/**
*  @ingroup group_xnclockfactory
*  @lib xnclockfactory.lib
*  @since S60
*/
class CXnClockFace : public CBase
    {
    public: // Destructor
        
        /**
        * Destructor.
        */
        virtual inline ~CXnClockFace() {};

    public: // New methods

        /**
        * Draws the given time to the given buffer bitmap.
        *
        * @since S60 5.1
        *
        * @param aAdapter Clock adapter
        * @param aGc Graphics context that can be used to draw to
        *                     the clock
        * @param aNode Node which contains drawing info
        * @param aTime Time to be used for drawing.
        * @param aAmpm Possible am\pm text to draw
        */
        virtual void DrawL( CXnClockAdapter& aAdapter,
                            CWindowGc& aGc, 
                            CXnNodePluginIf& aNode, 
                            const TTime& aTime,
                            CXnNodePluginIf* aAmpm ) = 0;
        
        /**
        * Reset font
        */
        virtual void ResetFont() = 0;
    };

class CXnClockFaceDigital : public CXnClockFace
    {
    public:
        /**
        * Type of font, digital or for am/pm text
        */
        enum TXnClockFontType {
                              EClock,
                              EAmpm
                              };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnClockFaceDigital* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnClockFaceDigital();
    
    public:  // Functions from CXnClockFace
        
        /**
        * @see CXnClockFace::DrawL
        */
        void DrawL( CXnClockAdapter& aAdapter,
                    CWindowGc& aGc, 
                    CXnNodePluginIf& aNode, 
                    const TTime& aTime,
                    CXnNodePluginIf* aAmpm );
        
        /**
        * @see CXnClockFace::ResetFont
        */
        void ResetFont();

    private:

        /**
        * C++ default constructor.
        */
        CXnClockFaceDigital();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Draws am/pm text
        * 
        * @param aAdapter Clock adapter
        * @param aGc Graphics context that can be used to draw to
        *                     the clock
        * @param aTime Time to be used for drawing.
        * @param aAmpm Contains info for drawing
        */
        void DrawAmpmL( CXnClockAdapter& aAdapter,
                        CWindowGc& aGc,
                        const TTime& aTime,
                        CXnNodePluginIf& aAmpm );
        
        /**
        * Creates font
        * 
        * @param aAdapter Clock adapter
        * @param aNode Contains info to create font
        * @param aFontType Digital clock font or am/pm font
        * 
        * @return CAknLayoutFont
        */
        const CAknLayoutFont* CreateFontL( CXnClockAdapter& aAdapter,
                                           CXnNodePluginIf& aNode,
                                           TXnClockFontType aFontType );
        
        /**
        * Creates font color
        * 
        * @param aAdapter Clock adapter
        * @param aNode Contains info to create color
        * @param aFontType Digital clock font color or am/pm font color
        * 
        * @return Color
        */
        const TRgb& CreateColorL( CXnClockAdapter& aAdapter,
                                  CXnNodePluginIf& aNode,
                                  TXnClockFontType aFontType);

    private:    // Data
        // Date font, not owned
        CFont*  iClockFont;
        // Date font, not owned
        CFont*  iAmpmFont;
        // Digital clock font color
        TRgb    iFaceColor;
        // Indicates whether digital clock color should be created
        TBool   iIsFaceColorSet;
        // Am/pm text font color
        TRgb    iAmpmColor;
        // Indicates whether am/pm font color should be created
        TBool   iIsAmpmColorSet;
    };

class CXnClockFaceAnalog : public CXnClockFace
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnClockFaceAnalog* NewL();
                                                 
        /**
        * Destructor.
        */
        virtual ~CXnClockFaceAnalog();
    
    public:  // Functions from CXnClockFace
                    
        /**
        * @see CXnClockFace::DrawL
        */           
        void DrawL( CXnClockAdapter& aAdapter,
                    CWindowGc& aGc, 
                    CXnNodePluginIf& aNode, 
                    const TTime& aTime,
                    CXnNodePluginIf* aAmpm );
        
        /**
        * @see CXnClockFace::ResetFont
        */
        void ResetFont();

    private: // New functions

        /**
        * Draws the hands of the clock.
        *
        * @param aGc Graphics context that can be used to draw to the
        *                     clock
        * @param aRect Rectangle defining the size of the clock.
        * @param aDateTime Time to be used for drawing.
        */                       
        void DrawHandsL( CWindowGc& aGc, const TRect& aRect,
                         const TDateTime& aDateTime );

    private:

        /**
        * C++ default constructor.
        */
        CXnClockFaceAnalog();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data                
    };

#endif      // _XNCLOCKFACE_H
            
// End of File
