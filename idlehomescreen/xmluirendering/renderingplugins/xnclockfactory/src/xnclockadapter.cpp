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
#include "c_xnutils.h"

// Constants
_LIT8( KDateInformation, "Clock/DateInformation" );
_LIT8( KDayInformation, "Clock/DayInformation" );

_LIT( KDigitalFont, "EAknLogicalFontSecondaryFont" );
_LIT( KAmPmFont, "EAknLogicalFontSecondaryFont" );
_LIT( KDateFont, "EAknLogicalFontSecondaryFont" );

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
    
    RPointerArray< CXnNodePluginIf > children( iNode.ChildrenL() );
    CleanupClosePushL( children );
    
    for( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNodePluginIf* child( children[i] );
        
        CXnProperty* id( child->IdL() );
        
        if( id )         
            {
            if( id->StringValue() == KDateInformation )
                {
                iDate = child;                               
                }
            else if( id->StringValue() == KDayInformation )
                {
                iDay = child;
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &children );
    
    TBool useLocale;

    CXnProperty* prop( 
        iNode.GetPropertyL( XnPropertyNames::clock::KS60Format ) );    
    
    TClockFormat format( ClockFormat( prop, useLocale ) );   
    
    iClockControl = CXnClockControl::NewL( this, useLocale, format );    
    
    iCoeEnv->AddMessageMonitorObserverL( *this );
	}
 
// -----------------------------------------------------------------------------
// CXnClockAdapter::DoHandlePropertyChangeL
//
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {        
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
        
        if( name == XnPropertyNames::clock::KS60Format )
            {
            TBool useLocale;
            
            TClockFormat format( ClockFormat( aProperty, useLocale ) );
            
            iClockControl->SetFormatL( useLocale, format );
            }
        else if( name == XnPropertyNames::clock::KS60DigitalFontSize )            
            {
            iDigitalFont = NULL;            
            }
        else if( name == XnPropertyNames::clock::KS60AmPmFontSize )
            {
            iAmPmFont = NULL;
            }
        else if( name == XnPropertyNames::clock::KS60DateFontSize )
            {
            iDateFont = NULL;
            }
        else if( name == XnPropertyNames::appearance::common::KColor )
            {
            iColorSet = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::UpdateDisplay
// Draws the clock component
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
// Draws the clock component
// -----------------------------------------------------------------------------
//
void CXnClockAdapter::Draw( const TRect& aRect ) const
    {                           
    CXnControlAdapter::Draw( aRect );
    
    CWindowGc& gc( SystemGc() );
           
    iClockControl->Draw( gc, iNode.Rect() );
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::MakeVisible
// 
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
// 
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
// 
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
// CXnClockAdapter::FontL
// 
// -----------------------------------------------------------------------------
//    
const CAknLayoutFont* CXnClockAdapter::FontL( const TInt aType )
    {
    CFont* font( NULL );
    
    if( aType == EDigitalFont )
        {
        if( !iDigitalFont )
            {            
            CreateFontL( aType );
            }
        
        font = iDigitalFont;        
        }
    else if( aType == EAmPmFont )
        {
        if( !iAmPmFont )
            {
            CreateFontL( aType );        
            }
        
        font = iAmPmFont;
        }
    else if( aType == EDateFont )
        {
        if( !iDateFont )
            {
            CreateFontL( aType );        
            }
        
        font = iDateFont;        
        }
    
    if( font )
        {
        return CAknLayoutFont::AsCAknLayoutFontOrNull( font );
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::TextColorL
// 
// -----------------------------------------------------------------------------
//    
const TRgb& CXnClockAdapter::TextColorL()
    {    
    if( !iColorSet )
        {
        CreateColorL();
        }
        
    return iColor;            
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::Date
// 
// -----------------------------------------------------------------------------
//    
CXnNodePluginIf* CXnClockAdapter::Date() const
    {
    return iDate;
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::Day
// 
// -----------------------------------------------------------------------------
//    
CXnNodePluginIf* CXnClockAdapter::Day() const
    {
    return iDay;
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::CreateFontL
// 
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::CreateFontL( const TInt aType )   
    {
    TFontSpec spec;
    TInt dummy;

    if( aType == EDigitalFont )
        {
        iDigitalFont = NULL;
        
        CXnProperty* prop( iNode.GetPropertyL( 
            XnPropertyNames::clock::KS60DigitalFontSize ) );

        // default
        TInt height( ( iNode.Rect().Height() / 3 ) * 2 );

        if ( prop )
            {
            height = iNode.UiEngineL()->VerticalPixelValueL(
                prop, iNode.Rect().Height() );
            }
                                            
        // No need to relase avkon font
        CXnUtils::CreateFontL( KDigitalFont, 
                               height,
                               spec.iFontStyle,
                               iDigitalFont,
                               dummy );
        }
    else if( aType == EAmPmFont )
        {
        iAmPmFont = NULL;

        CXnProperty* prop( iNode.GetPropertyL( 
            XnPropertyNames::clock::KS60AmPmFontSize ) );
        
        // default
        TInt height( ( iNode.Rect().Height() / 5 ) );

        if ( prop )
            {
            height = iNode.UiEngineL()->VerticalPixelValueL(
                prop, iNode.Rect().Height() );
            }        
        
        // No need to relase avkon font
        CXnUtils::CreateFontL( KAmPmFont, 
                               height,
                               spec.iFontStyle,
                               iAmPmFont,
                               dummy );      
        }
    else if( aType == EDateFont )
        {
        iDateFont = NULL;
        
        CXnProperty* prop( iNode.GetPropertyL( 
            XnPropertyNames::clock::KS60DateFontSize ) );
        
        // default 
        TInt height( ( iNode.Rect().Height() / 5 ) );
        
        if ( prop )
            {
            height = iNode.UiEngineL()->VerticalPixelValueL(
                prop, iNode.Rect().Height() );
            }
        
        // No need to relase avkon font
        CXnUtils::CreateFontL( KDateFont, 
                               height,
                               spec.iFontStyle,
                               iDateFont,
                               dummy );              
        }
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::CreateColorL
// 
// -----------------------------------------------------------------------------
//    
void CXnClockAdapter::CreateColorL()
    {        
    TRgb color;
            
    TInt result( KErrNone );
        
    CXnProperty* prop( iNode.GetPropertyL( 
        XnPropertyNames::appearance::common::KColor ) );
    
    if( !prop )
        {
        return;
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
        iColorSet = ETrue;
        iColor = color;
        }            
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::SkinChanged
// Skin change notification
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::SkinChanged()
    {
    iAmPmFont = iDigitalFont = iDateFont = NULL;
    iColorSet = EFalse;
    
    CXnControlAdapter::SkinChanged();
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::HandleScreenDeviceChangedL
// Screen device change notification
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::HandleScreenDeviceChangedL()
    {
    iAmPmFont = iDigitalFont = iDateFont = NULL;
    iColorSet = EFalse;
    
    CXnControlAdapter::HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::SizeChanged
// Size changed notification
// -----------------------------------------------------------------------------
//  
void CXnClockAdapter::SizeChanged()
    {
    iAmPmFont = iDigitalFont = iDateFont = NULL;
    iColorSet = EFalse;
    
    CXnControlAdapter::SizeChanged();    
    }

// -----------------------------------------------------------------------------
// CXnClockAdapter::StartClock
// 
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
// 
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
