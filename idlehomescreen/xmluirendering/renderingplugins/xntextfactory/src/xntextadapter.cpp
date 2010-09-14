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
* Description:  Implementation for wrapper for a label
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include <gulcolor.h>
#include <utf.h>

#include <AknsUtils.h>
#include <AknPictographInterface.h> // for japanese variants.
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>

#include "xnnodepluginif.h"
#include "xndompropertyvalue.h"
#include "xndomproperty.h"
#include "xndomlist.h"
#include "xnproperty.h"
#include "xncomponent.h"
#include "xnuienginepluginif.h"
#include "xncomponent.h"
#include "xncontroladapter.h"
#include "c_xnutils.h"
#include "xntype.h"
#include "xntextadapter.h"
#include "xncomponentnodeimpl.h"
#include "xnviewnodeimpl.h"
#include "xneditmode.h"

namespace XnText
    {
    const TUint8 KClip = 0x01;   
    const TUint8 KEllipse = 0x02; 
    const TUint8 KWrap = 0x04;    
    const TUint8 KOverflowVisible = 0x08; 
    const TUint8 KLtr = 0x10;     
    const TUint8 KConstructText = 0x20;

    _LIT8( tooltip, "tooltip" );
    _LIT8( clock, "clock" );
    }
    
// -----------------------------------------------------------------------------
// IsBitSet
// Test whether bit is set in a flag
// -----------------------------------------------------------------------------
static TBool IsBitSet(TUint8 aBit,TUint8 aFlag)
    {
    return ( aFlag & aBit );    
    }
    
// -----------------------------------------------------------------------------
// SetLabelTextL
// Prepares text to label, wraps or clips if needed
// -----------------------------------------------------------------------------
static void SetLabelTextL( CEikLabel& aLabel, const TDesC& aSource, TInt aMaxLineWidth, 
                           TInt aMaxLines, TUint8 aFlags, TBool aMeasureOnly = EFalse )    
    {   
    // By default, let the label do the logical to visual conversion                
    TBool conversion( ETrue );
    
    if( IsBitSet( XnText::KOverflowVisible, aFlags ) || aSource == KNullDesC )
        {        
        // no clipping or wrapping needed if,
        // overflow is visible
        // no text or no space,
        // or text fits to given space
        aLabel.SetTextL( aSource );                        
        }
    else
        {
        AknBidiTextUtils::TParagraphDirectionality dir( AknBidiTextUtils::EImplicit );
        
        const CFont* font( aLabel.Font() );
        
        HBufC* buffer( NULL );
        
        if( IsBitSet( XnText::KEllipse, aFlags ) )
    	    {	    
            buffer = aSource.AllocLC();
    	    
    	    TPtr ptr( buffer->Des() );
    	    	    	    
            AknBidiTextUtils::ConvertToVisualAndClipL(
    	                ptr, *font, aMaxLineWidth, aMaxLineWidth, dir );

            
            // The text is already in visual form, no need for conversion
            conversion = EFalse;            
            
            aLabel.SetTextL( *buffer );            
            
            CleanupStack::PopAndDestroy( buffer );	        	                                    
    	    }
    	else if( IsBitSet( XnText::KWrap, aFlags ) )
    	    {        	    
    	    if( aMaxLines > 0 )
    	        {
        	    CArrayFixFlat< TInt >* array = new ( ELeave ) CArrayFixFlat< TInt >( 8 );
        	    CleanupStack::PushL( array );
    	        
        	    for( TInt i = 0; i < aMaxLines; i++ )
        	        {
        	        array->AppendL( aMaxLineWidth );
        	        }
        	        
                buffer = HBufC::NewLC( aSource.Length() + aMaxLines * 
                                        ( KAknBidiExtraSpacePerLine + 1 ) );
                
                TPtr ptr( buffer->Des() );
                
        	    AknBidiTextUtils::ConvertToVisualAndWrapToStringL(
        	                aSource, *array, *font, ptr, ETrue, dir );    	        
    	        }
            else
                {
        	    CArrayFixFlat< TPtrC >* array = new ( ELeave ) CArrayFixFlat< TPtrC >( 8 );
        	    CleanupStack::PushL( array );
                
                HBufC* temp = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
                            aSource, aMaxLineWidth, *font, *array, dir );
                            
                CleanupStack::PushL( temp );
                
                TInt lineCount( array->Count() );
                
                TInt bufLen = temp->Length() + ( lineCount - 1 );
                if ( bufLen > 0 )
                    {
                    buffer = HBufC::NewLC( bufLen );
                    
                    TPtr ptr( buffer->Des() );
                                                    
                    for( TInt i = 0; i < lineCount; i++ )
                        {
                        TPtrC line( array->At( i ) );
                        
                        ptr.Append( line );
                        
                        if( i + 1 < lineCount )
                            {
                            ptr.Append( '\n' );
                            }                    
                        }
                        
                    CleanupStack::Pop();
                    CleanupStack::PopAndDestroy( temp );
                    CleanupStack::PushL( buffer );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( temp );
                    }
                }    	        

            // The text is already in visual form, no need for conversion
            conversion = EFalse;

            if ( buffer )
                {
                aLabel.SetTextL( *buffer );
                CleanupStack::PopAndDestroy( buffer );
                }
            
            CleanupStack::PopAndDestroy(); // array                                       
    	    }
        else
            {
            aLabel.SetTextL( aSource );            
            }    	                   
        } 
    
    if( !aMeasureOnly )
        {     
        // Text is truly set, set conversion and defer draw
        aLabel.UseLogicalToVisualConversion( conversion );                    
        }
    }
      
