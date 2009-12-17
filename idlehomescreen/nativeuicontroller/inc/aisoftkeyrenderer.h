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


#ifndef C_AISOFTKEYRENDERER_H
#define C_AISOFTKEYRENDERER_H


#include <e32base.h>
#include <coeaui.h> //ECoeStackPriorityDefault
#include "ainativerenderer.h"
#include "ainativeuimodel.h"


namespace AiNativeUiController
{
// CONSTANTS
// VIEW IDENTIFIERS; VALUES MUST BE UNIQUE!

// PRIORITIES FOR SOFTKEYS

// Offset
const TInt KNativeUiCbaPriorityOffset = 5;

// Real priority.
const TInt KNativeUiCbaPriority = ECoeStackPriorityDefault + 
                                    KNativeUiCbaPriorityOffset;

/**
 *  Soft Key renderer.
 *
 *  This class is used to handle publishing to Soft Key labels.
 *  Published data can be either text or resource.
 *
 *  @since S60 3.2
 */
class CAiSoftKeyRenderer : public CAiNativeRenderer
    {
public:

    static CAiSoftKeyRenderer* NewLC();

    virtual ~CAiSoftKeyRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( MAiPropertyExtension& aPlugin,
                        TInt aContent,
                        const TDesC16& aText,
                        TInt aIndex );
    void DoPublishL( MAiPropertyExtension& aPlugin,
                        TInt aContent,
                        TInt aResource,
                        TInt aIndex );
    void DoPublishL( MAiPropertyExtension& aPlugin,
                        TInt aContent,
                        const TDesC8& aBuf,
                        TInt aIndex );

private:

    CAiSoftKeyRenderer();

    void ConstructL();

    void CreateDefaultSoftKeysL();

    void UpdateSoftKeyL( TInt aKey, CGulIcon* aIcon );

private: // data

      // Softkeys.
      CEikButtonGroupContainer* iCba;

      // Pointer to App UI.
      CAknAppUi* iAppUi;
      
      // Soft Key label. Own.
      HBufC* iText;

    };

} // namespace AiNativeUiController

#endif // C_AISOFTKEYRENDERER_H
