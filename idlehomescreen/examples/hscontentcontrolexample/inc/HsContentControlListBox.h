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
* Description:  Content Control example
*
*/


#ifndef __HSCONTENTCONTROLLISTBOX_H__
#define __HSCONTENTCONTROLLISTBOX_H__

//  Includes
#include <coecntrl.h>        
#include <akntreelistobserver.h>
#include <AknTabObserver.h>
#include <hscontentcontrol.h>

//  Forward declarations
class MEikCommandObserver;        
class CAknSingleStyleTreeList;
class CAknsBasicBackgroundControlContext;
class CAknNavigationDecorator;
class CAknTabGroup;
class CAknStylusPopUpMenu;
class CHsCcApiClient;
class CHsContentInfoArray;

/**
 * Container class for HsContentControlListBox
 * 
 * @class    CHsContentControlListBox HsContentControlListBox.h
 */
class CHsContentControlListBox : public CCoeControl,     
    public MAknTreeListObserver,
    public MAknTabObserver,
    public MEikMenuObserver,
    public MHsContentControl    
    {
public:
    // constructors and destructor
    
    /**
     * Two-phased constructor.
     */    
    static CHsContentControlListBox* NewL( 
        const TRect& aRect, 
        const CCoeControl* aParent, 
        MEikCommandObserver* aCommandObserver );

    /**
     * Two-phased constructor.
     */    
    static CHsContentControlListBox* NewLC( 
        const TRect& aRect, 
        const CCoeControl* aParent, 
        MEikCommandObserver* aCommandObserver );
        
    /**
     * Destructor
     */    
    ~CHsContentControlListBox();

public:
    // from CCoeControl
    
    /**
     * @see CCoeControl
     */            
    TInt CountComponentControls() const;

    /**
     * @see CCoeControl
     */                
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
    /**
     * @see CCoeControl
     */                
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType ); 
    
    /**
     * @see CCoeControl
     */                
    void HandleResourceChange( TInt aType );

    /**
     * @see CCoeControl
     */                
    void SizeChanged();

    /**
     * @see CCoeControl
     */                
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
            
private:
   // from MAknTreeListObserver

    /**
     * @see MAknTreeListObserver
     */
    TInt HandleTreeListEvent(       
        CAknTreeList& aList,
        TAknTreeItemID aItem,
        TEvent aEvent );
    
private:    
    // From MEikMenuObserver

    /**
     * @see MEikMenuObserver
     */
    void ProcessCommandL( TInt aCommandId );
    
    /**
     * @see MEikMenuObserver
     */
    void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );
    
private:
    // from MAknTabObserver
    
    /**
     * @see MAknTabObserver
     */    
    void TabChangedL( TInt aIndex );
    
private:
    // from MHsContentControl
    
    /**
     * @see MHsContentControl
     */
    void NotifyWidgetListChanged();

    /**
     * @see MHsContentControl
     */
    void NotifyViewListChanged();

    /**
     * @see MHsContentControl
     */
    void NotifyAppListChanged();
          
private:
    // new functions
    
    void InitializeControlsL();    
    void LayoutControls();
    void PopulateListL();

    static TInt PopulateList( TAny* aAny );
    
private:
    // constructors

    /**
     * C++ default constructor
     */    
    CHsContentControlListBox();

    /** 
     * 2nd phase constructor
     */
    void ConstructL( 
        const TRect& aRect, 
        const CCoeControl* aParent, 
        MEikCommandObserver* aCommandObserver );
    
private:
    // data

    /** Homescreen content control client, owned */
    CHsCcApiClient* iHsCcApiClient;    
    /** Callback, owned */
    CAsyncCallBack* iCallback;
    /** Popup menu, owned */
    CAknStylusPopUpMenu* iPopup;    
    /** List box, owned */
    CAknSingleStyleTreeList* iListBox;
    /** Bg context, owned */
    CAknsBasicBackgroundControlContext* iBgContext;
    /** Content info array, owned */
    CHsContentInfoArray* iArray;
    /** Navigation decorator, owned */
    CAknNavigationDecorator* iDecorator;
    /** Tab group, not owned */
    CAknTabGroup* iTabGroup;   
    /** Selected tree item */
    TAknTreeItemID iSelectedItem;    
    /** Active tab index */
    TInt iTabIndex;
    };
                
#endif // __HSCONTENTCONTROLLISTBOX_H__
