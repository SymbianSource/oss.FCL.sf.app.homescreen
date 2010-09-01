/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
* ==============================================================================
*  Name        : contentpublisher.h
*  Part of     : 
*  Interface   : Implemetation of CContentPublisher
*  Description :
*  Version     :

*/

#ifndef CONTENTPUBLISHER_H
#define CONTENTPUBLISHER_H

#include <e32base.h>
#include <liwcommon.h>

// FORWARD DECLARATIONS
class CLiwGenericParamList;
class CLiwServiceHandler;
class MLiwInterface;

namespace AiXmlUiController
{
class CXmlUiController;

/**
 *  @ingroup group_xmluicontroller
 * 
 *  Interface to publish data to Content Publishing Service
 * 
 *  @since S60 5.1
 */
class CContentPublisher : public CBase, public MLiwNotifyCallback
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CContentPublisher* NewL(CXmlUiController& aUiController);

        /**
        * Destructor.
        */
        virtual ~CContentPublisher();
        
    private:
        CContentPublisher(CXmlUiController& aUiController);
        void ConstructL();

    public:  //from MLiwNotifyCallbackc
        
        /**
        * Handles notifications caused by an asynchronous Execute*CmdL call
        * or an event.
        *
        * @param aCmdId The service command associated to the event.
        * @param aEventId occurred event, see LiwCommon.hrh.
        * @param aEventParamList Event parameters, if any, as defined per
        *        each event.
        * @param aInParamList Input parameters, if any, given in the
        *        related HandleCommmandL.
        * @return Error code for the call back.
        */
        virtual TInt HandleNotifyL(
            TInt aCmdId,
            TInt /*aEventId*/,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& /*aInParamList*/);
        
    public: // New functions
    	
		/**
		* Sets given values to Content Publisher Service.
		* @since Series 60 5.1
		* @param aElementName The name of the element, which values are set.
		* @param aWidth Width of the element
		* @param aHeight Height of the element
		*/    
		void PublishSizeL( const TDesC& aElementName,
				TInt aWidth, TInt aHeight );
    
    private:
    	
        void InitCpsInterfaceL();
        void AddDataL( const TDesC& aPublisherId,
                const TDesC& aContentType, const TDesC& aContentId,
                const TInt aData1, const TDesC8& aDataKey1,
                const TInt aData2, const TDesC8& aDataKey2 );
        
        void GetDataL( const TDesC& aPublisherId, const TDesC& aContentType, 
        		const TDesC& aContentId, CLiwDefaultMap* aOutDataMap);
        
        void HandlePublisherInfoL( const TDesC& aPubInfo);
        void RegisterListenerL();
        void UnRegisterListenerL();
        
    private:
		MLiwInterface* iCpsInterface;
		CLiwServiceHandler* iServiceHandler;
		
	    /**
	     * Reference to XML ui controller.
	     */
	    CXmlUiController& iUiController;
    };

}// namespace AiXmlUiController
#endif //CONTENTPUBLISHER_H





