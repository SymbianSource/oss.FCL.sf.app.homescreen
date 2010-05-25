/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View control adapter
*
*/

#ifndef _XNVIEWCONTROLADAPTER_H
#define _XNVIEWCONTROLADAPTER_H

//  System includes
#include <e32base.h>

// User includes
#include "xncontroladapter.h"
#include "xnuistatelistener.h"

// Forward declarations
class CXnNodePluginIf;
class CXnAppUiAdapter;

// Class declaration
/**
*  @ingroup group_xnlayoutengine
*  @lib xn3layoutengine.lib
*  @since S60 5.2
*/   
NONSHARABLE_CLASS( CXnViewControlAdapter ) : public CXnControlAdapter,
    public MXnUiStateObserver
   {
public: 
    // Constructors and destructor
    
    /**
     * 2 phase construction.
     */
    static CXnViewControlAdapter* NewL( CXnNodePluginIf& aNode );

    /**
     * Destructor.
     */
    ~CXnViewControlAdapter();

public: 
    // From base classes

    /**
    * @see CCoeControl documentation        
    */         
    void MakeVisible( TBool aVisible );
    
    /**
     * @see CCoeControl
     */
    void Draw( const TRect& aRect ) const;
            
    /**
     * @see CCoeControl
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

public:
    // new functions
    
    /**
     * Resets pointer grabbing
     * 
     * @since S60 5.2
     */
    void ResetGrabbing();
            
private:
    // from MXnUiStateObserver
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyForegroundChanged( TForegroundStatus aStatus );
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyLightStatusChanged( TBool aLightsOn );
    
    /**
     * @see MXnUiStateObserver
     */
    void NotifyInCallStateChaged( TBool aInCall );   
    
private: 
    // constructors
    
    /**
     * C++ default constructor
     */    
    CXnViewControlAdapter( CXnNodePluginIf& aNode );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL( CXnNodePluginIf& aNode );
    
private: 
    // data

    /** Node */
    CXnNodePluginIf& iNode;
    /** AppUi */
    CXnAppUiAdapter& iAppUi;
    };
    
#endif // _XNVIEWCONTROLADAPTER_H
            
// End of File
