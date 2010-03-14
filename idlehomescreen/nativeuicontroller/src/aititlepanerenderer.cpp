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
* Description:  Handles publishing to title pane.
*
*/

// System includes
#include <StringLoader.h>
#include <AiNativeUi.rsg>
#include <gulicon.h>

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>

#include "aititlepanerenderer.h"
#include "aistatuspanel.h"

#include <aicontentrequest.h>
#include "ainativeuiplugins.h"
#include "debug.h"

using namespace AiNativeUiController;

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::NewLC()
//
// ----------------------------------------------------------------------------
//
CAiTitlePaneRenderer* CAiTitlePaneRenderer::NewLC( 
    CAiStatusPanel& aStatusPanel )
    {
    CAiTitlePaneRenderer* self = 
        new( ELeave ) CAiTitlePaneRenderer( aStatusPanel );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::CAiTitlePaneRenderer()
//
// ----------------------------------------------------------------------------
//
CAiTitlePaneRenderer::~CAiTitlePaneRenderer()
    {
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::CAiTitlePaneRenderer()
//
// ----------------------------------------------------------------------------
//
CAiTitlePaneRenderer::CAiTitlePaneRenderer( CAiStatusPanel& aStatusPanel )
	: iStatusPanel( aStatusPanel )
    {
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::DoPublishL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::DoPublishL( CHsContentPublisher& aPlugin,
    TInt aContent, const TDesC16& aText, TInt /*aIndex*/ )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KDeviceStatusPluginUid )
	    {
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentNetworkIdentity:
	            {
	            __PRINT(__DBG_FORMAT("XAI: Render title pane - %d, %S"), aContent, &aText );
	        	iStatusPanel.SetTitlePaneTextL( aText );
	        	// we're publishing text -> set bitmap as invalid
	        	iStatusPanel.SetTitlePaneBitmapL( KErrNotFound, KErrNotFound );
	            iStatusPanel.RenderTitlePaneL();
	        	__PRINTS( "XAI: Render title pane - done");

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

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::DoPublishL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::DoPublishL( CHsContentPublisher& aPlugin,
	TInt aContent, const TDesC8& aBuf, TInt /*aIndex*/ )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
   	if( info.Uid() == KDeviceStatusPluginUid )
	   	{
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentNetworkIdentity:
	            {
	           	__PRINTS( "XAI: Render title pane - oper logo");

	            CGulIcon* icon = NULL;
	            TPckg<CGulIcon*>( icon ).Copy( aBuf );

	            if( icon )
	                {
	                TInt bitmapHandle = icon->Bitmap() ?
	                					icon->Bitmap()->Handle() : KErrNotFound;
	                TInt maskHandle = icon->Mask() ?
	                					icon->Mask()->Handle() : KErrNotFound;

	                // SetTitlePaneBitmapL checks if handle was valid, so no 
	                // problem passing KErrNotFound
	                iStatusPanel.SetTitlePaneBitmapL( bitmapHandle, maskHandle );
	                // We're publishing bitmap -> set text as invalid
	                iStatusPanel.SetTitlePaneTextL( KNullDesC );
	                iStatusPanel.RenderTitlePaneL();
	                 __PRINTS("XAI: Rendering title pane - done");
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

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::DoPublishL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::DoPublishL( CHsContentPublisher& aPlugin, 
    TInt aContent, TInt aResource, TInt /*aIndex*/ )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KDeviceStatusPluginUid )
	    {
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentNetworkIdentity:
	            {
	            HBufC* text = StringLoader::LoadLC( aResource );      
	            iStatusPanel.SetTitlePaneTextL( *text );
	        	// We're publishing text -> set bitmap as invalid
	        	iStatusPanel.SetTitlePaneBitmapL( KErrNotFound, KErrNotFound );
	            iStatusPanel.RenderTitlePaneL();
	       
	            CleanupStack::PopAndDestroy(text); //text
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

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::DoCleanL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::DoCleanL( CHsContentPublisher& aPlugin, 
    TInt aContent )
	{
    __PRINT(__DBG_FORMAT("XAI: Clean title pane - %d"), aContent );
	
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KDeviceStatusPluginUid )
		{
	    switch( aContent )
	        {
	        case EAiDeviceStatusContentNetworkIdentity:
	            {
	            // Publish empty text
	            iStatusPanel.SetTitlePaneTextL( KNullDesC );
	        	iStatusPanel.SetTitlePaneBitmapL( KErrNotFound, KErrNotFound );
	            iStatusPanel.RenderTitlePaneL();
	            __PRINTS( "XAI: Clean title pane - done");
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

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::TransactionCommittedL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::TransactionCommittedL()
    {
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::KeylockEnabledL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::KeylockEnabledL()
	{
	iStatusPanel.StopTitlePaneScrollingL();
	}

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::FocusObtainedL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::FocusObtainedL()
    {
    iStatusPanel.RenderTitlePaneL();
    }

// ----------------------------------------------------------------------------
// CAiTitlePaneRenderer::FocusLostL()
//
// ----------------------------------------------------------------------------
//
void CAiTitlePaneRenderer::FocusLostL()
	{
	iStatusPanel.StopTitlePaneScrollingL();
	}

// End of file
