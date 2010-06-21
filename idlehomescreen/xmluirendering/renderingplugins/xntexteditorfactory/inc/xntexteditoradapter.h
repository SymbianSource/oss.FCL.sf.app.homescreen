/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for CEikEdwin
*
*/

#ifndef _XNTEXTEDITORADAPTER_H
#define _XNTEXTEDITORADAPTER_H

//  System includes
#include <e32base.h>

// User includes
#include "xncontroladapter.h"
#include "xnuienginepluginif.h"

// Forward declarations
class CXnNodePluginIf;
class CEikEdwin;
class CXnTextEditorPublisher;
class CXnAppUiAdapter;

// CLASS DECLARATION
class CXnTextEditorAdapter : public CXnControlAdapter, public MCoeControlBackground
    {
public:
    // constructor and destructor
    
    /**
     * 2 phase construction.
     */
    static CXnTextEditorAdapter* NewL( 
        CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
    
    /**
     * Destructor.
     */
    ~CXnTextEditorAdapter();

public: 
    // New functions

    /**
     * Gets editor
     * 
     * @since S60 5.0
     * @return Editor, ownership is not transfered
     */
    CEikEdwin* Editor() const { return iEditor; };

    /**
     * Set text to the text field
     * 
     * @since S60 5.0
     * @param aText Text to set     
     */
    void SetTextL( const TDesC& aText );
    
    /**
     * Return the text of the text field
     * 
     * @since S60 5.0
     * @return Text, ownership is transfered      
     */
    HBufC* Text() const;

    /**
     * Handles editor events
     * 
     * @since S60 5.2
     * @param aReason, editor event        
     */
    void HandleEditorEvent( TInt aReason );

public: // from base classes

    /**
    * See CCoeControl documentation
    */    	
    TInt CountComponentControls() const;

    /**
    * See CCoeControl documentation
    */    	
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * See CCoeControl documentation
    */    	
    void SizeChanged();

    /**
    * See CCoeControl documentation
    */    	
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);    
    
    /**
    * See CXnControlAdapter documentation
    */          
    TBool RefusesFocusLoss() const;
    
    /**
    * See CCoeControl documentation
    */    	
    void FocusChanged( TDrawNow aDrawNow );

    /**
    * See CCoeControl documentation
    */          
    void Draw( const TRect& aRect ) const;

    /**
    * See CCoeControl documentation
    */          
    void HandleResourceChange( TInt aType );
    
    /**
    * See CCoeControl documentation
    */  
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * See CCoeAppUiBase.
     */
    void HandleScreenDeviceChangedL();

private: // from base classes

    /**
    * See MCoeControlBackground documentation
    */          
    void Draw( CWindowGc& aGc, const CCoeControl& aControl, 
        const TRect& aRect ) const;

private:
    // private constrcutors 

	CXnTextEditorAdapter( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
	
	void ConstructL();

private:
    // new functions
                 
    void SetPropertiesL();
    
    void SetEditorMarginPropertiesL();

private:
    // data
    /** Parent control, not owned */
    CXnControlAdapter*  	iParent;
    /** UI node, not owned */
    CXnNodePluginIf&    	iNode; 
    /** UI engine, not owned */
    TXnUiEnginePluginIf*    iUiEngine;
    /** Appui adapter, not owned */
    CXnAppUiAdapter*        iAppui;
    /** Editor, owned */
    CEikEdwin*              iEditor; 
    /** CPS publisher wrapper, owned */
    CXnTextEditorPublisher* iEditorPublisher;
    // Font, owned */
    CFont*                  iFont;  
    /** Flag to indicate whether the font needs to be released or not */
    TBool                   iReleaseFont;
    /** Max line amount */
    TInt                    iMaxLines; 
    /** Flag to indicate whether focus loss is refused */
    TBool                   iRefusesFocusLoss;
    /** Split input states*/
    TInt                    iSplitInputFlags;
    };

#endif      // _XNTEXTEDITORADAPTER_H
            
// End of File
