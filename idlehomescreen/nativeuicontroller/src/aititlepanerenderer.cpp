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


#include <StringLoader.h>
#include <AiNativeUi.rsg>
#include <gulicon.h>
#include "aititlepanerenderer.h"
#include "aistatuspanel.h"
#include "aipropertyextension.h"
#include <aicontentrequest.h>
#include "ainativeuiplugins.h"
#include "debug.h"

using namespace AiNativeUiController;


void CAiTitlePaneRenderer::ConstructL()
    {
    }


CAiTitlePaneRenderer* CAiTitlePaneRenderer::NewLC( CAiStatusPanel& aStatusPanel )
    {
    CAiTitlePaneRenderer* self = new( ELeave ) CAiTitlePaneRenderer( aStatusPanel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CAiTitlePaneRenderer::~CAiTitlePaneRenderer()
    {
    }


CAiTitlePaneRenderer::CAiTitlePaneRenderer( CAiStatusPanel& aStatusPanel )
	: iStatusPanel( aStatusPanel )
    {
    }




void CAiTitlePaneRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
										TInt aContent,
										const TDesC16& aText,
										TInt /*aIndex*/ )
    {
    if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
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

void CAiTitlePaneRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
										TInt aContent,
										const TDesC8& aBuf,
										TInt /*aIndex*/ )
    {
   	if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
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

void CAiTitlePaneRenderer::DoPublishL( MAiPropertyExtension& aPlugin, 
                                    	TInt aContent, 
                                    	TInt aResource,
                                    	TInt /*aIndex*/ )
    {
    if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
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


void CAiTitlePaneRenderer::DoCleanL( MAiPropertyExtension& aPlugin, TInt aContent )
	{
    __PRINT(__DBG_FORMAT("XAI: Clean title pane - %d"), aContent );
	if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
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

void CAiTitlePaneRenderer::TransactionCommittedL()
    {
    }

void CAiTitlePaneRenderer::KeylockEnabledL()
	{
	iStatusPanel.StopTitlePaneScrollingL();
	}

void CAiTitlePaneRenderer::FocusObtainedL()
    {
    iStatusPanel.RenderTitlePaneL();
    }

void CAiTitlePaneRenderer::FocusLostL()
	{
	iStatusPanel.StopTitlePaneScrollingL();
	}
