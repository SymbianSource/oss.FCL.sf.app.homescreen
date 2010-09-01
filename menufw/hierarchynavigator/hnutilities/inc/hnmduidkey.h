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


#ifndef C_HNMDUIDKEY_H
#define C_HNMDUIDKEY_H

#include <e32base.h>

#include "hnmdbasekey.h"

/**
 * Defines particular key type which is UID.
 *
 *  @lib hierarchynavigatorengine
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHnMdUidKey ) : public CHnMdBaseKey
    {
public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUidKey* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUidKey* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUidKey* NewL( const CHnMdUidKey* aKey );

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUidKey* NewLC( const CHnMdUidKey* aKey );
    
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
    virtual ~CHnMdUidKey();

    /**
     * Gets key content.
     *
     * @since S60 v5.0
     * @param aContent Output content.
     */
    void GetKeyContent( TInt& aContent ) const;
    
    /**
     * Change to variant.
     * 
     * @since S60 v5.0
     * @param aRet Output variant.
     * 
     */
    virtual void ToVariantL( TLiwVariant& aRet ) const;
    
protected:
    friend class HnMdKeyFactory;  

    /**
     * Standard constructor.
     * 
     * @since S60 v5.0
     */
    CHnMdUidKey();

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
    void ConstructL( const CHnMdUidKey* aKey );
    };

#endif // C_HNMDUIDKEY_H 
