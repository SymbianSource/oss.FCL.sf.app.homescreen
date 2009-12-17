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



#ifndef C_HNMDINTEGER64KEY_H
#define C_HNMDINTEGER64KEY_H

#include <e32base.h>

#include "hnmdbasekey.h"

/**
 * Defines particular key type which is 64-bit integer.
 *
 * @lib hierarchynavigatorengine
 * @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdInteger64Key ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdInteger64Key* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdInteger64Key* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdInteger64Key* NewL( const CHnMdInteger64Key* aKey );

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdInteger64Key* NewLC( const CHnMdInteger64Key* aKey );
   
    /**
     * Standard destructor.
     */
    virtual ~CHnMdInteger64Key();
    
// from base class CHnMdBaseKey
    
    /**
     * Factory method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */
    virtual CHnMdBaseKey* CopyLC();
    
    /**
     * Gets key content.
     *
     * @since S60 v5.0
     * @param aContent Output content.
     */
    void GetKeyContent( TInt64& aContent ) const;

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
    CHnMdInteger64Key();

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
    void ConstructL( const CHnMdInteger64Key* aKey );
    };

#endif // C_HNMDINTEGER64KEY_H 
