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

// INCLUDE FILES
#include <coemain.h>
#include <mcsmenuitem.h>
#include <mcsmenuiconutility.h>
#include <mcsdef.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <eikedwin.h>

#include <aknsconstants.h>
#include <aknsconstants.hrh>

#include <MCSExample_0xEB753B18.rsg>
#include "MCSExampleAppView.h"
#include "mcspluginwatcher.h"
#include "MCSExample.hrh"

_LIT( KMenuData, "matrixmenudata" );

CMyForm::CMyForm( TMyData& aData ) : iData( aData )
    {
    }

CMyForm::~CMyForm()
    {
    }
        
void CMyForm::PreLayoutDynInitL()
    {
    CAknForm::PreLayoutDynInitL();
    LoadFormDataL();
    }

void CMyForm::LoadFormDataL()
    {
    CEikEdwin* textEd1 =
        (CEikEdwin*)Control( EMyFirstLine );
    textEd1->SetTextL( &iData.majorID );
    
    CEikEdwin* textEd2 =
        (CEikEdwin*)Control( EMySecondLine );
    textEd2->SetTextL( &iData.minorID );
    
    CEikEdwin* textEd3 =
        (CEikEdwin*)Control( EMyThirdLine );
    textEd3->SetTextL( &iData.iconFile );
    
    CEikEdwin* textEd4 =
        (CEikEdwin*)Control( EMyFourthLine );
    textEd4->SetTextL( &iData.iconID );
    
    CEikEdwin* textEd5 =
        (CEikEdwin*)Control( EMyFifthLine );
    textEd5->SetTextL( &iData.maskID );
    
    CEikEdwin* textEd6 =
        (CEikEdwin*)Control( EMySixthLine );
    textEd6->SetTextL( &iData.statusIcon );
    }

TBool CMyForm::SaveFormDataL()
    {
    CEikEdwin* textEd1 =
        (CEikEdwin*)Control( EMyFirstLine );
    textEd1->GetText( iData.majorID );
    
    CEikEdwin* textEd2 =
        (CEikEdwin*)Control( EMySecondLine );
    textEd2->GetText( iData.minorID );
    
    CEikEdwin* textEd3 =
        (CEikEdwin*)Control( EMyThirdLine );
    textEd3->GetText( iData.iconFile );
    
    CEikEdwin* textEd4 =
        (CEikEdwin*)Control( EMyFourthLine );
    textEd4->GetText( iData.iconID );
    
    CEikEdwin* textEd5 =
        (CEikEdwin*)Control( EMyFifthLine );
    textEd5->GetText( iData.maskID );
    
    CEikEdwin* textEd6 =
        (CEikEdwin*)Control( EMySixthLine );
    textEd6->GetText( iData.statusIcon );
    return ETrue;
    }

void CMyForm::DoNotSaveFormDataL()
    {
    LoadFormDataL();
    }

TBool CMyForm::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId  == EAknSoftkeyOk)
        {
        SaveFormDataL();
        iData.updated = ETrue;
        }
    else if( aButtonId == EAknSoftkeyCancel )
        {
        iData.updated = EFalse;
        }
    return ETrue;
    }

