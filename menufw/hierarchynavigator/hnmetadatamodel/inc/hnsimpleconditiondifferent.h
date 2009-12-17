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



#ifndef C_HNSIMPLECONDITIONDIFFERENT_H
#define C_HNSIMPLECONDITIONDIFFERENT_H

#include "hnsimplecondition.h"

/**
 * Simple Condition Different.
 * 
 * Class compares condition with criterium 'different'.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
 NONSHARABLE_CLASS( CHnSimpleConditionDifferent ) : public CHnSimpleCondition
     {
     /**
      * Gets positions. From CHnMenuCondition
      * 
      * @param aBuffer Buffer.
      * @return Position.
      */
     TInt GetPositionL( const TDesC8& aBuffer );
     
     /**
      * Returns result. From CHnMenuCondition
      *
      * @since S60 v3.2
      * @param aVarLeft Left Liv variant condition to compare.
      * @param aVarRight Right Liv variant condition to compare.
      * @return True if condition is satisfied.
      */    
     TBool CheckCondition( TLiwVariant& aVarLeft, TLiwVariant& aVarRight );
    
     /**
      * Gets length. From CHnMenuCondition
      * 
      * @return Length of an operator.
      */
     TInt SignLength();
     };

#endif // C_HNSIMPLECONDITIONDIFFERENT_H

//End of file
