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
        *   
        * @param aGc Graphics context that can be used to draw to
        *   the clock
        *
        * @param aRect Rect where to scale the clock
        *                   
        * @param aTime Time to be used for drawing.
        */
        virtual void DrawL( CXnClockAdapter& aAdapter,
                            CWindowGc& aGc, 
                            const TRect& aRect, 
                            const TTime& aTime ) = 0;                                    
    };

class CXnClockFaceDigital : public CXnClockFace
    {
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
                    const TRect& aRect, 
                    const TTime& aTime );

    private:

        /**
        * C++ default constructor.
        */
        CXnClockFaceDigital();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
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
                    const TRect& aRect, 
                    const TTime& aTime ); 

    private: // New functions

        /**
        * Draws the hands of the clock.
        *
        * @param aGc Graphics context that can be used to draw to the
        *   clock
        *
        * @param aRect Rectangle defining the size of the clock.
        *
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
