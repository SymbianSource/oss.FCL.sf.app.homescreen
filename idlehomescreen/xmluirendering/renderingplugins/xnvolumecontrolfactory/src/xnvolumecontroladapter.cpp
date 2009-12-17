/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a volume control component
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>
#include <barsc.h>
#include <Aknvolumecontrol.h>

#include "xnnodepluginif.h"
#include "xndompropertyvalue.h"
#include "xndomproperty.h"
#include "xndomlist.h"
#include "xnproperty.h"
#include "xncomponent.h"
#include "xnuienginepluginif.h"
#include "xncomponentnodeimpl.h"
#include "xncontroladapter.h"
#include "xntype.h"

#include "xnvolumecontroladapter.h"
#include "xnvolumecontrolobserver.h"
#include "xnvolumecontrol.rsg"

_LIT( KResourceFileName, "z:\\resource\\plugins\\xn3volumecontrol.rsc" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnVolumeControlAdapter* CXnVolumeControlAdapter::NewL(CXnControlAdapter* aParent, 
                                                       CXnNodePluginIf& aNode)
    {
	CXnVolumeControlAdapter* self = new( ELeave ) CXnVolumeControlAdapter(aParent, aNode);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;	
    }
    
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::CXnVolumeControlAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnVolumeControlAdapter::CXnVolumeControlAdapter(CXnControlAdapter* aParent, 
                                                 CXnNodePluginIf& aNode)
    : iParent( aParent ), iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::~CXnVolumeControlAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnVolumeControlAdapter::~CXnVolumeControlAdapter()
    {
    delete iVolumeControl;
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnVolumeControlAdapter::ConstructL()
    { 
    ConstructVolumeControlL();
    
    iVolumeControl->SetValue( ValueL() );
    
    CXnControlAdapter::ConstructL( iNode );
    }
    
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::ConstructVolumeControL
// -----------------------------------------------------------------------------
//    
void CXnVolumeControlAdapter::ConstructVolumeControlL()
	{	
	delete iVolumeControl;
	iVolumeControl = NULL;
	
    iVolumeControl = new ( ELeave ) CAknVolumeControl;
    iVolumeControl->SetContainerWindowL( *iParent );		
    iVolumeControl->SetBackground( this );
		
	RFs fsSession;
	
	TInt err( fsSession.Connect() );

    if( err )	    
	    {
	    User::Leave( err );
	    }
		
	CleanupClosePushL( fsSession );
	
	RResourceFile resourceFile;		
	resourceFile.OpenL( fsSession, KResourceFileName );	
	CleanupClosePushL( resourceFile );
    
	HBufC8* resource = resourceFile.AllocReadLC( R_XNVOLUMECONTROLDYNAMIC );
	
	TResourceReader reader;	
	reader.SetBuffer( resource );
	
	iVolumeControl->ConstructFromResourceL( reader );
	iVolumeControl->SetRect( iNode.Rect() );
	
	CleanupStack::PopAndDestroy( 3, &fsSession  ); // resource, resourceFile, fsSession
				
	SetVolumeControlPropertiesL();		
	}
    
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::SetObserver
// -----------------------------------------------------------------------------
//      
void CXnVolumeControlAdapter::SetObserver( MXnVolumeControlObserver* aObserver )
    {
    iObserver = aObserver;
    }
    
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::SetValue
// -----------------------------------------------------------------------------
//    
void CXnVolumeControlAdapter::SetValueL( TInt aValue )
	{
	TInt minRange;
	TInt maxRange;

	GetRangeL( minRange, maxRange );

	if( aValue >= minRange && aValue <= maxRange )
		{		    	
		CXnDomStringPool& sp = iNode.UiEngineL()->StringPool();		
		CXnDomPropertyValue* value = CXnDomPropertyValue::NewL( sp );
		CleanupStack::PushL( value );
		value->SetFloatValueL( CXnDomPropertyValue::ENumber, aValue );
		CXnProperty* valueProperty = CXnProperty::NewL( XnPropertyNames::action::KValue, value, sp );
		CleanupStack::Pop( value );
		CleanupStack::PushL( valueProperty );
		iNode.SetPropertyL( valueProperty );
		CleanupStack::Pop( valueProperty );
		}
	}

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::Value
// -----------------------------------------------------------------------------
//
TInt CXnVolumeControlAdapter::ValueL()
	{
	return GetCurrentValueL();
	}
	
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::SetRange
// -----------------------------------------------------------------------------
//	
void CXnVolumeControlAdapter::SetRangeL( TInt aMinimumValue, TInt aMaximumValue )
	{
	CXnDomStringPool& sp = iNode.UiEngineL()->StringPool();
	
	// Set min value property
	CXnDomPropertyValue* minValue = CXnDomPropertyValue::NewL( sp );
	CleanupStack::PushL( minValue );
	minValue->SetFloatValueL( CXnDomPropertyValue::ENumber, aMinimumValue );
	CXnProperty* minValueProperty = CXnProperty::NewL( XnPropertyNames::volumecontrol::KMinRange, minValue, sp );
	CleanupStack::Pop( minValue );
	CleanupStack::PushL( minValueProperty );
	iNode.SetPropertyL( minValueProperty );
	CleanupStack::Pop( minValueProperty );

	// Set max value property
	CXnDomPropertyValue* maxValue = CXnDomPropertyValue::NewL( sp );
	CleanupStack::PushL( maxValue );
	maxValue->SetFloatValueL( CXnDomPropertyValue::ENumber, aMaximumValue );
	CXnProperty* maxValueProperty = CXnProperty::NewL( XnPropertyNames::volumecontrol::KMaxRange, maxValue, sp );
	CleanupStack::Pop( maxValue );
	CleanupStack::PushL( maxValueProperty );
	iNode.SetPropertyL( maxValueProperty );
	CleanupStack::Pop( maxValueProperty );
	
	iVolumeControl->SetRange( aMinimumValue, aMaximumValue );
	iSteps = aMaximumValue - aMinimumValue;
	}
	
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::GetRange
// -----------------------------------------------------------------------------
//	
void CXnVolumeControlAdapter::GetRangeL( TInt& aMinimumValue, TInt& aMaximumValue )
	{
	CXnProperty* minRange = iNode.GetPropertyL( XnPropertyNames::volumecontrol::KMinRange );
	aMinimumValue = static_cast<TInt>( minRange->FloatValueL() );
	
	CXnProperty* maxRange = iNode.GetPropertyL( XnPropertyNames::volumecontrol::KMaxRange );
	aMaximumValue = static_cast<TInt>( maxRange->FloatValueL() );
	}

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnVolumeControlAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {
    if (aProperty)
    	{
    TInt minValue;
    TInt maxValue;
    GetRangeL( minValue, maxValue );
    
    if( aProperty->Property()->Name() == XnPropertyNames::volumecontrol::KVolumeLevelAdjustment )
    	{
    	CXnDomStringPool& sp = iNode.UiEngineL()->StringPool();
    	CXnDomPropertyValue* value = CXnDomPropertyValue::NewL( sp );
    	CleanupStack::PushL( value );
    	
    	const TDesC8& strValue = aProperty->StringValue();
    	// Increase volume
    	if( strValue == XnPropertyNames::volumecontrol::KIncrease )
    		{
    		TInt newValue = ValueL() + 1;
    		if( newValue >= minValue && newValue <= maxValue )
    			{
    			value->SetFloatValueL( CXnDomPropertyValue::ENumber, ValueL() + 1 );
    			}
    		else
    			{
    			CleanupStack::PopAndDestroy( value );    			
    			return;
    			}
    		}
    	// Decrease volume
    	else if( strValue == XnPropertyNames::volumecontrol::KDecrease )
    		{
    		TInt newValue = ValueL() - 1;
    		if( newValue >= minValue && newValue <= maxValue )
    			{    		
    			value->SetFloatValueL( CXnDomPropertyValue::ENumber, ValueL() - 1 );
    			}
    		else
    			{
    			CleanupStack::PopAndDestroy( value );    			
    			return;
    			}    			
    		}
    	// Mute
    	else if( strValue == XnPropertyNames::volumecontrol::KMute )
    		{
    		if( iMuted )
    			{
    			value->SetFloatValueL( CXnDomPropertyValue::ENumber, iMutedValue );
    			iMuted = EFalse;
    			}
    		else
    			{
    			value->SetFloatValueL( CXnDomPropertyValue::ENumber, minValue );
    			iMutedValue = GetCurrentValueL();
    			iMuted = ETrue;
    			}
    		}
    	CXnProperty* valueProperty = CXnProperty::NewL( XnPropertyNames::action::KValue, value, sp );
    	CleanupStack::Pop( value );
    	CleanupStack::PushL( valueProperty );
    	iNode.SetPropertyL( valueProperty );
    	CleanupStack::Pop( valueProperty );  	
    	}
   	else if( aProperty->Property()->Name() == XnPropertyNames::action::KValue )
   		{
   		TInt newValue = static_cast<TInt>( aProperty->FloatValueL() );
   		if( newValue >= minValue && newValue <= maxValue )
   			{
   			iVolumeControl->SetValue( newValue );
   			NotifyObserverL();
	   			}
   			}
    	}
    else
    	{
    	return;
   		}
    }

// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//
void CXnVolumeControlAdapter::HandleScreenDeviceChangedL()
    {
    
    ConstructVolumeControlL();
            
    CXnControlAdapter::HandleScreenDeviceChangedL();
    }

// ---------------------------------------------------------
// CXnVolumeControlAdapter::SizeChanged()
// Called by framework when the view size is changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CXnVolumeControlAdapter::SizeChanged()
    {
    CXnControlAdapter::SizeChanged();
    if( iVolumeControl )
        {
        iVolumeControl->SetRect( iNode.Rect() );
        }	
    }
    
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::CountComponentControls
// Returns the number of component controls.
// -----------------------------------------------------------------------------
//    
TInt CXnVolumeControlAdapter::CountComponentControls() const
	{
    return 1;
	}
	
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::ComponentControl
// Returns the component control of the given index
// -----------------------------------------------------------------------------
//	
CCoeControl* CXnVolumeControlAdapter::ComponentControl( TInt aIndex ) const
	{
	if( aIndex == 0 )
	    {
	    return iVolumeControl;
	    }
	
	return NULL;
	}	

// -----------------------------------------------------------------------------
// CXnNewstickerAdapter::Draw
// -----------------------------------------------------------------------------
// 
void CXnVolumeControlAdapter::Draw(CWindowGc& aGc, const CCoeControl& /*aControl*/,
    const TRect& aRect) const
    {
    CXnControlAdapter::Draw( aRect, aGc );
    }
  	           
// -----------------------------------------------------------------------------
// CXnVolumeControlAdapter::Draw
// Draws the text component
// -----------------------------------------------------------------------------
//
void CXnVolumeControlAdapter::Draw(const TRect& aRect) const
    {
    CXnControlAdapter::Draw( aRect );
    }  

// ----------------------------------------------------------------------------
// CXnVolumeControlAdapter::HandlePointerEventL
// Handles the pointer events.
// ----------------------------------------------------------------------------
//
void CXnVolumeControlAdapter::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {    
    TRect rect( iVolumeControl->Rect() );
    
    TReal incrementWidth( TReal( rect.Width() ) / iSteps );
                                    
    TInt leftX( rect.iTl.iX );
       
    if( aPointerEvent.iType == TPointerEvent::EButton1Down || 
        aPointerEvent.iType == TPointerEvent::EMove && iButton1Down )        
        {   
        // By default change the value
        TBool valueSet( EFalse );
                            
        for( TInt i = 1; i <= iSteps && !valueSet; i++ )
            {            
            // Calculate pixel range 
            TReal minX( leftX + ( ( i - 1 ) * incrementWidth ) );
            TReal maxX( minX + incrementWidth );
                       
            if( aPointerEvent.iPosition.iX >= minX && aPointerEvent.iPosition.iX <= maxX )                
                {                     
                TBool changeValue( ETrue );
                             
                if( iHitRect == TRect::EUninitialized )
                    {
                    // Initialize                                    
                    iHitRect = TRect( minX, rect.iTl.iY, maxX, rect.iBr.iY );                       
                    }
                else if( !( iHitRect.Contains( aPointerEvent.iPosition ) ) )
                    {
                    // Reset, as position is changed
                    iHitRect = TRect( TRect::EUninitialized );
                    changeValue = EFalse;
                    }                        
                else
                    {
                    // Still in same slot, do not change value
                    changeValue = EFalse;
                    }                        
                
                if( changeValue )
                    {
                    TInt value = ValueL();
                    
                    if( value == i )
                        {
                        value = value - 1;
                        }
                    else
                        {
                        value = i;
                        }                    
                                                  
                    //SetValueL will notify also the observer
                    SetValueL( value );                       
                    DrawDeferred();                      
                    valueSet = ETrue;                                        
                    }
                }                                        
            }            
        }
                
    if( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iButton1Down = ETrue;
        }
        
    else if( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Reset
        iButton1Down = EFalse;        
        iHitRect = TRect( TRect::EUninitialized );
        }        
	}
	
// ----------------------------------------------------------------------------
// CXnVolumeControlAdapter::SetVolumeControlProperties
// Sets CSS properties for the volume control
// ----------------------------------------------------------------------------
//	
void CXnVolumeControlAdapter::SetVolumeControlPropertiesL()
	{
	// Read minrange property
	CXnProperty* minRange = iNode.GetPropertyL( XnPropertyNames::volumecontrol::KMinRange );
	TInt minValue = 0;
	
	if( minRange )
		{
		minValue = static_cast<TInt>( minRange->FloatValueL() );
		}
		
	// Read maxrange property
	CXnProperty* maxRange = iNode.GetPropertyL( XnPropertyNames::volumecontrol::KMaxRange );
	TInt maxValue = 20;
	
	if( maxRange )
		{
		maxValue = static_cast<TInt>( maxRange->FloatValueL() );
		}
				
	SetRangeL( minValue, maxValue );
	
	TInt midRange( iSteps / 2 );
	TInt volValue( 0 );
	
	if( iVolumeControl->Value() < minValue ||
	    iVolumeControl->Value() > maxValue )
		{
		// Set around to mid
		volValue = midRange;	
		}
			
    // Read value property
	CXnProperty* value = iNode.GetPropertyL( XnPropertyNames::action::KValue );
	
	if( value )
		{
		TInt valueProp = static_cast< TInt >( value->FloatValueL() );
		
		if( valueProp >= minValue && valueProp <= maxValue )
		    {
		    volValue = valueProp;
		    }
		}
    	
    SetValueL( volValue );			
	}
	
// ----------------------------------------------------------------------------
// CXnVolumeControlAdapter::GetCurrentValue
// ----------------------------------------------------------------------------
//		
TInt CXnVolumeControlAdapter::GetCurrentValueL()
	{
	CXnProperty* valueProperty = iNode.GetPropertyL( XnPropertyNames::action::KValue );
    return static_cast<TInt>( valueProperty->FloatValueL() );    
	}
	
// ----------------------------------------------------------------------------
// CXnVolumeControlAdapter::NotifyObserver
// ----------------------------------------------------------------------------
//	
void CXnVolumeControlAdapter::NotifyObserverL()
	{	
	if( iObserver )
		{
		iObserver->VolumeChanged( GetCurrentValueL() );
		}
	}