// -----------------------------------------------------------------------------
// CopyBitmapData
// Copies a data from source bitmap to target bitmap.
// -----------------------------------------------------------------------------
//
static TInt CopyBitmapData(
    CFbsBitmap& aTarget,
    const CFbsBitmap& aSource,
    TPoint aSourcePoint)
	{
    TSize targetSize(aTarget.SizeInPixels());
    TSize sourceSize(aSource.SizeInPixels());
    TInt lineLength(targetSize.iWidth);
    TInt maxSourceLineLength(sourceSize.iWidth - aSourcePoint.iX);
    if(lineLength > maxSourceLineLength)
        {
        lineLength = maxSourceLineLength;
        }
    TInt rowCount(targetSize.iHeight);
    TInt maxSourceRowCount(sourceSize.iHeight - aSourcePoint.iY);
    if(rowCount > maxSourceRowCount)
        {
        rowCount = maxSourceRowCount;
        }

    // Get bitmap display mode
	TDisplayMode displayMode(aSource.DisplayMode());

    // Create buffer for a scan line
	HBufC8* scanLine = HBufC8::New(
        aSource.ScanLineLength(lineLength, displayMode));
    if(!scanLine)
    	{
    	return KErrNoMemory;
    	}
     
	TPtr8 scanPtr(scanLine->Des());

    // Copy all rows to destination bitmap
	for(TInt row(0); row < rowCount; row++)
		{
		aSource.GetScanLine(scanPtr,
            TPoint(aSourcePoint.iX, aSourcePoint.iY + row),
            lineLength, displayMode);
		aTarget.SetScanLine(scanPtr, row);
		}
    delete scanLine;
	return KErrNone;
	}        

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnTextAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnTextAdapter* CXnTextAdapter::NewL(CXnControlAdapter* aParent, CXnNodePluginIf& aNode)
    {
	CXnTextAdapter* self = new( ELeave ) CXnTextAdapter( aParent, aNode );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }
    
