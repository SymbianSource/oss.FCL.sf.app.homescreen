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


#ifndef C_HNMDLISTKEY_H
#define C_HNMDLISTKEY_H

#include <liwcommon.h>
#include <e32base.h>

#include "hnmdbasekey.h"


/**
 * Defines particular key type which is LIW list.
 *
 * @lib hierarchynavigatorengine
 * @since S60 v5.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdListKey ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdListKey* NewL();

    /**
     * Standard factory method.
     * 
     * @return Fully constructed object.
     */
    static CHnMdListKey* NewLC();

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdListKey* NewL( const CHnMdListKey* aKey );

    /**
     * Standard factory method.
     * 
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdListKey* NewLC( const CHnMdListKey* aKey );
   
    /**
     * Standard destructor.
     */
    virtual ~CHnMdListKey();
    
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
     */
    virtual void ToVariantL( TLiwVariant& aRet ) const;
    
protected:

    /**
     * Standard constructor.
     * 
     * @since S60 v5.0
     */
    CHnMdListKey();

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
    void ConstructL( const CHnMdListKey* aKey );

// from base class CHnMdBaseKey

    /**
     * Evaluates the key value.
     * 
     * @since S60 v5.0
     * @param aParamList List of parameters.
     * @param aPos Position in the param list. Default is 0.
     */
    void EvaluateKeyValueL( const CLiwGenericParamList& aParamList,
                            TInt aPos );

private: // data
    
    /**
     * Own. Variant.
     */
    TLiwVariant iVariantContent;
    
    };

#endif // C_HNMDLISTKEY_H 
