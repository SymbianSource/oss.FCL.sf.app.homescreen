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


#ifndef HNMDUINTEGERKEY_H_
#define HNMDUINTEGERKEY_H_

#include <e32base.h>
#include "hnmdbasekey.h"

/**
 * Defines particular key type which is unsigned integer.
 *
 *  @lib hierarchynavigatorengine
 *  @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdUIntegerKey ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUIntegerKey* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUIntegerKey* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUIntegerKey* NewL( const CHnMdUIntegerKey* aKey );

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUIntegerKey* NewLC( const CHnMdUIntegerKey* aKey );
    
    /**
     * Factory method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */
    virtual CHnMdBaseKey* CopyLC();
   
    /**
     * Standard destructor.
     */
    virtual ~CHnMdUIntegerKey();
    
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
    CHnMdUIntegerKey();

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
    void ConstructL( const CHnMdUIntegerKey* aKey );
    };

#endif // HNMDUINTEGERKEY_H_