// -----------------------------------------------------------------------------
// CXnTextAdapter::CXnTextAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnTextAdapter::CXnTextAdapter(CXnControlAdapter* aParent, CXnNodePluginIf& aNode)
    : iParent( aParent ), iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::~CXnTextAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnTextAdapter::~CXnTextAdapter()
    {    
    delete iLabel;
    
    delete iText;
    
    if( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont( iFont ); 
        }
        
    if( iPictographInterface )
    	{
    	delete iPictographInterface;
    	}

    delete iBackgroundBitmap;
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnTextAdapter::ConstructL()
    {
    iGc = &SystemGc();
       
    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *iParent );
    iLabel->SetTextL( KNullDesC );
    
    CXnControlAdapter::ConstructL( iNode );
            
    iPictographInterface = CAknPictographInterface::NewL( *iLabel, *this );
    iLabel->EnablePictographsL( *iPictographInterface );
                            
    SetTextPropertiesL( iNode );
    UpdateTextL( iNode.GetPCData() );    
	}

// -----------------------------------------------------------------------------
// CXnTextAdapter::ConstructTextL
// Construct the text component according to properties.
// -----------------------------------------------------------------------------
//    
void CXnTextAdapter::ConstructTextL()
    {   
    if( !iText )        
        {
        // CEikLabel will panic in ::Draw if text is not set
        SetLabelTextL( *iLabel, KNullDesC, 0, 0, 0 );
        return;
        }
    
    if( !( iFlags & XnText::KConstructText ) )
        {
        return;
        }
                                   
    TRect rect( iNode.Rect() );

	SetLineSpacingL( rect.Height() );
	
	TInt flags( iFlags );
	               
    // Now we must restrict side into parent        
    flags = RestrictSizeL();
                                                                                                                  
    SetLabelTextL( *iLabel, *iText, iLabel->Rect().Width(), iMaxLines, flags );                       
    }
    
