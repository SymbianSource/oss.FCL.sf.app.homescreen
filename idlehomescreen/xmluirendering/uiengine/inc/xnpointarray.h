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


#ifndef _XNPOINTARRAY_H_
#define _XNPOINTARRAY_H_

// System includes
#include <e32std.h>

/**
 * XnGestureHelper namespace
 * Used for the whole gesture family - Gesture recognizer, gesture helper, 
 * Point array
 */
namespace XnGestureHelper
    {
    
    /** 
     * Represents a single point given at certain time
     */
    struct TXnPointEntry
        {
        
        /** constructor */
        inline TXnPointEntry( const TPoint& aPos, const TTime& aTime );
        
        TPoint iPos;
        TTime iTime;
        };
    
    /**
     *  Array of points that wraps another point array
     *  The purpose of this class is to allow deriving classes to override []
     *  and in that member function adjust the point, e.g., by setting one
     *  axis to always a constant value. This allows providing a sequence of
     *  points to the recogniser in which the other axis is ignored.
     *
     *  @ingroup group_xnlayoutengine
     */
    NONSHARABLE_CLASS( TXnPointArray )
        {
    public:
        
        /** 
         * Constructor
         * 
         * @param aPoints points array to wrap (and filter)
         */
        TXnPointArray( const RArray< TXnPointEntry >& aPoints );
        
        /**
         * Length of the array
         * 
         * @return Length of the array 
         */
        TInt Count() const;
        
        /** 
         * Returns point at position, may be filtered
         * virtual so deriving classes can modify the point 
         * (e.g., filter one axis)
         *
         * @return A filtered point at aIndex. Default implementation same
         *          as Raw(...)
         */
        virtual TPoint operator[]( TInt aIndex ) const;
        
        /** 
         * Nonfiltered index
         * 
         * @return An raw, non-filtered point at aIndex 
         */
        const TPoint& Raw( TInt aIndex ) const;

    private:
        
        /** Array of points, Not owned */
        const RArray< TXnPointEntry >& iPoints;
        };
    } // namespace XnGestureHelper


// Inline functions
#include "xnpointarray.inl"

#endif // _XNPOINTARRAY_H_
