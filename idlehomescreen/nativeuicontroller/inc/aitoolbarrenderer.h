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
* Description:  Touch Toolbar
*
*/


#ifndef C_AITOOLBARRENDERER_H
#define C_AITOOLBARRENDERER_H

#include <e32std.h>
#include <akntoolbarobserver.h>
#include "ainativerenderer.h"
#include "ainativeuimodel.h"
#include "aitoolbarbutton.h"

class MAiFwEventHandler;
class CAknToolbar;

namespace AiNativeUiController
{

    
/**
 *  Toolbar class of Native UI Controller application
 *
 *  @lib AiNatiUi
 */
// inheritance order must be this, despite codescanner warnings
class CAiToolbarRenderer : public MAknToolbarObserver,
                           public CAiNativeRenderer
    {

    
public:   // Constructors and destructor

    static CAiToolbarRenderer* NewLC( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar);

    static CAiToolbarRenderer* NewL( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar );
    
    ~CAiToolbarRenderer();
    
public:   // from CAiNativeRenderer    
    
    void DoPublishL( MAiPropertyExtension& aPlugin,
					 TInt aContent,
					 const TDesC16& aText,
					 TInt aIndex );
    
    void DoPublishL( MAiPropertyExtension& aPlugin,
                     TInt aContent,
                     const TDesC8& aBuf,
                     TInt aIndex );
                     
    void TransactionCommittedL();

                     
private:  // from MAknToolbarObserver 
     
  	void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );
  	
 	void OfferToolbarEventL( TInt aCommand );

private:

    CAiToolbarRenderer( MAiFwEventHandler& aAiFwEventHandler, CAknToolbar& aToolbar );

    void ConstructL();
    
    
    /**
     * Get a button with id. If no button exists at the given
     * id a new button will be created and returned. 
     *     
     * Ownership NOT transferred.     
     */
    CAiToolbarButton* ButtonL( TInt aShortcutId );

    
private:     // Data

    /** 
     * The actual toolbar    
     */
	CAknToolbar& iToolbar;
	
	/**
	 * Handler to send the toolbar events
	 */
 	MAiFwEventHandler& iAiFwEventHandler;
    
    /**
     * Array of buttons. Buttons are owned.
     */
    RPointerArray<CAiToolbarButton> iButtons;
    
    /**
     * Have we already committed
     */
    TBool iCommitted;

    /**
     * Is the toolbar enabled in the cenrep
     */
    TBool iToolbarEnabled;
    };
    
}  // namespace AiNativeUiController
    
#endif  // C_AITOOLBARRENDERER_H

// End of File.
