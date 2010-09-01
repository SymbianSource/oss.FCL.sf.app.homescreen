/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Native ui controller main view
*
*/


#ifndef CAINATIVEUIVIEW_H
#define CAINATIVEUIVIEW_H

#include <coecntrl.h>
#include <coeview.h>
namespace AiNativeUiController
{

/**
*  It is the main view of the Idle. It consists of a single component
*  control.
*
*  @sinces S60 3.2
*/
class CAiNativeUiView
    : public CCoeControl,
      public MCoeView
    {

public:  // Constructors and destructor

        static CAiNativeUiView* NewL(
                        const TRect& aRect,
                         CAppUi* aAppUi );

        ~CAiNativeUiView();

// From base class

        void SizeChanged();

        TInt CountComponentControls() const;

        CCoeControl* ComponentControl( TInt aIndex ) const;

        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

        void SetControl( CCoeControl* aControl );

protected:

// from base classes

        void Draw( const TRect& aRect ) const;

// from MCoeView

        TVwsViewId ViewId() const;

        void ViewActivatedL( const TVwsViewId& aPrevViewId,
                             TUid aCustomMessageId,
                             const TDesC8& aCustomMessage );

        void ViewDeactivated();

private:


// Construction

        CAiNativeUiView( CAppUi* aAppUi );

        void ConstructL( const TRect& aRect );

private:    // Data

    /**
     * Pointer to our app ui.
     * Not owned.
     */
    CAppUi* iAppUi;

    /**
     * View control. Not own.
     */
    CCoeControl* iControl;

    };

} // AiNativeUiController

#endif      // CAINATIVEUIVIEW_H

// End of File
