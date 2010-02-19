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
* Description:  Implementation wrapper for CEikEdwin
*
*/

// System includes
#include <e32base.h>
#include <e32const.h>
#include <e32property.h>

#include <eikedwin.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <aknview.h>
#include <aknedsts.h>
#include <txtglobl.h>
#include <txtfmlyr.h>
#include <txtfrmat.h>
#include <txtrich.h>
#include <gdi.h>

#include <activeidle2domainpskeys.h>

// User includes
#include "xnappuiadapter.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xndomproperty.h"
#include "xnproperty.h"
#include "xnnodepluginif.h"
#include "xncontroladapter.h"
#include "xntexteditoradapter.h"
#include "xntexteditorpublisher.h"
#include "xnnodepluginif.h"
#include "xneditmode.h"
#include "c_xnutils.h"
#include "xntexteditor.h" 

const TInt KMaxLength = 100;

_LIT8( KCpsPublishing, "cpspublishing" );
_LIT8( KMaxLineAmount, "max-line-amount" );
_LIT8( KMaxCharAmount, "max-char-amount" );
_LIT8( KEnablePartialInput, "splitinputenabled" );

_LIT( KEnterChar, "\x2029" );

// Local macros
#define IS_ARROW_KEY( k ) \
    ( k == EStdKeyLeftArrow || k == EStdKeyRightArrow || \
      k == EStdKeyUpArrow || k == EStdKeyDownArrow ) 

