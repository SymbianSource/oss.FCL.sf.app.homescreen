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
* Description:  represents the notification requests
*
*/



#ifndef C_HNMDREQUESTS_H
#define C_HNMDREQUESTS_H

#include <e32base.h>
#include <xmlengelement.h> 

class CHnItemId;
class CHnMdNotifyRequest;
struct THnMdCommonPointers;

/**
 * Notify Requests.
 * 
 * This class stores CHnMdNotifyRequest objects.
 * 
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdNotifyRequests) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Element.
     * @param aItemId Item's identifier.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdNotifyRequests* NewL( TXmlEngElement aElement, 
            const CHnItemId & aItemId,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Element.
     * @param aItemId Item's identifier.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdNotifyRequests* NewLC( TXmlEngElement aElement,
            const CHnItemId & aItemId,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdNotifyRequests();

    /**
     * Adds request.
     *
     * @since S60 5.0
     * @param aRequests Request parameters.
     * @return Status code.
     */
    TInt AddNotifyRequest( CHnMdNotifyRequest* aRequests );
    
    /**
     * Gets request.
     *
     * @since S60 5.0
     * @param aPosition Position of a query.
     * @return Query.
     */
    CHnMdNotifyRequest& NotifyRequest( TInt aPosition ) const;
    
    /**
     * Gets number of requests.
     *
     * @since S60 5.0
     * @return Number of queries.
     */
    TInt Count() const;
    
    /**
     * Requests notifucation from all services.
     *
     * @since S60 5.0
     * @param aParams Parameters in a form of a CLiwGenericParamList object.
     * @param aCmdOptions Command options.
     */
    void SendAllRequestsL( CLiwGenericParamList& aParams,
            TInt aCmdOptions = 0);
    
    /**
     * Check if requests were sent.
     * 
     * @return Returns true if requests were sent.
     */
    TBool Sent();


private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdNotifyRequests();

    /**
     * Standard symbian 2nd phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aItemId Item's id.
     * @param aCmnPtrs Common pointers container.
     */
    void ConstructL(  TXmlEngElement aElement, 
            const CHnItemId & aItemId,
            THnMdCommonPointers* aCmnPtrs );

private: // data

    /**
     * Own - Requests.
     */
    RPointerArray<CHnMdNotifyRequest> iNotifyRequests;
    
    
    /**
     * Indicates if requests were sent.
     */
    TBool iSent;

    };

#endif // C_HNMDREQUESTS_H
