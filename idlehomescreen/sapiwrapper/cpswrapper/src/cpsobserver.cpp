/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: 
*
* Description:   Implements an observer to CPS
*
*/


// INCLUDE FILES
#include <LiwCommon.h>
#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>

#include "cpsobserver.h"
#include "cpswrapper.h"
#include "cpsconst.h"

namespace cpswrapper
    {
	
	// =============================================================================
	// --------------------------------------------------------------------------
	// Cleanup utility for RPointerArray
	// --------------------------------------------------------------------------
	//
	static void DoCleanup( TAny* aPtr )
		{
		__ASSERT_DEBUG( aPtr, User::Invariant() );
		static_cast< RCriteriaArray* >( aPtr )->ResetAndDestroy();
		}
	
	// ============================ MEMBER FUNCTIONS ==============================
	
	// ----------------------------------------------------------------------------
	// Constructor
	// ----------------------------------------------------------------------------
	//
	CCpsObserver::CCpsObserver( CCpsWrapper* aCpsWrapper )
		: iCpsWrapper( aCpsWrapper )
		{
		}
	
	// ----------------------------------------------------------------------------
	// Two-phased constructor.
	// ----------------------------------------------------------------------------
	//
	CCpsObserver* CCpsObserver::NewLC( CCpsWrapper* aCpsWrapper )
		{
		CCpsObserver* self( 
			new( ELeave ) CCpsObserver( aCpsWrapper ) );
			
		CleanupStack::PushL( self );
		self->ConstructL();
		return self;
		}
	
	// ---------------------------------------------------------------------------
	// Destructor
	// ---------------------------------------------------------------------------
	//
	CCpsObserver::~CCpsObserver()
		{
		ReleaseL();
		ReleaseLiw();
		}
	
	// ----------------------------------------------------------------------------
	// Symbian 2nd phase constructor can leave.
	// ----------------------------------------------------------------------------
	//
	void CCpsObserver::ConstructL()
		{
		InitLiwL();
		}
	
	// ---------------------------------------------------------------------------
	// CCpsObserver::HandleNotifyL
	//
	// Handles Published content notification
	// ---------------------------------------------------------------------------
	//
	TInt CCpsObserver::HandleNotifyL( 
		TInt /*aErrorCode */, 
		TInt /*aEventId*/,
		CLiwGenericParamList& aEventParamList, 
		const CLiwGenericParamList& /*aInParamList*/ )
		{
	    TInt pos( 0 );

		aEventParamList.FindFirst( pos, KChangeInfo );

		if ( pos != KErrNotFound )
			{
			// Get list of maps
			TLiwVariant variant( (aEventParamList)[pos].Value() );
			variant.PushL();

			const CLiwList* changeMapsList( variant.AsList() );

			// Iter through list content
			for ( TInt i = 0; i < changeMapsList->Count(); ++i )
				{
				TLiwVariant changeMap;
				changeMap.PushL();
				if ( changeMapsList->AtL( i, changeMap ) )
					{
					const CLiwMap* map = changeMap.AsMap();

					if ( map->FindL( KContentType, changeMap ) )
						{
						HBufC* publisher = NULL;
						HBufC* contentType = NULL;
						
						contentType = changeMap.AsDes().AllocLC();
						if ( KHSTemplate() == contentType &&
							 map->FindL( KPublisher, changeMap ) )
							{
                            publisher = changeMap.AsDes().AllocLC();

                            if ( map->FindL( KOperation, changeMap ) )
                                {
                                HBufC* operation = changeMap.AsDes().AllocLC();
                                
                                // unregitered publisher's widget
                                // from home screen if exist or add
                                // new widget to widgetmanager
                                iCpsWrapper->HandleChangedPublisherL( *publisher,
                                        *operation );
                                CleanupStack::PopAndDestroy( operation );
                                }
							CleanupStack::PopAndDestroy( publisher );
							}
						else if( KWRTTemplate() == contentType  &&
								  map->FindL( KContentId, changeMap ) )
							 {
                             publisher = changeMap.AsDes().AllocLC();
                             if ( map->FindL( KOperation, changeMap ) )
                                 {
                                 HBufC* operation = changeMap.AsDes().AllocLC();
                                 
                                 // unregitered publisher's widget
                                 // from home screen if exist or add
                                 // new widget to widgetmanager
                                 iCpsWrapper->HandleChangedPublisherL( *publisher,
                                         *operation );
                                 CleanupStack::PopAndDestroy( operation );
                                 }
                             CleanupStack::PopAndDestroy( publisher );
							 }
						CleanupStack::PopAndDestroy( contentType );
						}
					}
				CleanupStack::PopAndDestroy( &changeMap );
				}

			CleanupStack::PopAndDestroy( &variant );
			}
		return KErrNone;
		}
	
	// ---------------------------------------------------------------------------
	// CCpsObserver::RegisterL
	//
	// Register for notifications
	// ---------------------------------------------------------------------------
	//
	void CCpsObserver::RegisterL( CLiwDefaultMap* aFilter, const TDesC& aRegistry )
		{
		CLiwGenericParamList* inParamList( CLiwGenericParamList::NewLC() );
		CLiwGenericParamList* outParamList( CLiwGenericParamList::NewLC() );
		
		// Fill in input list for RequestNotification command
		inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant( aRegistry )));
		inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant( aFilter )));
			
		iInterface->ExecuteCmdL( KRequestNotification,  *inParamList,
											 *outParamList, 0, this );
		
		CleanupStack::PopAndDestroy( outParamList );
		CleanupStack::PopAndDestroy( inParamList ); 
		}
	
	// ---------------------------------------------------------------------------
	// CCpsObserver::ReleaseL
	//
	// Sing off to notification
	// ---------------------------------------------------------------------------
	//
	void CCpsObserver::ReleaseL()
		{
		if( iInterface )
			{
			CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
			CleanupStack::PushL( inParamList );
			CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
			CleanupStack::PushL( outParamList );
			
			iInterface->ExecuteCmdL( KRequestNotification, *inParamList, *outParamList,
													 KLiwOptCancel, this );
	
			CleanupStack::PopAndDestroy( outParamList );
			CleanupStack::PopAndDestroy( inParamList );
			}
		}
	
	
	// ------------------------------------------------------------------------
	// CCpsObserver::InitLiwL
	//
	// Init LIW interface.
	// ------------------------------------------------------------------------
	void CCpsObserver::InitLiwL()
		{
		if ( !iLiwHandler )
			{
			TInt error( KErrNone );
			CLiwGenericParamList* in( NULL );
			CLiwGenericParamList* out( NULL );
			CLiwCriteriaItem* criterion( NULL );
	
			RCriteriaArray array( 1 );
			TCleanupItem cleanup( DoCleanup, &array );
			CleanupStack::PushL( cleanup );
	
			// Do the basic initialization.
			iLiwHandler = CLiwServiceHandler::NewL();
	
			// Get iCPSIface.
			in = CLiwGenericParamList::NewLC();
			out = CLiwGenericParamList::NewLC();
	
			criterion = CLiwCriteriaItem::NewL( 
				KLiwCmdAsStr,
				KCPSConfigurationIf, 
				KCPS );
				
			error = array.Append( criterion );
			if ( error )
				{
				delete criterion; 
				criterion = NULL;
				User::Leave( error );
				}
	
			criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );
				
			iLiwHandler->AttachL( array );
			iLiwHandler->ExecuteServiceCmdL( *criterion, *in, *out ); 
	
			out->FindFirst( error, KCPSConfigurationIf );
			User::LeaveIfError( error );
	
			iInterface = (*out)[ error ].Value().AsInterface();
	
			CleanupStack::PopAndDestroy( 3 );
			}
		}
	
	// ---------------------------------------------------------------------------
	// CCpsObserver::ReleaseLiw
	// ---------------------------------------------------------------------------
	//
	void CCpsObserver::ReleaseLiw()
		{    
		if ( iInterface )
			{
			iInterface->Close();
			iInterface = NULL;
			}
			
		delete iLiwHandler;
		iLiwHandler = NULL;
		}
}
 //  End of File

