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
* Description:  Clock rendering plugin
*
*/

// System includes
#include <e32base.h>
#include <e32const.h>
#include <AknLayoutFont.h>
#include <AknsUtils.h>

// User includes
#include "xndomproperty.h"
#include "xndomlist.h"
#include "xnproperty.h"
#include "xntype.h"
#include "xnuienginepluginif.h"
#include "xnnodepluginif.h"
#include "xncontroladapter.h"
#include "xnclockcontrol.h"
#include "xnclockadapter.h"
#include "xndatecontrol.h"
#include "c_xnutils.h"

// Constants
_LIT8( KDisplay, "display" );
_LIT8( KNone, "none" );
_LIT8( KBlock, "block" );

// ============================ LOCAL FUNCTIONS ===============================    

// -----------------------------------------------------------------------------
// ClockFormat
// Resolves the clock format
// -----------------------------------------------------------------------------
//
static TClockFormat ClockFormat( CXnProperty* aProperty, TBool& aUseLocale )
    {
    // Default, from locale    
    TClockFormat format( TLocale().ClockFormat() );
    
    aUseLocale = ETrue;
    
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
        
        if( name == XnPropertyNames::clock::KS60Format )
            {                                                        
            const TDesC8& value( aProperty->StringValue() );            
                                    
            if( value == XnPropertyNames::clock::format::KDigital )
                {                          
                format = EClockDigital;
                aUseLocale = EFalse;
                }
            else if( value == XnPropertyNames::clock::format::KAnalog )
                {
                format = EClockAnalog;
                aUseLocale = EFalse;
                }                        
            }    
        }
    
    return format;
    }
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnClockAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnClockAdapter* CXnClockAdapter::NewL( CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode )
    {
	CXnClockAdapter* self = new( ELeave ) CXnClockAdapter( aParent, aNode );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;	
    }
    
// -----------------------------------------------------------------------------
// CXnClockAdapter::CXnClockAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnClockAdapter::CXnClockAdapter( CXnControlAdapter* aParent, 
    CXnNodePluginIf& aNode )
    : iParent( aParent ), iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::~CXnClockAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnClockAdapter::~CXnClockAdapter()
    {    
    iCoeEnv->RemoveMessageMonitorObserver( *this );    

    delete iLightObserver;    
    delete iClockControl;
    delete iDateControl;
    delete iDayControl;
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::ConstructL()
    {          
    CXnControlAdapter::ConstructL( iNode );   
    
    iForeground = iAvkonAppUi->IsForeground();
    
    iLightObserver = CHWRMLight::NewL( this );
    
    TBool useLocale;
    CXnProperty* prop( 
        iNode.GetPropertyL( XnPropertyNames::clock::KS60Format ) );
    TClockFormat format( ClockFormat( prop, useLocale ) );
    
    RPointerArray< CXnNodePluginIf > children( iNode.ChildrenL() );
    CleanupClosePushL( children );
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNodePluginIf* child( children[i] );
        const TDesC8& clocktype = child->Type()->Type();
        
        if ( clocktype == XnPropertyNames::clock::format::KDigital )
            {
            iDigital = child;
            }
        else if ( clocktype == XnPropertyNames::clock::format::KAnalog )
            {
            iAnalog = child;
            }
        else if ( clocktype == XnPropertyNames::clock::KDate )
            {
            if ( !iDateControl )
                {
                iDateControl = CXnDateControl::NewL( *this, EDate );
                }
            iDate = child;
            }

        RPointerArray< CXnNodePluginIf > clockChildren( child->ChildrenL() );
        CleanupClosePushL( clockChildren );
        for ( TInt j = 0; j < clockChildren.Count(); j++ )
            {
            CXnNodePluginIf* clockChild( clockChildren[j] );
            const TDesC8& childType = clockChild->Type()->Type();
            
            if ( childType == XnPropertyNames::clock::KFace12 )
                {
                if ( !iClockControl )
                    {
                    iClockControl = CXnClockControl::NewL( *this, useLocale, format );
                    }
                iDigitalFace12 = clockChild;
                }
            else if ( childType == XnPropertyNames::clock::KFace24 )
                {
                if ( !iClockControl )
                    {
                    iClockControl = CXnClockControl::NewL( *this, useLocale, format );
                    }
                iDigitalFace24 = clockChild;
                }
            else if ( childType == XnPropertyNames::clock::KFace )
               {
                if ( !iClockControl )
                    {
                    iClockControl = CXnClockControl::NewL( *this, useLocale, format );
                    }
                iAnalogFace = clockChild;
               }
            else if ( childType == XnPropertyNames::clock::KDate &&
                      clocktype == XnPropertyNames::clock::format::KDigital )
                {
                if ( !iDateControl )
                    {
                    iDateControl = CXnDateControl::NewL( *this, EDate );
                    }
                iDigitalDate = clockChild;
                }
            else if ( childType == XnPropertyNames::clock::KDate &&
                      clocktype == XnPropertyNames::clock::format::KAnalog )
                {
                if ( !iDateControl )
                    {
                    iDateControl = CXnDateControl::NewL( *this, EDate );
                    }
                iAnalogDate = clockChild;
                }
            else if ( childType == XnPropertyNames::clock::KDay )
               {
                if ( !iDayControl )
                    {
                    iDayControl = CXnDateControl::NewL( *this, EDay );
                    }
                iDay = clockChild;
               }
            else if ( childType == XnPropertyNames::clock::KAmpm )
               {
                iAmpm = clockChild;
               }
            }
        CleanupStack::PopAndDestroy( &clockChildren );
        }
        
    CleanupStack::PopAndDestroy( &children );
    
    SetClockFormatL( format );
    
    iCoeEnv->AddMessageMonitorObserverL( *this );
	}