CGulIcon* ConvertToGulIconLC( CAknIcon* aIcon )
    {
    CGulIcon* icon = CGulIcon::NewL( aIcon->Bitmap(), aIcon->Mask() );
    CleanupStack::PushL( icon );
    // Detach bitmap and mask
    aIcon->SetBitmap( NULL );
    aIcon->SetMask( NULL );
    return icon;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMCSExampleAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMCSExampleAppView* CMCSExampleAppView::NewL(const TRect& aRect)
    {
    CMCSExampleAppView* self = CMCSExampleAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMCSExampleAppView* CMCSExampleAppView::NewLC(const TRect& aRect)
    {
    CMCSExampleAppView* self = new (ELeave) CMCSExampleAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    
    iMCSMenu.OpenL( KMenuData );
    iWatcher = CMCSExampleWatcher::NewL();
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::CMCSExampleAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMCSExampleAppView::CMCSExampleAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::~CMCSExampleAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CMCSExampleAppView::~CMCSExampleAppView()
    {
    iMCSMenu.Close();
    delete iListBox;
    iItemArray.Close();
    delete iWatcher;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::ListAllItemsL()
    {
    

    CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
    CleanupStack::PushL(icons);
   
    iListBox->Model()->SetItemTextArray(GetArrayL(icons));
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    //
    // Inform the listbox of item additions. 
    // This should be done if you want your items displayed correctly
    iListBox ->HandleItemAdditionL();
 
    CleanupStack::Pop(icons);
    CArrayPtr<CGulIcon>* oldArray = iListBox->ItemDrawer()->ColumnData()->IconArray();
    delete oldArray;
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::MakeListBoxL()
    {
    TInt MySetIndex(0);
 
    if(iListBox)
    {
        MySetIndex = iListBox->CurrentItemIndex();
    }
 
    delete iListBox;
    iListBox = NULL;
 
    iListBox = new( ELeave ) CAknSingleLargeStyleListBox();
    iListBox->ConstructL(this,EAknListBoxSelectionList);
    iListBox->SetListBoxObserver( this );
    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
 
    iListBox->SetRect(Rect());
 
    iListBox->View()->SetListEmptyTextL(_L("No data"));
 
    iListBox->ActivateL();
 
    TInt ItemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
 
    if(ItemsCount > MySetIndex && MySetIndex >= 0)
        iListBox->SetCurrentItemIndex(MySetIndex);
    else if(ItemsCount > 0)
        iListBox->SetCurrentItemIndex(0);
 
    UpdateScrollBar(iListBox);
    iListBox->DrawNow();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CDesCArray* CMCSExampleAppView::GetArrayL(CArrayPtr<CGulIcon>* aIcons)
{   
    TBuf<256> listString;
    CDesCArrayFlat* myArray = new(ELeave)CDesCArrayFlat(1);
    CleanupStack::PushL(myArray);
 
    const TInt rootId = iMCSMenu. RootFolderL();
    iItemArray.Reset();
    //CleanupClosePushL( itemArray );
    // Get all the items from MCS
    iMCSMenu.GetItemsL( iItemArray, rootId, NULL, ETrue );

    for( TInt i = 0; i < iItemArray.Count(); i++ )
        {
        // Open the actual item
        CMenuItem* menuItem = CMenuItem::OpenL( iMCSMenu, iItemArray[i] );
        CleanupStack::PushL( menuItem );
        
        // Get the name of the item
        TBool attrExists( EFalse );
        TPtrC attr = menuItem->GetAttributeL(_L("short_name" ), attrExists );

        //Create listbox string
        listString.Delete(0, 256);
        listString.AppendNum( i );
        listString.Append( _L("\t"));
        listString.Append( attr );
        myArray->AppendL( listString );
        
        //Fetch the item icon from MCS
        CAknIcon* icon = MenuIconUtility::GetItemIconL( *menuItem );
        CleanupStack::PushL( icon );
        if ( icon )
            {
            CGulIcon* gIcon = ConvertToGulIconLC( icon );
            aIcons->AppendL( gIcon );
            CleanupStack::Pop( gIcon );
            }
        CleanupStack::PopAndDestroy( icon );
        CleanupStack::PopAndDestroy( menuItem );
        } 
    CleanupStack::Pop(myArray);
    return myArray;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect(Rect());

    // Clears the screen
    gc.Clear(drawRect);

    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::SizeChanged()
    {
    TRAP_IGNORE( MakeListBoxL() );
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CMCSExampleAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {

    // Call base class HandlePointerEventL()
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if( iListBox && aEventType == EEventItemSingleClicked )
        {
        TInt index = iListBox->CurrentItemIndex();
        if( index < iItemArray.Count())
            {
            ShowFormL( index );
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMCSExampleAppView::ShowFormL( TInt aItemIndex)
    {
    CMenuItem* menuItem = CMenuItem::OpenL( iMCSMenu, iItemArray[aItemIndex] );
    CleanupStack::PushL( menuItem );
    TBool attrExists( EFalse );
    TPtrC attr = menuItem->GetAttributeL(_L("short_name" ), attrExists );
    // Initialize  form data.
    TBool exists( EFalse );;
    TMyData myData;
    myData.majorID = menuItem->GetAttributeL( KMenuAttrIconSkinMajorId, exists );
    myData.minorID = menuItem->GetAttributeL( KMenuAttrIconSkinMinorId, exists );
    myData.iconFile = menuItem->GetAttributeL( KMenuAttrIconFile, exists );
    myData.iconID = menuItem->GetAttributeL( KMenuAttrIconId, exists );
    myData.maskID = menuItem->GetAttributeL( KMenuAttrMaskId, exists );
    // e.g. skin(0x101F86E3 0x1ee2)
    myData.statusIcon = menuItem->GetAttributeL( _L("presenceicon"), exists );
    myData.updated = EFalse;

    // Launch the dialog to view/edit data
    CAknForm* dlg = new ( ELeave ) CMyForm( myData );
    CleanupStack::PushL( dlg );
    dlg->ConstructL( 0 ); // default menu items only
    CleanupStack::Pop( dlg );
    dlg->ExecuteLD( R_ICON_QUERY_DIALOG );
    // myData now contains the edited values.

    if( myData.updated )
        {
        // Changes the main icon, if all values are empty then application icon is used.
        // Piority 0
        menuItem->SetAttributeL(KMenuAttrIconSkinMajorId, myData.majorID );
        menuItem->SetAttributeL(KMenuAttrIconSkinMinorId, myData.minorID );
        // Pririty 1
        menuItem->SetAttributeL(KMenuAttrIconFile, myData.iconFile );
        menuItem->SetAttributeL(KMenuAttrIconId, myData.iconID );
        menuItem->SetAttributeL(KMenuAttrMaskId, myData.maskID );
        //Changes the presence icon
        menuItem->SetAttributeL( _L("presenceicon"), myData.statusIcon );
                
        CMenuOperation* operation = menuItem->SaveL( iWatcher->iStatus );
        // Deletes the operation after its completion
        iWatcher->Watch( operation, this );
        }
    CleanupStack::PopAndDestroy( menuItem );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CCoeControl* CMCSExampleAppView::ComponentControl( TInt /*aIndex*/) const
{
    return iListBox;
}
 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CMCSExampleAppView::CountComponentControls() const
{   
    if(iListBox)
        return 1;
    else
        return 0;
}
// End of File
