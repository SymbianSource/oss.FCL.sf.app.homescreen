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


#ifndef HNMDREQUESTNOTIFY_
#define HNMDREQUESTNOTIFY_

#include <e32base.h>
#include <xmlengelement.h> 
#include <liwcommon.h>

#include "hnmdbasekey.h"
#include "hnmdservice.h"
#include "hnitemid.h"


class CHnServiceHandler;
class CHnMdServiceCommand;
struct THnMdCommonPointers;

/**
 * Notify Request.
 * 
 * This class is responsible for managing notifications coming from
 * services and refreshing content of the view.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnMdNotifyRequest ):
    public CBase,
    public MLiwNotifyCallback
    {
public:

    /**
     * Two-phased constructor.
     * Constructs an object from xml element.
     * 
     * @since S60 5.0
     * @param aElement A XML element holding information.
     * @param aItemId Item id.
     * @param aCmnPtrs Common pointers structure.
     * @return Fully constructed notify request.
     */
     static CHnMdNotifyRequest* NewL( TXmlEngElement aElement, 
             const CHnItemId & aItemId,
             THnMdCommonPointers* aCmnPtrs );

     /**
      * Two-phased constructor.
      * Constructs an object from xml element.
      * 
      * @since S60 5.0
      * @param aElement A XML element holding information.
      * @param aItemId Item id.
      * @param aCmnPtrs Common pointers structure.
      * @return Fully constructed notify request.
      */
     static CHnMdNotifyRequest* NewLC( TXmlEngElement aElement,
             const CHnItemId & aItemId,
             THnMdCommonPointers* aCmnPtrs );
     
     /**
      * Virtual C++ destructor.
      */
     virtual ~CHnMdNotifyRequest();
     
     /**
      * Requests notification from the service, which was set up
      * in the configuration.
      * 
      * @param aParams Parameters list.
      * @param aCmdOptions Command options.
      */
     void RequestNotifyL( CLiwGenericParamList& aParams,
             TInt aCmdOptions = 0 );
     
     /**
      * Method is invoked when the notifucation occurs.
      * It is called as a callback by the service which want to
      * notify change.
      * 
      * @see MLiwNotifyCallback::HandleNotifyL
      * @param aCmdId Command id.
      * @param aEventId Event id.
      * @param aEventParamList Event parameters list.
      * @param aInParamList Input parameters list.
      *
      * @return error code
      */
     TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
             CLiwGenericParamList& aEventParamList,
             const CLiwGenericParamList& aInParamList );

  
private:

    /**
     * Gets command object.
     *
     * @since S60 5.0
     * @return Service command.
     */
    const CHnMdServiceCommand& GetService() const;
    
    /**
     * Second phase constructor.
     * @param aElement A XML element holding information. 
     * @param aItemId Item's id.
     */
    void ConstructL( TXmlEngElement aElement, 
            const CHnItemId & aItemId );
    
    /**
     * Standard C++ constructor.
     * @param aCmnPtrs Common pointers structure.
     */
    CHnMdNotifyRequest(THnMdCommonPointers* aCmnPtrs);
           
private:
    
    /**
     * Own - Associated item id.
     */
    CHnItemId* iItemId;
    
    /**
     * Not own - Common pointers.
     */
    THnMdCommonPointers* iCmnPtrs;
   
    /**
     * Own - Service Handler
     */
    CHnMdService* iService; 
    
    /**
     * Own - Service Handler
     */
    CHnServiceHandler* iSh; 
    
    };

#endif // HNMDREQUESTNOTIFY_
