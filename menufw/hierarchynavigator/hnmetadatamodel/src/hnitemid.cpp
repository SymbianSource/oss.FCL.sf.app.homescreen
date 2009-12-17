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


#include "hnitemid.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnItemId::ConstructL( const TDesC& aFile,
    const TDesC& aID )
    {
    iSuiteName.CreateL( aFile );
    iSuiteName.LowerCase();
    iId.CreateL( aID );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemId::UpdateL(const CHnItemId & aItemId )
    {
    iSuiteName.Close();
    iId.Close();
    
    iSuiteName.CreateL( aItemId.SuiteName() );
    iSuiteName.LowerCase();
    iId.CreateL( aItemId.Id() );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemId* CHnItemId::NewL( const TDesC& aFile,
    const TDesC& aID )
    {
    CHnItemId* self = CHnItemId::NewLC( aFile, aID );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemId* CHnItemId::NewLC(const TDesC& aFile,
    const TDesC& aID )
    {
    CHnItemId* self = new( ELeave ) CHnItemId();
    CleanupStack::PushL( self );
    self->ConstructL( aFile, aID );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnItemId::CHnItemId()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnItemId::~CHnItemId()
    {
    iSuiteName.Close();
    iId.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnItemId::SuiteName() const
    {
    return iSuiteName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnItemId::Id() const
    {
    return iId;
    }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
EXPORT_C TBool CHnItemId::Compare( const CHnItemId& aFilter ) const
     {
     TBool result( EFalse );
     if( !iSuiteName.Compare( aFilter.SuiteName() ) &&
         !iId.Compare( aFilter.Id() ) )
         {
         result = ETrue;
         }
     return result;
     }