// ============================ LOCAL FUNCTIONS ================================   

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnTextEditorAdapter* CXnTextEditorAdapter::NewL( CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode )
    {
	CXnTextEditorAdapter* self = 
        new( ELeave ) CXnTextEditorAdapter( aParent, aNode );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;	
    }
    
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::CXnTextEditorAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnTextEditorAdapter::CXnTextEditorAdapter( CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode )
    : iParent( aParent ), iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::~CXnTextEditorAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnTextEditorAdapter::~CXnTextEditorAdapter()
    {    
    if ( iAvkonAppUi )
        {
        iAvkonAppUi->RemoveFromStack( iEditor );
        }
    
    if( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev( iCoeEnv->ScreenDevice() );
        dev->ReleaseFont( iFont );
        }

    delete iEditorPublisher;
    delete iEditor;   
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::ConstructL()
    {
    CXnControlAdapter::ConstructL( iNode );    
    iUiEngine = iNode.UiEngineL();

    // Max line amount
    iMaxLines = 0;
    
    CXnProperty* maxlinesProp( iNode.GetPropertyL( KMaxLineAmount ) );
        
    if ( maxlinesProp )
        {
        iMaxLines = maxlinesProp->FloatValueL();
        }

    // Max char amount
    TInt maxChars( KMaxLength );
    
    CXnProperty* maxcharsProp( iNode.GetPropertyL( KMaxCharAmount ) );    
    
    if ( maxcharsProp )
        {
        maxChars = maxcharsProp->FloatValueL();
        }
    
    iEditor = new ( ELeave ) CEikEdwin;

    iEditor->SetContainerWindowL( *iParent );
    iEditor->ConstructL( 0, maxChars, maxChars, iMaxLines );
    
    // Set allowed input modes
    iEditor->SetAknEditorAllowedInputModes( EAknEditorAllInputModes );

    // Set the default input mode
    iEditor->SetAknEditorInputMode( 
        EAknEditorTextInputMode | EAknEditorNumericInputMode );
                                    
    // Set allowed case modes
    iEditor->SetAknEditorPermittedCaseModes( EAknEditorAllCaseModes );

    // Set the default case mode
    iEditor->SetAknEditorCase( EAknEditorTextCase );
        
    iEditor->AddFlagToUserFlags( 
        CEikEdwin::ENoAutoSelection | CEikEdwin::EAllowUndo );
                                     
    // Set numeric keymap
    iEditor->SetAknEditorNumericKeymap( EAknEditorPlainNumberModeKeymap );
    
    iEditor->SetSuppressBackgroundDrawing( ETrue );

    // Enable partial Screen
    CXnProperty* enablepartialinput( iNode.GetPropertyL( KEnablePartialInput ) );
    iPartialInputEnabled = EFalse;
    iPartialInputOpen = EFalse;
     
    if ( enablepartialinput && 
         enablepartialinput->StringValue() == XnPropertyNames::KTrue )
        {
        iEditor->SetAknEditorFlags( EAknEditorFlagEnablePartialScreen );
        iPartialInputEnabled = ETrue;
        }
    
    iEditor->SetObserver( this );
                                        
    // Default not focused                                  
    iEditor->SetFocus( EFalse );                              
        
    SetPropertiesL();
	}
 
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::CountComponentControls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt CXnTextEditorAdapter::CountComponentControls() const
    {
    if( iEditor )
        {
        return 1;
        }
        
    return 0;        
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::ComponentControl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CCoeControl* CXnTextEditorAdapter::ComponentControl( TInt aIndex ) const
    {
    if( aIndex == 0 )
        {
        return iEditor;
        }
        
    return NULL;        
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::SizeChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CXnTextEditorAdapter::SizeChanged()
    {        
    iEditor->SetRect( iNode.Rect() );  
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TKeyResponse CXnTextEditorAdapter::OfferKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse ret( EKeyWasNotConsumed );

    if ( !IsFocused() )
        {
        return ret;
        }

    ret = iEditor->OfferKeyEventL( aKeyEvent, aType );                    

    if ( IS_ARROW_KEY( aKeyEvent.iScanCode ) )    
        {                        
        if ( ret == EKeyWasNotConsumed && aType == EEventKey )
            {            
            iRefusesFocusLoss = EFalse;
            
            ret = CXnControlAdapter::OfferKeyEventL( aKeyEvent, aType );
            }                    
        }

    if ( aKeyEvent.iCode == EKeyEnter )
        {
        if ( iEditorPublisher && iMaxLines == 1 )
            {
            TInt length( iEditor->TextLength() );
            
            HBufC* content = HBufC::NewLC( length + 1 );
            TPtr ptr( content->Des() );
            
            iEditor->GetText( ptr );
            
            ptr.Append( KEnterChar );
            
            iEditorPublisher->PublishTextL( *content );
            
            CleanupStack::PopAndDestroy( content );
            }
        }

    return ret;        
    }
 
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::RefusesFocusLoss
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TBool CXnTextEditorAdapter::RefusesFocusLoss() const
    {    
    return iRefusesFocusLoss;
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::FocusChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CXnTextEditorAdapter::FocusChanged( TDrawNow aDrawNow )
    {
    CXnAppUiAdapter* appui( 
        static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );
    
    TBool isFocused( IsFocused() ? ETrue : EFalse );
      
    TInt value;

    if ( isFocused )
        {      
        value = EPSAiDontForwardNumericKeysToPhone;

        if( !iPartialInputEnabled )
            {
            TRAP_IGNORE( appui->AddToStackL( appui->View(), iEditor ) );            
            // AddToStackL calls iEditor->SetFocus( ETrue ); 
            }

        }
    else
        {
        value = EPSAiForwardNumericKeysToPhone;
                                      
        if( !iPartialInputEnabled )
            {    
            appui->RemoveFromStack( iEditor );            
            iEditor->SetFocus( EFalse, aDrawNow );
            }
        else if(iPartialInputEnabled && iRemoveSplitInputFromStack )
            {
            appui->RemoveFromStack( iEditor );            
            iEditor->SetFocus( EFalse, aDrawNow );
            iPartialInputOpen = EFalse;
            iRemoveSplitInputFromStack = EFalse;
            }
            
        }

    if(iPartialInputOpen)
        {
        value = EPSAiDontForwardNumericKeysToPhone;
        }

    iRefusesFocusLoss = isFocused;
    
    RProperty::Set( KPSUidAiInformation,            
                    KActiveIdleForwardNumericKeysToPhone,
                    value );    
    }
    
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::Draw
// Draws the editor component
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::Draw( const TRect& aRect ) const
    {                           
    CXnControlAdapter::Draw( aRect );
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::HandleControlEventL
// 
// -----------------------------------------------------------------------------
//    
void CXnTextEditorAdapter::HandleControlEventL( CCoeControl* aControl, 
    TCoeEvent aEventType )
    {
    if ( aControl == iEditor )
        {
        // If background drawing is suppressed, then we need to call draw here
        if ( aEventType == EEventStateChanged )
            {
            DrawNow();                                    
            }        
        }
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::SetTextL
// Sets the new content to the underlying CEikEdwin
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::SetTextL( const TDesC& aText )
    {
    iEditor->SetTextL( &aText );
    iNode.SetDirtyL();
    }
    
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::Text
// Returns the text contained in the underlying CEikEdwin
// -----------------------------------------------------------------------------
//
HBufC* CXnTextEditorAdapter::Text() const
    {
    HBufC* text( NULL );
    
    TRAP_IGNORE( text = iEditor->GetTextInHBufL() );
    
    // Ownership is transfered to the calller
    return text;
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::HandleEditorEvent
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::HandleEditorEvent( TInt aReason )
    {
    CXnAppUiAdapter* appui( 
        static_cast< CXnAppUiAdapter* >( iAvkonAppUi ) );

    switch( aReason )
            {           
            case CXnTextEditor::KActivateTextEditor:
                {
                if( !iPartialInputOpen )
                     {
                     iUiEngine->EnablePartialTouchInput(iNode , ETrue);
                     TRAP_IGNORE( appui->AddToStackL( appui->View(), iEditor ) ); 
                     iPartialInputOpen = ETrue;
                     }  
                break;
                }
            case CXnTextEditor::KDeactivateTextEditor:
                {
                if( iPartialInputOpen )
                     {
                     iUiEngine->EnablePartialTouchInput(iNode, EFalse);
                     iPartialInputOpen = EFalse;
                     appui->RemoveFromStack( iEditor );
                     iEditor->SetFocus( EFalse );
                     }              
                break;
                }
            case CXnTextEditor::KRemoveSplitInputFromStack:
                {
                iRemoveSplitInputFromStack = ETrue;
                break;
                }
            case CXnTextEditor::KKeepSplitInputInStack:
                {
                iRemoveSplitInputFromStack = EFalse;
                break;
                }
            default:                    
                break;    
            }
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::SetPropertiesL
// Sets text properties
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::SetPropertiesL()
    {    
    // Whether to publish text to CPS
    CXnProperty* cpspublishingProp( iNode.GetPropertyL( KCpsPublishing ) );
    CXnProperty* idProp( iNode.IdL() );
    
    if ( cpspublishingProp && idProp && 
        cpspublishingProp->StringValue() == XnPropertyNames::KTrue )
        {
        iEditorPublisher = CXnTextEditorPublisher::NewL( 
                *this, idProp->StringValue() );
        }    

    // Store current state
    if ( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev( iCoeEnv->ScreenDevice() );
        dev->ReleaseFont( iFont ); 
        iFont = NULL;
        }
    
    // Get new font
    CXnUtils::CreateFontL( iNode, iFont, iReleaseFont );
    
    // And set font
    TCharFormat cf;
    TCharFormatMask cfm;
    cfm.SetAttrib( EAttColor );
    cfm.SetAttrib( EAttFontTypeface );
    cfm.SetAttrib( EAttFontHeight );
    cfm.SetAttrib( EAttFontPosture );
    cfm.SetAttrib( EAttFontStrokeWeight );
    cfm.SetAttrib( EAttLineSpacing );
    cfm.SetAttrib( EAttLineSpacingControl );
    cf.iFontSpec = iFont->FontSpecInTwips();

    // Set linespacing    
    CXnProperty* lineSpaceProp( 
        iNode.GetPropertyL( XnPropertyNames::appearance::common::KFontLineSpace ) );
        
    if ( lineSpaceProp )
        {
        TInt lineSpace = 
            iUiEngine->VerticalPixelValueL( lineSpaceProp, iNode.Rect().Height() );
        
        CParaFormatLayer*pFormatLayer = CEikonEnv::NewDefaultParaFormatLayerL();
        CleanupStack::PushL(pFormatLayer);
        CParaFormat* paraformat = CParaFormat::NewLC(); 
        TParaFormatMask paraFormatMask; 
        paraFormatMask.SetAttrib(EAttLineSpacing);
        paraFormatMask.SetAttrib(EAttLineSpacingControl);
        paraformat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips;
        CGraphicsDevice* screenDevice = iEikonEnv->ScreenDevice();
        TInt lineHeight = screenDevice->VerticalPixelsToTwips( iFont->HeightInPixels() + lineSpace );
        paraformat->iLineSpacingInTwips = lineHeight;
        pFormatLayer->SetL(paraformat, paraFormatMask);
        iEditor->SetParaFormatLayer(pFormatLayer); // Edwin takes the ownership
        CleanupStack::PopAndDestroy(paraformat);
        CleanupStack::Pop(pFormatLayer); 
        }
 
    // Get text color and set it
    TRgb textColor;
    CXnProperty* colorProperty( 
        iNode.GetPropertyL( XnPropertyNames::appearance::common::KColor ) );
    
    if ( colorProperty )
        {
        CXnDomProperty* domProperty( colorProperty->Property() );
        
        if ( domProperty )
            {
            TInt error( KErrNotSupported );
            
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >( 
                domProperty->PropertyValueList().Item( 0 ) );

            if ( value->IsAutoIdent() )
                {
                MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                error = AknsUtils::GetCachedColor(skinInstance, textColor, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6);
                }
            else if ( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
                {
                textColor = value->RgbColorValueL();
                error = KErrNone;
                }
            else
                {
                HBufC* colorString = colorProperty->StringValueL();
                CleanupStack::PushL( colorString );
                CXnUtils::StripQuotes( colorString );
                TInt index = 0;
                TAknsItemID skinID;
                TBool idResolved = CXnUtils::ResolveSkinItemIDL( colorString->Des(), skinID, index );
                
                if ( idResolved )
                    {
                    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                    error = AknsUtils::GetCachedColor( skinInstance, textColor, skinID, index );
                    }
                else // use auto value if skin id is invalid.
                    {
                    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                    error = AknsUtils::GetCachedColor(skinInstance, textColor, KAknsIIDQsnTextColors,
                    EAknsCIQsnTextColorsCG6);
                    }
                
                CleanupStack::PopAndDestroy( colorString );                 
                }
            
            if ( error == KErrNone )
                {
                cf.iFontPresentation.iTextColor = textColor;
                }
            else
                {
                cf.iFontPresentation.iTextColor = KRgbBlack;                
                }
            }
        }    
    
    CCharFormatLayer *pCharFL = CCharFormatLayer::NewL(cf,cfm);
    iEditor->SetCharFormatLayer(pCharFL);
    iEditor->SetTextBaselineSpacing( 2 );
    }

// End of file
