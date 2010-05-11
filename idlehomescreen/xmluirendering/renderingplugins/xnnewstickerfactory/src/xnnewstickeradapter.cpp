/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a label
*
*/

// System includes
#include <utf.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknMarqueeControl.h>
#include <AknBidiTextUtils.h>
#include <debug.h>

// User includes
#include "xnviewnodeimpl.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xnproperty.h"
#include "xnnodepluginif.h"
#include "xntype.h" 

#include "c_xnutils.h"

#include "xnnewstickeradapter.h"
#include "xnnewstickercontrol.h"
#include "xncomponentnodeimpl.h"


const TInt KThousandCoef = 1000;
const TInt KByteLength = 8;


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GetIntPropertyL
// Gets an integer property from the node.
// Returns: aNode UI node
//          aproperty The property that is to be found
//          return The value of the property. If not found, KErrNotFound returned.
// -----------------------------------------------------------------------------
// 
static TInt GetIntPropertyL( CXnNodePluginIf& aNode, const TDesC8& aProperty )
    {
    CXnProperty* prop = aNode.GetPropertyL(aProperty);
    if(prop)
        {        
        CXnDomPropertyValue* value = 
            static_cast<CXnDomPropertyValue*>( 
                    prop->Property()->PropertyValueList().Item(0) );
        
        if(CXnDomPropertyValue::ENumber == value->PrimitiveValueType())
            {
            TInt integer = static_cast<TInt>( prop->FloatValueL() );
            
            return integer;
            }
        }
    
    return KErrNotFound;
    }       

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetNewstickerPropertiesL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetNewstickerPropertiesL()
    {
    // scroll amount 
    TInt scrollamount = GetIntPropertyL( iNode, 
        XnPropertyNames::newsticker::KXnScrollAmount );   // px
    
    if( KErrNotFound == scrollamount )
        {
        scrollamount = KXnNewstickerScrollAmount;
        }

    CXnProperty* prop( iNode.GetPropertyL(
            XnPropertyNames::newsticker::KScrollBehaviour ) );
    
    if( prop )
        {
        if ( prop->StringValue() == 
                XnPropertyNames::newsticker::scroll_behaviour::KAlternate )
            {
            scrollamount = 0;
            iScrollBehaviour = EAlternate;
            }
        else if ( prop->StringValue() == 
                XnPropertyNames::newsticker::scroll_behaviour::KScrollAlternate )
            {
            iScrollBehaviour = EScrollAlternate;
            }
        else
            {
            iScrollBehaviour = EScroll;
            }
        }
    else
        {
        iScrollBehaviour = EScroll;
        }        
        
    iMarqueeControl->SetSpeedInPixels( scrollamount );
    
    if( iScrollBehaviour == EScroll )
        {                    
        // scroll delay
        TInt scrolldelay = GetIntPropertyL( iNode, 
            XnPropertyNames::newsticker::KXnScrollDelay );  // ms
        
        if( KErrNotFound == scrolldelay )
            {
            scrolldelay = KXnNewstickerInterval;
            }
        else 
            {
            scrolldelay *= KThousandCoef;    // change to microseconds
            }
        
        iMarqueeControl->SetInterval( scrolldelay ); 
        iAlternateInterval = scrolldelay;
            
        // start delay
        TInt startdelay = GetIntPropertyL( iNode, 
            XnPropertyNames::newsticker::KXnStartDelay ); // ms
        
        if( KErrNotFound == startdelay )
            {
            startdelay = KXnNewstickerDelay;
            }
        else 
            {
            startdelay *= KThousandCoef;    // change to microseconds
            }
        
        iMarqueeControl->SetDelay( startdelay );
        iAlternateDelay = startdelay;
        }
    else if( iScrollBehaviour == EScrollAlternate )
		{
        // scroll delay
        TInt scrolldelay = GetIntPropertyL( iNode, 
            XnPropertyNames::newsticker::KXnScrollDelay );  // ms
        
        if( KErrNotFound == scrolldelay )
            {
            scrolldelay = KXnNewstickerInterval;
            }
        else 
            {
            scrolldelay *= KThousandCoef;    // change to microseconds
            }
        
        iMarqueeControl->SetInterval( scrolldelay );
        iAlternateInterval = scrolldelay;

        TInt alternateTime( GetIntPropertyL( iNode, XnPropertyNames::newsticker::KDisplayTime ) );
        
        if( alternateTime == KErrNotFound )
            {
            alternateTime = 0;
            }

        alternateTime *= KThousandCoef; // change to ms                        

        iAlternateInterval = alternateTime;

        // start delay
        TInt startdelay = GetIntPropertyL( iNode, 
            XnPropertyNames::newsticker::KXnStartDelay ); // ms
        
        if( KErrNotFound == startdelay )
            {
            startdelay = 0;
            }
        else 
            {
            startdelay *= KThousandCoef;    // change to microseconds
            }
        
        iMarqueeControl->SetDelay( startdelay );
        iAlternateDelay = startdelay;
        }
    else // TScrollBehaviour::EAlternate
        {
        TInt alternateTime( GetIntPropertyL( iNode, XnPropertyNames::newsticker::KDisplayTime ) );
        
        if( alternateTime == KErrNotFound )
            {
            alternateTime = 0;
            }

        alternateTime *= KThousandCoef; // change to ms                        

        iMarqueeControl->SetDelay( 0 );
        iMarqueeControl->SetInterval( alternateTime );
        
        iAlternateDelay = 0;
        iAlternateInterval = alternateTime;
        }        
    
    // _s60-scroll-loop property. True by default.
    CXnProperty* loopProp( iNode.GetPropertyL(
            XnPropertyNames::newsticker::KScrollLoop ) );
    
    if( loopProp && loopProp->StringValue() == XnPropertyNames::KFalse )
        {
        iScrollLooping = EFalse;
        }
    else
        {
        iScrollLooping = ETrue;
        }    
    
    CXnProperty* restartProperty( iNode.GetPropertyL(
            XnPropertyNames::newsticker::KRestartAfterUpdate ) );
    
    if ( restartProperty && restartProperty->StringValue() == XnPropertyNames::KTrue )
        {
        iRestartAfterUpdate = ETrue;
        }
     }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnNewstickerAdapter* CXnNewstickerAdapter::NewL( CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode )
    {
	CXnNewstickerAdapter* self = new( ELeave ) CXnNewstickerAdapter( aParent, aNode );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop();
    
    return self;	
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::ConstructL( CXnControlAdapter* aParent )
    {
    CXnControlAdapter::ConstructL( iNode );
    
    iControl = CXnNewstickerControl::NewL( this );    
    
    iMarqueeControl = CAknMarqueeControl::NewL();
    TCallBack callback( RedrawCallback, this );
    iMarqueeControl->SetRedrawCallBack( callback );
    iMarqueeControl->SetContainerWindowL( *aParent );
    iMarqueeControl->ActivateL();
    
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    
    SetTextPropertiesL();
    SetNewstickerPropertiesL();
    
    RPointerArray<CXnNodePluginIf> children = iNode.ChildrenL();
    CleanupClosePushL( children );
    
    for( TInt i = 0; i < children.Count(); ++i )
        {
        CXnNodePluginIf* child( children[i] );
        
        CXnType* xnType = child->Type();
        
        // We only care for <title> nodes
        if ( xnType && xnType->Type() == XnPropertyNames::title::KTitle )
            {
            // Append  title, if one is given from the theme use that.
            // If no content yet available create empty title so that 
            // indexing is always current
            const TDesC8& pcData( child->GetPCData() );
    
            HBufC* pcData16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( pcData );
            CleanupStack::PushL( pcData16 );
    
            TPtr ptr( pcData16->Des() );
    
            CXnUtils::CollapseWhiteSpace( iNode, ptr );
    
            if( ptr.Length() > 0 )
                {
                iControl->AppendTitleL( *pcData16 );
                }
            else
                {
                iControl->AppendTitleL( KNullDesC );
                }
    
            CleanupStack::PopAndDestroy( pcData16 );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    
    iControl->SetCurrentTitle( ETrue );
    
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::CXnNewstickerAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnNewstickerAdapter::CXnNewstickerAdapter( CXnControlAdapter* /*aParent*/, 
    CXnNodePluginIf& aNode )
    : iNode( aNode ),
      iPowerSaveMode( ETrue ),
      iRestartAfterUpdate( EFalse ),
      iTextColor( KRgbBlack ),
      iTextAlignment( ELayoutAlignLeft ),
      iTextBaseline( 0 ),
      iUnderlining( EUnderlineOff ),
      iStrikethrough( EStrikethroughOff ),
      iScrollLooping( ETrue ),
      iRedraw( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::~CXnNewstickerAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnNewstickerAdapter::~CXnNewstickerAdapter()
    {           
    if( iPeriodicTimer && iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }
        
    delete iPeriodicTimer;

    if( iFont && ( iReleaseFont == 1 ) )
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont( iFont ); 
        }
    
    delete iMarqueeControl;
    delete iControl;

    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetTextPropertiesL
// Sets text properties, such as color, font, etc.
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetTextPropertiesL()
    {
    if (iFont && ( iReleaseFont == 1 ) )
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont(iFont); 
        iFont = NULL;
        }
    
    CXnUtils::CreateFontL(iNode, iFont, iReleaseFont);
    
    TBool colorSet(EFalse);
    CXnProperty* colorProperty = iNode.GetPropertyL(XnPropertyNames::appearance::common::KColor);
    
    if( colorProperty )
        {
        CXnDomProperty* domProperty = colorProperty->Property();
        if(domProperty)
            {
            TInt error( KErrNotSupported );
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >
                                            ( domProperty->PropertyValueList().Item( 0 ) );
            
            if( value->IsAutoIdent() )
                {
                MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                error = AknsUtils::GetCachedColor( skinInstance, iTextColor, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6 );
                }
            else if( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
                {
                iTextColor = value->RgbColorValueL();
                error = KErrNone;
                }
            else
                {
                HBufC* colorString = colorProperty->StringValueL();
                CleanupStack::PushL( colorString );
                
                CXnUtils::StripQuotes( colorString );
                
                TInt index = 0;
                TAknsItemID skinID;
                
                TBool idResolved = CXnUtils::ResolveSkinItemIDL(
                        colorString->Des(), skinID, index );
                
                if( idResolved )
                    {
                    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                    error = AknsUtils::GetCachedColor(
                            skinInstance, iTextColor, skinID, index );
                    }
                else // use auto value if skin id is invalid.
                    {
                    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                    error = AknsUtils::GetCachedColor(skinInstance, iTextColor,
                            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
                    }
                CleanupStack::PopAndDestroy( colorString );                 
                }
            
            if ( error == KErrNone )
                {
                colorSet = ETrue;
                }
            }
        }

    if( !colorSet ) // Use text skin color
        {
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TRgb textColor;
        TInt error = AknsUtils::GetCachedColor( skinInstance, textColor, 
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        if (error == KErrNone)
            {
            iTextColor = textColor;
            }
        }
    
    CXnProperty* textDecorationProp = 
        iNode.GetPropertyL(XnPropertyNames::appearance::common::KTextDecoration);
    
    if( textDecorationProp )
        {
        CXnDomList& propertyValueList = textDecorationProp->Property()->PropertyValueList();
        TInt valueCount = propertyValueList.Length();
        
        for( TInt i = 0; i < valueCount; ++i )
            {
            CXnDomPropertyValue* value = 
                static_cast<CXnDomPropertyValue*>(propertyValueList.Item(i));
            if (value->StringValueL() == 
                XnPropertyNames::appearance::common::textdecoration::KUnderline)
                {
                iUnderlining = EUnderlineOn;
                }
            if (value->StringValueL() == 
                XnPropertyNames::appearance::common::textdecoration::KLinethrough)
                {
                iStrikethrough = EStrikethroughOn;
                }
            }
        }
    
    TGulAlignmentValue alignment = CXnUtils::TextAlignment( iNode );
    
    switch( alignment )
        {
        case EHCenterVCenter:            
            iTextAlignment = ELayoutAlignCenter;                 
            break;
            
        case EHRightVCenter:            
            iTextAlignment = ELayoutAlignRight;                
            break;
            
        default: 
            iTextAlignment = ELayoutAlignLeft;
            break;    
        }   
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::AppendTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::AppendTitleL( const TDesC& aTitle )
    {
    iControl->AppendTitleL( aTitle );
    iNode.SetDirtyL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::InsertTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::InsertTitleL( const TDesC& aTitle, TInt aIndex )
    {
    iControl->InsertTitleL( aTitle, aIndex );
    iNode.SetDirtyL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::UpdateTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::UpdateTitleL( const TDesC& aTitle, TInt aIndex )
    {
    iControl->UpdateTitleL( aTitle, aIndex );
    iNode.SetDirtyL();
    
    if ( iRestartAfterUpdate )
        {
        Stop();
        Start();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DeleteTitleL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DeleteTitleL( TInt aIndex )
    {
    iControl->DeleteTitleL( aIndex );
    iNode.SetDirtyL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::CurrentTitleIndex
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerAdapter::CurrentTitleIndex()
    {
    return iControl->CurrentTitleIndex();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::Title
// -----------------------------------------------------------------------------
//
const TDesC& CXnNewstickerAdapter::Title( TInt aIndex )
    {
    return iControl->Title( aIndex );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::ClearTitles
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::ClearTitles()
    {
    iControl->ClearTitles();
    TRAP_IGNORE( iNode.SetDirtyL() );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetCallbackInterfaceL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetCallbackInterfaceL(
        XnNewstickerInterface::MXnNewstickerCallbackInterface* aCallback )
    {
    iCallback = aCallback;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::TitleScrolled
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::TitleScrolled( TInt aTitleIndex )
    {
    if( iCallback )
        {
        iCallback->TitleScrolled( aTitleIndex );
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::StartL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::Start()
    {
    if( IsVisible() && !iPowerSaveMode )
        {
        if( iControl->TitleCount() < 1 )
            {
            // Nothing to show
            return;
            }
        
        switch( iScrollBehaviour )
            {
            case EScroll:
                {
                iMarqueeControl->EnableMarquee( ETrue );
                iRedraw = ETrue;
                }
                break;
                
            case EAlternate:
                {
                iMarqueeControl->EnableMarquee( EFalse );
                StartAlternateCounter();  
                }
                break;
                
            case EScrollAlternate:
                {
                iMarqueeControl->EnableMarquee( ETrue );
                StartAlternateCounter(); 
                iRedraw = ETrue;
                }
                break;
                
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::Stop
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::Stop()
    {  
    StopAlternateCounter();
    StopMarquee();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::StopAlternateCounter
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::StopAlternateCounter()
    {
    if( iPeriodicTimer && iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::StopMarquee
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::StopMarquee()
    {
    iMarqueeControl->Reset();
    iMarqueeControl->EnableMarquee( EFalse );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::StartCounter
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::StartAlternateCounter()
    {
    if( iPeriodicTimer && iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }

    iPeriodicTimer->Start(                
        TTimeIntervalMicroSeconds32( iAlternateDelay ),
        TTimeIntervalMicroSeconds32( iAlternateInterval ), 
        TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) ); 
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::PeriodicEventL
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerAdapter::PeriodicEventL( TAny* aPtr )
    {
    __PRINTS( "CXnNewstickerAdapter::PeriodicEventL, timer runs" );
    CXnNewstickerAdapter* self = static_cast< CXnNewstickerAdapter* >( aPtr );
    self->DoScroll();
  
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::MakeVisible()
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::MakeVisible( TBool aVisible )
    {
    CCoeControl::MakeVisible( aVisible );

    if( aVisible )
        {
        Start();
        }
    else
        {
        Stop();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SizeChanged()
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SizeChanged()
    {
    CXnControlAdapter::SizeChanged();         
    iMarqueeControl->SetExtent( iNode.Rect().iTl, iNode.Rect().Size() );
    
    iTextBaseline = iNode.Rect().Height() / 2 + iFont->AscentInPixels() / 2;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::CountComponentControls() const
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerAdapter::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::ComponentControl(TInt aIndex) const
// -----------------------------------------------------------------------------
//
CCoeControl* CXnNewstickerAdapter::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iMarqueeControl;
            
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DoHandlePropertyChangeL( CXnProperty* /*aProperty*/ )
    {
    SetTextPropertiesL();
    SetNewstickerPropertiesL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SkinChanged
// -----------------------------------------------------------------------------
// 
void CXnNewstickerAdapter::SkinChanged() 
    {
    CXnControlAdapter::SkinChanged();
    TRAP_IGNORE( SetTextPropertiesL() );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::ReportNewstickerEventL
// -----------------------------------------------------------------------------
// 
void CXnNewstickerAdapter::ReportNewstickerEvent( const TDesC8& aEventName )
    { 
    TBuf8<KByteLength>index( KNullDesC8 );
    index.Num( CurrentTitleIndex() );
    
    TRAP_IGNORE( iNode.ReportTriggerEventL( aEventName,
            XnPropertyNames::action::trigger::name::KTitleIndex, index ); )
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::Draw
// Draws the text component
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::Draw( const TRect& aRect ) const
    {    
    const_cast<CXnNewstickerAdapter*>(this)->DrawText( iControl->SelectTitle(), aRect );
    }
    
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoEnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DoEnterPowerSaveModeL( TModeEvent /*aEvent*/ )
    {     
    iPowerSaveMode = ETrue;
    Stop();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoExitPowerSaveModeL
// Exit power save mode
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DoExitPowerSaveModeL( TModeEvent /*aEvent*/ )
    {
    iPowerSaveMode = EFalse;
    
    if( IsVisible() )
        {
        Start();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawText
// Text drawing function is selected by scrolling behaviour
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DrawText( const TDesC& aText, const TRect& aRect )
    {    
    CWindowGc& gc = SystemGc();
    
    TRect rect = iNode.Rect();
           
    CXnControlAdapter::Draw( aRect );
    
    gc.SetPenColor( iTextColor );
    gc.UseFont( iFont );
    gc.SetUnderlineStyle( iUnderlining );
    gc.SetStrikethroughStyle( iStrikethrough );
           
    TInt textWidth = iFont->TextWidthInPixels( aText );
    TInt rectWidth = rect.Width();
    
    switch( iScrollBehaviour )
        {  
        case EScroll:
        case EScrollAlternate:
            {
            if( !iRedraw || textWidth < rectWidth )
                {
                iMarqueeControl->Stop();
                DrawStaticText( gc, aText );
                }
            else
                {
                iMarqueeControl->Start();
                TBool isLast = DrawMarqueeText( gc, aText );
                
                if( isLast )
                    {
                    if( !iScrollLooping )
                        {
                        iRedraw = EFalse;
                        }
                    
                    iMarqueeControl->Reset();
                    StartAlternateCounter();
                    }
                }
            }
            break;
            
        case EAlternate:
            {
            DrawStaticText( gc, aText );
            }
            break;
            
        default:
            break;
        }
      }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::RedrawCallback
// Callback function for marquee control
// -----------------------------------------------------------------------------
//
TInt CXnNewstickerAdapter::RedrawCallback( TAny* aPtr )
    {
    CXnNewstickerAdapter* self = static_cast<CXnNewstickerAdapter*>( aPtr );
    self->DrawNow();

    return self->Redraw();
 
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoScroll
// Scrolls alternative text. Function is called by periodic timer
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DoScroll()
    {
    ReportNewstickerEvent( XnPropertyNames::action::trigger::name::KTitleScrolled );
    
    if( iControl->SetCurrentTitle() )
        {
        // stop alternate scrolling if current index is last
        StopAlternateCounter();
        }
    
    DrawNow();
 
    ReportNewstickerEvent( XnPropertyNames::action::trigger::name::KTitleToScroll );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawStatic
// Draws text directly to screen when scrolling is not needed
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DrawStaticText( CWindowGc& aGc, const TDesC& aText ) const
    { 
    HBufC* text = HBufC::New( aText.Length() + KAknBidiExtraSpacePerLine );
    
    if( text )
        {
        TRect rect = iNode.Rect();
        TInt maxLength = rect.Width();
        TPtr ptr = text->Des();
        AknBidiTextUtils::ConvertToVisualAndClip(
                aText, ptr, *iFont, maxLength, maxLength );
        
        aGc.DrawText( *text, rect, iTextBaseline,
                ( CGraphicsContext::TTextAlign )iTextAlignment );
        
        delete text;
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawScrolling
// Draws scrolling text to screen via marquee control
// -----------------------------------------------------------------------------
//
TBool CXnNewstickerAdapter::DrawMarqueeText( CWindowGc& aGc, const TDesC& aText ) const
    {
    TRect rect = iNode.Rect();
    
    // returns true when all loops have been executed
    return iMarqueeControl->DrawText( aGc, rect, aText, iTextBaseline,
            ( CGraphicsContext::TTextAlign )iTextAlignment, *iFont );
    }


// End of file
