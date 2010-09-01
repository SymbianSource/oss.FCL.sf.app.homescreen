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



#ifndef C_HNMDUNDEFKEY_H
#define C_HNMDUNDEFKEY_H

#include <e32base.h>

#include "hnmdbasekey.h"

/**
 * Defines key which has undefined type.
 *
 * @lib hierarchynavigatorengine
 * @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdUndefKey ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory function.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUndefKey* NewL();

    /**
     * Standard factory function.
     * 
     * @return Fully constructed object.
     */
    static CHnMdUndefKey* NewLC();
    
    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUndefKey* NewL( const CHnMdUndefKey* aKey );    

    /**
     * Instance copy method.
     *
     * since S60 v5.0
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdUndefKey* NewLC( const CHnMdUndefKey* aKey );    

    /**
     * Instance copy method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */  
    virtual CHnMdBaseKey* CopyLC();
    
    /**
     * Standard destructor.
     */
    virtual ~CHnMdUndefKey();
        
protected:

    /**
     * Standard constructor.
     * 
     * @since S60 v5.0
     */
    CHnMdUndefKey();
        
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
    void ConstructL( const CHnMdUndefKey* aKey );
    };

#endif // C_HNMDUNDEFKEY_H 
