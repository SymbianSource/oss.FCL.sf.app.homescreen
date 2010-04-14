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
* implementation of wait dialog that doesn't consume key events.
*
*/

// System includes

// User includes
#include "xnwaitdialog.h"

// CONSTANTS

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CXnWaitDialog::CXnWaitDialog
// ---------------------------------------------------------------------------
//
CXnWaitDialog::CXnWaitDialog( CEikDialog** aSelfPtr,
                              TBool aVisibilityDelayOff )
  : CAknWaitDialog( aSelfPtr, aVisibilityDelayOff )
  {
  }

// ---------------------------------------------------------------------------
// CXnWaitDialog::~CXnWaitDialog
// ---------------------------------------------------------------------------
//
CXnWaitDialog::~CXnWaitDialog()
  {
  }

// ---------------------------------------------------------------------------
// CXnWaitDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CXnWaitDialog::OfferKeyEventL( 
        const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/ )
  {
  return EKeyWasConsumed; // consume all keys while open
  }

// End of file
