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
* Description:  Implementation for the interface class CXnNewsticker.
*
*/


#include <gulicon.h>

#include "xnnewsticker.h"
#include "xnnewstickeradapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewsticker::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnNewsticker* CXnNewsticker::NewL()
    {
	CXnNewsticker* self = new(ELeave) CXnNewsticker;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;	
    }

// -----------------------------------------------------------------------------
// CXnNewsticker::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewsticker::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnNewsticker::CXnNewsticker
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnNewsticker::CXnNewsticker()
    {
    }

// -----------------------------------------------------------------------------
// CXnNewsticker::~CXnNewsticker
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnNewsticker::~CXnNewsticker()
    {
    }


// ---------------------------------------------------------
// CXnNewsticker::AppendTitleL
// ---------------------------------------------------------
//
void CXnNewsticker::AppendTitleL(const TDesC& aTitle)
    {
    (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->AppendTitleL(aTitle);
    }


// ---------------------------------------------------------
// CXnNewsticker::InsertTitleL
// ---------------------------------------------------------
//
void CXnNewsticker::InsertTitleL(const TDesC& aTitle, TInt aIndex)
    {
    (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->InsertTitleL(aTitle, aIndex);
    }

// ---------------------------------------------------------
// CXnNewsticker::UpdateTitleL
// ---------------------------------------------------------
//
void CXnNewsticker::UpdateTitleL(const TDesC& aTitle, TInt aIndex)
    {
    (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->UpdateTitleL(aTitle, aIndex);
    }


// ---------------------------------------------------------
// CXnNewsticker::DeleteTitle
// ---------------------------------------------------------
//
void CXnNewsticker::DeleteTitle(TInt aIndex)
    {
    CXnNewstickerAdapter* adapter = 
        static_cast<CXnNewstickerAdapter*>(ControlAdapter());
    TRAP_IGNORE(adapter->DeleteTitleL(aIndex));
    }


// ---------------------------------------------------------
// CXnNewsticker::CurrentTitleIndex
// ---------------------------------------------------------
//
TInt CXnNewsticker::CurrentTitleIndex()
    {
    return (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->CurrentTitleIndex();
    }


// ---------------------------------------------------------
// CXnNewsticker::Title
// ---------------------------------------------------------
//
const TDesC& CXnNewsticker::Title(TInt aIndex)
    {
    return (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->Title(aIndex);
    }

// ---------------------------------------------------------
// CXnNewsticker::ClearTitles
// ---------------------------------------------------------
//
void CXnNewsticker::ClearTitles()
    {
    (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->ClearTitles();
    }

// ---------------------------------------------------------
// CXnNewsticker::SetCallbackInterfaceL
// ---------------------------------------------------------
//
void CXnNewsticker::SetCallbackInterfaceL(
    XnNewstickerInterface::MXnNewstickerCallbackInterface* aCallback)
    {
    (static_cast<CXnNewstickerAdapter*>
            (ControlAdapter()))->SetCallbackInterfaceL(aCallback);
    }

// ---------------------------------------------------------
// CXnNewsticker::MakeInterfaceL
// ---------------------------------------------------------
//
XnComponentInterface::MXnComponentInterface* CXnNewsticker::MakeInterfaceL(const TDesC8& aType)
   {
   if (aType != XnNewstickerInterface::KType)
       {
       return NULL;
       }
    XnNewstickerInterface::MXnNewstickerInterface* newstickerintr = 
        static_cast<XnNewstickerInterface::MXnNewstickerInterface*>(this);
    return newstickerintr;
   }
    
// End of File
