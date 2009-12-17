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
* Description:  Callback handler for newsticker component callbacks
*
*/


#include "newstickercallbackhandler.h"
#include "aifweventhandler.h"
#include "aiconsts.h"
#include "aixmluiconstants.h"
#include "aistrcnv.h"

using namespace AiXmlUiController;

const TInt KRadix = 10;
const TText16 KPluginEventParameterSeparator = ',';
const TText16 KEventParameterTerminator = ')';

static TInt IndexLength( TInt aIndex )
    {
    TInt length = 0;

    if ( aIndex < 0 )
        {
        ++length;
        }

    do
        {
        aIndex /= KRadix;
        ++length;
        }
    while ( aIndex != 0 );

    return length;
    }

// ======== MEMBER FUNCTIONS ========

CNewstickerCallbackHandler::CNewstickerCallbackHandler( MAiFwEventHandler& aFwEventHandler )
    : iFwEventHandler( aFwEventHandler )
    {
    }

CNewstickerCallbackHandler* CNewstickerCallbackHandler::NewLC( MAiFwEventHandler& aFwEventHandler )
    {
    CNewstickerCallbackHandler* self = new( ELeave ) CNewstickerCallbackHandler( aFwEventHandler );
    CleanupStack::PushL( self );
    return self;
    }


CNewstickerCallbackHandler::~CNewstickerCallbackHandler()
    {
    delete iEventBuffer;
    }


void CNewstickerCallbackHandler::TitleScrolledL( const TDesC& aPublisherName,
                                                 const TDesC& aPublishingClass,
                                                 TInt aIndex )
    {
    SendEventToNewstickerPluginL( AiUiDef::xml::event::KNewstickerTitleScrolled,
                                    aPublisherName,
                                    aPublishingClass,
                                    aIndex);
    }

void CNewstickerCallbackHandler::TitleToScrollL( const TDesC& aPublisherName,
                                                 const TDesC& aPublishingClass,
                                                 TInt aIndex )
    {
    SendEventToNewstickerPluginL( AiUiDef::xml::event::KNewstickerTitleToScroll,
                                    aPublisherName,
                                    aPublishingClass,
                                    aIndex);
    }


void CNewstickerCallbackHandler::SendEventToNewstickerPluginL( const TDesC& aEvent,
                                                        const TDesC& aPublisherName,
                                                        const TDesC& aPublishingClass,
                                                        TInt aIndex)
    {
        // Calculate event buffer length
    TInt length = aPublisherName.Length()
                  + KPluginEventSeparatorLength
                  + aEvent.Length()
                  + KEventParameterSeparatorLength
                  + aPublishingClass.Length()
                  + KEventParameterSeparatorLength
                  + IndexLength( aIndex )
                  + KEventParameterSeparatorLength;

    // Allocate event buffer
    TPtr event = AiUtility::EnsureBufMaxLengthL( iEventBuffer, length );

    // Create event string
    event.Zero();
    event.Append( aPublisherName );
    event.Append( KPluginEventSeparator );
    event.Append( aEvent );
    event.Append( KEventParameterSeparator );
    event.Append( aPublishingClass );
    event.Append( KPluginEventParameterSeparator );
    event.AppendNum( aIndex );
    event.Append( KEventParameterTerminator );

    // Send event to plugin
    // TODO: fix namespace
    iFwEventHandler.HandlePluginEvent( event );
    }
