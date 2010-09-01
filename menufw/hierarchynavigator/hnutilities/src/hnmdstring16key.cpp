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

#include "hnmdstring16key.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key* CHnMdString16Key::NewL()
    {
    CHnMdString16Key* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key* CHnMdString16Key::NewLC()
    {
    CHnMdString16Key* self = new ( ELeave ) CHnMdString16Key();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key* CHnMdString16Key::NewL( const CHnMdString16Key* aKey )
    {
    CHnMdString16Key* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key* CHnMdString16Key::NewLC( const CHnMdString16Key* aKey )
    {
    CHnMdString16Key* self = new ( ELeave ) CHnMdString16Key();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdString16Key::CopyLC()
    {
    CHnMdString16Key* self = CHnMdString16Key::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key::~CHnMdString16Key()
    {
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString16Key::CHnMdString16Key()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString16Key::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString16Key::ConstructL( const CHnMdString16Key* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString16Key::ToVariantL( TLiwVariant& aRet ) const
    {
    aRet.SetL( TLiwVariant( KeyContent() ) );
    }

