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
#include <babitflags.h>

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
    // new functions

    /**
     * Gets editor
     * 
     * @since S60 5.0
     * @return Editor, ownership is not transfered
     */
    CEikEdwin* Editor() const;

    /**
     * Set text to the text field
     * 
     * @since S60 5.0
     * @param aText Text to set     
     */
    void SetTextL( const TDesC& aText );
    
    /**
     * Queries wheter text content change is caused by call to SetTextL
     * 
     * @since S60 5.2
     * @return ETrue if set text, EFalse otherwise
     */
    TBool IsSetText() const;
        
    /**
     * Return the text of the text field
     * 
     * @since S60 5.0
     * @return Text, ownership is transfered      
     */
    HBufC* Text() const;

public: 
    // from base classes

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
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private: 
    // from MCoeControlBackground 

    /**
    * @see MCoeControlBackground 
    */          
    void Draw( CWindowGc& aGc, const CCoeControl& aControl, 
        const TRect& aRect ) const;

private:
    // private constrcutors 

    /**
     * C++ constructor
     */
	CXnTextEditorAdapter( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
	
	/**
	 * 2nd phase constructor
	 */
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
    /** Flags */
    TBitFlags32             iFlags;
    };

#endif      // _XNTEXTEDITORADAPTER_H
            
// End of File
