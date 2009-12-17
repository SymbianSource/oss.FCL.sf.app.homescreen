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



#ifndef C_HNMDQUERIES_H
#define C_HNMDQUERIES_H

#include <e32base.h>
#include <liwcommon.h>

class CHnMdQuery;
class CLiwGenericParamList;

/**
 * Queries.
 *
 * This class stores CHnMdQuery objects.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdQueries) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdQueries* NewL( TXmlEngElement aElement );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdQueries* NewLC( TXmlEngElement aElement );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdQueries();

    /**
     * Adds query.
     *
     * @since S60 5.0
     * @param aQuery Query.
     * @return Status code.
     */
    TInt AddQuery( CHnMdQuery* aQuery );
        
    /**
     * Gets query.
     *
     * @since S60 5.0
     * @param aPosition Position of a query.
     * @return Query.
     */
    CHnMdQuery& Query( TInt aPosition ) const;
    
    /**
     * Gets number of queries.
     *
     * @since S60 5.0
     * @return Number of queries.
     */
    TInt Count() const;
    
private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdQueries();

    /**
     * Standard symbian 2nd phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );

private: // data

    /**
     * Own - Queries.
     */
    RPointerArray<CHnMdQuery> iQueries;

    };

#endif // C_HNMMQUERIES_H
