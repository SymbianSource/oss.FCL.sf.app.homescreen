/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Helper system for handling Out-Of-Memory cases.
*
*/

#ifndef XNOOMSYSHANDLER_H
#define XNOOMSYSHANDLER_H

// System includes
#include <e32base.h>

// Forward declarations

// Macros

// Bytes to kilobytes
#define B2KB(val_in_bytes) (TInt)( val_in_bytes / 1024 )
// Kilobytes to bytes 
#define KB2B(val_in_kbytes) (TInt)( val_in_kbytes * 1024 )

#define WIDGET_MIN_MEM  CXnOomSysHandler::EMem1MB
#define VIEW_MIN_MEM    CXnOomSysHandler::EMem4MB

/**
 *  Class helper for handling (and preventing) out-of-memory failures
 *
 *  This Class has only some static method. 
 *  E.g. for showing error popup window or for checking of available memory on
 *  the heap. etc.
 *
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CXnOomSysHandler ) : public CBase
    {
public:     // data types

    /**
     * Enumeration of predefined values of memory amount for possible use 
     */
    enum TMemPredefinedValues
        {
#ifdef _DEBUG
        EMemOutOfRangeTest = 209715200,
#endif
        EMinNotDef = 0,
        EMem1kB = 1024,
        EMem2kB = 2048,
        EMem4kB = 4096,
        EMem8kB = 8192,
        EMem16kB = 16384,
        EMem32kB = 32768,
        EMem64kB = 65536,
        EMem128kB = 131072,
        EMem256kB = 262144,
        EMem512kB = 524288,
        EMem1MB = 1024000,
        EMem2MB = 2048000,
        EMem3MB = 3072000,
        EMem4MB = 4096000,
        EMem6MB = 6144000 
        };

public: // Construction / destruction.
    /**
     * Two-phased constructor.
     */
    static CXnOomSysHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CXnOomSysHandler();
    
public: // methods.
    /**
     * Checks if required amount of free memory is available on heap.
     * 
     * @param aAmount required amount of free memory (in Bytes - use some 
     * value from TMemPredefinedValues enumeration for your comfort)
     * @return ETrue if required memory is available on heap, otherwise EFalse
     */
    static TBool HeapAvailable( TInt aAmount = EMinNotDef );

    /**
     * Handle potential OOM failure.
     */
    void HandlePotentialOomL();

private: // Constructors.
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();   
    
    /**
     * C++ constructor.
     */
    CXnOomSysHandler();
    
private:     // functions
    /**
     * Check whether OOM dialog can be shown.
     * Prevents OOM dialogs popping up more often than
     * defined in KMinInterval (defined in .cpp).
     */
    TBool AllowDialog();
    
private:    // Data.    
    /**
     * OOM dialog message.
     * 
     * Allocated early because memory might not
     * be available when HandlePotentialOomL is called.
    */
    HBufC* iMsg;    
    
    /**
     * Stored time stamp representing when dialog was last shown.
    */
    TInt64 iPreviousDialogPopupTime;
    };

#endif // XNOOMSYSHANDLER_H
