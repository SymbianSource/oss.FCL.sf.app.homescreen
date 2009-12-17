/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmImageConverter inline function definitions
*
*/
#ifndef IMAGECONVERTER_INL_
#define IMAGECONVERTER_INL_

// ---------------------------------------------------------------------------
// Sets logo size
// ---------------------------------------------------------------------------
//
inline void CWmImageConverter::SetLogoSize( const TSize& aSize )
    {
    iSize = aSize;
    };

// ---------------------------------------------------------------------------
// Returns engine state
// ---------------------------------------------------------------------------
//
inline CWmImageConverter::TState CWmImageConverter::EngineState()
    {
    return iState;
    };

#endif /* IMAGECONVERTER_INL_ */
