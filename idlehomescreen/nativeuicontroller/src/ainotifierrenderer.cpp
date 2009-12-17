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


#include <e32property.h>
#include <activeidle2domainpskeys.h>
#include <aipropertyextension.h>

#include <AiNativeUi.rsg>

#include "ainotifierrenderer.h"
#include "ainativeuiplugins.h"

using namespace AiNativeUiController;

// 1-minute timeout before showing soft notification
const TInt KNetworkLostTimeout = 60*1000000;

// ======== MEMBER FUNCTIONS ========

CAiNotifierRenderer* CAiNotifierRenderer::NewLC()
    {
    CAiNotifierRenderer* self = new( ELeave ) CAiNotifierRenderer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CAiNotifierRenderer::~CAiNotifierRenderer()
    {
    delete iSoftNotifier;
    delete iTimer;
    }


CAiNotifierRenderer::CAiNotifierRenderer()
    {
    }

void CAiNotifierRenderer::ConstructL()
    {
    iSoftNotifier = CAknSoftNotifier::NewL();
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

void CAiNotifierRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                    TInt aContent,
                                    TInt aResource,
                                    TInt /*aIndex*/  )
    {
    if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
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
	        };
    	}
    else
   		{
   		User::Leave( KErrNotFound );
   		}
    }

void CAiNotifierRenderer::DoCleanL( MAiPropertyExtension& /*aPlugin*/,
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
        };

    }


void CAiNotifierRenderer::AddNotification( TAknSoftNotificationType aType )
    {
    TRAP_IGNORE( iSoftNotifier->AddNotificationL( aType, 1 ); );
    }

void CAiNotifierRenderer::RemoveNotification( TAknSoftNotificationType aType)
    {
    TRAP_IGNORE( iSoftNotifier->CancelSoftNotificationL( aType ); );
    }

TInt CAiNotifierRenderer::NWLostDelayCallBack(TAny* aParam)
    {
    CAiNotifierRenderer* self = reinterpret_cast<CAiNotifierRenderer *> (aParam);
    if ( self )
        {
        self->iTimer->Cancel();
        self->AddNotification( ESelectNetworkNotification );
        }
    return KErrNone;
    }


