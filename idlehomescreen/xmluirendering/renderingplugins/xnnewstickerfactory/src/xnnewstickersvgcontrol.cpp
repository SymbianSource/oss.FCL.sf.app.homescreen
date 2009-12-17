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
* Description:  Text scrolling functionality.
*
*/


// INCLUDE FILES
#include <SVGEngineInterfaceImpl.h>

#include "xnnewstickersvgcontrol.h"
#include "xnnewstickeradapter.h"

   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::CXnNewstickerSvgControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNewstickerSvgControl::CXnNewstickerSvgControl(CXnNewstickerAdapter* aAdapter) :
    iAdapter(aAdapter)
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerSvgControl::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerSvgControl* CXnNewstickerSvgControl::NewL(CXnNewstickerAdapter* aAdapter)
    {
    CXnNewstickerSvgControl* self = new(ELeave)CXnNewstickerSvgControl(aAdapter);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();    
    return self;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::~CXnNewstickerSvgControl()
// Destructor.
// -----------------------------------------------------------------------------
//
CXnNewstickerSvgControl::~CXnNewstickerSvgControl()
    {
    delete iBitmap;
	delete iMaskBitmap;
	if(iSVGEngine)
	    {
	    iSVGEngine->Destroy();
		delete iSVGEngine;
	    }
    }

// ---------------------------------------------------------
// CXnNewstickerSvgControl::StartL
// ---------------------------------------------------------
//
void CXnNewstickerSvgControl::StartL(const TDesC8& aSvgData)
    {
    iHasContent = ETrue;
    if(!iSVGEngine)
        {        
    	TFontSpec fspec;
    	iSVGEngine = CSvgEngineInterfaceImpl::NewL(iBitmap,this, fspec);
	    }

	MSvgError* error = iSVGEngine->Load(aSvgData);
	User::LeaveIfError(error->SystemErrorCode());

	// Get the size of the bitmap and set the SVG size to match
 	TSize tmpsize = iBitmap->SizeInPixels();
 	iSVGEngine->SetSvgDimensionToFrameBuffer(tmpsize.iWidth, tmpsize.iHeight);
    iSVGEngine->SetBackgroundColor(0x00FFFFFF, NULL);
	iSVGEngine->Start();
    }

// ---------------------------------------------------------
// CXnNewstickerSvgControl::StopL
// ---------------------------------------------------------
//
void CXnNewstickerSvgControl::StopL()
    {
    iHasContent = EFalse;
    if(iSVGEngine)
	    {
		iSVGEngine->Stop();
	    }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::SetSvgRectL
// Set visible rect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerSvgControl::SetSvgRectL(TRect& aContentRect, CGraphicsDevice& aDevice )
    {
    iContentRect = aContentRect;    
	
	TDisplayMode displayMode = aDevice.DisplayMode();
    
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    iBitmap = new (ELeave) CFbsBitmap();
 	iBitmap->Create(iContentRect.Size(),displayMode);

    if(iMaskBitmap)
        {
        delete iMaskBitmap;
        iMaskBitmap = NULL;
        }
    iMaskBitmap = new (ELeave) CFbsBitmap();
 	iMaskBitmap->Create(iContentRect.Size(),EGray256);
 	
 	if(iSVGEngine && iHasContent)
        {
        iSVGEngine->SetFrameBuffer(iBitmap);
    	TSize tmpsize = iBitmap->SizeInPixels();
    	iSVGEngine->SetSvgDimensionToFrameBuffer(tmpsize.iWidth, tmpsize.iHeight);
        }
    }
 
// -----------------------------------------------------------------------------
// CXnNewstickerSvgControl::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXnNewstickerSvgControl::Draw() const
    {       
    TPoint point(iContentRect.iTl);
    iAdapter->BufferGc()->BitBlt(TPoint(0,0), iAdapter->BackgroundBitmap());
    iAdapter->BufferGc()->BitBltMasked(TPoint(0,0), iBitmap, 
        TRect(iBitmap->SizeInPixels()), iMaskBitmap, ETrue);   
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// CXnNewstickerSvgControl::UpdateScreen
// ---------------------------------------------------------------------------
//
void CXnNewstickerSvgControl::UpdateScreen()
    {
	iSVGEngine->GenerateMask(iMaskBitmap, NULL);
    
    iAdapter->DrawNow();
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// CXnNewstickerSvgControl::ScriptCall
// ---------------------------------------------------------------------------
//
TBool  CXnNewstickerSvgControl::ScriptCall(
    const TDesC& /*aScript*/,
    CSvgElementImpl* /*aCallerElement*/)
    {
	return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// CXnNewstickerSvgControl::FetchImage
// ---------------------------------------------------------------------------
//
TInt CXnNewstickerSvgControl::FetchImage(
    const TDesC& /*aUri*/,
    RFs& /*aSession*/,
    RFile& /*aFileHandle*/)
    {
	return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// CXnNewstickerSvgControl::FetchFont
// ---------------------------------------------------------------------------
//
TInt CXnNewstickerSvgControl::FetchFont(
    const TDesC& /*aUri*/,
    RFs& /*aSession*/,
    RFile& /*aFileHandle*/)
    {
	return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// CXnNewstickerSvgControl::UpdatePresentation
// ---------------------------------------------------------------------------
//
void CXnNewstickerSvgControl::UpdatePresentation(const TInt32&  /*aNoOfAnimation*/)
    {
    }

//  End of File  
