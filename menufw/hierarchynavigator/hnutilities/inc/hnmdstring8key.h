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



#ifndef C_HNMDSTRING8KEY_H
#define C_HNMDSTRING8KEY_H

#include <e32base.h>

#include "hnmdbasekey.h"

/**
 *  Defines particular key type which is narrow string.
 *
 *  @lib hierarchynavigatorengine
 *  @since S60 v5.0
 *  @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdString8Key ) : public CHnMdBaseKey
    {
public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdString8Key* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdString8Key* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdString8Key* NewL( const CHnMdString8Key* aKey );

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdString8Key* NewLC( const CHnMdString8Key* aKey );
    
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
    virtual ~CHnMdString8Key();
    
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
    CHnMdString8Key();

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
    void ConstructL( const CHnMdString8Key* aKey );
    };

#endif // C_HNMDSTRING8KEY_H 