// -----------------------------------------------------------------------------
// CXnTextAdapter::SetTextPropertiesL
// Sets text properties, such as color, font, etc.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CXnTextAdapter::SetTextPropertiesL( CXnNodePluginIf& aNode )
    {
    // Store current state
    const CFont* font( iFont );
    TInt flags( iFlags );
    TInt maxLines( iMaxLines );
    
    if( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont( iFont ); 
        iFont = NULL;
        }
    
    // Find out the font and set it
    CXnUtils::CreateFontL( aNode, iFont, iReleaseFont );
    iLabel->SetFont( iFont );
    
    // Turn off all graphic context effects
    iLabel->SetUnderlining( EFalse );
    iLabel->SetStrikethrough( EFalse );
    
    //following data types are used due to we are handling text via CEikLabel instead of direct
    //font color and effects manipulation via graphics context.
    //We manipulate pen and brush colors indirectly via
    //Label's OverrideColorL method applying EColorControlBackground and EColorLabelText parameters
    //with corresponding RGB value.
    TRgb textColor;
    TRgb effectsColor;
    TBool textColorSet( EFalse );
    TBool effectsColorSet( EFalse );
    TBool effectsDevColorSet( EFalse );
    
    CXnProperty* colorProperty = aNode.GetPropertyL( XnPropertyNames::appearance::common::KColor );
    
    if( colorProperty != NULL )
        {
        CXnDomProperty* domProperty = colorProperty->Property();
        
        if( domProperty )
            {
            TInt error( KErrNotSupported );

            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >
                                            ( domProperty->PropertyValueList().Item( 0 ) );
            
            if( value->IsAutoIdent() )
                {
                MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                error = AknsUtils::GetCachedColor(skinInstance, textColor, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6);
                }
            else if( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
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
                
                if( idResolved )
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
            if (error == KErrNone)
                {
                textColorSet = ETrue;
                }
            }
        }
    
    CXnProperty* effectsProperty = aNode.GetPropertyL(XnPropertyNames::appearance::common::KTextEffects);
    if ( effectsProperty )
        {
        TInt error(KErrNotSupported);
        effectsDevColorSet = ETrue; //some color needed, device color by default
        CXnProperty* effectsColorProperty = aNode.GetPropertyL(XnPropertyNames::appearance::common::KEffectsColor);
        if (effectsColorProperty)
            {
            CXnDomProperty* domProperty = effectsColorProperty->Property();
            if( domProperty )
                {
                CXnDomPropertyValue* value = static_cast<CXnDomPropertyValue*>(domProperty->PropertyValueList().Item(0));
                    if (value->IsAutoIdent())
                        {
                        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                        error = AknsUtils::GetCachedColor(skinInstance, effectsColor, KAknsIIDQsnTextColors,
                        EAknsCIQsnTextColorsCG6);
                        }
                    else if (value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor)
                        {
                        effectsColor = value->RgbColorValueL();
                        error = KErrNone;
                        }
                        else
                        	{
                            HBufC* colorString = effectsColorProperty->StringValueL();
                            CleanupStack::PushL( colorString );
                            CXnUtils::StripQuotes( colorString );
                            
                            TInt index = 0;
                            TAknsItemID skinID;
                            
                            TBool idResolved = CXnUtils::ResolveSkinItemIDL( colorString->Des(), skinID, index );
                            if( idResolved )
                            	{
                            	MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                            	error = AknsUtils::GetCachedColor( skinInstance, effectsColor, skinID, index );
                            	}
                            else // use auto value if skin id is invalid.
                            	{
            	                MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            	                error = AknsUtils::GetCachedColor(skinInstance, effectsColor, KAknsIIDQsnTextColors,
            	                EAknsCIQsnTextColorsCG6);
            	                }
                        	CleanupStack::PopAndDestroy( colorString );                	
                        	}
                if ( error == KErrNone )
                	{
                    effectsColorSet = ETrue; //theme color found
                    effectsDevColorSet = EFalse;//device color not needed
                    }
                }
            }
        }

    if ( textColorSet )
        {
        if ( effectsColorSet )
            {//theme font color and theme effects color
            iLabel->OverrideColorL(EColorControlBackground, textColor);
            iLabel->OverrideColorL(EColorLabelText, effectsColor);
            }
        else if ( effectsDevColorSet )
            {//theme font color and device effects color
            iLabel->OverrideColorL(EColorControlBackground, textColor);
            }
        else
            {//theme font color, but no effects defined
            iLabel->OverrideColorL(EColorLabelText, textColor);
            }
        }
    else if ( effectsColorSet )
            {//device font color and theme effects color
            iLabel->OverrideColorL(EColorLabelText, effectsColor);
            }//else device font color and device effects color (default)

    CXnProperty* textDecorationProp = aNode.GetPropertyL( XnPropertyNames::appearance::common::KTextDecoration );
    
    if( textDecorationProp )
        {
        CXnDomList& propertyValueList = textDecorationProp->Property()->PropertyValueList();
        
        TInt valueCount = propertyValueList.Length();
        
        for( TInt i = 0; i < valueCount; ++i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >( propertyValueList.Item( i ) );
            
            if( value->StringValueL() == XnPropertyNames::appearance::common::textdecoration::KUnderline )
                {
                iLabel->SetUnderlining( ETrue );
                }

            if( value->StringValueL() == XnPropertyNames::appearance::common::textdecoration::KLinethrough )
                {
                iLabel->SetStrikethrough( ETrue );
                }
            }
        }
       
    iFlags = 0;
    iFlags |= XnText::KEllipse; //default
    iFlags |= XnText::KLtr; 
    iMaxLines = 0;
    
    TGulAlignmentValue alignment = CXnUtils::TextAlignment( aNode );
    
    TInt labelAlignment( ELayoutAlignLeft ); // all are vertically top aligned by default (avkon)
    
    switch( alignment )
        {
        case EHCenterVCenter:            
            labelAlignment = ELayoutAlignCenter;                 
            break;
        case EHRightVCenter:            
            labelAlignment = ELayoutAlignRight;                
            break;
        default: 
            break;    
        }
    
    iLabel->SetLabelAlignment( labelAlignment ); // avkon extension needs this, it modifys horizontal alignment
    iLabel->SetAlignment( alignment ); // call to have vertical alignment
  
    CXnProperty* direction = iNode.GetPropertyL( XnPropertyNames::style::common::KDirection );
    const TDesC8* directionValue = &XnPropertyNames::style::common::direction::KLTR;
    
    if( direction )
        {
        directionValue = &direction->StringValue();    
        }
    
    if( *directionValue == XnPropertyNames::style::common::direction::KRTL )
        {
        iLabel->SetLabelAlignment( ELayoutAlignBidi );     
        iFlags &= ~XnText::KLtr;
        }
        	            
    CXnProperty* overflowProp = aNode.GetPropertyL( XnPropertyNames::appearance::common::KTextOverflowMode );
    
    if( overflowProp )
        {
        CXnDomList& propertyValueList = overflowProp->Property()->PropertyValueList();
        
        TInt valueCount = propertyValueList.Length();
        
        for( TInt i = 0; i < valueCount; ++i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >( propertyValueList.Item( i ) );
            
            if( value->StringValueL() == XnPropertyNames::appearance::common::textoverflow::KClipMode )
                {
                iFlags |= XnText::KClip;
                iFlags &= ~XnText::KWrap;
                iFlags &= ~XnText::KEllipse;                
                }
            else if( value->StringValueL() == XnPropertyNames::appearance::common::textoverflow::KWrapMode )
                {
                iFlags |= XnText::KWrap;
                iFlags &= ~XnText::KClip;
                iFlags &= ~XnText::KEllipse;
                }                
            }
            
        //Get max lines amount for wrap
        if( iFlags & XnText::KWrap )
            {
            CXnProperty* maxlinesProp = aNode.GetPropertyL( XnPropertyNames::appearance::common::textoverflow::KMaxLineAmount );
            
            if( maxlinesProp )
                {
                iMaxLines = static_cast< TInt >( maxlinesProp->FloatValueL() );
                }
            }
        }
        
    CXnProperty* overflowProperty = iNode.GetPropertyL( XnPropertyNames::style::common::KOverflow );
    
    if( overflowProperty )
        {
        const TDesC8& overflowValue = overflowProperty->StringValue();
        
        if( overflowValue == XnPropertyNames::style::common::visibility::KVisible )
            {
            iFlags |= XnText::KOverflowVisible;
            }
        }
        
    if( flags & XnText::KConstructText )
        {
        iFlags |= XnText::KConstructText;
        }
        
    if( font != iFont || flags != iFlags || maxLines != iMaxLines )
        {
        return ETrue;
        }
        
    return EFalse;        
    }
        
// -----------------------------------------------------------------------------
// CXnTextAdapter::SetTextL
// Sets the new content to the underlying CEikLabel
// -----------------------------------------------------------------------------
//
void CXnTextAdapter::SetTextL( const TDesC& aText )
    {      
    UpdateTextL( &aText );          
    }
	
// -----------------------------------------------------------------------------
// CXnTextAdapter::Text
// Returns the text contained in the underlying CEikLabel
// -----------------------------------------------------------------------------
//
const TDesC* CXnTextAdapter::Text() const
    {
    if( iFlags & XnText::KConstructText )
        {
        return iText;
        }
    else
        {
        return iLabel->Text();
        }            
    }
    
// -----------------------------------------------------------------------------
// CXnTextAdapter::CountComponentControls
// Returns the number of component controls.
// -----------------------------------------------------------------------------
//
TInt CXnTextAdapter::CountComponentControls() const
    {
    return 1;
    }
    
// -----------------------------------------------------------------------------
// CXnTextAdapter::ComponentControl
// Returns the component control of the given index
// -----------------------------------------------------------------------------
//
CCoeControl* CXnTextAdapter::ComponentControl(TInt aIndex) const
    {
    if( aIndex == 0 )
        {
        return iLabel;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnTextAdapter::DoHandlePropertyChangeL(CXnProperty* aProperty)
    {        
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
        
        if( name == XnPropertyNames::style::common::KDisplay || 
            name == XnPropertyNames::style::common::KVisibility ||
            name == XnPropertyNames::style::common::KWidth ||
            name == XnPropertyNames::style::common::KHeight )
            {
            return;
            }
        
        if( name == XnPropertyNames::common::KPCData )
            {
            UpdateTextL( iNode.GetPCData() );
            }
        }
               
    if( SetTextPropertiesL( iNode ) )
        {
        iFlags |= XnText::KConstructText;
        }
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CXnTextAdapter::HandleScreenDeviceChangedL()
    {
    if( SetTextPropertiesL( iNode ) )
        {
        iFlags |= XnText::KConstructText;
        }

    CXnControlAdapter::HandleScreenDeviceChangedL();        
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::HandlePointerEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CXnTextAdapter::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
	if( AknLayoutUtils::PenEnabled() )
	    {
	    CXnControlAdapter::HandlePointerEventL( aPointerEvent );
	    }
    }

// ---------------------------------------------------------
// CXnTextAdapter::SizeChanged()
// Called by framework when the view size is changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CXnTextAdapter::SizeChanged()
    {
    TRAP_IGNORE( SizeChangedL() );
    }
    
 // ---------------------------------------------------------
// CXnTextAdapter::SizeChangedL()
// ---------------------------------------------------------
//
void CXnTextAdapter::SizeChangedL()
    {
    if( SetTextPropertiesL( iNode ) )
        {
        iFlags |= XnText::KConstructText;
        }
    
    TRect rect( iNode.Rect() );

    TRect labelRect( iLabel->Rect() );
    iLabel->SetRect( rect );
    
    if( iText )
        {
    	TInt textWidth( iLabel->Font()->TextWidthInPixels( *iText ) );
           
        TInt rectWidth( rect.Width() );
                
        if( iFlags & XnText::KOverflowVisible ) 
            {            
            TBool isTooltip( iNode.ParentL()->Type()->Type() == XnText::tooltip );

            // Tooltip can grow and shrink, others just grow                        
            if( isTooltip || textWidth > rectWidth ) 
                {    
                rect = Rect();
                                        
                TInt dx( textWidth - rectWidth );
                                                                                                                        
                if( dx != 0 )
                    {                
                    rect.Resize( dx, 0 );
                    SetSizeWithoutNotification( rect.Size() );                                                                

                    // Update                 
                    iLabel->SetRect( rect );                               
                    }                       
                }
            }            
        }
    if( labelRect != iLabel->Rect() )
    	{
        iFlags |= XnText::KConstructText;
    	}
    }

// ---------------------------------------------------------
// CXnTextAdapter::SkinChanged()
// Called by framework when the skin is changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CXnTextAdapter::SkinChanged()
    {    
    TRAP_IGNORE
        ( 
        if( SetTextPropertiesL( iNode ) )
            {
            iFlags |= XnText::KConstructText;
            }
        );    
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::Draw
// Draws the text component
// -----------------------------------------------------------------------------
//
void CXnTextAdapter::Draw(const TRect& aRect) const
    {   
    CXnTextAdapter* adapter = const_cast< CXnTextAdapter* >( this );

    if( iFlags & XnText::KConstructText )
        {
        TRAPD( err, adapter->ConstructTextL() );                        
        
        adapter->iFlags &= ~XnText::KConstructText;
        
        if( err )
            {
            return;
            }
        }
                               
    CXnControlAdapter::Draw( aRect );
    
    if( iPictographInterface )
    	{
	    TRAP_IGNORE( UpdateBackgroundL() );
	    iGc->BitBlt( iNode.Rect().iTl, iBackgroundBitmap );
    	}
    }
    
// -----------------------------------------------------------------------------
// CXnTextAdapter::DrawPictographArea
// Draws the text component
// -----------------------------------------------------------------------------
//    
void CXnTextAdapter::DrawPictographArea()
    {
    DrawNow();	
    }
	
// -----------------------------------------------------------------------------
// CXnTextAdapter::UpdateBackgroundL
// -----------------------------------------------------------------------------
//   	
void CXnTextAdapter::UpdateBackgroundL() const
	{
	CGraphicsDevice* gdevice = iGc->Device();
	TDisplayMode displayMode = gdevice->DisplayMode();
	
	// create "screenshot" from the background appearance
	if( iBackgroundBitmap )
		{
		delete iBackgroundBitmap;
		iBackgroundBitmap = NULL;
		}

	iBackgroundBitmap = new (ELeave) CFbsBitmap();
	iBackgroundBitmap->Create( iNode.Rect().Size(), displayMode );
	CFbsBitmap* tmpBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL( tmpBitmap );
	
	CWsScreenDevice* scrDevice = static_cast<CWsScreenDevice*>( iGc->Device() );
	TSize tmpSize = scrDevice->SizeInPixels();
	tmpBitmap->Create( tmpSize, displayMode );
	
	CFbsBitmapDevice* tmpDevice = CFbsBitmapDevice::NewL( tmpBitmap );
	CleanupStack::PushL( tmpDevice );
	CBitmapContext* bc( NULL );
	tmpDevice->CreateBitmapContext( bc );
	CleanupStack::PushL( bc );
	DrawBackground( iNode.Rect(), reinterpret_cast<CWindowGc&>( *bc ) );
	CopyBitmapData( *iBackgroundBitmap, *tmpBitmap, iNode.Rect().iTl );
	CleanupStack::PopAndDestroy( 3 );
	}
	
// -----------------------------------------------------------------------------
// CXnTextAdapter::DrawBackground
// -----------------------------------------------------------------------------
//   	
void CXnTextAdapter::DrawBackground(const TRect& aRect, CWindowGc& aGc) const
	{
	RPointerArray<CXnControlAdapter> adapters;

	for( CXnNodePluginIf* node = &iNode; node; )
		{
        TRAP_IGNORE( 
		    CXnControlAdapter* adapter( node->Control() );

		    if ( adapter )
    		    {
	    	    adapters.AppendL( adapter );  
		        }

		    node = node->ParentL()
		    );
        }

    for( TInt i = adapters.Count() - 1; i >= 0; --i )
        {
        adapters[i]->Draw( aRect, aGc );
        }

    adapters.Reset();
	}    

// -----------------------------------------------------------------------------
// CXnTextAdapter::MeasureAdaptiveContentL
// -----------------------------------------------------------------------------
// 
TSize CXnTextAdapter::MeasureAdaptiveContentL( const TSize& aAvailableSize )
    {         
    TSize size;
    
    if( ( aAvailableSize.iWidth > 0 ) && ( aAvailableSize.iHeight > 0 ) )
        {  
        size = MeasureTextL( aAvailableSize );           
        }
        
    return size;
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::RestrictSizeL
// -----------------------------------------------------------------------------
//     
TInt CXnTextAdapter::RestrictSizeL()
    {        
    CXnNodePluginIf* parent( iNode.ParentL() );
    
    if( !parent )
        {
        // No parent where to restrict own size
        return iFlags;
        }
       
    TRect parentRect( parent->Rect() );
    
    if( parentRect.IsEmpty() )
        {
        return iFlags;
        }
    
    TRect rect( Rect() );
    
    TInt dx( 0 );
    TInt dy( 0 );
            
    // Restrict own size inside parent rect to prevent flowing outside parent's rect
    if( rect.iBr.iX > parentRect.iBr.iX )
        {
        dx = rect.iBr.iX - parentRect.iBr.iX;       
        }
        
    if( rect.iBr.iY > parentRect.iBr.iY )
        {
        dy = rect.iBr.iY - parentRect.iBr.iY;
        }

    if( dx == 0 && dy == 0 )
        {
        // No need to change sizes
        return iFlags;
        }
                
    TInt flags( iFlags );
    
    if( parent->Type()->Type() == XnText::tooltip )
        {
        // Remove temporarily to allow tooltip truncation
        flags &= ~XnText::KOverflowVisible;
        }
    
    // Clip text again, parent size restriction may result in re-clipping    
    if( !IsBitSet( XnText::KOverflowVisible, flags ) )
        {
        TSize size( rect.Size() );
        
        size.iWidth -= dx;
        size.iHeight -= dy;
        
        // Update own size ...    
        SetSizeWithoutNotification( size );

        // ... and label        
        rect = iLabel->Rect();
        
        rect.Resize( -dx, -dy );
            
        iLabel->SetRect( rect );               
        }
        
    return flags;                                
    }
        
// -----------------------------------------------------------------------------
// CXnTextAdapter::SetLineSpacingL
// -----------------------------------------------------------------------------
//     
void CXnTextAdapter::SetLineSpacingL( TInt aReference )
    {
    // Set linespacing, one pixel is the default
    TInt lineSpace( 1 );
    
    CXnProperty* lineSpaceProp( 
        iNode.GetPropertyL( XnPropertyNames::appearance::common::KFontLineSpace ) );

    if( lineSpaceProp )
        {        
        lineSpace = iNode.UiEngineL()->VerticalPixelValueL( lineSpaceProp, aReference );
        }
        
    if( lineSpace < 0 )
        {
        // Default
        lineSpace = 1;
        }

    const CAknLayoutFont* layoutFont( CAknLayoutFont::AsCAknLayoutFontOrNull( iFont ) );         
        
    if( layoutFont )
        {
        TInt textPaneHeight = layoutFont->TextPaneHeight();
        
        lineSpace += ( textPaneHeight - iFont->HeightInPixels() );        
        }
    /* end of CEikLabel fix */        
    
    iLabel->SetPixelGapBetweenLines( lineSpace );   
    }

// -----------------------------------------------------------------------------
// MeasureTextL
// Measures the text dimensions fitted to available size
// -----------------------------------------------------------------------------
//    
TSize CXnTextAdapter::MeasureTextL( const TSize& aAvailableSize )    
    {
    TSize size;
    
    if( !iText || *iText == KNullDesC )
        {
        return size;
        }
                                              
    // Save the current text before measure
    HBufC* original( iLabel->Text()->AllocLC() );

    SetLineSpacingL( 0 );
                                                                                                                                                      
    SetLabelTextL( *iLabel, *iText, aAvailableSize.iWidth, iMaxLines, iFlags, ETrue );
    
    TPtrC text( *iLabel->Text() );
                    
    // help CEikLabel to calculate its content size more precisely
    iLabel->iMargin.iBottom = iLabel->Font()->DescentInPixels();

    // Get the size needed for the text                        
    size = iLabel->CalcMinimumSize( text );            
    
    // Restore original text
    iLabel->SetTextL( *original );
    CleanupStack::PopAndDestroy( original );        
           
    return size;                            	                  
    }

// -----------------------------------------------------------------------------
// UpdateTextL
// -----------------------------------------------------------------------------
//    
void CXnTextAdapter::UpdateTextL( const TDesC8& aText )
    {
    HBufC* text( CnvUtfConverter::ConvertToUnicodeFromUtf8L( aText ) );
    
    CleanupStack::PushL( text );
    
    UpdateTextL( text );
    
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// UpdateTextL
// -----------------------------------------------------------------------------
//    
void CXnTextAdapter::UpdateTextL( const TDesC* aText )
    {
    if( aText )
        {
        if( iText && *iText == *aText )
            {
            // New text is same as current
            return;
            }
            
        if( iText && iText->Des().MaxLength() >= aText->Length() )
            {
            // New text fits to earlier allocated space
            *iText = *aText;
            }
        else
            {
            // Need to reserve space for new text
            delete iText;
            iText = NULL;
            iText = aText->AllocL();
            }            
           
    	TPtr ptr( iText->Des() );
    	
    	CXnUtils::CollapseWhiteSpace( iNode, ptr );	             
            
        iFlags |= XnText::KConstructText;
        
        iNode.SetDirtyL();
        
        const TDesC8& parentType( iNode.ParentL()->Type()->Type() );
        
        if( parentType == XnText::tooltip )
        	{
        	// Force tooltip text size to be recalculted by layout algorithm
        	SetSizeWithoutNotification( TSize() );
        	}
        else if( parentType == XnText::clock )
            {
            iNode.ParentL()->SetDirtyL();
            }
        }
    }

// End of file

