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
* Description:  Navipane renderer.
*
*/


#ifndef C_AINAVIPANERENDERER_H
#define C_AINAVIPANERENDERER_H

// System includes
#include <e32base.h>

// User includes
#include "ainativerenderer.h"
#include "ainavipaneanimatorcallback.h"
#include "ainativeuimodel.h"

namespace AiNativeUiController
{
class CAiStatusPanel;
class CAiNaviPaneAnimator;


/**
 *  Navipane renderer.
 *
 *  Used to render text on navipane.
 *
 *  @since S60 3.2
 */
// inheritance order must be this, despite codescanner warnings
NONSHARABLE_CLASS( CAiNaviPaneRenderer ) : public MAiNaviPaneAnimatorCallback,
    public CAiNativeRenderer                                  
    {
public:

    static CAiNaviPaneRenderer* NewLC( CAiStatusPanel& aStatusPanel );

    virtual ~CAiNaviPaneRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( CHsContentPublisher& aPlugin, 
                        TInt aContent, 
                        const TDesC16& aText,
                        TInt aIndex );

    void DoCleanL( CHsContentPublisher& aPlugin, TInt aContent );

    void FocusObtainedL();

    void FocusLostL();
    
    void KeylockDisabledL();
    
    void KeylockEnabledL();

    void TransactionCommittedL();

// from base class MAiNaviPaneAnimatorCallback

    void AnimationCompleteL();
    
private:

    CAiNaviPaneRenderer( CAiStatusPanel& aStatusPanel );

    void ConstructL();
    
    HBufC* ConstructProfileTextLC();


private: // data

     /// Status pane handler.
    CAiStatusPanel& iStatusPanel;

     /// Navi pane animator. Own.
    CAiNaviPaneAnimator* iAnimator;
    
    /// Navi pane rendering priorities
    AiNativeUiModel::TRenderingPriorities iRenderingPriorities;
    
    /// Profile name or date.
    HBufC* iProfileText;
    
    /// Silent Indicator
    HBufC* iSilentIndicator;
    
    /// Timed profile indicator
    HBufC* iTimedProfileIndicator;
    
    /// Id of current content in this UI control.
    TInt iCurrentContent;
    
    /// Rendering priority of current content in this UI control.
    TInt iCurrentPriority;
    
    /// ETrue if we got the focus.
    TBool iFocusObtained;
    
    /// ETrue is transaction was committed.
    TBool iCommitted;
     
    /// ETrue is new content was published.
    TBool iIsChanged;
    
    /// ETrue is last content was changed.
    TBool iIsOldTextChanged;
    
    /// ETrue is new content was silent.
    TBool iIsSilent;
    
    /// ETtrue if new content was timed profile
    TBool iIsTimed;
    
    };

} // namespace AiNativeUiController

#endif // C_AINAVIPANERENDERER_H
