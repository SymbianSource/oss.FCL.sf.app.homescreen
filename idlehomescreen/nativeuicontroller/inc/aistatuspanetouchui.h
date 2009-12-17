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
* Description:  Status pane touch ui
*
*/


#ifndef C_AISTATUSPANETOUCHUI_H
#define C_AISTATUSPANETOUCHUI_H

#include <e32base.h>
#include <badesca.h>
#include <AknNaviDecoratorObserver.h>
#include <StringLoader.h>
#include <eikmobs.h>

#include "ainativerenderer.h"

class MAiFwEventHandler;

class CAknStylusPopUpMenu;

namespace AiNativeUiController
{

// Stylus activated popup command ids
enum TLinkCommandID
	{
    EAIGotoCalendarCmdLink = 1,
    EAIShowAllProfileCmdLink,
    EAIProfileCmdLink
    };


class CAiStatusPanel;

class CAiStatusPaneTouchUi : public CAiNativeRenderer,
							 public MAknNaviDecoratorObserver,
					  	 	 public MEikMenuObserver
	{

public:

// Construction and destructor

    static CAiStatusPaneTouchUi* NewL( CAiStatusPanel& aStatusPanel,
    									MAiFwEventHandler& aEventHandler);

    static CAiStatusPaneTouchUi* NewLC( CAiStatusPanel& aStatusPanel,
    									MAiFwEventHandler& aEventHandler );

    ~CAiStatusPaneTouchUi();

// from MAknTitlePaneObserver

    void HandleTitlePaneEventL( TInt aEventID );

// from MAknNaviDecoratorObserver

    void HandleNaviDecoratorEventL( TInt aEventID );

// from base class CAiNativeRenderer

    void DoPublishL( MAiPropertyExtension& aPlugin,
                        TInt aContent,
                        const TDesC16& aText,
                        TInt aIndex );

    void LoadUIDefinitionL();

// from MEikCommandObserver

    void ProcessCommandL( TInt aCommandId );

    void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );

private:
    // Construction

	CAiStatusPaneTouchUi( CAiStatusPanel& aStatusPanel, MAiFwEventHandler& aEventHandler );
	void ConstructL();

private: //data

    /// Status pane handler.
    CAiStatusPanel& iStatusPane;

    /// Profile name pointer array
    RPointerArray<HBufC> iProfileNamePointerArray;

    /// AIFW event handler
    MAiFwEventHandler& iEventHandler;

    ///Event buffer.
    HBufC* iEventBuffer;

    ///Stulys popup menu
    CAknStylusPopUpMenu* iMenu;
    };

} // namespace AiNativeUiController

#endif //C_AISTATUSPANETOUCHUI_H