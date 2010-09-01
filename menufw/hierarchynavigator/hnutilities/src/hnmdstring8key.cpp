/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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

#include "hnconvutils.h"
#include "hnmdstring8key.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key* CHnMdString8Key::NewL()
    {
    CHnMdString8Key* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key* CHnMdString8Key::NewLC()
    {
    CHnMdString8Key* self = new ( ELeave ) CHnMdString8Key();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key* CHnMdString8Key::NewL( const CHnMdString8Key* aKey )
    {
    CHnMdString8Key* self = NewLC( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key* CHnMdString8Key::NewLC( const CHnMdString8Key* aKey )
    {
    CHnMdString8Key* self = new ( ELeave ) CHnMdString8Key();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdString8Key::CopyLC()
    {
    CHnMdString8Key* self = CHnMdString8Key::NewL( this );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key::~CHnMdString8Key()
    {
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdString8Key::CHnMdString8Key()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString8Key::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString8Key::ConstructL( const CHnMdString8Key* aKey )
    {
    BaseConstructL( aKey );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdString8Key::ToVariantL( TLiwVariant& aRet ) const
    {
    TLiwVariant ret;
    HBufC8* keyVal8 = HnConvUtils::StrToStr8LC( KeyContent() );

    aRet.SetL(TLiwVariant( *keyVal8) ); // crates a copy of the data

    // clean up
    CleanupStack::PopAndDestroy(keyVal8);    
    }

