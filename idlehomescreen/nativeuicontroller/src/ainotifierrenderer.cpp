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
* Description:  Dialog renderer.
*
*/

// System includes 
#include <e32property.h>

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include <activeidle2domainpskeys.h>
#include <ainativeui.rsg>
#include "ainotifierrenderer.h"
#include "ainativeuiplugins.h"

using namespace AiNativeUiController;

// Constants

// 1-minute timeout before showing soft notification
const TInt KNetworkLostTimeout = 60*1000000;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::NewLC()
//
// ----------------------------------------------------------------------------
//
CAiNotifierRenderer* CAiNotifierRenderer::NewLC()
    {
    CAiNotifierRenderer* self = new( ELeave ) CAiNotifierRenderer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::~CAiNotifierRenderer()
//
// ----------------------------------------------------------------------------
//
CAiNotifierRenderer::~CAiNotifierRenderer()
    {
    delete iSoftNotifier;
    delete iTimer;
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::CAiNotifierRenderer()
//
// ----------------------------------------------------------------------------
//
CAiNotifierRenderer::CAiNotifierRenderer()
    {
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiNotifierRenderer::ConstructL()
    {
    iSoftNotifier = CAknSoftNotifier::NewL();
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::DoPublishL()
//
// ----------------------------------------------------------------------------
//
void CAiNotifierRenderer::DoPublishL( CHsContentPublisher& aPlugin,
    TInt aContent, TInt aResource, TInt /*aIndex*/ )                                                                       
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KDeviceStatusPluginUid )
    	{
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentNWStatus:
	            {
	            if ( aResource == EAiDeviceStatusResourceNWLost )
	                {
	                // Start 60 second timer and when expired show the
	                // notifier
	                iTimer->Cancel();
	                iTimer->Start( KNetworkLostTimeout,
	                               KNetworkLostTimeout,
	                               TCallBack( NWLostDelayCallBack, this ));
	                }
	            else if ( aResource == EAiDeviceStatusResourceNWOk )
	                {
	                iTimer->Cancel();
	                RemoveNotification( ESelectNetworkNotification );
	                }
	            break;
	            }
	        default:
	            {
	            User::Leave( KErrNotFound );
	            break;
	            }
	        }
    	}
    else
   		{
   		User::Leave( KErrNotFound );
   		}
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::DoCleanL()
//
// ----------------------------------------------------------------------------
//
void CAiNotifierRenderer::DoCleanL( CHsContentPublisher& /*aPlugin*/,
    TInt aContent )
    {
    switch( aContent )
        {
        case EAiDeviceStatusContentNWStatus:
            {
            iTimer->Cancel();
            RemoveNotification( ESelectNetworkNotification );
            break;
            }
        default:
            {
            User::Leave( KErrNotFound );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::AddNotification()
//
// ----------------------------------------------------------------------------
//
void CAiNotifierRenderer::AddNotification( TAknSoftNotificationType aType )
    {
    TRAP_IGNORE( iSoftNotifier->AddNotificationL( aType, 1 ) );
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::RemoveNotification()
//
// ----------------------------------------------------------------------------
//
void CAiNotifierRenderer::RemoveNotification( TAknSoftNotificationType aType )
    {
    TRAP_IGNORE( iSoftNotifier->CancelSoftNotificationL( aType ); );
    }

// ----------------------------------------------------------------------------
// CAiNotifierRenderer::NWLostDelayCallBack()
//
// ----------------------------------------------------------------------------
//
TInt CAiNotifierRenderer::NWLostDelayCallBack( TAny* aParam )
    {
    CAiNotifierRenderer* self = 
        reinterpret_cast< CAiNotifierRenderer* >( aParam );
    
        if ( self )
        {
        self->iTimer->Cancel();
        self->AddNotification( ESelectNetworkNotification );
        }
        
    return KErrNone;
    }

// End of file

