/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture class - inline section
*
*/


// Protection for nested includes
#ifndef _XNGESTURE_INL_
#define _XNGESTURE_INL_

// -----------------------------------------------------------------------------
// TRealPoint inline implementation
// -----------------------------------------------------------------------------
//
namespace XnGestureHelper
    {

    // -------------------------------------------------------------------------
    // Default Constructor for the real point
    // -------------------------------------------------------------------------
    //
    inline TRealPoint::TRealPoint() :
        iX( 0 ), iY( 0 )
        {
        }
    

    // -------------------------------------------------------------------------
    // Copy constructor for real point
    // -------------------------------------------------------------------------
    //
    inline TRealPoint::TRealPoint( const TRealPoint& aPoint ) :
        iX( aPoint.iX ), iY( aPoint.iY )
        {
        }

    // -------------------------------------------------------------------------
    // Copy constructor for real point
    // -------------------------------------------------------------------------
    //
    inline TRealPoint::TRealPoint( TReal aX, TReal aY ) :
        iX( aX ), iY( aY )
        {
        }

    // -------------------------------------------------------------------------
    // Default constructor for real point
    // -------------------------------------------------------------------------
    //
    inline TBool TRealPoint::operator==( const TRealPoint& aPoint ) const
        {
        return iX == aPoint.iX && iY == aPoint.iY;
        }
    }
	
#endif //_XNGESTURE_INL_
