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
* Description:  External rendering plugin wrapper header
*
*/

#ifndef XNEXTRENDERINGPLUGINWRAPPER_H
#define XNEXTRENDERINGPLUGINWRAPPER_H

// System includes
#include <e32base.h>
#include <xnextrenderingpluginadapter.h>

// User includes
#include "xncontroladapter.h"
#include "xnproperty.h"
#include "xncomponent.h"
#include "xnnodepluginif.h"

/**
 *  CXnExtRenderingPluginWrapper class
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib extrenderingplugin.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CXnExtRenderingPluginWrapper ) : public CXnControlAdapter
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    static CXnExtRenderingPluginWrapper* NewL(
            CXnNodePluginIf& aNode, CXnExtRenderingPluginAdapter& aAdapter );

    /**
     * Destructor.
     */
    virtual ~CXnExtRenderingPluginWrapper();

private: // Constructors
    /**
     * Constructor
     */
    CXnExtRenderingPluginWrapper( CXnExtRenderingPluginAdapter& aAdapter );

    /** 
     * Second phase constructor 
     */
    void ConstructL( CXnNodePluginIf& aNode );

private:
    TInt CountComponentControls() const;
    
    CCoeControl* ComponentControl(TInt aIndex) const;    
    
    void HandleScreenDeviceChangedL();
    
    void MakeVisible(TBool aVisible);
    
    void FocusChanged(TDrawNow aDrawNow);

    void SetContainerWindowL(const CCoeControl& aContainer);
    
    void SkinChanged();    
    
    void SizeChanged();    
    
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    
public: // New functions
    
    /**
     * @see CXnControlAdapter
     */
    void DoEnterPowerSaveModeL( TModeEvent aEvent );
    
    /**
     * @see CXnControlAdapter
     */
    void DoExitPowerSaveModeL( TModeEvent aEvent );  

private: // Data

    /**
     * Rendering plugin adapter. Own.
     */
    CXnExtRenderingPluginAdapter* iAdapter;
    
    /**
     *  Node. Not own.
     */
    CXnNodePluginIf* iNode;
    };

#endif // XNEXTRENDERINGPLUGINFACTORY_H

// End of file
