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


#ifndef HNLIWUTILS_H
#define HNLIWUTILS_H

#include <e32base.h>
#include <e32hashtab.h>
#include <liwcommon.h>


class CHnMdBaseKey;
class CLiwGenericParamList;
class TLiwVariant;

/**
 *  Provides static methods which handles operations
 *  like extracting data from and inserting data to 
 *  LIW composite classes ( CLiwGenericParamList, TLiwVariant ).   
 * 
 *  @lib hnutilities.lib
 *  @since S60 v5.0
 *  @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( HnLiwUtils )
    {   
public:
    /**
     * Appends key-value pairs stored in aKeys to aInParam.
     * Keys are appended in the same order as they occur in the aKeys array.
     * Every key value is converted to a TLiwVariant before appending to the
     * param list.
     * 
     * @since S60 v5.0
     * @param aKeys Keys to append.
     * @param aInParam Param list to be modified.
     */    
    IMPORT_C static void SetGenericParamListL(
        const RPointerArray<CHnMdBaseKey> & aKeys,
        CLiwGenericParamList& aInParam );

    /**
     * Creates a string representing a selected value from the param list.
     * The aPath paramter is the path to the selected param list element.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @param aBuf Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetStringL(
        const CLiwGenericParamList& aParam, 
        const TDesC8& aPath, 
        RBuf& aBuf );

    /**
     * Creates a string representing a selected value from the param list.
     * The aPath paramter is the path to the selected param list element.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @param aBuf Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetStringL(
        const CLiwGenericParamList& aParam, 
        const TDesC8& aPath, 
        RBuf8& aBuf );
    
    /**
     * Returns TInt64 value using the given path.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @param aPos Position.
     * @param aRet Return TInt64 value.
     * @return TInt64 value or KErrNotFound.
     */
    IMPORT_C static TInt GetInt64L( const CLiwGenericParamList& aParam,
    	    const TDesC8& aPath, TInt aPos, TInt64& aRet );
    
    /**
     * Creates a string representing a selected value from the param list.
     * The aPath paramter is the path to the selected param list element.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @param aPos Position.
     * @param aBuf Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetStringL(
        const CLiwGenericParamList& aParam, 
        const TDesC8& aPath, 
        TInt aPos,
        RBuf& aBuf );

    /**
     * Creates a string representing a selected value from the param list.
     * The aPath paramter is the path to the selected param list element.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI Path.
     * @param aPos Position.
     * @param aBuf Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetStringL(
        const CLiwGenericParamList& aParam, 
        const TDesC8& aPath, 
        TInt aPos,
        RBuf8& aBuf );
    
    /**
     *  Exctracts variant from param list.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI path.
     * @param aPos Position.
     * @param aRet Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetVariantL(
        const CLiwGenericParamList& aParam,
        const TDesC8& aPath,
        TInt aPos,
        TLiwVariant& aRet );

    /**
     *  Exctracts variant from param list.
     * 
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aPath URI path.
     * @param aRet Output buffer.
     * @return Error code.
     */
    IMPORT_C static TInt GetVariantL(
        const CLiwGenericParamList& aParam,
        const TDesC8& aPath,
        TLiwVariant& aRet );
    
    /**
     * Creates a string representation of a variant value.
     * Supported variant types are: EVariantTypeDesC, 
     * EVariantTypeDesC8, EVariantTypeTUid, EVariantTypeTInt32,
     * and EVariantTypeTUint.
     * 
     * @since S60 v5.0
     * @param aVariant Source variant.
     * @param aBuf Output buffer.
     * @return EFalse if variant type is not supported.
     */
    IMPORT_C static TBool VariantToStringL( TLiwVariant& aVariant,
                                           RBuf& aBuf );
    
    /**
     * Creates a string representation of a variant value.
     * Supported variant types are: EVariantTypeDesC, 
     * EVariantTypeDesC8, EVariantTypeTUid, EVariantTypeTInt32,
     * and EVariantTypeTUint.
     * 
     * @since S60 v5.0
     * @param aVariant Source variant.
     * @param aBuf Output buffer.
     * @return EFalse if variant type is not supported.
     */
    IMPORT_C static TBool VariantToStringL( TLiwVariant& aVariant,
                                           RBuf8& aBuf );

    /**
     * Gets total number of items held by an iterable.
     * 
     * @param aIterable the iterable
     * @return total number of items stored in aIterable
     */
    IMPORT_C static TInt GetIterableItemCountL( CLiwIterable& aIterable );

private:
    
    /**
     * Exctracts namespace from param list.
     *
     * @since S60 v5.0
     * @param aParam Param list.
     * @param aNameSpace Namespace.
     * @param aRet Output variant.
     * @return Error code.
     */
    static TInt ExtractNameSpaceL( const CLiwGenericParamList& aParam,
                                   const TDesC8& aNameSpace,
                                   TLiwVariant& aRet );
    
    /**
     * Process path for selecting elements separated by colon or slash.
     *
     * @since S60 v5.0
     * @param aPath Path.
     * @param aPathParts Elements of the path.
     */
    static void ParsePathL( const TDesC8& aPath, 
                           RArray< TPtrC8 >& aPathParts );

    /**
     * Replaces the first occurence of the string "$index" with a number.
     * On successful execution the string contained in aPath8 will have
     * have the first occurence of the substring "$index" replaced by
     * a string representation of the number given in aPos.
     *  
     * @since S60 v5.0
     * @param aPath8 A path to the variant in the generic param list.
     * @param aPos A position of the item in the group of items.
     * @return Length of the new path, ending at the index object.
     */
    static TInt ReplaceIndexL( RBuf8& aPath8, TInt aPos );
        
    /**
     * Extracts a number enclosed in square brackets from a string.
     * The number is assumed to be non-negative (otherwise it would be
     * impossible to determine if the function executed successfully).
     * If the string contains more than one pair of square brackets, only
     * the first pair is processed by this method.
     * 
     * @since S60 v5.0
     * @param aPosition String containing the position number.
     * @return Number extracted from string or error code on failure.
     */
    static TInt GetPosition( const TDesC8& aPosition );

    /**
     * Uses iterator object to extract variant pointed by position. 
     *
     * @since S60 v5.0
     * @param aIterable Iterator to use.
     * @param aPos Position of the variant.
     * @param aVariant Output variant.
     * @return ETrue if variant found.
     */
    static TBool GetIterableByPositionL(
        CLiwIterable & aIterable,
        TInt aPos,
        TLiwVariant & aVariant );
     
     };

#endif // HNLIWUTILS_H

