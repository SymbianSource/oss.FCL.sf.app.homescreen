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
* Description:  abstract base class for attribute models.
*
*/


#include "hnattributebase.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnAttributeBase::SetNameL( const TDesC8& aName )
    {
    iName.Close();
    iName.CreateL( aName.Length() + 1 );
    iName.Copy( aName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnAttributeBase::Name( ) const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnAttributeBase::~CHnAttributeBase( )
    {
    iName.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TAttributeType CHnAttributeBase::Type()
    {
    return iAttributeType;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnAttributeBase::RemoveLiwObjects()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnAttributeBase::Value()
    {
    // The descendant should override this if such functionality is provided.
    // if that functionality is not provided, generally it means that this
    // should not be invoked this assert in debug.
    ASSERT(EFalse);
    return KNullDesC8;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CHnAttributeBase::GetIcon( TSize* /*aDesiredIconSize*/ )
    {
    // The descendant should override this if such functionality is provided.
    // if that functionality is not provided, generally it means that this
    // should not be invoked this assert in debug.    
    ASSERT(EFalse);
    return NULL;
    }

CHnIconHolder* CHnAttributeBase::GetIconHolder( TSize* /*aDesiredIconSize*/ )
    {
    // The descendant should override this just like the other methods.
    ASSERT( EFalse );
    return NULL;
    }
