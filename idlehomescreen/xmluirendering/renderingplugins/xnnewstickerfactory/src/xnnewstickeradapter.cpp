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
#include "xnnewstickersvgcontrol.h"
#include "xncomponentnodeimpl.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GetIntPropertyL
// Gets an integer property from the node.
// Returns: aNode UI node
//          aproperty The property that is to be found
//          return The value of the property. If not found, KErrNotFound returned.
// -----------------------------------------------------------------------------
// 
static TInt GetIntPropertyL(
    CXnNodePluginIf& aNode,
    const TDesC8& aProperty)
    {
    CXnProperty* prop = aNode.GetPropertyL(aProperty);
    if(prop)
        {        
        CXnDomPropertyValue* value = 
            static_cast<CXnDomPropertyValue*>(prop->Property()->PropertyValueList().Item(0));
        if(CXnDomPropertyValue::ENumber == value->PrimitiveValueType())
            {
            TInt integer = static_cast<TInt>(prop->FloatValueL());
            return integer;
            }
        }
    return KErrNotFound;
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

// ---------------------------------------------------------
// CXnNewstickerAdapter::CheckDisplayL
// ---------------------------------------------------------
//
TBool CXnNewstickerAdapter::CheckDisplayL( CXnNodePluginIf& aNode )
    {
    if( iPowerSaveMode )
        {
        iDisplay = EFalse;
        
        return iDisplay;
        }
        
	CXnProperty* displayProp( aNode.DisplayL() );

	if( displayProp )
	    {
	    const TDesC8& display( displayProp->StringValue() );
	    
	    if( display != XnPropertyNames::style::common::display::KBlock )
	        {
	        return EFalse;
	        }
	    }
    
	CXnProperty* visibilityProp( aNode.VisibilityL());

	if( visibilityProp )
	    {
	    const TDesC8& visibility( visibilityProp->StringValue() );
	    
	    if( visibility != XnPropertyNames::style::common::visibility::KVisible )
	        {
	        return EFalse;
	        }
	    }
    
    
    CXnNodePluginIf* parent( aNode.ParentL() );
    
    TBool ret( ETrue );
    
    if( parent )
        {
        ret = CheckDisplayL( *parent );
        }
        
    iDisplay = ret;  
    
    return iDisplay;          
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::SetNewstickerPropertiesL
// ---------------------------------------------------------
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

    CXnProperty* prop( iNode.GetPropertyL( XnPropertyNames::newsticker::KScrollBehaviour ) );
    
    if( prop )
        {
        if ( prop->StringValue() == XnPropertyNames::newsticker::scroll_behaviour::KAlternate )
            {
            scrollamount = 0;
            iScrollBehaviour = EAlternate;
            }
        else if ( prop->StringValue() == XnPropertyNames::newsticker::scroll_behaviour::KSlide )
            {
            iScrollBehaviour = ESlide;
            }
        else if ( prop->StringValue() == XnPropertyNames::newsticker::scroll_behaviour::KScrollAlternate )
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
        
    iControl->SetScrollAmount( scrollamount );
    
    if( iScrollBehaviour == EScroll || iScrollBehaviour == ESlide )
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
            scrolldelay *= 1000;    // change to microseconds
            }
        
        iInterval = scrolldelay;
            
        // start delay
        TInt startdelay = GetIntPropertyL( iNode, 
            XnPropertyNames::newsticker::KXnStartDelay ); // ms
        
        if( KErrNotFound == startdelay )
            {
            startdelay = KXnNewstickerDelay;
            }
        else 
            {
            startdelay *= 1000;    // change to microseconds
            }
        
        iDelay = startdelay;
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
            scrolldelay *= 1000;    // change to microseconds
            }
        
        iInterval = scrolldelay;

        TInt alternateTime( GetIntPropertyL( iNode, XnPropertyNames::newsticker::KDisplayTime ) );
        
        if( alternateTime == KErrNotFound )
            {
            alternateTime = 0;
            }

        alternateTime *= 1000; // change to ms                        

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
            startdelay *= 1000;    // change to microseconds
            }
        
        iDelay = startdelay;
        }
    else // TScrollBehaviour::EAlternate
        {
        TInt alternateTime( GetIntPropertyL( iNode, XnPropertyNames::newsticker::KDisplayTime ) );
        
        if( alternateTime == KErrNotFound )
            {
            alternateTime = 0;
            }

        alternateTime *= 1000; // change to ms                        

        iDelay = 0;
        iInterval = alternateTime;
        }        

    //  animation time
    iAnimationTime = GetIntPropertyL( iNode, 
        XnPropertyNames::newsticker::KXnNewstickerAnimationDelay );  // ms
    
    if( KErrNotFound == iAnimationTime )
        {
        iAnimationTime = KAnimationTime;
        }
    else 
        {
        iAnimationTime *= 1000;    // change to microseconds
        }
    
    // _s60-scroll-loop property. True by default.
    CXnProperty* loopProp( iNode.GetPropertyL( XnPropertyNames::newsticker::KScrollLoop ) );
    
    if( loopProp && loopProp->StringValue() == XnPropertyNames::KFalse )
        {
        iControl->SetScrollLooping( EFalse );
        }
    else
        {
        iControl->SetScrollLooping( ETrue );
        }            
    CXnProperty* restartProperty( iNode.GetPropertyL( XnPropertyNames::newsticker::KRestartAfterUpdate ) );
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
CXnNewstickerAdapter* CXnNewstickerAdapter::NewL(CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode)
    {
	CXnNewstickerAdapter* self = new( ELeave ) CXnNewstickerAdapter( aParent, aNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;	
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::ConstructL()
    {
    CXnControlAdapter::ConstructL( iNode );
    
    iControl = CXnNewstickerControl::NewL( this );    
    iSvgControl = CXnNewstickerSvgControl::NewL( this );
    
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    
    iPowerSaveMode = ETrue;
    
    iRestartAfterUpdate = EFalse;
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
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::CXnNewstickerAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnNewstickerAdapter::CXnNewstickerAdapter(CXnControlAdapter* /*aParent*/, 
    CXnNodePluginIf& aNode)
    : iState(ENotStarted), iNode(aNode)
    {
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::~CXnNewstickerAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnNewstickerAdapter::~CXnNewstickerAdapter()
    {           
    if( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        }
        
    delete iPeriodicTimer;

    if( iFont && iReleaseFont )
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont( iFont ); 
        }
        
    delete iControl;
    
    if( iState == EAnimation )
        {
        TRAP_IGNORE( iSvgControl->StopL() );
        }
        
    delete iSvgControl;
    delete iBufferDevice;
	delete iDrawingBuffer;
	delete iBufferGc;
    delete iBackgroundBitmap;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetTextPropertiesL
// Sets text properties, such as color, font, etc.
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetTextPropertiesL()
    {
    if (iFont && iReleaseFont)
        {
        CWsScreenDevice* dev = iCoeEnv->ScreenDevice();
        dev->ReleaseFont(iFont); 
        iFont = NULL;
        }
    CXnUtils::CreateFontL(iNode, iFont, iReleaseFont);
    iControl->SetFont(iFont);
    
    TBool colorSet(EFalse);
    CXnProperty* colorProperty = iNode.GetPropertyL(XnPropertyNames::appearance::common::KColor);
    if (colorProperty != NULL)
        {
        CXnDomProperty* domProperty = colorProperty->Property();
        if(domProperty)
            {
            TRgb textColor;
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
                iControl->SetTextColor(textColor);
                colorSet = ETrue;
                }
            }
        }

    if(!colorSet) // Use text skin color
        {
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TRgb textColor;
        TInt error = AknsUtils::GetCachedColor( skinInstance, textColor, 
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        if (error == KErrNone)
            {
            iControl->SetTextColor(textColor);
            }
        }
    
    CXnProperty* textDecorationProp = 
        iNode.GetPropertyL(XnPropertyNames::appearance::common::KTextDecoration);
    if(textDecorationProp)
        {
        CXnDomList& propertyValueList = textDecorationProp->Property()->PropertyValueList();
        TInt valueCount = propertyValueList.Length();
        for (TInt i = 0; i < valueCount; ++i)
            {
            CXnDomPropertyValue* value = 
                static_cast<CXnDomPropertyValue*>(propertyValueList.Item(i));
            if (value->StringValueL() == 
                XnPropertyNames::appearance::common::textdecoration::KUnderline)
                {
                iControl->SetTextUnderlineStyle(EUnderlineOn);
                }
            if (value->StringValueL() == 
                XnPropertyNames::appearance::common::textdecoration::KLinethrough)
                {
                iControl->SetTextStrikethroughStyle(EStrikethroughOn);
                }
            }
        }
    
    TGulAlignmentValue alignment = CXnUtils::TextAlignment( iNode );
    
    TInt textAlignment( ELayoutAlignLeft ); // all are vertically top aligned by default (avkon)
    
    switch( alignment )
        {
        case EHCenterVCenter:            
            textAlignment = ELayoutAlignCenter;                 
            break;
        case EHRightVCenter:            
            textAlignment = ELayoutAlignRight;                
            break;
        default: 
            break;    
        }
    
    iControl->SetTextAlignment(textAlignment);
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::AppendTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::AppendTitleL(const TDesC& aTitle)
    {
    iControl->AppendTitleL(aTitle);
    iNode.SetDirtyL();
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::InsertTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::InsertTitleL(const TDesC& aTitle, TInt aIndex)
    {
    iControl->InsertTitleL(aTitle, aIndex);
    iNode.SetDirtyL();
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::UpdateTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::UpdateTitleL(const TDesC& aTitle, TInt aIndex)
    {
    iControl->UpdateTitleL(aTitle, aIndex);
    iNode.SetDirtyL();
    if ( iRestartAfterUpdate )
        {
        RestartL();
        }
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::DeleteTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::DeleteTitleL(TInt aIndex)
    {
    if( iState == EAnimation && iControl->CurrentTitleIndex() == aIndex )
        {
        //  About to delete the currently showing animation
        iSvgControl->StopL();
        iControl->DeleteTitleL( aIndex );

        RestartL();
        }
    else
        {
        iControl->DeleteTitleL(aIndex);
        }

    iNode.SetDirtyL();
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::CurrentTitleIndex
// ---------------------------------------------------------
//
TInt CXnNewstickerAdapter::CurrentTitleIndex()
    {
    return iControl->CurrentTitleIndex();
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::Title
// ---------------------------------------------------------
//
const TDesC& CXnNewstickerAdapter::Title(TInt aIndex)
    {
    return iControl->Title(aIndex);
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::SetSeparatorImageL
// ---------------------------------------------------------
//
TInt CXnNewstickerAdapter::SetSeparatorImageL(CGulIcon* aIcon)
    {
    return iControl->SetSeparatorImageL(aIcon);
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::ClearTitles
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::ClearTitles()
    {
    iControl->ClearTitles();
    TRAP_IGNORE(iNode.SetDirtyL());
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::AppendSvgTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::AppendSvgTitleL(const TDesC8& aByteData)
    {
    iControl->AppendSvgTitleL(aByteData);
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::InsertSvgTitleL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::InsertSvgTitleL(const TDesC8& aByteData, TInt aIndex)
    {
    iControl->InsertSvgTitleL(aByteData, aIndex);
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::SetCallbackInterfaceL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::SetCallbackInterfaceL(
        XnNewstickerInterface::MXnNewstickerCallbackInterface* aCallback)
    {
    iCallback = aCallback;
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::TitleScrolled
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::TitleScrolled(TInt aTitleIndex)
    {
    if (iCallback)
        {
        iCallback->TitleScrolled(aTitleIndex);
        }
    // if not looping, see if the last item was scrolled...
    if( !iControl->ScrollLooping() &&
         aTitleIndex == iControl->TitleCount() - 1 )
        {
        iState = EScrollEnded;
        TRAP_IGNORE( StopL() );
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawBackground()
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DrawBackgroundL(const TRect& aRect, CWindowGc& aGc) const
    {
    RPointerArray<CXnControlAdapter> adapters;
    
    for ( CXnNodePluginIf* node = &iNode; node; ) 
        {
        CXnControlAdapter* adapter( node->Control() );
        
        if ( adapter )
            {
            adapters.Append( adapter );
            }
                        
        node = node->ParentL();
        }
    
    for ( TInt i = adapters.Count() - 1; i >= 0; --i )
        {
        adapters[i]->Draw( aRect, aGc );
        }
    
    adapters.Reset();        
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetTimerToDisplaytime()
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetTimerToDisplaytime()
    {
    if( iCurrentInterval != iAlternateInterval )
        {
        iPeriodicTimer->Cancel();
        
        iPeriodicTimer->Start(
            TTimeIntervalMicroSeconds32( iAlternateInterval ),
            TTimeIntervalMicroSeconds32( iAlternateInterval ), 
            TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) );
        iCurrentInterval = iAlternateInterval;
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::SetTimerToScrolltime()
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::SetTimerToScrolltime()
    {
    if( iCurrentInterval != iInterval )
        {
        iPeriodicTimer->Cancel();
        
        iPeriodicTimer->Start(
            TTimeIntervalMicroSeconds32( iInterval ),
            TTimeIntervalMicroSeconds32( iInterval ), 
            TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) );
        iCurrentInterval = iInterval;
        }
    }
// ---------------------------------------------------------
// CXnNewstickerAdapter::StartL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::StartL()
    {
    if( iDisplay && !iPowerSaveMode )
        {
        if( iControl->TitleCount() == 0 )
            {
            // Nothing to show
            return;
            }

        if( iPeriodicTimer && !iPeriodicTimer->IsActive() )
            {
            TInt startDelay( ( iState == ENotStarted ) ? iDelay : KNoDelay );

            //  Start
            if ( ScrollBehaviour() == CXnNewstickerAdapter::EScrollAlternate &&
                 iControl->CalculateCurrentTextFitInNewstickerRect() )
                {
                iPeriodicTimer->Start(                
                    TTimeIntervalMicroSeconds32( startDelay ),
                    TTimeIntervalMicroSeconds32( iAlternateInterval ), 
                    TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) );
                iCurrentInterval = iAlternateInterval;
                }
            else
                {
                iPeriodicTimer->Start(                
                    TTimeIntervalMicroSeconds32( startDelay ),
                    TTimeIntervalMicroSeconds32( iInterval ), 
                    TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) );
                iCurrentInterval = iInterval;
                }
        
            iState = EText;                       
            }            
        }
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::StopL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::StopL()
    {
    if( iState == EAnimation )
        {
        iSvgControl->StopL();
        }

    //  Have we already been started
    if( iPeriodicTimer->IsActive() )
        {
        //  Yes, cancel and set the state
       iPeriodicTimer->Cancel();
       
       if ( iState == EScrollEnded )
           {
           iControl->MoveToLastL();
           iControl->DrawStatic();
           }
       else if ( iState == EText || iState == EAnimation )
           {
           iState = EPaused;
           }
        }
    // Entering power save mode => Move to last item and draw it
    if ( ( iScrollBehaviour == EAlternate ||
          iScrollBehaviour == EScrollAlternate )
        && iPowerSaveMode )
       {
       iState = EPaused;
       iControl->MoveToLastL();
       iControl->DrawStatic();
       }        
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::ShowSvgL
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::ShowSvgL()
    {
    //  Get next animation from model and start it
    iSvgControl->StartL( iControl->CurrentSvgTitle() );

    //  Set the end timer
    iState = EAnimation;
    
    iPeriodicTimer->Cancel();
    
    iPeriodicTimer->Start(
        TTimeIntervalMicroSeconds32( iAnimationTime ),
        TTimeIntervalMicroSeconds32( iAnimationTime ), 
        TCallBack( CXnNewstickerAdapter::PeriodicEventL, this ) );
    iCurrentInterval = iAnimationTime;
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::PeriodicEventL
// ---------------------------------------------------------
//
TInt CXnNewstickerAdapter::PeriodicEventL( TAny* aPtr )
    {
    CXnNewstickerAdapter* self = static_cast< CXnNewstickerAdapter* >( aPtr );
    
    if( self->iState == EText )
        {
        //  Do the text scroll
        self->iControl->DoScrollL();
        }
    else if( self->iState == EAnimation )
        {
        self->iSvgControl->StopL();
        self->TitleScrolled( self->iControl->CurrentTitleIndex() );
        //  Animation scroll has been ended, start the next scroll
        self->iState = ENotStarted;
        self->iPeriodicTimer->Cancel();
        self->StartL();
        //  Do the first draw
        self->iControl->SetBeginningState();
        self->iControl->DoScrollL();
        }

    return KErrNone;
    }

void CXnNewstickerAdapter::RestartL()
    {
    if ( iPowerSaveMode )
        {
        return;
        }
    //  Restart
     iState = ENotStarted;
     iPeriodicTimer->Cancel();
     
     iControl->MoveToFirstL();
     StartL();
     
     //  Do the first draw
     iControl->SetBeginningState();
    }
// ---------------------------------------------------------
// CXnNewstickerAdapter::SizeChanged()
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::SizeChanged()
    {
    CXnControlAdapter::SizeChanged();
    TRAP_IGNORE( SizeChangedL() );
    }

// ---------------------------------------------------------
// CXnNewstickerAdapter::SizeChangedL()
// ---------------------------------------------------------
//
void CXnNewstickerAdapter::SizeChangedL()
    {        
    UpdateDoubleBufferL();

    TRect rect( Rect() );            
            
    iControl->SetNewstickerRect( rect );
    iSvgControl->SetSvgRectL( rect, *SystemGc().Device() );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::HandlePointerEventL
// -----------------------------------------------------------------------------
// 
void CXnNewstickerAdapter::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
	if ( AknLayoutUtils::PenEnabled() )
	    {
        CXnControlAdapter::HandlePointerEventL(aPointerEvent);
        }
    }
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {
    SetTextPropertiesL();
    SetNewstickerPropertiesL();
    //New code to check do we need Start or Stop Newsticker
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
               
        if( name == XnPropertyNames::style::common::KDisplay ||
        	name == XnPropertyNames::style::common::KVisibility )
        	{               
            if( CheckDisplayL( iNode ) )
                {
                StartL();
                }
            else
                {
                StopL();
                }                   
        	}
        }
    }
    	
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::FocusChanged
// -----------------------------------------------------------------------------
// 
void CXnNewstickerAdapter::FocusChanged(TDrawNow /*aDrawNow*/) 
    {
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

void CXnNewstickerAdapter::ReportNewstickerEventL(const TDesC8& aEventName)
    { 
    TBuf8<8>index(KNullDesC8);
    index.Num(CurrentTitleIndex());
    
    iNode.ReportTriggerEventL(aEventName,
            XnPropertyNames::action::trigger::name::KTitleIndex, index);
    }
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::OfferKeyEventL
// -----------------------------------------------------------------------------
// 
TKeyResponse CXnNewstickerAdapter::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse    resp = EKeyWasNotConsumed;
    TBool           restart = EFalse;

    CXnControlAdapter::OfferKeyEventL(aKeyEvent, aType);

    if(aType == EEventKey)
        {
        if (aKeyEvent.iScanCode == EStdKeyRightArrow)
            {
            restart = ETrue;
            if(iControl->IsWestern())
                {
                iControl->MoveToNextL();
                }
            else
                {
                iControl->MoveToPrevL();
                }
            StopL();
            }
        else if (aKeyEvent.iScanCode == EStdKeyLeftArrow)
            {
            restart = ETrue;
            if(iControl->IsWestern())
                {
                iControl->MoveToPrevL();
                }
            else
                {
                iControl->MoveToNextL();
                }
            StopL();
            }
        }
    if(aType == EEventKeyUp)
        {
        if (aKeyEvent.iScanCode == EStdKeyDevice3) // rocker
            {
            restart = ETrue;
            StopL();
            iControl->MoveToCurrent();            
            }
        }
        
    if( restart )
        {
        //  Restart and do the first draw
        iState = ENotStarted;
        StartL();
        iControl->DoScrollL();
        }
        
    return resp;
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::Draw
// Draws the text component
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::Draw(const TRect& aRect) const
    {
    CXnControlAdapter::Draw( aRect );
            
    TRAP_IGNORE( UpdateBackgroundL() );
    // In powersave-/editmode only draw the last title. 
    if ( iScrollBehaviour == EAlternate && iPowerSaveMode )
        {
        iControl->DrawStatic();
        
        // Blit double buffer
        SystemGc().BitBlt( Rect().iTl, iDrawingBuffer );    
        }
    else if( iState == EText || iState == EPaused && 
            iControl->CurrentTitleIndex() >= 0 )
        {
        iControl->Draw();

        // Blit double buffer
        SystemGc().BitBlt( Rect().iTl, iDrawingBuffer );                
        }
    else if( iState == EAnimation )
        {                            
        iSvgControl->Draw();
        
        // Blit double buffer
        SystemGc().BitBlt( Rect().iTl, iDrawingBuffer );            
        }
    else if( iState == EScrollEnded )
        {
        iControl->DrawStatic();
        SystemGc().BitBlt( Rect().iTl, iDrawingBuffer );
        }
    }
    
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::UpdateDoubleBufferL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::UpdateDoubleBufferL()
    {
	TDisplayMode displayMode( SystemGc().Device()->DisplayMode() );
	
    // Create double buffer
    if( iDrawingBuffer ) 
        {
        delete iDrawingBuffer;
        iDrawingBuffer = NULL;
        delete iBufferDevice;
        iBufferDevice = NULL;
        delete iBufferGc;
        iBufferGc = NULL;        
        }
        
	iDrawingBuffer = new ( ELeave ) CFbsBitmap();
	iDrawingBuffer->Create( iNode.Rect().Size(), displayMode );
    iBufferDevice = CFbsBitmapDevice::NewL( iDrawingBuffer );
    iBufferDevice->CreateBitmapContext( iBufferGc );
    iBufferGc->UseFont( iControl->Font() );
    iBufferGc->SetPenColor( iControl->TextColor() );
    iBufferGc->SetStrikethroughStyle( iControl->TextStrikethroughStyle() );
    iBufferGc->SetUnderlineStyle( iControl->TextUnderlineStyle() );
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::UpdateBackgroundL
// -----------------------------------------------------------------------------
//
void CXnNewstickerAdapter::UpdateBackgroundL() const
    {
    TDisplayMode displayMode( SystemGc().Device()->DisplayMode() );
    
    // Create "screenshot" from the background appearance
    if( iBackgroundBitmap )
        {
        delete iBackgroundBitmap;
        iBackgroundBitmap = NULL;        
        }
    
    iBackgroundBitmap = new ( ELeave ) CFbsBitmap();
	iBackgroundBitmap->Create( iNode.Rect().Size(), displayMode );
	
	CFbsBitmap* tmpBitmap = new ( ELeave ) CFbsBitmap();
	CleanupStack::PushL( tmpBitmap );
	
	CWsScreenDevice* scrDevice = static_cast< CWsScreenDevice* >( SystemGc().Device() );
    
    TSize tmpSize( scrDevice->SizeInPixels() );
    
	tmpBitmap->Create( tmpSize, displayMode );
	
    CFbsBitmapDevice* tmpDevice = CFbsBitmapDevice::NewL( tmpBitmap );
	CleanupStack::PushL( tmpDevice );
    
    CBitmapContext* bc( NULL );
    
    tmpDevice->CreateBitmapContext( bc );    
    CleanupStack::PushL( bc );
    
    DrawBackgroundL( iNode.Rect(), reinterpret_cast< CWindowGc& >( *bc ) );
    
    CopyBitmapData( *iBackgroundBitmap, *tmpBitmap, iNode.Rect().iTl );
    
    CleanupStack::PopAndDestroy( 3 );
    }
    
// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoEnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DoEnterPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    iDisplay = EFalse;        
    iPowerSaveMode = ETrue;
        
    StopL();
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DoExitPowerSaveModeL
// Exit power save mode
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DoExitPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    iPowerSaveMode = EFalse;
            
    if( CheckDisplayL( iNode ) )
        {
        RestartL();
        
        if( iDrawingBuffer )
            {
            DrawNow(); 
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawNow
// Draw
// -----------------------------------------------------------------------------
void CXnNewstickerAdapter::DrawNow() const
    {
    TBool parentDrawn = EFalse;
    TRAP_IGNORE( parentDrawn = DrawFocusableParentL() );
    if ( !parentDrawn )
        {
        CCoeControl::DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::DrawFocusableParentL
// Draws focusable parent
// -----------------------------------------------------------------------------
TBool CXnNewstickerAdapter::DrawFocusableParentL() const
    {
    TBool parentDrawn = EFalse;
    
    if ( !iNode.IsFocusedState() )
        {
        // node is not focusable, find focusable and focused parent
        CXnNodePluginIf* parent = iNode.ParentL();
        while ( parent && !parent->IsFocusedState() )
            {
            parent = parent->ParentL();
            }
        
        if (parent)
            {
            // focusable parent found, draw it
            parent->Control()->DrawNow();
            parentDrawn = ETrue;
            }
        }
    
    return parentDrawn;
    }

// End of file
