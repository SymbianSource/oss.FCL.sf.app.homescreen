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
* Description:  
*
*/


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline HBufC* HnConvUtils::Str8ToStrFastLC( const TDesC8& aStr )
    {
    HBufC* ret = HBufC::NewLC( aStr.Length() );
    ret->Des().Copy( aStr );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline HBufC8* HnConvUtils::StrToStr8FastLC( const TDesC16& aStr )
    {
    HBufC8* ret = HBufC8::NewLC( aStr.Length() );
    ret->Des().Copy( aStr );
    return ret;
    }
