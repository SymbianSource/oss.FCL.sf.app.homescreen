/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Encapsulates hsps liw service
 *
 */

#ifndef CPSWRAPPER_H
#define CPSWRAPPER_H

// System includes
#include <e32base.h>
#include <liwcommon.h>      // for MLiwNotifyCallback

// Forward declarations
class CLiwServiceHandler;
class CLiwCriteriaItem;
class MLiwInterface;

namespace cpswrapper
    {
    /**
     * Content publishing service wrapper namespace
     */

    // Forward declarations
    class CPublisherMap;
    class CCpsObserver;
   
    /**
     *  Observer class for cpswrapper
     *
     *  @code
     *
     *  @endcode
     *
     *  @lib cpswrapper.lib
     *  @since S60 v5.0
     */
    class MCpsWrapperObserver
        {
    public:
        /**
         * Callback methot for unregistered widget notify.
         *
         * @param aPublisher a Publisher
         */
        virtual void NotifyWidgetUnregisteredL(const TDesC& aPublisher) = 0;
        
        /**
         * Callback methot for registered widget notify.
         */
        virtual void NotifyWidgetRegisteredL() = 0;
        };

    /**
     *  Wrapper for cps liw service
     *
     *
     *  @code
     *
     *  @endcode
     *
     *  @lib cpswrapper.lib
     *  @since S60 v5.0
     */
    NONSHARABLE_CLASS( CCpsWrapper ) : public CBase
        {
    public:
        /**
         * Two-phased constructor.
         * @param aObserver Observer
         */
        IMPORT_C static CCpsWrapper* NewL( MCpsWrapperObserver& aObserver );

        /**
         * Two-phased constructor.
         * @param aObserver Observer
         */
        IMPORT_C static CCpsWrapper* NewLC( MCpsWrapperObserver& aObserver );

        /**
         * Destructor.
         */
        IMPORT_C ~CCpsWrapper();

    protected:
        /**
         * Constructor
         * @param aObserver Observer
         */
        CCpsWrapper( MCpsWrapperObserver& aObserver );

        /** 
         * Second phase constructor 
         */
        void ConstructL();

    public:
        /**
         * Gets templated publishers
         *
         * @return map of publisher infos. Ownership is transfered.
         */
        IMPORT_C CPublisherMap* GetTemplatedPublishersL();
        
        /**
		 * Handles changed publisher. Deleted or added. 
		 * 
		 * @param aPublisher publisher
		 * @return 
		 */
        void HandleChangedPublisherL(const TDesC& aPublisher, 
                const TDesC& aOperation);

    private:
        // new functions

        /**
         * Register for notification from publisher registry
         */
        void StartListeningPublisherRegistryL();
        
        /**
		 * Get maps of WRT widget publishers info. 
		 * 
		 * @param aPublisherMap a map of publishers info.
		 */
        void GetWRTWidgetsL(CPublisherMap& aPublisherMap);

        /**
		 * Register an observer for request notification  
		 * 
		 * @param aPublisher  publisher name
		 * @param aContentType content type
		 * @param aContentId content id
		 * @param aOperation operation (add,delete,update,execute)
		 * @param aRegistry registry type (publisher/cp_data)
		 */
        void RequestForNotificationL(
        		const TDesC& aPublisher, 
        		const TDesC& aContentType,
        		const TDesC& aContentId,
        		const TDesC8& aOperation,
        		const TDesC& aRegistry);
        
    private: // Data
        /**
         * Wrapper observer.
         * Not owned
         */
        MCpsWrapperObserver& iObserver;

        /**
         * SAPI service handler.
         * Owned.
         */
        CLiwServiceHandler* iServiceHandler;

        /**
         * CPS SAPI service.
         * Owned.
         */
        CLiwCriteriaItem* iCpsService;

        /**
         * Provides hsps services.
         * Owned.
         */
        MLiwInterface* iCpsInterface;
        
        /**
	     * Registered cps observers.
	     * Owned.
	     */
        RPointerArray<CCpsObserver> iObservers;
        };
    
    }

#endif // CPSWRAPPER_H
// End of file
