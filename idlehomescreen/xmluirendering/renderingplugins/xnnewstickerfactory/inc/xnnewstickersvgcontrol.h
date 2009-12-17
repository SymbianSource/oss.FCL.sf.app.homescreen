/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Svg scrolling functionality.
*
*/


#ifndef XNNEWSTICKERSVGCONTROL_H
#define XNNEWSTICKERSVGCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <SVGRequestObserver.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CXnNewstickerAdapter;
class CSvgEngineInterfaceImpl;

// CLASS DECLARATION

/**
* @ingroup group_xnnewstickerfactory
* 
*  Newsticker control for XUIKON svg scrolling functionality.
*
*  @lib xn3newstickerfactory.dll
*  @since Series 60 3.2
*/
class CXnNewstickerSvgControl : public CBase, public MSvgRequestObserver
    {
    public:
        /**
         * Two-phased constructor.
         * @param aAdapter Parent control
         * @return Pointer to this.
         */
        static CXnNewstickerSvgControl* NewL(CXnNewstickerAdapter* aAdapter);

        /** 
        * Destructor
        */
        virtual ~CXnNewstickerSvgControl();

    public: //  New functions
        /**
        * Start showing the SVG until StopL is called.
        * @param aSvgData The svg byte data to show.
        */
        void StartL(const TDesC8& aSvgData);

        /**
        * Stop the SVG showing and started with the StartL.
        */
        void StopL();
        
        /** 
        * Sets the rect of the visible area.
        * @since S60 v3.1
        * @param aContentRect The visible rect.
        * @param aDevice The current graphics device.
        * @return void
        */        
        void SetSvgRectL(TRect& aContentRect, CGraphicsDevice& aDevice );
        
        /**
        * Draws the SVG into screen
        */
        void Draw() const;

    public: //From MSvgRequestObserver
    
        /**
        * This method is called by the SVG Engine when all data for a certain
        * step, such as animation frame, have been updated.  This usually means
        * the off-screen image (passed in to 'ConstructL' of CSvgEngineInterface)
        * has been updated.  The client usually redraws the image onto
        * the screen.
        */
        void UpdateScreen();

        /**
        * This method is for future extension, in which an external script engine
        * could be used to evaluate a script description.
        *
        * @param : aScript -- A description of script from the "on" attribute.
        * @param : aCallerElement -- The element that calls the script by a
        *                            an event of the "on" attribute.
        * @return : For future use.  Value is ignored.
        */
        TBool ScriptCall(const TDesC& aScript, CSvgElementImpl* aCallerElement);

        /**
        * This method is called to retrieve the file handle of an image name.
        * The session (RFs) object passed in is guaranteed to be connected, so
        * the client must NOT call aSession.Connect().
        *
        * The implementation for the client of this method is usually if not always:
        * TInt X::FetchImage(const TDesC& aFilename, RFs& aSession, RFile& aFileHandle)
        *     {
        *     // Check for "http", get data from remote site, save to disk
        *     // Get absolute path to aFilename: absolutepath
        *     return aFileHandle.Open(aSession, absolutepath, EFileRead);
        *     }
        *
        * Note: Use EFileShareReadersOnly to prevent file locking.
        *
        * The API AssignImageData() can be used to avoid block the parsing
        * thread due this method taking too long to return.  This is
        * accomplished by returning a non-KErrNone value  (i.e. KErrNotFound),
        * and storing the URI parameter.  The image data then could be
        * assigned the the URI in another thread/Active-Object.
        *
        * @param : aUri -- the relative path of an image file.
        * @param : aSession -- File session used by client to open file.
        * @param : aFileHandle -- File handle used by client to open file.
        * @return : KErrNone if Open command successful, otherwise error-code
        *           from Open command.  The SVG engine will proceed loading the file
        *           only if the return value is KErrNone.
        */
        TInt FetchImage(const TDesC& aUri, RFs& aSession, RFile& aFileHandle);

        /**
        * This method is called to retrieve the file handle of an svg font file.
        * This file must be SVG 1.1 .svg file.  When the engine comes acrossed
        * a font-family name that it doesnt know it will request "fontfamilyname".svg
        * file through this method.  The client can either provide the svg file
        * or the engine will default to system text.
        *
        * The usage of this method is exactly the same as the FetchImage method above
        *
        * @param : aUri -- the relative path of an image file.
        * @param : aSession -- File session used by client to open file.
        * @param : aFileHandle -- File handle used by client to open file.
        * @return : KErrNone if Open command successful, otherwise error-code
        *           from Open command.  The SVG engine will proceed loading the file
        *           only if the return value is KErrNone.
        */
        TInt FetchFont(const TDesC& aUri, RFs& aSession, RFile& aFileHandle);

        /**
        * This Method updates the presentation status
        * @param : TInt32&  aNoOfAnimation
        */
        void UpdatePresentation(const TInt32&  aNoOfAnimation);

    private: 

        CXnNewstickerSvgControl(CXnNewstickerAdapter* aAdapter);

        void ConstructL();
        
    private:    // Data
        /**
        * Parent control adapter
        * Not own.
        */	            
        CXnNewstickerAdapter*       iAdapter;
        
        /**
        * Pointer to SVG rendering engine
        * Owned.
        */
        CSvgEngineInterfaceImpl*    iSVGEngine;
        
        /**
        * Pointer to svg bitmap.
        * Own.
        */
        CFbsBitmap*                 iBitmap;
        
        /**
        * Pointer to svg mask.
        * Own.
        */
        CFbsBitmap*                 iMaskBitmap;
        
        /**
        * A rect in which the svg is shown.
        */
        TRect                       iContentRect;
        
        /**
        * Defines whether the content has been loaded
        */
        TBool                       iHasContent;
    };

#endif // XNNEWSTICKERSVGCONTROL_H

// End of File
