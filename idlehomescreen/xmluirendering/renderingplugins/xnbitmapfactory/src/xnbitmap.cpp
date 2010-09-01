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
* Description:  Xuikon Bitmap component.
*
*/



#include "xnbitmap.h"
#include "xnbitmapadapter.h"
// ============================ MEMBER FUNCTIONS ===============================




// -----------------------------------------------------------------------------
// CXnBitmap::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnBitmap* CXnBitmap::NewL()
    {
	CXnBitmap* self = new( ELeave ) CXnBitmap;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnBitmap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnBitmap::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnBitmap::CXnBitmap
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnBitmap::CXnBitmap()
    {
    }

// -----------------------------------------------------------------------------
// CXnBitmap::~CXnBitmap
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnBitmap::~CXnBitmap()
    {
    }

// -----------------------------------------------------------------------------
// CXnBitmap::SetContentBitmaps
// -----------------------------------------------------------------------------
//
void CXnBitmap::SetContentBitmaps(CFbsBitmap* aBitmap, CFbsBitmap* aMask)
    {
    (static_cast<CXnBitmapAdapter*>(ControlAdapter()))->SetContentBitmaps(aBitmap, aMask);
    }   

// -----------------------------------------------------------------------------
// CXnBitmap::SetContentBitmaps
// -----------------------------------------------------------------------------
//
void CXnBitmap::SetContentBitmaps( TFileName& aFilename )
    {
    (static_cast<CXnBitmapAdapter*>(ControlAdapter()))->SetContentBitmaps(aFilename);
    }   

// -----------------------------------------------------------------------------
// CXnBitmap::ContentBitmaps
// -----------------------------------------------------------------------------
//
void CXnBitmap::ContentBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask)
    {
    (static_cast<CXnBitmapAdapter*>(ControlAdapter()))->ContentBitmaps(aBitmap, aMask);
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnBitmap::MakeInterfaceL(const TDesC8& aType)
    {
    if (aType != XnImageInterface::KType)
        {
        return NULL;
        }
    XnImageInterface::MXnImageInterface* imgintr = static_cast<XnImageInterface::MXnImageInterface*>(this);
    return imgintr;
    }           
