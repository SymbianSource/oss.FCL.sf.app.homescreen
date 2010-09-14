/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Control example
*
*/


// [[[ begin generated region: do not modify [Generated System Includes]
#include <eikmenub.h>
#include <hscontentcontrolexample.rsg>
// ]]] end generated region [Generated System Includes]

// [[[ begin generated region: do not modify [Generated User Includes]
#include "HsContentControlAppUi.h"
#include "HsContentControlListBox.h"
// ]]] end generated region [Generated User Includes]

// [[[ begin generated region: do not modify [Generated Constants]
// ]]] end generated region [Generated Constants]

/**
 * Construct the CHsContentControlAppUi instance
 */ 
CHsContentControlAppUi::CHsContentControlAppUi()
    {
    // [[[ begin generated region: do not modify [Generated Contents]
    iHsContentControlListBox = NULL;
    // ]]] end generated region [Generated Contents]
    
    }

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CHsContentControlAppUi::~CHsContentControlAppUi()
    {
    // [[[ begin generated region: do not modify [Generated Contents]
    if ( iHsContentControlListBox != NULL )
        {
        RemoveFromStack( iHsContentControlListBox );
        delete iHsContentControlListBox;
        iHsContentControlListBox = NULL;
        }
    // ]]] end generated region [Generated Contents]
    
    }

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here. 
 */ 
void CHsContentControlAppUi::ConstructL()
    {
    // [[[ begin generated region: do not modify [Generated Contents]
    
    BaseConstructL( EAknEnableSkin | EAknEnableMSK |                      
        EAknTouchCompatible | EAknSingleClickCompatible );
                        
    InitializeContainersL();
    // ]]] end generated region [Generated Contents]
    
    }

// [[[ begin generated function: do not modify
void CHsContentControlAppUi::InitializeContainersL()
    {
    iHsContentControlListBox = CHsContentControlListBox::NewL( ClientRect(), NULL, this );
    iHsContentControlListBox->SetMopParent( this );
    AddToStackL( iHsContentControlListBox );
    
    iHsContentControlListBox->DrawNow();
    }
// ]]] end generated function

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CHsContentControlAppUi::HandleCommandL( TInt aCommand )
    {
    // [[[ begin generated region: do not modify [Generated Code]
    TBool commandHandled = EFalse;
    switch ( aCommand )
        { // code to dispatch to the AppUi's menu and CBA commands is generated here
        default:
            break;
        }
    
        
    if ( !commandHandled ) 
        {
        if ( aCommand == EAknSoftkeyExit || 
            aCommand == EAknSoftkeyBack || 
            aCommand == EEikCmdExit )
            {
            Exit();
            }
        }
    // ]]] end generated region [Generated Code]
    
    }

/** 
 * Override of the HandleResourceChangeL virtual function
 */
void CHsContentControlAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    // [[[ begin generated region: do not modify [Generated Code]
    // ]]] end generated region [Generated Code]
    
    }
                
