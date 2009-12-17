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
* Description:  Array of points
*
*/

#include "xnpointarray.h"

using namespace XnGestureHelper;

// ----------------------------------------------------------------------------
// TXnPointArray constructor
// ----------------------------------------------------------------------------
//
TXnPointArray::TXnPointArray( const RArray< TXnPointEntry >& aPoints )
    : iPoints( aPoints )
    {
    }

// ----------------------------------------------------------------------------
// TXnPointArray length
// ----------------------------------------------------------------------------
//
TInt TXnPointArray::Count() const
    {
    return iPoints.Count();
    }

// ----------------------------------------------------------------------------
// point at index, no filtering
// ----------------------------------------------------------------------------
//
TPoint TXnPointArray::operator[]( TInt aIndex ) const
    {
    return iPoints[aIndex].iPos;
    }

// ----------------------------------------------------------------------------
// Unfiltered point at index
// ----------------------------------------------------------------------------
//
const TPoint& TXnPointArray::Raw( TInt aIndex ) const
    {
    return iPoints[aIndex].iPos;
    }
