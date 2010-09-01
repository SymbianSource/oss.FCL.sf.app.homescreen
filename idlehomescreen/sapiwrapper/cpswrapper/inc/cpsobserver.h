/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines an observer to CPS
*
*/

#ifndef CPSOBSERVER_H
#define CPSOBSERVER_H

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
    class CCpsWrapper;
    
    /**
	 *  Observer class for cps registry 
	 *
	 *  @code
	 *
	 *  @endcode
	 *
	 *  @lib cpswrapper.lib
	 *  @since S60 v5.0
	 */
    NONSHARABLE_CLASS( CCpsObserver ) : public CBase, public MLiwNotifyCallback
		{
		public: // Constructors and destructor
			/**
			 * Two-phased constructor.
			 */
			static CCpsObserver* NewLC( CCpsWrapper* aCpsWrapper);
			
			/**
			 * Destructor.
			 */
			~CCpsObserver();
	
		public:  //from MLiwNotifyCallback
			
			/**
			* Handles notifications caused by an asynchronous Execute*CmdL call
			* or an event.
			*
			* @param aCmdId The service command associated to the event.
			* @param aEventId Occured event, see LiwCommon.hrh.
			* @param aEventParamList Event parameters, if any, as defined per
			*        each event.
			* @param aInParamList Input parameters, if any, given in the
			*        related HandleCommmandL.
			* @return Error code for the callback.
			*/
			TInt HandleNotifyL(
				TInt aCmdId,
				TInt aEventId,
				CLiwGenericParamList& aEventParamList,
				const CLiwGenericParamList& aInParamList);
		
		public:
			
			/**
			* Registers to CPS for add, delete , update and execute notifications
			* 
			* @param aFilter - filter for input parameter list
			* @param aRegistry - registry to register observer
			*/
			void RegisterL( CLiwDefaultMap* aFilter,  const TDesC& aRegistry  );
			
			/**
			* Cancel all the registered notifications.
			*/
			void ReleaseL();
	
		private:
			
			/**
			* Perform the second phase construction 
			*/      
			void ConstructL( );
			
			/**
			* Constructor.
			* 
			* @param aCpsWrapper cps wrapper 
			*/
			CCpsObserver( CCpsWrapper* aCpsWrapper );
			
			/**
			* Initialise Liw connection to Content publishing service.
			*/
			void InitLiwL();
			
			/**
			* Closes Liw connection to Content publishing service.
			*/
			void ReleaseLiw();
			
		private: // data
				
			// Reference of MLiwInterface
			// Owned
			MLiwInterface* iInterface;
			
			// Reference of CLiwServiceHandler
			// Owned
			CLiwServiceHandler* iLiwHandler;	            
					
			// Reference of the CCpsWrapper
			// Not owned
			CCpsWrapper* iCpsWrapper;
			
			// Call back error code
			TInt iError;
		};
    }
#endif /* CPSOBSERVER_H_ */
