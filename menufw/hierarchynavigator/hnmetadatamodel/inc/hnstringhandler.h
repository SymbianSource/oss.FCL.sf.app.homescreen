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


#ifndef HNSTRINGHANDLER_H
#define HNSTRINGHANDLER_H

#include <e32base.h>

class CDesC16Array;
struct THnMdCommonPointers;

/**
 * String Handler.
 * 
 * Contains static methods for strings manipulation.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(HnStringHandler)
    {
public:
    /**
     * Localize given string
     * it is given in format 
     * @verbatim <namespace>:<id> @endverbatim 
     * namespace can be declared in XML
     * it also accepts format myapp.rsc:MY_RESOURCE
     * 
     * @param aDestination destiation where localized text is returned
     * @param aSource string to localize
     * @param aCmnPtrs common pointers, if not given, 
     *  taken from THnMdCommonPointers::Static()
     */
    static void LocaliseL(  RBuf& aDestination, RBuf& aSource,
                           THnMdCommonPointers* aCmnPtrs );
    /**
     * Localize given string
     * it is given in format 
     * @verbatim <namespace>:<id> @endverbatim
     * namespace can be declared in XML
     * it also accepts format myapp.rsc:MY_RESOURCE
     * It formats string based on parameters provided.
     * 
     * @param aDestination destiation where localized text is returned
     * @param aSource string to localize
     * @param aDesParams list descriptor parameters to format %U, %0U
     * @param aIntParams list integer parameters to format %N, %0N
     * @param aCmnPtrs common pointers, if not given, 
     *  taken from THnMdCommonPointers::Static()
     */
    static void LocaliseL( RBuf& aDestination, RBuf& aSource, 
                            CDesC16Array* aDesParams,
                            CArrayFix<TInt>* aIntParams,
                            THnMdCommonPointers* aCmnPtrs );
    
    /**
     * Compares strings ignoring character case.
     * 
     * @param aLeft Base descriptor.
     * @param aRight Descriptor to be compared with the base descriptor.
     */
    static TInt CompareIgnoreCaseL( const TDesC& aLeft, const TDesC& aRight );
    
    /**
     * Compares strings ignoring character case.
     * 
     * @param aLeft Base descriptor.
     * @param aRight Descriptor to be compared with the base descriptor.
     */
    static TInt CompareIgnoreCaseL( const TDesC8& aLeft, const TDesC8& aRight );
    };

#endif // HNSTRINGHANDLER_H
