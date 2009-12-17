/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  External rendering plugin adapter header
*
*/

#ifndef XNEXTRENDERINGPLUGINADAPTER_H
#define XNEXTRENDERINGPLUGINADAPTER_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>
#include <coecntrl.h>

// User includes

/**
 *  CXnExtRenderingPluginAdapter class
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib extrenderingplugin.lib
 *  @since S60 v5.0
 */
class CXnExtRenderingPluginAdapter : public CCoeControl
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CXnExtRenderingPluginAdapter* NewL( TUid aImplUid );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CXnExtRenderingPluginAdapter();  

private: // Constructors
    /**
     * Constructor
     */
    // CXnExtRenderingPluginAdapter();

    /** 
     * Second phase constructor 
     */
    // void ConstructL();

public: // New functions,

    /**
     * Returns the implementation uid.
     */
    IMPORT_C TUid ImplUid() const;

    /**
     * Enters power save mode.
     */
    IMPORT_C virtual void EnterPowerSaveModeL();

    /**
     * Exits power save mode.
     */    
    IMPORT_C virtual void ExitPowerSaveModeL();

    /**
     * Informs skin change.
     */    
    IMPORT_C virtual void SkinChanged();
    
    /**
     * Informs focuschange.
     */
    IMPORT_C virtual void FocusChanged( TDrawNow aDrawNow );
    
    /**
     * Informs size change.
     */
    IMPORT_C virtual void SizeChanged();

private: // Data

    /**
     * Destruct key
     */
    TUid iDestructKey;
    
    /**
     * Implementation UID
     */
    TUid iImplUid;
    };


#endif // XNEXTRENDERINGPLUGINADAPTER_H

// End of file
