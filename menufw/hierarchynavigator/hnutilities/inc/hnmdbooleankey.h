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



#ifndef C_HNMDBOOLEANKEY_H
#define C_HNMDBOOLEANKEY_H

#include <e32base.h>

#include "hnmdbasekey.h"

/**
 * Defines particular key type which is bool.
 *
 * @lib hierarchynavigatorengine
 * @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdBooleanKey ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdBooleanKey* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdBooleanKey* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdBooleanKey* NewL( const CHnMdBooleanKey* aKey );
    
    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdBooleanKey* NewLC( const CHnMdBooleanKey* aKey );
   
    /**
     * Standard destructor.
     */
    virtual ~CHnMdBooleanKey();
    
    /**
     * Gets key boolean value.
     *
     * @since S60 v5.0
     * @return Boolean value represented by the key.
     */
    TBool GetBooleanValueL() const;//Only in CHnMdBooleanKey

// from base class CHnMdBaseKey
    
    /**
     * Factory method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */
    virtual CHnMdBaseKey* CopyLC();

    /**
     * Change to variant.
     * 
     * @since S60 v5.0
     * @param aRet Output variant.
     * 
     */
    virtual void ToVariantL( TLiwVariant& aRet ) const;
           
    
protected:

    /**
     * Standard constructor.
     * 
     * @since S60 v5.0
     */
    CHnMdBooleanKey();

    /**
     * Symbian constructor for performing 2nd stage construction.
     * 
     * @since S60 v5.0
     */
    void ConstructL();

    /**
     * Symbian constructor for performing 2nd stage construction.
     * 
     * @since S60 v5.0
     * @param aKey Reference key.
     */
    void ConstructL( const CHnMdBooleanKey* aKey );
    
    };

#endif // C_HNMDBOOLEANKEY_H 
