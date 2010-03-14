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


#ifndef C_AITITLEPANERENDERER_H
#define C_AITITLEPANERENDERER_H

// System includes
#include <e32base.h>

// User includes
#include "ainativerenderer.h"
#include "ainativeuimodel.h"

// Forward declarations
class CHsContentPublisher;

namespace AiUtility
	{
	class CContentPriorityMap;
	}

namespace AiNativeUiController
{

class CAiStatusPanel;


/**
 *  Title pane renderer.
 *
 *  This class is used to handle publishing to title pane.
 *  Published data can be either logo or text.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiTitlePaneRenderer ) : public CAiNativeRenderer
    {
public:

    static CAiTitlePaneRenderer* NewLC( CAiStatusPanel& aStatusPanel );

    virtual ~CAiTitlePaneRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( CHsContentPublisher& aPlugin, 
                        TInt aContent, 
                        const TDesC16& aText,
                        TInt aIndex );

    void DoPublishL( CHsContentPublisher& aPlugin, 
                        TInt aContent, 
                        const TDesC8& aBuf,
                        TInt aIndex );

    void DoPublishL( CHsContentPublisher& aPlugin, 
                        TInt aContent, 
                        TInt aResource,
                        TInt aIndex );

	void DoCleanL( CHsContentPublisher& aPlugin, TInt aContent );

    void TransactionCommittedL();

    void FocusObtainedL();

    void FocusLostL();
    
    void KeylockEnabledL();
    
private:

    CAiTitlePaneRenderer( CAiStatusPanel& aStatusPanel );

    void ConstructL();



private: // data

     /// Status pane handler.
    CAiStatusPanel& iStatusPanel;
    };

} // namespace AiNativeUiController

#endif // C_AITITLEPANERENDERER_H
