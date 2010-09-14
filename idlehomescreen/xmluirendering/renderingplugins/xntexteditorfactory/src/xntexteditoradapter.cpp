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
#include <AknPriv.hrh>
#include <txtglobl.h>
#include <txtfmlyr.h>
#include <txtfrmat.h>
#include <txtrich.h>
#include <gdi.h>
#include <gulutil.h>

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

// Constants
enum EFlags
    {
    EUsePartialScreenInput = 1,
    EIsAlreadyFocused,
    EIsFocused,
    ESetText    
    };

const TInt KDefaultLength( 100 );

_LIT8( KCpsPublishing, "cpspublishing" );
_LIT8( KMaxLineAmount, "max-line-amount" );
_LIT8( KMaxCharAmount, "max-char-amount" );
_LIT8( KEnablePartialInput, "splitinputenabled" );

_LIT( KEnterChar, "\x2029" );

// Macros
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
    if ( iAppui )
        {
        iAppui->RemoveFromStack( iEditor );
        }
        
    if ( iFlags.IsSet( EIsFocused ) )
        {
        RProperty::Set( KPSUidAiInformation, 
            KActiveIdleForwardNumericKeysToPhone, 
            EPSAiForwardNumericKeysToPhone );                    
        }
    
    if( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev( iCoeEnv->ScreenDevice() );
        dev->ReleaseFont( iFont );
        }

    delete iEditor;
    
    delete iEditorPublisher;       
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

    iAppui = static_cast< CXnAppUiAdapter* >( iAvkonAppUi );
    
    // Max line amount
    iMaxLines = 0;
    
    CXnProperty* maxlinesProp( iNode.GetPropertyL( KMaxLineAmount ) );
        
    if ( maxlinesProp )
        {
        iMaxLines = maxlinesProp->FloatValueL();
        }

    // Default char amount
    TInt maxChars( KDefaultLength );
    
    CXnProperty* maxcharsProp( iNode.GetPropertyL( KMaxCharAmount ) );    
    
    if ( maxcharsProp )
        {
        maxChars = maxcharsProp->FloatValueL();
        }
    
    iEditor = new ( ELeave ) CEikEdwin;

    iEditor->SetContainerWindowL( *iParent );
    
    TInt flags( CEikEdwin::ENoAutoSelection | CEikEdwin::EJustAutoCurEnd |
        CEikEdwin::EAllowUndo | CEikEdwin::EAvkonEditor );
    
    iEditor->ConstructL( flags, maxChars, maxChars, iMaxLines );
    iEditor->SetBackground( this );
    
    // Set allowed input modes
    iEditor->SetAknEditorAllowedInputModes( EAknEditorAllInputModes );

    // Set the default input mode
    iEditor->SetAknEditorInputMode( 
        EAknEditorTextInputMode | EAknEditorNumericInputMode );
                                    
    // Set allowed case modes
    iEditor->SetAknEditorPermittedCaseModes( EAknEditorAllCaseModes );

    // Set the default case mode
    iEditor->SetAknEditorCase( EAknEditorTextCase );
                                             
    // Set numeric keymap
    iEditor->SetAknEditorNumericKeymap( EAknEditorPlainNumberModeKeymap );

    // Enable partial Screen
    CXnProperty* prop( iNode.GetPropertyL( KEnablePartialInput ) );
               
    if ( prop && prop->StringValue() == XnPropertyNames::KTrue )          
        {
        iEditor->SetAknEditorFlags( EAknEditorFlagEnablePartialScreen );
                        
        iFlags.Set( EUsePartialScreenInput );        
        }
        
    iEditor->SetAlignment( AknLayoutUtils::LayoutMirrored() ? 
        EAknEditorAlignRight : EAknEditorAlignLeft ); 
                                                
    iEditor->CreateTextViewL();
    
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
// CXnTextEditorAdapter::FocusChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CXnTextEditorAdapter::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    TBool isFocused( IsFocused() ? ETrue : EFalse );
    TBool wasFocused( iFlags.IsSet( EIsFocused ) ? ETrue : EFalse );
    
    if ( isFocused != wasFocused )
        {
        TInt value;
        
        if ( isFocused )
            {      
            value = EPSAiDontForwardNumericKeysToPhone;
            
            iFlags.Set( EIsFocused );
            
            TRAP_IGNORE( iAppui->AddToStackL( iAppui->View(), iEditor ) );
            }
        else
            {
            value = EPSAiForwardNumericKeysToPhone;
            
            iFlags.Clear( EIsFocused );
            
            iAppui->RemoveFromStack( iEditor );            
            iEditor->SetFocus( EFalse );                       
            } 
        
        RProperty::Set( KPSUidAiInformation, 
            KActiveIdleForwardNumericKeysToPhone, value );                    
        }        
    }
    
// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::Draw
// Draws the editor component
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::Draw( const TRect& /*aRect*/ ) const
    { 
    // Do nothing. 
    // Background is drawn through MCoeControlBackground of CEikEdwin. 
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::HandlePointerEventL
//
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::HandlePointerEventL( 
    const TPointerEvent& aPointerEvent )
    {       
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iFlags.Clear( EIsAlreadyFocused );
        
        if ( IsFocused() )
            {
            iFlags.Set( EIsAlreadyFocused );
            }
        }
    
    if ( iFlags.IsSet( EIsAlreadyFocused ) )
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    else
        {
        TPointerEvent pointerEvent( aPointerEvent ); 
        TRect rect( iEditor->TextView()->ViewRect() );
        TPoint point( aPointerEvent.iPosition );
        
        // this opens partial screen also when margin is tapped
        if ( iFlags.IsSet( EUsePartialScreenInput ) )    
            {
            if( point.iX < rect.iTl.iX )
                {
                pointerEvent.iPosition.iX = rect.iTl.iX;
                }
            else if( point.iX > rect.iBr.iX )
                {
                pointerEvent.iPosition.iX = rect.iBr.iX;
                }
            
            if( point.iY < rect.iTl.iY )
                {
                pointerEvent.iPosition.iY = rect.iTl.iY;
                }
            else if( point.iY > rect.iBr.iY )
                {
                pointerEvent.iPosition.iY = rect.iBr.iY;
                }
            }
        
        CXnControlAdapter::HandlePointerEventL( pointerEvent );    
        }         
    }

// -----------------------------------------------------------------------------
// From MCoeControlBackground
// CXnTextEditorAdapter::Draw
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::Draw( CWindowGc& aGc, const CCoeControl& aControl, 
    const TRect& aRect) const
    {
    if( &aControl == iEditor )
        {
        CXnControlAdapter::Draw( aRect, aGc );    
        }
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::Editor
// Gets underlying CEikEdwin
// -----------------------------------------------------------------------------
//
CEikEdwin* CXnTextEditorAdapter::Editor() const       
    {
    return iEditor;
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::SetTextL
// Sets the new content to the underlying CEikEdwin
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::SetTextL( const TDesC& aText )
    {
    HBufC* text( Text() );
    
    TBool update( EFalse );
    
    if ( !text || *text != aText )
        {
        update = ETrue;
        }
    
    delete text;
    text = NULL;
    
    if ( update )
        {
        iFlags.Set( ESetText );
        
        TRAP_IGNORE( iEditor->SetTextL( &aText ) );
        
        iFlags.Clear( ESetText );
        
        iNode.SetDirtyL();        
        }
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::IsSetText
// Queries whether SetText API has generated edwin content change
// -----------------------------------------------------------------------------
//
TBool CXnTextEditorAdapter::IsSetText() const
    {
    return iFlags.IsSet( ESetText ) ? ETrue : EFalse;
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
    
    SetEditorMarginPropertiesL();
    
    CCharFormatLayer *pCharFL = CCharFormatLayer::NewL(cf,cfm);
    iEditor->SetCharFormatLayer(pCharFL);
    iEditor->SetTextBaselineSpacing( 2 );
    }

// -----------------------------------------------------------------------------
// CXnTextEditorAdapter::SetEditorMarginPropertiesL
// Sets text properties
// -----------------------------------------------------------------------------
//
void CXnTextEditorAdapter::SetEditorMarginPropertiesL()
    {
    TMargins8 margins;
    
    CXnProperty* leftMarginProp( 
        iNode.GetPropertyL( XnPropertyNames::texteditor::KEditorMarginLeft ) );
    if( leftMarginProp )
        {
        TInt leftValue = iUiEngine->HorizontalPixelValueL( leftMarginProp,
                iNode.Rect().Width() );
        margins.iLeft = leftValue;
        }
    
    CXnProperty* rightMarginProp( 
        iNode.GetPropertyL( XnPropertyNames::texteditor::KEditorMarginRight ) );
    if( rightMarginProp )
        {
        TInt rightValue = iUiEngine->HorizontalPixelValueL( rightMarginProp,
                iNode.Rect().Width() );
        margins.iRight = rightValue;
        }
    
    CXnProperty* topMarginProp( 
        iNode.GetPropertyL( XnPropertyNames::texteditor::KEditorMarginTop ) );
    if( topMarginProp )
        {
        TInt topValue = iUiEngine->VerticalPixelValueL( topMarginProp,
                iNode.Rect().Width() );
        margins.iTop = topValue;
        }
    
    CXnProperty* bottomMarginProp( 
        iNode.GetPropertyL( XnPropertyNames::texteditor::KEditorMarginBottom ) );
    if( bottomMarginProp )
        {
        TInt bottomValue = iUiEngine->VerticalPixelValueL( bottomMarginProp,
                iNode.Rect().Width() );
        margins.iBottom = bottomValue;
        }
    
    iEditor->SetBorderViewMargins( margins );
    }

// End of file
