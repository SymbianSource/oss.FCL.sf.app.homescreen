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


#ifndef HNHELPER_H
#define HNHELPER_H

#include <e32hashtab.h>

/**
 * Creates hash.
 * 
 * @param aBuf Descriptor.
 * @return Hash.
 */
inline TUint32 HBuf16Hash( HBufC16* const &  aBuf )
    {
    return DefaultHash::Des16(*aBuf);
    }
    
/**
 * Creates hash.
 * 
 * @param aBuf Descriptor.
 * @return Hash.
 */
inline TUint32 TDesC16Hash( const TDesC16* const &  aBuf )
    {
    return DefaultHash::Des16(*aBuf);
    }

/**
 * Checks for indentity.
 * 
 * @param aL Descriptor.
 * @param aR Descriptor.
 * @return Bool.
 */
inline TBool HBuf16Ident( HBufC16* const & aL, HBufC16* const & aR )
    {
    return DefaultIdentity::Des16(*aL, *aR);
    }

/**
 * Checks for indentity.
 * 
 * @param aL Descriptor.
 * @param aR Descriptor.
 * @return Bool.
 */
inline TBool TDesC16Ident( const TDesC16* const & aL, const TDesC16* const & aR )
    {
    return DefaultIdentity::Des16(*aL, *aR);
    }

/**
 * Destroys both key and value object of a hashmap.
 */
template <class TKey, class TValue>
void DestroyMapKeyAndValue( RHashMap< TKey*, TValue* > & aHashMap )
	{
	THashMapIter< TKey*, TValue* > iter( aHashMap );
	while ( iter.NextKey() )
		{
		delete *(iter.CurrentKey());
		delete *(iter.CurrentValue());
		}
	}

/**
 * Destroys just value object of a hashmap.
 */
template <class TKey, class TValue>
void DestroyMapValue( RHashMap< TKey*, TValue* > & aHashMap )
	{
	THashMapIter< TKey*, TValue* > iter( aHashMap );
	while ( iter.NextKey() )
		{
		delete *(iter.CurrentValue());
		}
	}

#endif /* HNHELPER_H */
