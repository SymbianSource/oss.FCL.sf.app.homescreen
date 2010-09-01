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



#ifndef C_HNMDQUERY_H
#define C_HNMDQUERY_H

#include <e32base.h>
#include <xmlengelement.h>

#include "hnglobals.h"

class CHnMdServiceCommand;
class CHnMdBaseKey;
class CLiwGenericParamList;
class CHnMdService;

/**
 * Query.
 * 
 * This class represents the query.
 * 
 * This class allows sending queries to different services.
 * 
 * Queries are mostly used during evaluation.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0 *** for example, S60 v3.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdQuery) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully cosntructed object.
     */
    static CHnMdQuery* NewL( TXmlEngElement aElement );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully cosntructed object.
     */
    static CHnMdQuery* NewLC( TXmlEngElement aElement );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdQuery();

    /**
     * Gets namespace.
     *
     * @since S60 5.0
     * @return Namespace.
     */
    const TDesC& NameSpace() const;
    
    /**
     * Gets namespace.
     *
     * @since S60 5.0
     * @return Service object.
     */
    CHnMdService& GetService() const;
    
private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdQuery();

    /**
     * Standard symbian 2nd phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void ConstructL(TXmlEngElement aElement);
    
     /**
     * Sets namespace.
     *
     * @since S60 5.0
     * @param aNameSpace Namespace descriptor.
     */
    void SetNameSpaceL( const TDesC& aNameSpace );

    /**
     * Sets namespace.
     *
     * @since S60 5.0
     * @param aAttr Xml attribute.
     */
    void SetNameSpaceL( TXmlEngAttr aAttr );
    
private: // data

    /**
     * Own - Namespace.
     */
    RBuf iNameSpace;

    /**
     * Own - service object.
     */
    CHnMdService* iService;
       
    };

#endif // C_HNMMQUERY_H
