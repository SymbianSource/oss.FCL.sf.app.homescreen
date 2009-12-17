/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <liwservicehandler.h>

#include "hnmdundefkey.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey* CHnMdUndefKey::NewL()
    {
    CHnMdUndefKey* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey* CHnMdUndefKey::NewLC()
    {
    CHnMdUndefKey* self = new ( ELeave ) CHnMdUndefKey();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey* CHnMdUndefKey::NewL( const CHnMdUndefKey* aKey )
    {
    CHnMdUndefKey* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey* CHnMdUndefKey::NewLC( const CHnMdUndefKey* aKey )
    {
    CHnMdUndefKey* self = new ( ELeave ) CHnMdUndefKey();
    CleanupStack::PushL(self);
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdUndefKey::CopyLC()
    {
    CHnMdUndefKey* self = CHnMdUndefKey::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey::~CHnMdUndefKey()
    {
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdUndefKey::CHnMdUndefKey()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUndefKey::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdUndefKey::ConstructL( const CHnMdUndefKey* aKey )
    {
    BaseConstructL( aKey );
    }

