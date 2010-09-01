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



#ifndef C_HNMDMAPKEY_H
#define C_HNMDMAPKEY_H

#include <liwcommon.h>
#include <e32base.h>

#include "hnmdbasekey.h"

/**
 *  Defines particular key type which is LIW map.
 *
 *  @lib hierarchynavigatorengine
 *  @since S60 v5.0
 *  @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdMapKey ) : public CHnMdBaseKey
    {
    friend class HnMdKeyFactory;

public:
    
    /**
     * Standard factory method.
     * 
     * @since S60 v5.0
     * @return Fully constructed object.
     */
    static CHnMdMapKey* NewL();

    /**
     * Standard factory method.
     * 
     * @since S60 v5.0
     * @return Fully constructed object.
     */
    static CHnMdMapKey* NewLC();

    /**
     * Standard factory method.
     * 
     * @since S60 v5.0
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdMapKey* NewL( const CHnMdMapKey* aKey );

    /**
     * Standard factory method.
     * 
     * @since S60 v5.0
     * @param aKey Reference key.
     * @return Fully constructed object.
     */
    static CHnMdMapKey* NewLC( const CHnMdMapKey* aKey );
    
    /**
     * Factory method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */
    virtual CHnMdBaseKey* CopyLC();
   
    /**
     * Standard destructor.
     * 
     * @since S60 v5.0
     */
    virtual ~CHnMdMapKey();
    
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
    CHnMdMapKey();

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
    void ConstructL( const CHnMdMapKey* aKey );
    
    /**
     * Evaluates the key value.
     * 
     * @since S60 v5.0
     * @param aParamList List of parameters.
     * @param aPos Position in the param list. Default is 0.
     */
    void EvaluateKeyValueL( const CLiwGenericParamList& aParamList,
            TInt aPos );
       
private:
    
    /**
     * Variant.
     */
    TLiwVariant iVariantContent;
    
    };

#endif // C_HNMDMAPKEY_H 
