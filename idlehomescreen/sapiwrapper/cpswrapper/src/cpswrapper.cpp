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
 * Description:  Encapsulates cps liw service
 *
 */

// System include files
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <bamdesca.h>

// User include files
#include "publisherinfo.h"
#include "publishermap.h"
#include "cpswrapper.h"
#include "cpsconst.h"
#include "cpsobserver.h"

namespace cpswrapper
    {

    // ======== MEMBER FUNCTIONS ========

    // -----------------------------------------------------------------------
    // CCpsWrapper::NewL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CCpsWrapper* CCpsWrapper::NewL( MCpsWrapperObserver& aObserver )
        {
        CCpsWrapper* self = CCpsWrapper::NewLC( aObserver );
        CleanupStack::Pop( self );
        return self;
        }

    // -----------------------------------------------------------------------
    // CCpsWrapper::NewLC()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CCpsWrapper* CCpsWrapper::NewLC(MCpsWrapperObserver& aObserver)
        {
        CCpsWrapper* self = new ( ELeave ) CCpsWrapper( aObserver );
        CleanupStack::PushL( self );
        self->ConstructL();
        return self;
        }

    // -----------------------------------------------------------------------
    // CCpsWrapper::ConstructL()
    // -----------------------------------------------------------------------
    //
    void CCpsWrapper::ConstructL()
        {
        iServiceHandler = CLiwServiceHandler::NewL();

        RCriteriaArray interestList;

        // Attach to CPS:
        iCpsService = CLiwCriteriaItem::NewL( 1, KCPSConfigurationIf, KCPS );
        iCpsService->SetServiceClass( TUid::Uid( KLiwClassBase ) );

        interestList.AppendL( iCpsService );
        iServiceHandler->AttachL( interestList );
        interestList.Reset();

        CLiwGenericParamList& inParamList( iServiceHandler->InParamListL() );
        CLiwGenericParamList& outParamList( iServiceHandler->OutParamListL() );

        iServiceHandler->ExecuteServiceCmdL(
                *iCpsService,
                inParamList,
                outParamList );

        TInt pos( 0 );

        outParamList.FindFirst( pos, KCPSConfigurationIf );

        if ( pos != KErrNotFound )
            {
            iCpsInterface = (outParamList)[pos].Value().AsInterface();
            inParamList.Reset();
            outParamList.Reset();
            }
        else
            {
            inParamList.Reset();
            outParamList.Reset();
            User::Leave( KErrNotFound );
            }

        StartListeningPublisherRegistryL();
        }

    // -----------------------------------------------------------------------
    // CCpsWrapper::CCpsWrapper()
    // -----------------------------------------------------------------------
    //
    CCpsWrapper::CCpsWrapper( MCpsWrapperObserver& aObserver )
        : iObserver( aObserver ),
        iCpsInterface( NULL )
        {
        }

    // -----------------------------------------------------------------------
    // CCpsWrapper::~CCpsWrapper()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CCpsWrapper::~CCpsWrapper()
        {
        iObservers.ResetAndDestroy();
        if ( iCpsInterface )
            {
            // Close interface
            iCpsInterface->Close();
            }

        if ( iServiceHandler && iCpsService )
            {
            // Detach services from the handler
            RCriteriaArray interestList;

            TRAP_IGNORE( interestList.AppendL( iCpsService );
                         iServiceHandler->DetachL( interestList ); );

            interestList.Reset();
            }

        delete iCpsService;
        delete iServiceHandler;
        }

    // -----------------------------------------------------------------------
    // CCpsWrapper::GetTemplatedPublishersL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherMap* CCpsWrapper::GetTemplatedPublishersL()
        {
        CLiwGenericParamList& inParamList( iServiceHandler->InParamListL() );
        CLiwGenericParamList& outParamList( iServiceHandler->OutParamListL() );

        TLiwGenericParam type( KType, TLiwVariant( KPublisher16 ) );
        inParamList.AppendL( type );

        CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();

        filter->InsertL( KContentType, TLiwVariant( KHSTemplate ) );

        //append filter to input param
        TLiwGenericParam item( KFilter, TLiwVariant( filter ));
        inParamList.AppendL( item );

        // Get all the publiser from CPS with publiser id ai3templatewidget
        iCpsInterface->ExecuteCmdL( KGetList, inParamList, outParamList );

        inParamList.Reset();

        CleanupStack::PopAndDestroy(filter);

        TInt pos(0);

        outParamList.FindFirst( pos, KResults );

        CPublisherMap* publishers = CPublisherMap::NewL();
        CleanupStack::PushL( publishers );
        
        if ( pos != KErrNotFound )
            {
            //extract iterator on results list
            TLiwVariant variant( outParamList[pos].Value() );
            CLiwIterable* iterable( variant.AsIterable() );
            iterable->Reset();

            //get all the templated widget publishers
            while ( iterable->NextL( variant ) )
                {
                //extract content map
                CLiwDefaultMap *publisherMap = CLiwDefaultMap::NewLC();
                variant.Get( *publisherMap );
                variant.Reset();
                CPublisherInfo* publisherInfo = CPublisherInfo::NewLC();
                if ( publisherMap->FindL( KPublisher, variant ) )
                    {
                    publisherInfo->SetPublisherIdL( variant.AsDes() );
                    variant.Reset();
                    }

                if ( publisherMap->FindL( KDataMap, variant) )
                    {
                    CLiwDefaultMap* dataMap = CLiwDefaultMap::NewLC();
                    variant.Get( *dataMap );
                    variant.Reset();
                    if ( dataMap->FindL( KTemplateType, variant) )
                        {
                        publisherInfo->SetTemplateTypeL( variant.AsDes() );
                        variant.Reset();
                        }

                    if ( dataMap->FindL( KWidgetName, variant ) )
                        {
                        publisherInfo->SetWidgetNameL( variant.AsDes() );
                        variant.Reset();
                        }

                    if ( dataMap->FindL( KMaxWidgets, variant ) )
                        {
                        publisherInfo->SetMaxWidgets(
                                variant.AsTInt32() );
                        variant.Reset();
                        }
                    else
                        {
                        // Use default value 1
                        publisherInfo->SetMaxWidgets( 1 );
                        }

                    if ( dataMap->FindL( KWidgetDesc, variant ) )
                        {
                        publisherInfo->SetDescriptionL( variant.AsDes() );
                        variant.Reset();
                        }

                    if ( dataMap->FindL( KWidgetIcon, variant ) )
                        {
                        publisherInfo->SetLogoIconL( variant.AsDes() );
                        variant.Reset();
                        }

                    CleanupStack::PopAndDestroy( dataMap );
                    }
                publisherInfo->SetContentTypeL( KHSTemplate() );
                publishers->AddPublisherInfoL( publisherInfo );
                CleanupStack::Pop( publisherInfo );
                
                variant.Reset();
                CleanupStack::PopAndDestroy( publisherMap );
                }
            }
        GetWRTWidgetsL( *publishers );

        CleanupStack::Pop( publishers );

        outParamList.Reset();

        return publishers;
        }

   // -----------------------------------------------------------------------
   // CCpsWrapper::GetWRTWidgetsL()
   // -----------------------------------------------------------------------
   //
	void CCpsWrapper::GetWRTWidgetsL(CPublisherMap& aPublisherMap)
		{
		CLiwGenericParamList& inParamList( iServiceHandler->InParamListL() );
		CLiwGenericParamList& outParamList( iServiceHandler->OutParamListL() );
	
		TLiwGenericParam type( KType, TLiwVariant( KPublisher16 ) );
		inParamList.AppendL( type );
	
		CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
	
		filter->InsertL( KPublisher, TLiwVariant( KWRTPublisher ));
		filter->InsertL( KContentType, TLiwVariant( KWRTTemplate ) );
	
		//append filter to input param
		TLiwGenericParam item( KFilter, TLiwVariant( filter ));
		inParamList.AppendL( item );
	
		// Get all the publiser from CPS with publiser id ai3templatewidget
		iCpsInterface->ExecuteCmdL( KGetList, inParamList, outParamList );
	
		inParamList.Reset();
	
		CleanupStack::PopAndDestroy(filter);
	
		TInt pos(0);
	
		outParamList.FindFirst( pos, KResults );
	
		if ( pos != KErrNotFound )
			{
			//extract iterator on results list
			TLiwVariant variant( outParamList[pos].Value() );
			CLiwIterable* iterable( variant.AsIterable() );
			iterable->Reset();
	
			//get all the templated widget publishers
			while ( iterable->NextL( variant ) )
				{
				//extract content map
				CLiwDefaultMap *publisherMap = CLiwDefaultMap::NewLC();
				variant.Get( *publisherMap );
				variant.Reset();
				CPublisherInfo* publisherInfo = CPublisherInfo::NewLC();
				if ( publisherMap->FindL( KContentId, variant ) )
					{
					publisherInfo->SetPublisherIdL( variant.AsDes() );
					variant.Reset();
					}
	
				if ( publisherMap->FindL( KDataMap, variant) )
					{
					CLiwDefaultMap* dataMap = CLiwDefaultMap::NewLC();
					variant.Get( *dataMap );
					variant.Reset();
					if ( dataMap->FindL( KWidgetInfo, variant ) )
						{
						CLiwDefaultMap* widgetInfoMap = CLiwDefaultMap::NewLC();
						variant.Get( *widgetInfoMap );
						if ( widgetInfoMap->FindL( KTemplateType, variant) )
							{
							publisherInfo->SetTemplateTypeL( variant.AsDes() );
							variant.Reset();
							}
						if ( widgetInfoMap->FindL( KWidgetName, variant ) )
							{
							publisherInfo->SetWidgetNameL( variant.AsDes() );
							variant.Reset();
							}
						if ( widgetInfoMap->FindL( KWidgetDesc, variant ) )
							{
							publisherInfo->SetDescriptionL( variant.AsDes() );
							variant.Reset();
							}
						if ( widgetInfoMap->FindL( KWidgetIcon, variant ) )
							{
							publisherInfo->SetLogoIconL( variant.AsDes() );
							variant.Reset();
							}
                            
						CleanupStack::PopAndDestroy( widgetInfoMap );
						}
					CleanupStack::PopAndDestroy( dataMap );
					}
				publisherInfo->SetContentTypeL( KWRTTemplate() );
				publisherInfo->SetMaxWidgets(1);
				aPublisherMap.AddPublisherInfoL( publisherInfo );
				CleanupStack::Pop( publisherInfo );
				
				variant.Reset();
				CleanupStack::PopAndDestroy( publisherMap );
				}
			}
		outParamList.Reset();
	
		}
	// -----------------------------------------------------------------------
	// CCpsWrapper::RequestForNotificationL()
	// -----------------------------------------------------------------------
	//
    void CCpsWrapper::RequestForNotificationL(
    		const TDesC& aPublisher, 
    		const TDesC& aContentType,
    		const TDesC& aContentId,
    		const TDesC8& aOperation,
    		const TDesC& aRegistry)
    	{
    	CCpsObserver* observer = CCpsObserver::NewLC( this );
		CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
		 
		filter->InsertL( KPublisher, TLiwVariant( aPublisher ) );
		filter->InsertL( KContentType, TLiwVariant( aContentType ) );
		filter->InsertL( KContentId, TLiwVariant( aContentId ) );
		filter->InsertL( KOperation, TLiwVariant( aOperation ));

		observer->RegisterL( filter, aRegistry );
		iObservers.AppendL( observer );
		
		CleanupStack::PopAndDestroy(filter);
		CleanupStack::Pop( observer );
    	}
    
    // -----------------------------------------------------------------------
    // CCpsWrapper::StartListeningPublisherRegistryL()
    // -----------------------------------------------------------------------
    //
    void CCpsWrapper::StartListeningPublisherRegistryL()
        {
        // Start Listening all the HS template using widgets unregistration
        RequestForNotificationL( KAll, KHSTemplate, KAll, KDelete, KPublisher16 );
                 
        // Start Listening all the WRT widgets unregistration
        RequestForNotificationL( KAll, KWRTTemplate, KAll, KDelete, KPublisher16 );
        
        // Start Listening all the WRT widgets registrations
        RequestForNotificationL( KAll, KWRTTemplate, KAll, KAdd, KPublisher16 );
        
        // Start Listening all the HS template widgets registrations
        RequestForNotificationL( KAll, KHSTemplate, KAll, KAdd, KPublisher16 );
        }
    
    // -----------------------------------------------------------------------
	// CCpsWrapper::HandleChangedPublisherL()
	// -----------------------------------------------------------------------
	//
    void CCpsWrapper::HandleChangedPublisherL(const TDesC& aPublisher, 
            const TDesC& aOperation)
    	{       
        if ( aOperation == KDelete16 )
            {
            iObserver.NotifyWidgetUnregisteredL( aPublisher );
            }
        else if ( aOperation == KAdd16 )
            {
            iObserver.NotifyWidgetRegisteredL();
            }
    	}
    }

// End of file
