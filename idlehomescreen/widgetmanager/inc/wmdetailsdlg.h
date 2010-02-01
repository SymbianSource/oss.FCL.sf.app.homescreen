/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Declares widget details dialog for WidgetManager
*
*/

#ifndef ___WMDETAILSDLG_H__
#define ___WMDETAILSDLG_H__

// INCLUDES
#include <e32base.h>
#include <AknDialog.h>

// FORWARD DECLARATIONS
class CAknMarqueeControl;
class CEikRichTextEditor;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATIONS
/**
 * CWmDetailsDlg
 */
NONSHARABLE_CLASS( CWmDetailsDlg ): public CAknDialog
	{
public:
    
    /**
     * Two-phased constructor.
     */
	static CWmDetailsDlg* NewL(
	        const TDesC& aName,
	        const TDesC& aDescription,
	        TBool  aCanBeAdded,
            const CFbsBitmap* aLogoBmp,
            const CFbsBitmap* aLogoMask );

    /** Destructor. */
    ~CWmDetailsDlg();

public: // New functions
    /** From CAknDialog Initializes the dialog and calls
        CAknDialog's ExecuteLD() */
    TInt ExecuteLD();

private: // Functions from base classes
	
	/** From CEikDialog Initializes dialog before layout*/
	void PreLayoutDynInitL();
	
	/** From CEikDialog Handles button events and looks 
		if the dialog can be closed */
	TBool OkToExitL(TInt aButtonId);
	
	/** From CCoeControl Handles key events */
	TKeyResponse OfferKeyEventL( 
	        const TKeyEvent& aKeyEvent,TEventCode aType );

	/** From CEikDialog Called by framework when dialog is activated */
	void ActivateL();

	/** From CAknDialog called to provide access to the contorols */	
	TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
	
	/** From CAknDialog */
    void SetSizeAndPosition( const TSize& aSize );

	/** From CCoeControl */
	void SizeChanged();
    
    /** From CCoeControl */
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw( const TRect& aRect ) const;
    void FocusChanged( TDrawNow aDrawNow );

private: // new functions
    
    /** Insert and format content */
    void InsertAndFormatContentL();
    
    /** Redraw call back for marque control */
    static TInt RedrawCallback( TAny* aPtr );
    
    /** start marque control */
    void StartMarquee();
    
    /** stop marque control */
    void StopMarquee();

private:

  	 /** Constructor for performing 1st stage construction */
    CWmDetailsDlg( TBool  aCanBeAdded );

    /** 2nd phase constructor */
    void ConstructL(
	        const TDesC& aName,
	        const TDesC& aDescription,
            const CFbsBitmap* aLogoBmp,
            const CFbsBitmap* aLogoMask );

private:
	
	/**
     * Background context (not owned)
     */
	CAknsBasicBackgroundControlContext* iBgContext;
	
	/**
     * Richtext editor
     */
	CEikRichTextEditor* iRtEditor;

    /**
     * Marquee control
     */
	CAknMarqueeControl* iMarquee;

	/** 
     * Name of widget 
     */
    HBufC*              iName;

	/** 
     * Description of widget 
     */
    HBufC*              iDescription;

	/** 
     * Logo image
     */
    CFbsBitmap*         iLogoBmp;

	/** 
     * Logo mask
     */
    CFbsBitmap*         iLogoMask;

    /** 
     * ETrue if widget can be added to HS 
     */
    TBool               iCanBeAdded;

    /** 
     * ETrue if widget name needs to be scrolled. 
     */
    TBool               iNeedToScroll;
	};

#endif ___WMDETAILSDLG_H__

// End of File
