/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Version     : 2 << Don't touch! Updated by Synergy at check-out.
*
*/

#include "mmwidgetsconstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TMmSubcellsSetupCode::TMmSubcellsSetupCode()
    : iCode( 0 )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void TMmSubcellsSetupCode::AddSubcellInfo( TSubcellType aSubcellType )
    {
    // 8 == 2 ^ KNumberOfBitsToStoreSubcellType
    __ASSERT_DEBUG( aSubcellType < 8, User::Invariant() );
    
    iCode <<= KNumberOfBitsToStoreSubcellType;
    iCode |= aSubcellType;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void TMmSubcellsSetupCode::AddTemplateInfo( TInt aTemplateIdentifier )
    {
    // 256 == 2 ^ KNumberOfBitsToStoreTemplateId
    __ASSERT_DEBUG( aTemplateIdentifier < 256, User::Invariant() );
    
    iCode <<= KNumberOfBitsToStoreTemplateId;
    iCode |= aTemplateIdentifier;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void TMmSubcellsSetupCode::AddIsCurrentInfo( TBool aIsCurrent )
    {
    iCode <<= KNumberOfBitsToStoreIsCurrentInfo;
    // remember that TBool is in fact an integer
    iCode |= ( aIsCurrent ? 1 : 0 );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline void TMmSubcellsSetupCode::Clear()
    {
    iCode = 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TBool TMmSubcellsSetupCode::operator==(
        const TMmSubcellsSetupCode aOther ) const
    {
    return iCode == aOther.iCode;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline TBool TMmSubcellsSetupCode::operator!=(
        const TMmSubcellsSetupCode aOther ) const
    {
    return iCode != aOther.iCode;
    }
