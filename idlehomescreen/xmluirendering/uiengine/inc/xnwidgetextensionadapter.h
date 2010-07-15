/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Widget extension adapter
*
*/

#ifndef __XNWIDGETEXTENSIONADAPTER_H__
#define __XNWIDGETEXTENSIONADAPTER_H__

// System includes
#include <e32base.h>

// User includes
#include "xncontroladapter.h"
#include "xnuistatelistener.h"

// Forward declarations
class CXnAppUiAdapter;
class CXnNode;
class CXnUiEngine;

// CLASS DECLARATION

/**
*  @ingroup group_xnlayoutengine
*  Popup Control adapter interface class
*  @lib xnlayoutengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( CXnWidgetExtensionAdapter ) : public CXnControlAdapter, 
    public MXnUiStateObserver,
    public MXnUiResourceChangeObserver
    {
public: // Constructors and destructor

    enum TPositionHint
           {
           ENone,
           EAboveLeft,
           EAboveRight,
           EBelowLeft,
           EBelowRight,
           ELeft,
           ERight
           };
    /**
    * Two-phased constructor.
    */
    static CXnWidgetExtensionAdapter* NewL( CXnNodePluginIf& aNode );
    
    /**
    * Destructor.
    */
    ~CXnWidgetExtensionAdapter();
public:
    /**
     * From CCoeAppUiBase.
     * Informs the components about the screen layout switch.
     */
     void HandleScreenDeviceChangedL();

     
private: 
    // Functions from base classes    
    /**
    * @see CCoeControl documentation        
    */         
    void MakeVisible( TBool aVisible );

    /**
    * see CCoeControl 
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
    * see CCoeControl 
    */
    void Draw( const TRect& aRect ) const;        

    /**
     * From CCoeControl.
     * Size change notification
     */
    void SizeChanged();

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
    // from MXnUiResourceChangeObserver
    
    /**
     * @see MXnUiResourceChangeObserver
     */
    void NotifyStatusPaneSizeChanged();

    /**
     * @see MXnUiResourceChangeObserver
     */
    void NotifyResourceChanged( TInt aType );
    
private: 
    // Constructors
    /**
    * C++ default constructor.
    */
    CXnWidgetExtensionAdapter( CXnNodePluginIf& aNode );

    /**
    * Two-phased constructor.        
    */ 
    void ConstructL();	   
    
private: 
    // New functions
    
    void HidePopupL();

    void ChangePopupPosition();

    void CalculatePosition();
    
private:    
    // Data
    
    /** UiEngine, not owned */
    CXnUiEngine* iUiEngine; 	    
    /** Position hint */
    TPositionHint iPositionHint;
    /** Node, not owned */
    CXnNodePluginIf& iNode;    
    /** AppUi, not owned */
    CXnAppUiAdapter* iAppUiAdapter;
    /** Flag to indicate whether <popup> is permanent */
    TBool iPermanent;
    /** Is popup element */
    TBool iPopup;
	};

#endif // __XNWIDGETEXTENSIONADAPTER_H__

// End of File