// -----------------------------------------------------------------------------
// CXnClockAdapter::SetDisplayToBlockL
// -----------------------------------------------------------------------------
// 
void CXnClockAdapter::SetDisplayToBlockL( CXnNodePluginIf& aNode, TBool aBlock )
    {
    CXnDomStringPool& sp = aNode.UiEngineL()->StringPool();
    CXnProperty* prop;
    if ( aBlock )
        {
        prop = CXnProperty::NewL( 
            KDisplay, KBlock, CXnDomPropertyValue::EString, sp );
        }
    else
        {
        prop = CXnProperty::NewL( 
            KDisplay, KNone, CXnDomPropertyValue::EString, sp );
        }
    CleanupStack::PushL( prop );
    aNode.SetPropertyL( prop );
    CleanupStack::Pop( prop );
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::SetClockFormatL
// -----------------------------------------------------------------------------
// 
void CXnClockAdapter::SetClockFormatL( TClockFormat aFormat )
    {
    if ( iAnalog && iDigital )
        {
        if ( aFormat == EClockAnalog  )
            {
            SetDisplayToBlockL( *iAnalog, ETrue );
            SetDisplayToBlockL( *iDigital, EFalse );
            }
        else
            {
            SetDisplayToBlockL( *iDigital, ETrue );
            SetDisplayToBlockL( *iAnalog, EFalse );
            // check whether time format is 12 or 24
            if ( iDigitalFace12 && iDigitalFace24 )
                {
                if ( TLocale().TimeFormat() == ETime12 )
                    {
                    SetDisplayToBlockL( *iDigitalFace12, ETrue );
                    if ( iAmpm )
                        {
                        SetDisplayToBlockL( *iAmpm, ETrue );
                        }
                    SetDisplayToBlockL( *iDigitalFace24, EFalse );
                    }
                else
                    {
                    SetDisplayToBlockL( *iDigitalFace24, ETrue );
                    SetDisplayToBlockL( *iDigitalFace12, EFalse );
                    if ( iAmpm )
                        {
                        SetDisplayToBlockL( *iAmpm, EFalse );
                        }
                    }
                }
            }
        iNode.UiEngineL()->LayoutUIL( &iNode );
        }
    }
 
// -----------------------------------------------------------------------------
// CXnClockAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {        
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
        
        if( iClockControl && name == XnPropertyNames::clock::KS60Format )
            {
            TBool useLocale;
            
            TClockFormat format( ClockFormat( aProperty, useLocale ) );
            
            iClockControl->SetFormatL( useLocale, format );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::UpdateDisplay
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::UpdateDisplay() const
    {    
    if ( !iNode.Rect().IsEmpty() )
        {
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::Draw
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::Draw( const TRect& aRect ) const
    {                           
    CXnControlAdapter::Draw( aRect );
    
    CWindowGc& gc( SystemGc() );

    if ( iClockControl )
        {
        TClockFormat format = iClockControl->Format();
        if ( format == EClockAnalog )
            {
            iClockControl->Draw( gc, iAnalogFace, NULL );
            }
        else
            {
            if ( TLocale().TimeFormat() == ETime12 )
                {
                iClockControl->Draw( gc, iDigitalFace12, iAmpm );
                }
            else
                {
                iClockControl->Draw( gc, iDigitalFace24, NULL );
                }
            }
        }
    
    if ( iDateControl )
        {
        if ( iClockControl && iClockControl->Format() == EClockAnalog && iAnalogDate )
            {
            TRAP_IGNORE( iDateControl->DrawL( gc, iAnalogDate ) );
            }
        else if ( iClockControl && iClockControl->Format() == EClockDigital && iDigitalDate )
            {
            TRAP_IGNORE( iDateControl->DrawL( gc, iDigitalDate ) );
            }
        else
            {
            TRAP_IGNORE( iDateControl->DrawL( gc, iDate ) );
            }
        }
    
    if ( iDayControl )
        {
        TRAP_IGNORE( iDayControl->DrawL( gc, iDay ) );
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::MakeVisible
// -----------------------------------------------------------------------------
//        
void CXnClockAdapter::MakeVisible( TBool aVisible )
    {
    TBool visible( IsVisible() );
    
    if ( visible != aVisible )
        {
        CCoeControl::MakeVisible( aVisible );
        
        if ( aVisible )
            {
            // Start clock ensures UI state
            StartClock();
            }
        else
            {
            StopClock();        
            }    
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::MonitorWsMessage
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::MonitorWsMessage( const TWsEvent& aEvent )
    {   
    TInt type( aEvent.Type() );
    
    TBool foreground( iForeground );
        
    if( type == KAknFullOrPartialForegroundGained )
        {
        iForeground = ETrue;        
        }
    else if( type == KAknFullOrPartialForegroundLost )        
        {
        iForeground = EFalse;        
        }

    if( foreground != iForeground )            
        {
        if( iForeground )
            {
            // check if clock format has changed
            if ( iClockControl )
                {
                TRAP_IGNORE( iClockControl->CheckClockFormatL() );
                }
            UpdateDisplay();
            // Start clock ensures UI state
            StartClock();
            }
        else
            {
            StopClock();
            }            
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::LightStatusChanged
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::LightStatusChanged( TInt aTarget, 
    CHWRMLight::TLightStatus aStatus )
    {
    if ( aTarget == CHWRMLight::EPrimaryDisplay )
        {
        if ( aStatus == CHWRMLight::ELightOn )
            {
            iLightsOn = ETrue;
            
            if ( iForeground )
                {
                UpdateDisplay();
                }
                        
            // Start clock ensures UI state
            StartClock();
            }
        else if ( aStatus == CHWRMLight::ELightOff )
            {
            iLightsOn = EFalse;
        
            StopClock();
            }        
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::GetTextAlignL
// -----------------------------------------------------------------------------
// 
CGraphicsContext::TTextAlign CXnClockAdapter::GetTextAlignL( CXnNodePluginIf& aNode )
    {
    CGraphicsContext::TTextAlign ret = CGraphicsContext::ECenter;
    CXnProperty* prop( aNode.GetPropertyL( 
            XnPropertyNames::appearance::common::KTextAlign ) );
    
    if( prop )
        {
        if ( prop->StringValue() ==
                XnPropertyNames::appearance::common::textalign::KAlignLeft )
            {
            ret = CGraphicsContext::ELeft;
            }
        else if ( prop->StringValue() ==
                XnPropertyNames::appearance::common::textalign::KAlignRight )
            {
            ret = CGraphicsContext::ERight;
            }
        else if ( prop->StringValue() ==
                XnPropertyNames::appearance::common::textalign::KAlignCenter )
            {
            ret = CGraphicsContext::ECenter;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::CreateFontL
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::CreateFontL( CXnNodePluginIf& aNode,
                                   const TDesC& aFontName,
                                   CFont*& aFont )
    {
    TFontSpec spec;
    TInt dummy;
    
    CXnProperty* prop( aNode.GetPropertyL( 
        XnPropertyNames::clock::KS60FontSize ) );

    // default
    TInt height( ( aNode.Rect().Height() / 5 ) );

    if ( prop )
        {
        height = aNode.UiEngineL()->VerticalPixelValueL(
            prop, aNode.Rect().Height() );
        }
    else
        {
        // check if fontsize is defined in clock element
        prop = iNode.GetPropertyL( 
            XnPropertyNames::clock::KS60FontSize );
        if ( prop )
            {
            height = iNode.UiEngineL()->VerticalPixelValueL(
                prop, iNode.Rect().Height() );
            }
        }
                                        
    // No need to relase avkon font
    CXnUtils::CreateFontL( aFontName, 
                           height,
                           spec.iFontStyle,
                           aFont,
                           dummy );
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::CreateColorL
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::CreateColorL( CXnNodePluginIf& aNode, TRgb& aColor )
    {        
    TRgb color;
            
    TInt result( KErrNone );
        
    CXnProperty* prop( aNode.GetPropertyL( 
        XnPropertyNames::appearance::common::KColor ) );
    
    if( !prop )
        {
        // check if color is defined in clock element
        prop = iNode.GetPropertyL( 
            XnPropertyNames::appearance::common::KColor );
        if ( !prop )
            {
            return;
            }
        }
        
    CXnDomProperty* domProp( prop->Property() );
    
    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
    
    if( domProp )
        {        
        CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >
                                        ( domProp->PropertyValueList().Item( 0 ) );
        
        if( value->PrimitiveValueType() == CXnDomPropertyValue::ERgbColor )
            {
            color = value->RgbColorValueL();                
            }
        else if( !value->IsAutoIdent() )
        	{
            HBufC* str( prop->StringValueL() );
            CleanupStack::PushL( str );
            
            CXnUtils::StripQuotes( str );
            
            TInt index( 0 );
            TAknsItemID skinID;
            
            TBool idResolved( CXnUtils::ResolveSkinItemIDL( *str, skinID, index ) );
            
            result = KErrNotFound;
            
            if( idResolved )
            	{                	
            	result = AknsUtils::GetCachedColor( skin, color, skinID, index );
            	}                        

        	CleanupStack::PopAndDestroy( str );                	
        	}
        	
        if( value->IsAutoIdent() || result != KErrNone )
            {                                
            result = AknsUtils::GetCachedColor( skin, color, 
                                                KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );
            }
        }
        
    if( result == KErrNone )
        {
        aColor = color;
        }            
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::SkinChanged
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::SkinChanged()
    {
    if ( iClockControl )
        {
        iClockControl->ResetFont();
        }
    if ( iDateControl )
        {
        iDateControl->ResetFont();
        }
    if ( iDayControl )
        {
        iDayControl->ResetFont();
        }
    
    CXnControlAdapter::SkinChanged();
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::HandleScreenDeviceChangedL()
    {
    if ( iClockControl )
        {
        iClockControl->ResetFont();
        }
    if ( iDateControl )
        {
        iDateControl->ResetFont();
        }
    if ( iDayControl )
        {
        iDayControl->ResetFont();
        }
    
    CXnControlAdapter::HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::SizeChanged
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::SizeChanged()
    {
    if ( iClockControl )
        {
        iClockControl->ResetFont();
        }
    if ( iDateControl )
        {
        iDateControl->ResetFont();
        }
    if ( iDayControl )
        {
        iDayControl->ResetFont();
        }
    
    CXnControlAdapter::SizeChanged();    
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::StartClock
// -----------------------------------------------------------------------------
// 
void CXnClockAdapter::StartClock()
    {    
    if ( iClockControl )
        {        
        if ( iForeground && iLightsOn && IsVisible() )
            {
            iClockControl->StartTimer();
            }
        else
            {
            StopClock();
            }    
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::StopClock
// -----------------------------------------------------------------------------
// 
void CXnClockAdapter::StopClock()
    {
    if ( iClockControl )
        {
        iClockControl->StopTimer();
        }
    }

// End of file
