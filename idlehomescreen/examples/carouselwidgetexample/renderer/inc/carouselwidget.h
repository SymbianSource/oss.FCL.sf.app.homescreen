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
 * Description:  Carousel widget for Symbian Homescreen
 *
 */

#ifndef _CCAROUSELWIDGET_H
#define _CCAROUSELWIDGET_H

//  INCLUDES
#include <xnextrenderingpluginadapter.h>
#include <aknphysicsobserveriface.h>

class MXnExtEventHandler;
class CCarouselItem;
class CAknPhysics;

// Class declaration
NONSHARABLE_CLASS( CCarouselItem ) : public CBase  
    {
public:

    // Constructors
        
    /**
     * C++ default constructor.
     */
CCarouselItem(/* CCarouselEditor& aEditor,*/ CFbsBitmap* aBitmap, CFbsBitmap* aMask);
    
    /**
     * Destructor.
     */
    ~CCarouselItem();
    
    void SetTextL( const TDesC& aText );

    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;
private:
   
    HBufC* iText;
    };

// CLASS DECLARATION

/**
*  
*/
class CCarouselWidget : public CXnExtRenderingPluginAdapter, 
    public MAknPhysicsObserver
    {

public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    static CCarouselWidget* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CCarouselWidget();

public: // from base classes

    /**
    * From CCoeControl Handles key events.
    * @since Series 60 3.1
    * @param aKeyEvent Key event.
    * @param aType Event type.
    * @return Key response
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
    * From CCoeControl
    */
    void SetContainerWindowL( const CCoeControl &aContainer );

    /**
    * See CCoeControl documentation
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
    * From CCoeControl, CountComponentControls
    * @return Number of component controls
    */
    TInt CountComponentControls() const;

    /**
    * From CCoeControl, ComponentControl
    * @param aIndex index of component control
    * @return pointer to the specified control
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * Skin change notification.
    * See CXnControlAdapter documentation
    * @since Series 60 3.1
    */
    void SkinChanged();

    /**
    * See CXnControlAdapter documentation
    */
    void EnterPowerSaveModeL();

    /**
    * See CXnControlAdapter documentation
    */
    void ExitPowerSaveModeL();

    // From MAknPhysicsObserver
    /**
     * Physics emulation has moved the view.
     */
    void ViewPositionChanged( const TPoint& aNewPosition, 
                TBool aDrawNow, TUint aFlags );

    /**
     * Called when emulation ended.
     */
    void PhysicEmulationEnded();

    /**
     * Returns the observer view position.
     *
     * @return Physics observer view position.
     */
    TPoint ViewPosition() const;

protected: // from base classes

    /**
    * From CCoeControl
    * Called if focus changes
    */
    void FocusChanged( TDrawNow aDrawNow );

    /**
    * From CCoeControl
    * Called if position or size changes
    */
    void SizeChanged();
    
    /**
     * Sets the external event handler interface.
     *
     * @since Series 60 5.2
     * @param aEventHandler Event handler interface.
     */
    void SetEventHandler( MXnExtEventHandler* aEventHandler );
    
    /**
     * Routes the data stream for the external rendering plugin.
     *
     * @since Series 60 5.2
     * @param aData Data stream.
     * @param aType Type of the stream.
     * @param aIndex Index of the data.
     */
    void SetDataL( const TDesC8& aData, const TDesC& aType, TInt aIndex );

private: // from base classes

    /**
    * From CCoeControl, Draw
    * See CCoeControl documentation
    */
    void Draw( const TRect& aRect ) const;

public:

    /**
    * C++ default constructor.
    */
    CCarouselWidget();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    TRect ViewPort() {return iViewPort; };

private:
    void InitPhysicEngineL();
    
    TInt ItemIndex( TPoint& aPoint );
    
    void LaunchItemL( TPoint& aPosition );
    
private: // Member data
    /**
     * Physics. 
     * Own.
     */
    CAknPhysics*        iPhysics;
    
    MXnExtEventHandler* iEventHandler; // not own
    
    /**
     * Pointer down start time.
     */
    TTime               iStartTime;

    /**
     * Start (pointer down) position.
     */
    TPoint              iStartPosition;
    
    /**
     * Current pointer position.
     */   
    TPoint              iStylusPosition;
    
    /**
     * Current view position.
     */
    TPoint              iCurrentPosition;
    
    /**
     * Adjusted view position
     */
    TPoint iViewPosition;
    
    /**
     * Visible area, i.e. area of the scollable control that is to be drawn.
     */          
    TRect                           iViewPort;
    
    RPointerArray<CCarouselItem>    iStripeItems;
    
    TBool                           iDrawHighlight;
    };

#endif      // _CCAROUSELWIDGET_H
// End of File
