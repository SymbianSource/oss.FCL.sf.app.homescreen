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
* Description:  Callback handler of newsticker component callbacks
*
*/


#ifndef C_NEWSTICKERCALLBACKHANDLER_H
#define C_NEWSTICKERCALLBACKHANDLER_H


#include <e32base.h>

class MAiFwEventHandler;

namespace AiXmlUiController
{
    
/**
 *  @ingroup group_xmluicontroller
 * 
 *  Helper class to handle newsticker component callbacks.
 *
 *  @lib AiXmlUiMain
 */
class CNewstickerCallbackHandler : public CBase
    {

public:

    static CNewstickerCallbackHandler* NewLC( MAiFwEventHandler& aFwEventHandler );

    virtual ~CNewstickerCallbackHandler();

    /**
     * Sends TitleScrolled event to newsticker plugin
     *
     * @param aPublisherName    Plugin name
     * @param aPublishingClass  content selector
     * @param aIndex            Index of the scrolled title
     */
    void TitleScrolledL( const TDesC& aPublisherName,
                         const TDesC& aPublishingClass,
                         TInt aIndex );

    /**
     * Sends TitleToScroll event to newsticker plugin
     *
     * @param aPublisherName    Plugin name
     * @param aPublishingClass  content selector
     * @param aIndex            Index of the scrolled title
     */
    void TitleToScrollL( const TDesC& aPublisherName,
                         const TDesC& aPublishingClass,
                         TInt aIndex );

private:

    CNewstickerCallbackHandler( MAiFwEventHandler& aFwEventHandler );

    void SendEventToNewstickerPluginL( const TDesC& aEvent, 
                                       const TDesC& aPublisherName,
                                       const TDesC& aPublishingClass,
                                       TInt aIndex);

private: // data

    /**
     * Event handler for events
     */
    MAiFwEventHandler& iFwEventHandler;

    /**
     * Event buffer.
     * Own.
     */
    HBufC* iEventBuffer;

    };

} // namespace AiXmlUiController

#endif // C_NEWSTICKERCALLBACKHANDLER_H
