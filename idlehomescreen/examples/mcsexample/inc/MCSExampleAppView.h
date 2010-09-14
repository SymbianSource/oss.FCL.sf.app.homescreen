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
* Description:
*
*/

#ifndef __MCSEXAMPLEAPPVIEW_h__
#define __MCSEXAMPLEAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>
#include <aknform.h>
#include <gulicon.h>
#include <aknlists.h>
#include <eiklbo.h> // MEikListBoxObserver
#include <mcsmenu.h>

class CMCSExampleWatcher;

struct TMyData
    {
public:     // data
    TBuf<256> majorID;
    TBuf<256> minorID;
    TBuf<256> iconFile;
    TBuf<256> iconID;
    TBuf<256> maskID;
    TBuf<256> statusIcon;
    TBool updated;
    };

class CMyForm : public CAknForm
    {
public:     // construction
    CMyForm( TMyData& aData );
    virtual ~CMyForm();
public:     // from CAknForm
    //void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    //void ProcessCommandL( TInt aCommandId );
    TBool SaveFormDataL();
    void DoNotSaveFormDataL();
    //void AddItemL() ;
    void PreLayoutDynInitL();
    //MEikDialogPageObserver::TFormControlTypes
    //    ConvertCustomControlTypeToBaseControlType( TInt aControlType ) const;
protected:
    TBool OkToExitL( TInt aButtonId );
    
private:
    void LoadFormDataL();
private:    // data
    TMyData& iData;
    };



// CLASS DECLARATION
class CMCSExampleAppView : public CCoeControl, public MEikListBoxObserver
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a CMCSExampleAppView object, which will draw itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CMCSExampleAppView.
     */
    static CMCSExampleAppView* NewL(const TRect& aRect);

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CMCSExampleAppView object, which will draw itself
     * to aRect.
     * @param aRect Rectangle this view will be drawn to.
     * @return A pointer to the created instance of CMCSExampleAppView.
     */
    static CMCSExampleAppView* NewLC(const TRect& aRect);

    /**
     * ~CMCSExampleAppView
     * Virtual Destructor.
     */
    virtual ~CMCSExampleAppView();

    void ListAllItemsL();
    
public:
    // Functions from base classes

    /**
     * From CCoeControl, Draw
     * Draw this CMCSExampleAppView to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw(const TRect& aRect) const;

    /**
     * From CoeControl, SizeChanged.
     * Called by framework when the view size is changed.
     */
    virtual void SizeChanged();

    /**
     * From CoeControl, HandlePointerEventL.
     * Called by framework when a pointer touch event occurs.
     * Note: although this method is compatible with earlier SDKs, 
     * it will not be called in SDKs without Touch support.
     * @param aPointerEvent the information about this event
     */
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
    CCoeControl* ComponentControl( TInt aIndex) const;
    
    TInt CountComponentControls() const;
    
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CMCSExampleAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL(const TRect& aRect);

    /**
     * CMCSExampleAppView.
     * C++ default constructor.
     */
    CMCSExampleAppView();
    
    CDesCArray* GetArrayL(CArrayPtr<CGulIcon>* aIcons);
    void MakeListBoxL(void);
    void UpdateScrollBar(CEikListBox* aListBox);
    void ShowFormL( TInt aItemIndex);
    
    RMenu iMCSMenu;
    CAknSingleLargeStyleListBox* iListBox;
    RArray<TMenuItem> iItemArray;
    /** MCS asynchronous operation watcher, owned */
    CMCSExampleWatcher* iWatcher;
    };

#endif // __MCSEXAMPLEAPPVIEW_h__
// End of File
