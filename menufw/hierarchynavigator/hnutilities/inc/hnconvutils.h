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



#ifndef HNCONVUTILS_H
#define HNCONVUTILS_H

#include <e32base.h>

/**
 * Provides static methods for converting numbers to
 * descriptors and vice versa.
 *  
 *  @lib hnutilities.lib
 *  @since S60 v5.0
 *  @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( HnConvUtils )
    {   
public:

	/**
     * Converts narrow string descriptor to default width string descriptor.
     *
	 * @since S60 v5.0
	 * @param aStr Narrow string descriptor.
	 * @return Default wide string descriptor.
	 */
	IMPORT_C static HBufC* Str8ToStr( const TDesC8& aStr );
	
    /**
     * Converts narrow string descriptor to default width string descriptor.
     * Does not handle special characters.
     *
     * @since S60 v5.0
     * @param aStr Narrow string descriptor.
     * @return Default wide string descriptor.
     */
    static HBufC* Str8ToStrFastLC( const TDesC8& aStr );
    
    /**
     * Converts wide string descriptor to narrow string descriptor.
     * Does not handle special characters.
     *
     * @since S60 v5.0
     * @param aStr Wide string descriptor input.
     * @return Narrow string.
     */    
    static HBufC8* StrToStr8FastLC( const TDesC16& aStr );
        
    /**
     * Converts narrow string descriptor to default width string descriptor.
     * Handles special characters.
     *
     * @since S60 v5.0
     * @param aStr Narrow string descriptor.
     * @return Default wide string descriptor.
     */
    IMPORT_C static HBufC* Str8ToStrLC( const TDesC8& aStr );

    /**
     * Converts wide string descriptor to narrow string descriptor.
     * Handles special characters.
     *
     * @since S60 v5.0
     * @param aStr Wide string descriptor input.
     * @return Narrow string.
     */    
    IMPORT_C static HBufC8* StrToStr8LC( const TDesC16& aStr );

    /**
     * Converts wide string descriptor to narrow string descriptor.
     *
     * @since S60 v5.0
     * @param aStr Wide string descriptor input.
     * @return Narrow string.
     */    
    IMPORT_C static HBufC8* StrToStr8L( const TDesC16& aStr );
    
    /**
     * Converts integer to narrow string descriptor.
     *
     * @since S60 v5.0
     * @param aNum Number.
     * @return Narrow string.
     */    
    IMPORT_C static HBufC8* NumToStr8LC( const TInt& aNum );
    
    /**
     * Converts narrow string descriptor to integer value.
     *
     * @since S60  v5.0
     * @param aValue Input descriptor.
     * @param aResult Result integer.
     * @return Error code.
     */
    IMPORT_C static TInt Str8ToInt( const TDesC8& aValue, TInt& aResult );

    /**
     * Converts string descriptor to integer value.
     *
     * @since S60  v5.0
     * @param aValue Input descriptor.
     * @param aResult Result integer.
     * @return Error code.
     */
    IMPORT_C static TInt StrToInt( const TDesC& aValue, TInt& aResult );
  
    /**
     * Converts narrow string descriptor to integer value.
     *
     * @since S60  v5.0
     * @param aValue Input descriptor.
     * @param aResult Result integer.
     * @return Error code.
     */
    IMPORT_C static TInt Str8ToInt( const TDesC8& aValue, TInt32& aResult );
     
    /**
     * Converts narrow string descriptor to integer value.
     *
     * @since S60  v5.0
     * @param aValue Input descriptor.
     * @param aResult Result integer.
     * @return Error code.
     */
    IMPORT_C static TInt StrToInt( const TDesC& aValue, TInt32& aResult );
     
    /**
     * Converts narrow string descriptor to integer value.
     *
     * @since S60  v5.0
     * @param aValue Input descriptor.
     * @param aResult Result integer.
     * @return Error code.
     */
    IMPORT_C static TInt Str8ToUint( const TDesC8& aValue, TUint& aResult );
    };

#include "hnconvutils.inl"

#endif // HNCONVUTILS_H

