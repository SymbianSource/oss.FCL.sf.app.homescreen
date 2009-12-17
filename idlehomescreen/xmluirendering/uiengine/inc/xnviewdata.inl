/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data class to hold view info - inline functions
*
*/


#ifndef _XNVIEWDATA_INL_
#define _XNVIEWDATA_INL_

// ---------------------------------------------------------------------------
// Returns if active or not
// ---------------------------------------------------------------------------
//
inline TBool CXnViewData::Active() const
    {
    return ( iFlags.IsSet( EIsActive ) ? ETrue : EFalse );
    };

// ---------------------------------------------------------------------------
// Sets viewdata to be initial
// ---------------------------------------------------------------------------
//
inline void CXnViewData::SetInitial() 
    {
    iFlags.Set( EIsInitial );
    };

// ---------------------------------------------------------------------------
// Returns if initial or not
// ---------------------------------------------------------------------------
//
inline TBool CXnViewData::Initial() const 
    {
    return ( iFlags.IsSet( EIsInitial ) ? ETrue : EFalse );
    };

// ---------------------------------------------------------------------------
// Sets focused node
// ---------------------------------------------------------------------------
//
inline void CXnViewData::SetFocusedNode( CXnNode* aNode )
    {
    iFocusedNode = aNode;
    };

// ---------------------------------------------------------------------------
// Returns focused node
// ---------------------------------------------------------------------------
//
inline CXnNode* CXnViewData::FocusedNode() const
    {
    return iFocusedNode;
    };

// ---------------------------------------------------------------------------
// Sets to use empty widget
// ---------------------------------------------------------------------------
//
inline void CXnViewData::SetUseEmptyWidget( TBool aUse )
    {
    if( aUse )
        {
        iFlags.Set( EUseEmpty );        
        }
    else
        {
        iFlags.Clear( EUseEmpty );        
        }    
    };

// ---------------------------------------------------------------------------
// Returns whether to use empty widget
// ---------------------------------------------------------------------------
//
inline TBool CXnViewData::UseEmptyWidget() const
    {
    return ( iFlags.IsSet( EUseEmpty ) ? ETrue : EFalse );
    };

#endif // _XNVIEWDATA_INL_ 
