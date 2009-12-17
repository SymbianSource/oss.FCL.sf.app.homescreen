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
* Description:  Array of points - inline functions
*
*/


#ifndef _XNPOINTARRAY_INL_
#define _XNPOINTARRAY_INL_

namespace XnGestureHelper
    {

    // -----------------------------------------------------------------------
    // TXnPointEntry's Constructor
    // -----------------------------------------------------------------------
    //    
    inline TXnPointEntry::TXnPointEntry( const TPoint& aPos, 
        const TTime& aTime ) : iPos( aPos ), iTime( aTime )
        {
        };  
    }

#endif // _XNPOINTARRAY_INL_
