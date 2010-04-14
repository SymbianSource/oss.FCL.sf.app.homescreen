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
* Description:
* Declares wait dialog that doesn't consume key events.
*
*/


#ifndef _XNWAITDIALOG_H
#define _XNWAITDIALOG_H

//  INCLUDES
#include <AknWaitDialog.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS   

// CLASS DECLARATION


/**  
 * Wait Dialog without key event handling
 * 
 * @ingroup group_xnlayoutengine
 * @lib xn3layoutengine.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CXnWaitDialog ) : public CAknWaitDialog
  {

public: 		
    // Constructors and destructor

    /**
     * Class constructor
     *
     * @param    aSelfPtr            Pointer to itself. The pointer needs to be
     *                               valid when the dialog is dismissed and must not
     *                               be on the stack.
     * @param    aVisibilityDelayOff If set ETrue the dialog will be visible
     *                               immediality. Use only when the length of
     *                               the process is ALWAYS over 1.5 seconds.
     */
    CXnWaitDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );
    
    /**
     * Destructor
     */
    ~CXnWaitDialog();

public: // From CAknWaitDialog
 
    /**
     * From CAknWaitDialog
     * @since S60 5.2
     * @param aKeyEvent the key event
     * @param aType the type of the event
     * @return TKeyResponse key event was used by this control or not
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType );
  };
#endif // _XNWAITDIALOG_H

