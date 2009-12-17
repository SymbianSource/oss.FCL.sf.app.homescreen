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


#include "hnmdevent.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdEvent::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdEvent::SetId( TInt aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEvent* CHnMdEvent::NewL( const TInt aId )
    {
    CHnMdEvent* self = CHnMdEvent::NewLC( aId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdEvent* CHnMdEvent::NewLC( const TInt aId )
    {
    CHnMdEvent* self = new( ELeave ) CHnMdEvent;
    CleanupStack::PushL( self );
    self->ConstructL( aId );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdEvent::ConstructL( const TInt aId )
    {
    SetId( aId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEvent::CHnMdEvent()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEvent::~CHnMdEvent()
    {

    }

