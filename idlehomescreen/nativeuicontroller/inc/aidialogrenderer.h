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


#ifndef C_AIDIALOGRENDERER_H
#define C_AIDIALOGRENDERER_H

// System incldues
#include <e32base.h>

// User includes
#include "ainativerenderer.h"

// Forward declarations
class CAknNoteDialog;
class CHsContentPublisher;

namespace AiNativeUiController
{

/**
 *  Dialog renderer.
 *
 *  Used to show CAknNoteDialog on screen.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiDialogRenderer ) : public CAiNativeRenderer
    {
public:

    static CAiDialogRenderer* NewLC();

    virtual ~CAiDialogRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    void DoCleanL( CHsContentPublisher& aPlugin, TInt aContent );

    void FocusObtainedL();

private:

    CAiDialogRenderer();

    /**
     * Show dialog.
     *
     * @since S60 3.2
     */
    void ShowDialogL();


private: // data

    /**
     * Dialog.
     * Own.
     */
    CAknNoteDialog* iDialog;
   
    /**
     * Text to be rendered in the dialog.
     * Own.
     */
    HBufC* iText;

    /// Indicates SIM registration failure.
    TBool iShowDialog;

#ifdef __COVER_DISPLAY
    /**
     * Dialog ID to be published.
     * Own.
     */
    TInt iDialogId;
#endif

    };

} // namespace AiNativeUiController

#endif // C_AIDIALOGRENDERER_H
